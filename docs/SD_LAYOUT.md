# SD card layout

The firmware stores mutable runtime state on the SD card under `/app/StackChan/`.

## Directory tree

```text
/app/StackChan/
  config/
    runtime.json
    gateway.json
    summary.json
  prompts/
    system.txt
    persona.txt
  secrets/
    gemini_api_key.txt
    wifi_password.txt
    gateway_token.txt
    web_password_sha256.txt
    meta.jsonl
  memory/
    events/YYYY-MM-DD.jsonl
    dialogues/YYYY-MM-DD.jsonl
    summaries.jsonl
    memories.jsonl
    profile.json
  camera/
    latest.jpg
```

The firmware creates some directories automatically after SD mount, but for first configuration it is easiest to prepare the files manually.

## Minimal Wi-Fi + Web UI setup

Create:

```text
/app/StackChan/config/runtime.json
/app/StackChan/secrets/wifi_password.txt
```

Example `runtime.json`:

```json
{
  "robot_id": "stackchan",
  "wifi_enabled": true,
  "wifi_ssid": "YOUR_WIFI_SSID",
  "web_enabled": true,
  "gemini_enabled": false,
  "gateway_enabled": false
}
```

Example `wifi_password.txt`:

```text
YOUR_WIFI_PASSWORD
```

Boot the robot, read the IP from serial logs, then open `http://ROBOT_IP/`.

## Gemini setup

Add:

```text
/app/StackChan/secrets/gemini_api_key.txt
```

and enable Gemini in `runtime.json`:

```json
{
  "gemini_enabled": true,
  "gemini_model": "models/gemini-3.1-flash-live-preview",
  "gemini_voice": "Puck"
}
```

You can also edit prompts:

```text
/app/StackChan/prompts/system.txt
/app/StackChan/prompts/persona.txt
```

## Optional gateway setup

Gateway integration is optional. Add `/app/StackChan/config/gateway.json` or set the equivalent fields in `runtime.json`:

```json
{
  "gateway_enabled": true,
  "gateway_base_url": "http://YOUR_GATEWAY_HOST:8811/stackchan",
  "robot_id": "stackchan"
}
```

If the gateway requires a token, store it in:

```text
/app/StackChan/secrets/gateway_token.txt
```

## Secrets policy

Never commit real files from `/app/StackChan/secrets/` or private runtime SD dumps. The Web/API status endpoints redact secret values as `set` or `missing`.
