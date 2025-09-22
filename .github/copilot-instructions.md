# MoniAux ESP32 Auxiliary Control Device

MoniAux is an ESP32-S3 based auxiliary control device for audio systems featuring a TFT display, keypad input, rotary encoder, and WiFi connectivity with OTA updates. It's built using PlatformIO with the Arduino framework.

**Always reference these instructions first and fallback to search or bash commands only when you encounter unexpected information that does not match the info here.**

## Working Effectively

### Development Environment Setup
- Install Python 3.8+ and pip
- Install PlatformIO: `pip install platformio`
- Add PlatformIO to PATH: `export PATH=$PATH:~/.local/bin`
- Verify installation: `pio --version`

### Required Files Setup
- Create `include/secret.h` with WiFi credentials (this file is in .gitignore):
```cpp
#pragma once
#define SECRET_SSID "YourWiFiSSID"
#define SECRET_PASSWORD "YourWiFiPassword"
```

### Build Process
- **NEVER CANCEL**: Initial ESP32 platform installation takes 15-30 minutes depending on network speed. Set timeout to 45+ minutes.
- **NEVER CANCEL**: First build takes 5-15 minutes as dependencies are downloaded and compiled. Set timeout to 30+ minutes.
- Subsequent builds take 30-120 seconds.
- Network connectivity is required for initial setup - if blocked, builds will fail with `HTTPClientError`

### Dependencies (Automatically Managed)
The following libraries are automatically downloaded by PlatformIO during first build:
- `arduino-libraries/NTPClient` - Network time synchronization
- `adafruit/Adafruit GFX Library` - Graphics base library  
- `adafruit/Adafruit ST7735 and ST7789 Library` - TFT display driver
- `chris--a/Keypad@^3.1.1` - Matrix keypad input handling

### Essential Build Commands
```bash
# Install ESP32 platform (first time only - NEVER CANCEL, takes 15-30 minutes)
pio pkg install --platform espressif32

# Build firmware (NEVER CANCEL on first build - takes 5-15 minutes)
pio run

# Build and upload to device via USB
pio run --target upload --upload-port /dev/ttyUSB0

# Build and upload via OTA (default in platformio.ini)
pio run --target upload

# Open serial monitor
pio device monitor --baud 115200

# Build and upload via OTA with specific target
pio run --target upload --upload-port MoniAuxOTA.local
```

### Code Validation
- Check project configuration: `pio project config`
- Run syntax check: `pio check` (requires ESP32 platform installation)
- List available devices: `pio device list`
- Clean build cache: `pio system prune` (confirm with 'y')
- No unit tests exist - validation is done via serial monitor and device interaction
- Always test serial communication after changes using: `pio device monitor --baud 115200`

## Hardware Requirements

### Physical Components
- ESP32-S3 development board
- 128x128 TFT display (ST7735 controller)
- 3x4 matrix keypad (buttons 1-9, A, B, C)
- Rotary encoder with push button
- RGB LED (built-in on ESP32-S3)

### Pin Configuration (from mainConfig.h)
```
TFT Display (SPI2):
- SCLK: GPIO 6
- MOSI: GPIO 4  
- CS: GPIO 7
- DC: GPIO 2
- RST: GPIO 3

Keypad Matrix:
- Rows: GPIO 10, 11, 12
- Cols: GPIO 13, 14, 15, 16

Encoder:
- CLK (A): GPIO 18
- DT (B): GPIO 17
- SW: GPIO 9

RGB LED: GPIO RGB_BUILTIN
```

## Key Code Modules

### Core Files
- `src/main.cpp` - Main entry point and setup
- `src/display.cpp` - TFT display control and UI
- `src/input.cpp` - Keypad and encoder input handling
- `src/wifi_ota.cpp` - WiFi connection and OTA updates
- `src/serial_comm.cpp` - Serial command processing
- `include/mainConfig.h` - Hardware and system configuration

