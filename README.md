# ClearPath Servo HLFB Monitor

This project is an Arduino-based tool for monitoring the High Level Feedback (HLFB) signals from up to four ClearPath servo motors. It is designed to run on an Adafruit Pro Trinket 5V (ATmega328) or compatible board, using the PlatformIO build system.
I will be using the output in my main DRO running on Arduino Due to display load meters on the display.
Because the HLFB runs on 5 volts and Due is 3.3 I decided to use this solution.
I couldn't get a level shifter to read the PWM directly from HLFB.
I still still need a level shifter but I am sending over UART.
This is what I had on hand. [text](https://www.adafruit.com/product/757)

## Features
- Monitors HLFB signals from up to 4 ClearPath motors (pins 9–12)
- Calculates and reports the load percentage for each motor based on HLFB duty cycle
- Detects motor inactivity or failure
- Configurable parameters via serial commands:
  - Motor count (1–4)
  - Per-motor adjustment factors
  - Monitoring delay interval
  - Pulse-in timeout
- Serial command interface for configuration and help
- Status indication via onboard LED and external pin

## Hardware Requirements
- Adafruit Pro Trinket 5V (or compatible ATmega328 Arduino board)
- Up to 4 ClearPath motors with HLFB outputs
- HLFB signals connected to digital pins 9, 10, 11, and 12
- Pin 8 used for external failure indication
- Onboard LED (pin 13) for status

## Software Requirements
- [PlatformIO](https://platformio.org/) (for building and uploading)
- Arduino framework

## Usage
1. **Build and upload** the firmware using PlatformIO:
   ```sh
   pio run --target upload
   ```
2. **Connect** to the device via serial terminal (115200 baud).
3. **Available serial commands:**
   - `MCT:<value>` — Set motor count (1–4)
   - `ADJ:<v1>[,<v2>,<v3>,<v4>]` — Set adjustment factors (floats, comma-separated)
   - `DELAY:<value>` — Set delay time in ms
   - `TIMEOUT:<value>` — Set pulseIn timeout in µs
   - `START` — Start HLFB monitoring
   - `STOP` — Stop HLFB monitoring
   - `HELP` — Show help message

## Example
```
MCT:3
ADJ:86.5,87.0,85.0
DELAY:1000
START
```

## File Structure
- `src/main.cpp` — Main firmware source code
- `platformio.ini` — PlatformIO project configuration
- `include/`, `lib/`, `test/` — Standard PlatformIO folders

## License
See [LICENSE](LICENSE) for details.
