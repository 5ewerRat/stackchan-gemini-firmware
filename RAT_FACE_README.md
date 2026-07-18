# StackChan Rat Face Test Guide

## Overview
This firmware modification adds a new "rat" emotion to StackChan that features a distinctive rat face with animated blinking, twitching ears, whiskers, and nose movements.

## Features Added

### Rat Face Features:
- **Large ears** with subtle twitching animation
- **Whiskers** with independent twitching movements
- **Nose** with subtle twitching behavior
- **Frequent blinking** (more frequent than neutral mode)
- **Brown color scheme** for authentic rat appearance

### LED Animation:
- **Twitching whisker-like patterns** on the RGB LEDs
- **Brownish color theme** matching the rat face
- **Random segment lighting** for realistic rat-like movement

## Usage

### Setting the Rat Emotion
To activate the rat face, send the emotion command "rat" to StackChan:

```
emotion.setEmotion("rat")
```

### Example Implementation
```cpp
#include "EmotionController.h"

EmotionController emotion;

void setup() {
  emotion.begin();
  // Set rat face
  emotion.setEmotion("rat");
}

void loop() {
  emotion.loop();
  // Other robot logic...
}
```

### Testing the Rat Face
1. Build and flash the modified firmware to your StackChan
2. Use the web interface or direct API calls to set emotion to "rat"
3. Observe the distinctive rat face with animated features
4. Watch the LED patterns that simulate rat-like twitching movements

## Technical Details

### Animation Timing:
- **Ears**: Twitch every 8 frames (subtle movement)
- **Whiskers**: Twitch every 12 frames (independent movement)
- **Nose**: Twitch every 15 frames (subtle movement)
- **Blinking**: Every 30 frames (more frequent than neutral mode's 55 frames)

### Colors Used:
- **Face**: TFT_BROWN (main color)
- **Ears/Details**: TFT_DARKGREY (inner ear details)
- **Whiskers**: TFT_WHITE
- **Nose**: TFT_BLACK
- **LEDs**: Brownish theme (red: 100, green: 60, blue: 100 scaled)

## Integration
The rat face integrates seamlessly with the existing emotion system:
- All original emotions remain unchanged
- The rat emotion is treated as a standard emotion mode
- LED animations are synchronized with face animations
- Maintains the same frame rate and performance as original emotions

## Files Modified
- `src/EmotionController.h`: Added Rat mode and renderRat declaration
- `src/EmotionController.cpp`: 
  - Added Rat mode to emotion parsing
  - Added rat-specific face rendering with ears, whiskers, nose
  - Added rat LED animation pattern
  - Fixed function name consistency (drawLabel → renderLabel)

## Building
The firmware builds successfully with PlatformIO:
```bash
pio run -e m5stack-cores3
```

The resulting firmware.bin can be flashed to your StackChan using standard methods.