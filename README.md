# NaoNao_OLED - ESP32 OLED Display Project

[English](#english) | [中文](#中文)

---

## English

### Project Overview

A PlatformIO-based ESP32 project that drives an SSD1306 OLED display using the U8g2 library. The project displays UTF-8 text including Chinese characters on a 128x64 OLED screen.

### Hardware Requirements

- **Board**: ESP32 DevKit (esp32dev)
- **Display**: SSD1306 128x64 OLED (Hardware I2C)
- **Connection**: Hardware I2C interface

### Project Structure

```
NaoNao_OLED/
├── platformio.ini      # PlatformIO configuration
├── src/
│   └── main.cpp        # Main application source
├── include/            # Project header files (config.h for Wi-Fi credentials)
├── lib/                # Local libraries (not included in repo)
│   ├── Adafruit_BusIO/
│   ├── Adafruit_GFX/
│   ├── Adafruit_SSD1306/
│   └── U8g2/
├── simple/             # Simple example projects
├── test/               # Test files
└── .vscode/            # VS Code configuration
```

### Configuration

#### Wi-Fi Setup

**⚠️ IMPORTANT**: Before using Wi-Fi features, configure your credentials in `include/config.h`:

```cpp
const char* WIFI_SSID = "YourWiFiSSID";        // Your WiFi name
const char* WIFI_PASSWORD = "YourWiFiPassword"; // Your WiFi password
```

Or create a `.env` file from `.env.example` for environment-based configuration.

#### platformio.ini

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```

### Dependencies

Install libraries via PlatformIO:

```bash
pio lib install olikraus/U8g2
```

Local libraries in `lib/` folder (not included in repository):
- **U8g2** - Monochrome graphics library for OLED displays
- **Adafruit_GFX** - Core graphics library
- **Adafruit_SSD1306** - SSD1306 OLED driver
- **Adafruit_BusIO** - Bus I/O abstraction layer

### Usage Examples

#### Example 1: Static Text Display

Displays three lines of text:
1. Device name
2. Greeting message
3. Chinese text support

#### Example 2: OLED Clock Display

A WiFi-connected clock with NTP time synchronization:
- Real-time clock display (HH:MM:SS)
- Date display (YYYY-MM-DD Day)
- WiFi status indicator
- Automatic NTP time sync

### Font Reference

| Font Name | Description | Use Case |
|-----------|-------------|----------|
| `u8g2_font_6x12_tr` | 6x12 pixel | Small English text |
| `u8g2_font_ncenB12_tr` | 12px bold | Emphasized English text |
| `u8g2_font_wqy16_t_gb2312` | 16px Chinese | Chinese characters (GB2312) |

### Build & Upload

```bash
# Build the project
pio run

# Upload to device
pio run -t upload

# Build and upload
pio run -t upload

# Serial monitor
pio device monitor
```

### Troubleshooting

1. **Display not showing**: Check I2C wiring (SDA, SCL, VCC, GND)
2. **Garbled text**: Ensure `enableUTF8Print()` is called
3. **Wi-Fi connection failed**: Verify SSID and password in `include/config.h`
4. **Compilation errors**: Install required libraries via `pio lib install`

### License

This project is for personal/educational use.

### References

- [PlatformIO Documentation](https://docs.platformio.org/)
- [U8g2 Library](https://github.com/olikraus/u8g2)
- [ESP32 Arduino Framework](https://github.com/espressif/arduino-esp32)

---

## 中文

### 项目简介

基于 PlatformIO 的 ESP32 项目，使用 U8g2 库驱动 SSD1306 OLED 显示屏。项目支持在 128x64 的 OLED 屏幕上显示 UTF-8 编码的文本，包括中文字符。

### 硬件要求

- **开发板**: ESP32 DevKit (esp32dev)
- **显示屏**: SSD1306 128x64 OLED (硬件 I2C)
- **连接方式**: 硬件 I2C 接口

### 项目结构

```
NaoNao_OLED/
├── platformio.ini      # PlatformIO 配置文件
├── src/
│   └── main.cpp        # 主程序源码
├── include/            # 项目头文件（Wi-Fi 配置在此）
├── lib/                # 本地库（不包含在仓库中）
│   ├── Adafruit_BusIO/
│   ├── Adafruit_GFX/
│   ├── Adafruit_SSD1306/
│   └── U8g2/
├── simple/             # 简单示例项目
├── test/               # 测试文件
└── .vscode/            # VS Code 配置
```

### 配置说明

#### Wi-Fi 设置

**⚠️ 重要**: 使用 Wi-Fi 功能前，请在 `include/config.h` 中配置你的网络凭证：

```cpp
const char* WIFI_SSID = "你的 WiFi 名称";
const char* WIFI_PASSWORD = "你的 WiFi 密码";
```

或者从 `.env.example` 创建 `.env` 文件进行环境变量配置。

#### platformio.ini

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
```

### 依赖库

通过 PlatformIO 安装库：

```bash
pio lib install olikraus/U8g2
```

`lib/` 文件夹中的本地库（不包含在仓库中）：
- **U8g2** - OLED 显示屏单色图形库
- **Adafruit_GFX** - 核心图形库
- **Adafruit_SSD1306** - SSD1306 OLED 驱动库
- **Adafruit_BusIO** - 总线 I/O 抽象层

### 使用示例

#### 示例 1：静态文本显示

显示三行文本：
1. 设备名称
2. 问候语
3. 中文文本支持

#### 示例 2：OLED 时钟显示

支持 WiFi 和 NTP 时间同步的时钟：
- 实时时钟显示 (HH:MM:SS)
- 日期显示 (YYYY-MM-DD 星期)
- WiFi 状态指示
- 自动 NTP 时间同步

### 字体参考

| 字体名称 | 描述 | 使用场景 |
|---------|------|---------|
| `u8g2_font_6x12_tr` | 6x12 像素 | 小号英文文本 |
| `u8g2_font_ncenB12_tr` | 12px 粗体 | 强调的英文文本 |
| `u8g2_font_wqy16_t_gb2312` | 16px 中文 | 中文字符 (GB2312) |

### 编译与上传

```bash
# 编译项目
pio run

# 上传到设备
pio run -t upload

# 编译并上传
pio run -t upload

# 串口监视器
pio device monitor
```

### 常见问题

1. **屏幕不显示**: 检查 I2C 接线 (SDA, SCL, VCC, GND)
2. **文字乱码**: 确保调用了 `enableUTF8Print()`
3. **Wi-Fi 连接失败**: 检查 `include/config.h` 中的 SSID 和密码
4. **编译错误**: 通过 `pio lib install` 安装所需库

### 许可证

本项目仅供个人/教育用途。

### 参考资料

- [PlatformIO 文档](https://docs.platformio.org/)
- [U8g2 库](https://github.com/olikraus/u8g2)
- [ESP32 Arduino 框架](https://github.com/espressif/arduino-esp32)
