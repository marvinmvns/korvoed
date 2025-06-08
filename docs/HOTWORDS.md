# Hotword Integration Guide

The examples in this repository focus solely on audio playback. No hotword detection is implemented out of the box. If you want to trigger playback through a voice command, you can integrate a third-party hotword detection library.

## Choosing a Library

- **ESP-SR** – Espressif's speech recognition library. Includes offline keyword spotting models for certain languages.
- **Snowboy** (deprecated) or other community projects providing small-footprint hotword engines.

Refer to the chosen library's documentation for installation steps.

## Adding Hotword Models

1. Obtain or train a model for your hotword. The library's tools usually produce a model file (e.g. `.bin` or `.pb`).
2. Store the model on the ESP32. Common approaches:
   - Include it in a SPIFFS partition (placed in a `/hotwords` directory).
   - Load it from an SD card at boot.
3. Modify your application to initialize the hotword engine with the path to your model.

## Configuration Example

Below is a high-level example of how you might integrate a hotword library with the Arduino sketch:

```cpp
#include <YourHotwordLib.h>

YourHotwordDetector detector;

void setup() {
  detector.begin("/hotwords/your_model.bin");
}

void loop() {
  if (detector.detected()) {
    // Call the existing audio playback routine here
  }
}
```

Consult the hotword library's documentation for exact APIs.


