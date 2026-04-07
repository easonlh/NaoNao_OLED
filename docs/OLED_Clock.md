# OLED Clock Display Documentation

## Overview

This clock implementation displays real-time on the SSD1306 OLED using NTP (Network Time Protocol) synchronization via WiFi.

## Features

- ✅ Real-time clock (HH:MM:SS)
- ✅ Date display (YYYY-MM-DD + Weekday)
- ✅ WiFi connection status indicator
- ✅ Automatic NTP time synchronization (every hour)
- ✅ Blinking colon separator for seconds
- ✅ Large 38px time digits for clear visibility

## File Structure

```
src/
└── main_clock.cpp    # Clock implementation
```

## Configuration

### WiFi Settings

Edit lines 10-11 in `main_clock.cpp`:

```cpp
const char* WIFI_SSID = "YourWiFiSSID";        // Change to your WiFi name
const char* WIFI_PASSWORD = "YourWiFiPassword"; // Change to your WiFi password
```

### Time Zone

Edit line 15 for your timezone (default is China GMT+8):

```cpp
const long GMT_OFFSET = 8 * 3600;  // GMT+8 for China
```

Common timezones:
- China (Beijing): `8 * 3600`
- Japan: `9 * 3600`
- UK (London): `0`
- US (Pacific): `-8 * 3600`
- US (Eastern): `-5 * 3600`

### NTP Server

Edit line 14 to change the NTP server:

```cpp
const char* NTP_SERVER = "pool.ntp.org";
```

Alternative servers:
- `time.nist.gov` - US NIST time server
- `time.google.com` - Google time server
- `time.windows.com` - Microsoft time server
- `pool.ntp.org` - Global NTP pool (default)

## Display Layout

```
Screen: 128x64 pixels
┌─────────────────────────────────┐
│ 0           64        100  127  │
│ 0                        [WiFi]│ ← Status (y=12)
│                                 │
│                                 │
│ 10    14:25:36            118   │ ← Time (y=35, 38px font)
│                                 │
│ 10  2024-01-15 Mon         118  │ ← Date (y=55, 10px font)
│                                 │
└─────────────────────────────────┘
```

## Functions

### `connectWiFi()`
Connects to the configured WiFi network.
- Timeout: 30 attempts (15 seconds)
- Displays connection status on OLED
- Prints debug info to Serial

### `initTime()`
Initializes time from NTP server.
- Configures GMT offset and daylight saving
- Timeout: 30 attempts (15 seconds)
- Updates `lastSyncTime` on success

### `updateTime()`
Re-synchronizes time with NTP server.
- Called automatically every hour
- Updates `lastSyncTime` on success

### `displayTime()`
Renders the current time (HH:MM:SS).
- Font: `u8g2_font_logisoso38_tf` (38px)
- Position: (10, 35)
- Format: 24-hour with leading zeros

### `displayDate()`
Renders the current date and weekday.
- Font: `u8g2_font_ncenB10_tr` (10px)
- Position: (10, 55)
- Format: `YYYY-MM-DD Ddd`

### `displayStatus()`
Renders status indicators.
- WiFi status: `[WiFi]` or `[No WiFi]`
- Blinking colons (every 2 seconds)

## Build & Upload

```bash
# Make sure main_clock.cpp is the active source
# (rename to main.cpp or configure platformio.ini)

# Build
pio run

# Upload
pio run -t upload

# Build and upload
pio run -t upload
```

## Serial Monitor

Open serial monitor to view debug output:

```bash
pio device monitor -b 115200
```

Expected output:
```
Connecting to YourWiFiSSID
............
WiFi connected!
IP: 192.168.1.100
Waiting for NTP time...
.
Time synchronized!
Setup complete!
```

## Troubleshooting

### WiFi Connection Failed

**Symptoms:**
- Display shows `[No WiFi]`
- Serial shows "WiFi connection failed!"

**Solutions:**
1. Verify WiFi SSID and password are correct
2. Check WiFi signal strength (2.4GHz only, ESP32 doesn't support 5GHz)
3. Move closer to the router
4. Check for special characters in password (may need escaping)

### Time Sync Failed

**Symptoms:**
- Time shows `00:00:00` or incorrect time
- Serial shows "Time sync failed!"

**Solutions:**
1. Ensure WiFi is connected first
2. Check NTP server address
3. Verify GMT_OFFSET is correct for your timezone
4. Try alternative NTP servers (see Configuration section)

### Display Not Updating

**Symptoms:**
- Time frozen or not refreshing

**Solutions:**
1. Check I2C connections (SDA, SCL, VCC, GND)
2. Verify display address (some OLEDs use 0x3D instead of 0x3C)
3. Increase delay in `loop()` if refresh is too fast

### Colon Not Blinking

**Symptoms:**
- Colon separators are static

**Solutions:**
- This is expected behavior if `getLocalTime()` fails
- Check time synchronization status in Serial monitor

## Customization

### Change Time Format

Edit `displayTime()` function:

```cpp
// 12-hour format with AM/PM
int hour = timeinfo.tm_hour;
const char* ampm = hour >= 12 ? "PM" : "AM";
hour = hour % 12;
hour = hour ? hour : 12; // 0 should be 12
sprintf(timeStr, "%02d:%02d:%02d %s", hour, timeinfo.tm_min, timeinfo.tm_sec, ampm);
```

### Add Temperature Display

If you have a temperature sensor:

```cpp
void displayTemp(float temp) {
  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.setCursor(100, 55);
  u8g2.print(temp);
  u8g2.print("C");
}
```

### Change Update Frequency

Edit `SYNC_INTERVAL` (in milliseconds):

```cpp
// Sync every 30 minutes
const unsigned long SYNC_INTERVAL = 1800000;

// Sync every 2 hours
const unsigned long SYNC_INTERVAL = 7200000;
```

### Add Alarm Functionality

```cpp
bool checkAlarm(struct tm* timeinfo) {
  return (timeinfo->tm_hour == 7 && timeinfo->tm_min == 0);
}

// In loop()
struct tm timeinfo;
if (getLocalTime(&timeinfo)) {
  if (checkAlarm(&timeinfo)) {
    // Trigger alarm (buzzer, LED, etc.)
  }
}
```

## Power Consumption Tips

1. **Reduce WiFi sync frequency** - Increase `SYNC_INTERVAL`
2. **Deep sleep between updates** - Use ESP32 deep sleep mode
3. **Dim display** - Use `u8g2.setContrast()` to reduce brightness
4. **Turn off display** - Use `u8g2.setPowerSave(1)` when not needed

## Resources

- [NTP Protocol](https://www.ntp.org/)
- [ESP32 WiFi](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html)
- [U8g2 Fonts](https://github.com/olikraus/u8g2/wiki/fntlistall)
- [U8g2 Font Tool](https://u8g2.github.io/u8g2_font_reference.html)
