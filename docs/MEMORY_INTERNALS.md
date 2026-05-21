# StackChan Memory v1 ТЗ

Цель: автономная SD-backed memory v1 для StackChan без зависимости от Hermes gateway и без embeddings/summarizer в первом этапе.

## Scope v1

Делаем:
- сначала проверяем, можно ли стабильно получать Gemini Live input/output transcription;
- append-only `events/YYYY-MM-DD.jsonl` для сырья;
- append-only recent dialogue logs for full transcripts of the latest 1-2 days when transcription is available;
- append-only semantic `memories.jsonl` как canonical store;
- active-set traversal для tombstones/supersedes;
- atomic mutable `profile.json`;
- configurable lexical/tag retrieval только по active memories;
- prompt context injection из retrieval + bounded recent dialogue context;
- HTTP APIs для remember/search/recent/forget/profile.

Не делаем в v1:
- Gemini Flash Lite summarizer after every call/turn;
- Embedding API;
- vector storage/search;
- physical compaction;
- retrieval по raw events;
- auto-saving всего разговора в memories;
- synchronous network calls in the hot audio/Gemini Live path.

## Files

```text
/app/StackChan/memory/
  events/YYYY-MM-DD.jsonl
  dialogues/YYYY-MM-DD.jsonl
  memories.jsonl
  profile.json
  profile.json.bak
  retrieval_config.json
```

Future, not v1:

```text
vectors.bin
vector_index.jsonl
embedding_config.json
embedding_state.json
```

## Events vs dialogues vs memories lifecycle

`events`:
- raw touch/system/audio metadata/state/debug events;
- append-only;
- may be written even before NTP sync;
- not used directly for retrieval.

`dialogues`:
- near-raw user/assistant text transcript turns from Gemini Live input/output transcription when available;
- append-only daily JSONL;
- keep latest 1-2 days as bounded fresh conversation context;
- can be injected directly into prompt within a strict char/token budget because it is recent and maximally detailed;
- not treated as durable semantic memory;
- later becomes input material for Gemini Flash Lite structured summarizer.

`memories`:
- semantic facts/preferences/summaries/explicit user memories only;
- append-only;
- retrieval only over active memories;
- in v1 only explicit/manual/system-curated memories, not raw conversation spam.

Architecture rule:
- first prove stable Live API transcription capture;
- if transcription works, keep full recent dialogues for 1-2 days and use them as fresh context;
- do not summarize after every call/turn;
- batch summarization later converts accumulated dialogues into semantic memories for retrieval/RAG.

## Memory record schema

```json
{
  "id": "YYYYMMDD-HHMMSS-random64",
  "ts": "ISO8601",
  "kind": "explicit|fact|preference|summary|open_thread|tombstone",
  "text": "...",
  "summary": "...",
  "tags": ["..."],
  "importance": 0.0,
  "source": "user_explicit|system|manual|future_summarizer",
  "supersedes": null,
  "deletes": null,
  "deleted": false
}
```

ID format: `YYYYMMDD-HHMMSS-random64`, where `random64` is 16 hex chars (~64 bits entropy), not 16 bits.

## Time/NTP rule

Memory IDs depend on valid wall-clock time.

- For `memories`: do not create canonical memory records until time is valid (`ts > 2024-01-01`). If remember is requested before NTP sync, buffer/pending-reject with clear status rather than writing a 1970 ID.
- For `events`: still log even without NTP because boot/touch diagnostics matter. Use `millis:` timestamp or `undated` path until time sync; optionally later annotate, but do not block events.

## JSONL corruption handling

Power loss can leave the last append partially written.

At boot/load:
- read JSONL line-by-line;
- parse each line independently;
- skip invalid line(s), especially incomplete final line;
- log skipped count;
- truncate file to last valid newline once during startup/repair if possible;
- never let one bad line make the entire memory store fail.

## Tombstones/superseding active set

Append-only deletion/replacement:

- tombstone: record with `kind=tombstone` and `deletes=<old_id>`;
- superseding: record with `supersedes=<old_id>`.

At read/startup:
1. collect `deleted_ids` from tombstones/deleted records;
2. collect `superseded_ids` from `.supersedes`;
3. active memory if `id` not in deleted/superseded and `kind != tombstone`.

Implementation note: keep `isActive()` / `buildActiveIndex()` separate from retrieval so later it can be replaced with persistent active-index.

## Tags in v1

No summarizer exists in v1, so tags come from:
- explicit `POST /api/memory/remember` tags;
- simple local auto-tagger if tags are missing.

Auto-tagger v1 suggestion:
- use `summary` if present, else `text`;
- lowercase/tokenize;
- remove small RU/EN stopword list;
- take top 3 rare-ish/long tokens;
- never call API;
- mark generated tags as local/heuristic if schema needs it.

Watch during first week: if tags are empty in >80% records, retrieval weights should be adjusted or auto-tagging improved.

## Retrieval v1

Retrieval reads only active `memories`, never raw `events`.

Score components:
- exact tag match;
- substring match;
- keyword overlap;
- importance;
- recency.

Weights live in `/app/StackChan/memory/retrieval_config.json`, not hardcoded.

Example:

```json
{
  "top_k": 6,
  "max_context_chars": 2500,
  "weights": {
    "tag": 0.35,
    "substring": 0.25,
    "keyword": 0.20,
    "importance": 0.10,
    "recency": 0.10
  },
  "min_score": 0.05
}
```

If real tags are sparse, config may need lower tag weight.

## profile.json atomicity

`profile.json` is mutable state, unlike append-only JSONL.

Write pattern:
1. write `profile.json.tmp`;
2. flush/close;
3. rename current `profile.json` to `profile.json.bak` if exists;
4. rename tmp to `profile.json`.

Read pattern:
1. try `profile.json`;
2. if invalid, try `profile.json.bak`;
3. if both invalid, create empty default profile.

## Future embedding notes, not v1

When embeddings are added:
- metadata lives in `vector_index.jsonl`, not inside canonical memory records;
- `vectors.bin` stores binary vectors;
- embedding model/dim/dtype is versioned;
- model mismatch means stale, not silently mixed;
- embed `summary`, not raw `text`;
- backfill is async, idle/charging only, rate-limited;
- persist `embedding_state.json` with daily calls, last 429/5xx, backoff, cursor.

Example future `embedding_state.json`:

```json
{
  "date_utc": "2026-05-07",
  "calls_today": 0,
  "last_error": null,
  "last_429_ts": null,
  "last_5xx_ts": null,
  "backoff_until": null,
  "backfill_cursor": null
}
```
