#include "ServoGestureController.h"
#include <M5StackChan.h>

namespace {
// StackChan-BSP units: 10 = 1 degree.
// Production examples use X +/-1000 for 100-degree turns, Y 0..900 for pitch.
// Keep expressive/emotional motions moderate, and expose separate wide look presets.
// User-calibrated neutral pitch: commanding Y=0 sits a little too low on this robot,
// so everyday gestures return to a slightly raised neutral instead of bottoming out.
constexpr int kNeutralY = 120;
// Visual search keeps the camera slightly raised so a horizontal sector scan
// sees more of the room without changing the normal conversational neutral.
constexpr int kSearchY = 180;
ServoGestureController::Step kCenter[] = {
    {0, kNeutralY, 500, 700},
};

ServoGestureController::Step kLookAtUser[] = {
    {0, kNeutralY, 500, 650},
};

ServoGestureController::Step kLookRightSmall[] = {
    {180, kNeutralY, 600, 360},
};

ServoGestureController::Step kLookLeftSmall[] = {
    {-180, kNeutralY, 600, 360},
};

ServoGestureController::Step kLookRight[] = {
    {650, kNeutralY, 650, 760},
};

ServoGestureController::Step kLookLeft[] = {
    {-650, kNeutralY, 650, 760},
};

ServoGestureController::Step kLookUp[] = {
    {0, 520, 620, 740},
};

ServoGestureController::Step kLookDown[] = {
    {0, 60, 620, 650},
};

ServoGestureController::Step kLookTopRight[] = {
    {420, 480, 620, 760},
};

ServoGestureController::Step kLookTopLeft[] = {
    {-420, 480, 620, 760},
};

ServoGestureController::Step kSearchLeftWide[] = {
    {-700, kSearchY, 650, 820},
};

ServoGestureController::Step kSearchLeft[] = {
    {-350, kSearchY, 620, 650},
};

ServoGestureController::Step kSearchCenter[] = {
    {0, kSearchY, 600, 620},
};

ServoGestureController::Step kSearchRight[] = {
    {350, kSearchY, 620, 650},
};

ServoGestureController::Step kSearchRightWide[] = {
    {700, kSearchY, 650, 820},
};

// Based on BSP examples/Servo/Dance/Dance.ino: quick keyframes, no long pause at extremes.
ServoGestureController::Step kShakeNo[] = {
    {0, 0, 500, 100, true},
    {120, 0, 700, 170, true},
    {-120, 0, 700, 170, true},
    {110, 0, 680, 160, true},
    {-110, 0, 680, 160, true},
    {0, 0, 580, 240, true},
};

ServoGestureController::Step kNodYes[] = {
    {0, 0, 500, 100, true},
    {0, 70, 700, 170, true},
    {0, -40, 700, 170, true},
    {0, 60, 680, 160, true},
    {0, 0, 580, 240, true},
};

ServoGestureController::Step kCuriousTilt[] = {
    {120, 60, 500, 340, true},
    {0, 0, 480, 420, true},
};

ServoGestureController::Step kSpeakingMicro[] = {
    {55, 20, 620, 150, true},
    {-50, 5, 620, 150, true},
    {35, 25, 600, 140, true},
    {0, 0, 580, 180, true},
};
}  // namespace

void ServoGestureController::begin() {
  stop();
  current_ = "idle";
}

void ServoGestureController::loop() {
  if (!active_) return;
  uint32_t now = millis();
  if (now < nextStepAt_) return;

  if (stepIndex_ >= stepCount_) {
    active_ = false;
    if (updateAnchorOnComplete_) {
      anchorX_ = clampInt(pendingAnchorX_, kMinX, kMaxX);
      anchorY_ = clampInt(pendingAnchorY_, kMinY, kMaxY);
      Serial.printf("ServoGesture: anchor x=%d y=%d\n", anchorX_, anchorY_);
    }
    updateAnchorOnComplete_ = false;
    current_ = "idle";
    return;
  }

  const Step& step = steps_[stepIndex_++];
  issueStep(step);
  nextStepAt_ = now + step.holdMs;
}

