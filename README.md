# ESP32-Korvo Audio Player

ESP32-Korvo is an ESP32-based development board with integrated audio features. This repository provides a simple Arduino sketch showing how to play random audio files from an SD card when a button is pressed.

## Dependencies

- [ESP32-AudioI2S](https://github.com/schreibfaul1/ESP32-audioI2S) – for decoding and outputting MP3/WAV/OGG files over the I2S interface.
- [FastLED](https://github.com/FastLED/FastLED) – used to drive an LED indicator.

Install these libraries through the Arduino Library Manager before compiling the example.

## Arduino IDE Setup

1. [Download and install the Arduino IDE](https://www.arduino.cc/en/software) for your operating system.
2. Open **Preferences** and add the ESP32 boards manager URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`.
3. Install the **esp32** core from **Tools → Board → Boards Manager**.
4. Install the **ESP32-AudioI2S** and **FastLED** libraries via **Tools → Manage Libraries**.
5. Clone this repository and open `examples/RandomAudioPlayer/RandomAudioPlayer.ino`.

## Running the Example

1. Copy your `.mp3`, `.wav` or `.ogg` files onto a microSD card inside a directory named `audio` at the root of the card (e.g. `/audio/song.mp3`).
2. Insert the SD card into the ESP32-Korvo board.
3. Wire a momentary push button between GPIO0 and GND. When pressed, it will trigger playback of a random file.
4. Connect an LED (or NeoPixel) to GPIO2 through an appropriate resistor. The example flashes the LED while audio is playing.

## Usage

Upload the sketch to the ESP32-Korvo. When you press the button the board selects a random file from `/audio` on the SD card and plays it through the on‑board audio output. The LED lights during playback.

## ESP-IDF Setup

Developers who prefer Espressif's official [ESP-IDF](https://docs.espressif.com/projects/esp-idf/) can build the example located in `examples/IDF_RandomAudioPlayer`.

1. Install the ESP-IDF toolchain by following the instructions for your platform.
2. Open a terminal, set up the environment (`export PATH` and run `idf.py export` if required).
3. Navigate to `examples/IDF_RandomAudioPlayer`.
4. Run the following commands to build and flash:

```bash
idf.py set-target esp32
idf.py build
idf.py flash monitor
```

The audio files must be placed inside `/sdcard/audio` on the SD card in standard WAV format.

## Wake Word Example

A simple IDF example combining wake word detection with audio playback is available in `examples/IDF_WakeWordPlayer`.
It uses the ESP-SR library to detect the built-in "Hi ESP" phrase via the onboard microphones and plays a random WAV file from the SD card when triggered.

## Hotword Integration

The repository does not include voice recognition. If you want to start playback in response to a voice command, integrate a hotword detection library and call the playback functions when the hotword is detected. See [docs/HOTWORDS.md](docs/HOTWORDS.md) for guidance.
