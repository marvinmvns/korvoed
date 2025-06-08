#include <Arduino.h>
#include <Audio.h>           // ESP32-AudioI2S library
#include <SD.h>
#include <FastLED.h>

#define BUTTON_PIN 0     // GPIO for the playback button
#define LED_PIN    2     // GPIO driving an LED or NeoPixel
#define NUM_LEDS   1

CRGB leds[NUM_LEDS];
Audio audio;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);

  if (!SD.begin()) {
    Serial.println("SD init failed");
    while (true);
  }

  audio.setPinout(26, 25, 22);      // Example I2S pins
  audio.setVolume(15);              // 0..21
  randomSeed(analogRead(0));
}

String randomFile() {
  File dir = SD.open("/audio");
  if (!dir) return "";
  int count = 0;
  File entry;
  while (true) {
    entry = dir.openNextFile();
    if (!entry) break;
    if (!entry.isDirectory()) count++;
  }
  if (count == 0) return "";
  int target = random(count);
  dir.rewindDirectory();
  for (int i = 0; i <= target; i++) {
    entry = dir.openNextFile();
  }
  String path = String("/audio/") + entry.name();
  entry.close();
  dir.close();
  return path;
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    leds[0] = CRGB::Green;
    FastLED.show();
    delay(50); // debounce
    String file = randomFile();
    if (file.length()) {
      Serial.println(file);
      audio.connecttoFS(SD, file.c_str());
      uint8_t b = 0;
      int step = 5;
      while (audio.isRunning()) {
        audio.loop();
        leds[0] = CRGB(0, b, 0);
        FastLED.show();
        b += step;
        if (b == 0 || b == 250) step = -step;
        delay(10);
      }
    }
    leds[0] = CRGB::Black;
    FastLED.show();
    delay(200);
  }
}