bool ServoGestureController::queueGesture(const String& name) {
  if (name == "center_head" || name == "center" || name == "home") {
    start("center_head", kCenter, sizeof(kCenter) / sizeof(kCenter[0]), true);
    return true;
  }
  if (name == "look_at_user") {
    start("look_at_user", kLookAtUser, sizeof(kLookAtUser) / sizeof(kLookAtUser[0]), true);
    return true;
  }
  if (name == "look_right_small") {
    start("look_right_small", kLookRightSmall, sizeof(kLookRightSmall) / sizeof(kLookRightSmall[0]), true);
    return true;
  }
  if (name == "look_left_small") {
    start("look_left_small", kLookLeftSmall, sizeof(kLookLeftSmall) / sizeof(kLookLeftSmall[0]), true);
    return true;
  }
  if (name == "look_right") {
    start("look_right", kLookRight, sizeof(kLookRight) / sizeof(kLookRight[0]), true);
    return true;
  }
  if (name == "look_left") {
    start("look_left", kLookLeft, sizeof(kLookLeft) / sizeof(kLookLeft[0]), true);
    return true;
  }
  if (name == "look_up") {
    start("look_up", kLookUp, sizeof(kLookUp) / sizeof(kLookUp[0]), true);
    return true;
  }
  if (name == "look_down") {
    start("look_down", kLookDown, sizeof(kLookDown) / sizeof(kLookDown[0]), true);
    return true;
  }
  if (name == "look_top_right") {
    start("look_top_right", kLookTopRight, sizeof(kLookTopRight) / sizeof(kLookTopRight[0]), true);
    return true;
  }
  if (name == "look_top_left") {
    start("look_top_left", kLookTopLeft, sizeof(kLookTopLeft) / sizeof(kLookTopLeft[0]), true);
    return true;
  }
  if (name == "search_left_wide") {
    start("search_left_wide", kSearchLeftWide, sizeof(kSearchLeftWide) / sizeof(kSearchLeftWide[0]), true);
    return true;
  }
  if (name == "search_left") {
    start("search_left", kSearchLeft, sizeof(kSearchLeft) / sizeof(kSearchLeft[0]), true);
    return true;
  }
  if (name == "search_center") {
    start("search_center", kSearchCenter, sizeof(kSearchCenter) / sizeof(kSearchCenter[0]), true);
    return true;
  }
  if (name == "search_right") {
    start("search_right", kSearchRight, sizeof(kSearchRight) / sizeof(kSearchRight[0]), true);
    return true;
  }
  if (name == "search_right_wide") {
    start("search_right_wide", kSearchRightWide, sizeof(kSearchRightWide) / sizeof(kSearchRightWide[0]), true);
    return true;
  }
  if (name == "shake_no") {
    start("shake_no", kShakeNo, sizeof(kShakeNo) / sizeof(kShakeNo[0]));
    return true;
  }
  if (name == "nod_yes") {
    start("nod_yes", kNodYes, sizeof(kNodYes) / sizeof(kNodYes[0]));
    return true;
  }
  if (name == "curious_tilt") {
    start("curious_tilt", kCuriousTilt, sizeof(kCuriousTilt) / sizeof(kCuriousTilt[0]));
    return true;
  }
  if (name == "speaking_micro_motion") {
    start("speaking_micro_motion", kSpeakingMicro, sizeof(kSpeakingMicro) / sizeof(kSpeakingMicro[0]));
    return true;
  }
  if (name == "stop_motion" || name == "stop") {
    stop();
    return true;
  }
  return false;
}

bool ServoGestureController::queueSteps(const String& name, const Step* steps, uint8_t count) {
  if (!steps || count == 0) return false;
  // Parametric motions are interpreted relative to the current persistent look
  // anchor. The final keyframe becomes the new anchor, so a deliberate "look a
  // little left" survives later nods/tilts/speaking micro-motions.
  start(name.length() ? name : String("custom_motion"), steps, count, true);
  return true;
}

void ServoGestureController::stop() {
  active_ = false;
  stepCount_ = 0;
  stepIndex_ = 0;
  nextStepAt_ = 0;
  current_ = "idle";
  M5StackChan.Motion.stop();
}

void ServoGestureController::start(const String& name, const Step* steps, uint8_t count, bool updateAnchorOnComplete) {
  if (count > kMaxSteps) count = kMaxSteps;
  for (uint8_t i = 0; i < count; ++i) {
    steps_[i] = steps[i];
  }
  stepCount_ = count;
  stepIndex_ = 0;
  current_ = name;
  active_ = true;
  nextStepAt_ = 0;
  updateAnchorOnComplete_ = updateAnchorOnComplete;
  if (updateAnchorOnComplete_ && count > 0) {
    const Step& last = steps_[count - 1];
    pendingAnchorX_ = last.relative ? anchorX_ + last.x : last.x;
    pendingAnchorY_ = last.relative ? anchorY_ + last.y : last.y;
  }
  Serial.printf("ServoGesture: queued %s steps=%u anchor=%d,%d update_anchor=%s\n",
                name.c_str(), static_cast<unsigned>(count), anchorX_, anchorY_,
                updateAnchorOnComplete_ ? "yes" : "no");
}

void ServoGestureController::issueStep(const Step& step) {
  int rawX = step.relative ? anchorX_ + step.x : step.x;
  int rawY = step.relative ? anchorY_ + step.y : step.y;
  int x = clampInt(rawX, kMinX, kMaxX);
  int y = clampInt(rawY, kMinY, kMaxY);
  int speed = clampInt(step.speed, kMinSpeed, kMaxSpeed);
  Serial.printf("ServoGesture: step x=%d y=%d speed=%d hold=%u relative=%s\n",
                x, y, speed, static_cast<unsigned>(step.holdMs), step.relative ? "yes" : "no");
  M5StackChan.Motion.move(x, y, speed);
}

int ServoGestureController::clampInt(int value, int lo, int hi) {
  if (value < lo) return lo;
  if (value > hi) return hi;
  return value;
}