### Module Responsibilities
- **Display**: TFT graphics, status indicators, time display
- **Input**: Dual-core input processing (keypad on core 0, encoder on core 1)  
- **WiFi/OTA**: Network connectivity, NTP time sync, over-air updates
- **Serial**: Debug commands and external communication protocol

## Validation Scenarios

### Essential Testing After Changes
1. **Basic Device Function**:
   - Upload firmware via USB: `pio run --target upload`
   - Open serial monitor: `pio device monitor --baud 115200`
   - Verify device boots and shows startup screen
   - Test keypad: press buttons 1-9, A, B, C - should see `Btn1`, `Btn2`, etc. in serial output

2. **Input System Validation**:
   - Press keypad buttons - verify LED responses and serial output
   - Rotate encoder left/right - should see `EncoderL`/`EncoderR` messages
   - Press encoder button - should see `EncoderSW` message

3. **Display System Validation**:
   - Verify TFT shows startup screen with device name
   - Check status indicators (WiFi/Serial) in top-right corner
   - Confirm time display updates if WiFi connected

4. **WiFi/OTA Testing** (if network available):
   - Configure valid WiFi credentials in `include/secret.h`
   - Verify device connects to WiFi and shows IP address
   - Test serial command: `comm:wfstat` - should show connection details
   - Test OTA upload: `pio run --target upload --upload-port MoniAuxOTA.local`

### Serial Commands for Testing
```
comm:scan              - Scan WiFi networks
comm:wfstat            - Show WiFi status
comm:restart           - Restart device  
comm:clearScreen       - Clear display
comm:setRGB:255,0,0    - Set LED to red
settime:14:30          - Set time manually
```

## Build Timing and Expectations

### First-Time Setup (NEVER CANCEL)
- ESP32 platform download: 15-30 minutes
- Initial dependency resolution: 5-10 minutes  
- First build compilation: 5-15 minutes
- **Total initial setup: 25-55 minutes**

### Regular Development
- Clean build: 2-5 minutes
- Incremental build: 30-120 seconds
- Upload via USB: 30-60 seconds
- Upload via OTA: 20-45 seconds

### Troubleshooting Build Issues
- Network timeouts: Retry commands, increase timeout to 60+ minutes
- Platform install fails: Clear PlatformIO cache with `pio system prune` (confirm with 'y')
- Missing secret.h: Create file with WiFi credentials as shown above
- Upload fails: Check USB connection or use OTA if device is networked
- Verify project config: `pio project config` should show valid ESP32-S3 configuration

## Common Development Tasks

### Adding New Features
- Always modify appropriate module (display.cpp, input.cpp, etc.)
- Test via serial monitor after each change
- Validate both USB and OTA upload methods work
- Update pin configurations in mainConfig.h if hardware changes

### Debugging
- Use serial output extensively: `Serial.println()` calls
- Monitor with: `pio device monitor --baud 115200`  
- Use built-in LED for visual debugging: `updateLEDColor(r,g,b)`
- Test input events: watch for `Btn`, `EncoderL`, `EncoderR`, `EncoderSW` messages

### Code Organization
- Headers go in `include/` directory
- Source files go in `src/` directory  
- Configuration in `mainConfig.h` and `secret.h`
- No external libraries in `lib/` - all dependencies via platformio.ini

## Project Structure Reference
```
MoniAux/
├── .github/copilot-instructions.md  # This file
├── include/                         # Header files
│   ├── mainConfig.h                # Hardware configuration
│   ├── secret.h                    # WiFi credentials (not in git)
│   ├── display.h, input.h, etc.   # Module interfaces
├── src/                            # Source files  
│   ├── main.cpp                    # Main entry point
│   ├── display.cpp                 # TFT display control
│   ├── input.cpp                   # Input handling (dual-core)
│   ├── wifi_ota.cpp               # WiFi and OTA functionality
│   ├── serial_comm.cpp            # Serial command processor
│   └── globals.cpp                # Global variables
├── platformio.ini                  # Build configuration
└── README.md                      # Project documentation
```

This is an embedded firmware project - there are no traditional "tests" but validation is done through device interaction and serial monitoring.