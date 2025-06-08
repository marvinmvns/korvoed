# ESP32-Korvo Audio Player

ESP32-Korvo is an ESP32-based development board with integrated audio features. This repository provides a simple Arduino sketch showing how to play random audio files from an SD card when a button is pressed.

## Dependencies

- [ESP32-AudioI2S](https://github.com/schreibfaul1/ESP32-audioI2S) – for decoding and outputting MP3/WAV/OGG files over the I2S interface.
- [FastLED](https://github.com/FastLED/FastLED) – used to drive an LED indicator.

Install these libraries through the Arduino Library Manager before compiling the example.

## Setup

1. Clone this repository and open `examples/RandomAudioPlayer/RandomAudioPlayer.ino` in the Arduino IDE or your preferred environment.
2. Copy your `.mp3`, `.wav` or `.ogg` files onto a microSD card inside a directory named `audio` at the root of the card (e.g. `/audio/song.mp3`).
3. Insert the SD card into the ESP32-Korvo board.
4. Wire a momentary push button between GPIO0 and GND. When pressed, it will trigger playback of a random file.
5. Connect an LED (or NeoPixel) to GPIO2 through an appropriate resistor. The example flashes the LED while audio is playing.

## Usage

Upload the sketch to the ESP32-Korvo. When you press the button the board selects a random file from `/audio` on the SD card and plays it through the on‑board audio output. The LED lights during playback.

## ESP-IDF Example

For developers using the ESP-IDF, a minimal example is provided in `examples/IDF_RandomAudioPlayer`. It plays a random WAV file from the SD card whenever GPIO0 is pressed. Build it with:

```bash
idf.py set-target esp32
idf.py flash monitor
```

The audio files must be placed inside `/sdcard/audio` on the SD card in standard WAV format.
