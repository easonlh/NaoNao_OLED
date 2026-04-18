# NaoNao_OLED - ESP32 OLED Smart Display

[English](#english) | [中文](#中文)

---

## English

### Project Overview

A feature-rich PlatformIO ESP32 project that drives an SSD1306 128x64 OLED display via hardware I2C. Features include multi-screen modes, NTP clock, message queue, web UI, auto-dimming, screen saver, and rich animations.

### Quick Start

```bash
# 1. Install PlatformIO (if not already installed)
pip install platformio

# 2. Clone and configure
git clone https://github.com/YOUR_USERNAME/NaoNao_OLED.git
cd NaoNao_OLED
cp .env.example .env
# Edit .env with your WiFi credentials

# 3. Build and upload
source .env && pio run --target upload

# 4. Monitor serial output
pio device monitor -b 115200
```

### Features

#### Multi-Screen Display
- **Clock Mode**: NTP-synced real-time clock (HH:MM:SS) with blinking colon
- **Date Mode**: Full date with Chinese weekday names
- **Notification Mode**: Scrolling text messages with queue navigation
- **Weather Mode**: Weather info with animated icons (placeholder for API)
- **System Status**: IP address, WiFi signal, uptime, free heap

#### Auto Screen Cycling
- Cycles through all modes every 5 seconds
- Activity timer resets on new messages
- Manual mode switching via API

#### Message Queue
- Up to 10 messages in a circular queue
- Messages queue instead of overwriting
- Visual navigation dots show position

#### Web UI Dashboard
- Responsive web interface at `http://<ESP32_IP>/`
- Send messages from browser
- Real-time status monitoring
- Preset messages and clear button
- JSON API: `/status`, `/messages`, `/clear`

#### Smart Brightness & Screen Saver
- Auto-dimming based on time of day (full brightness 07:00-22:00, dimmed at night)
- Screen saver activates after 60s inactivity with floating stars
- OLED burn-in prevention

#### Animations
- Animated bird mascot (3-state wing flapping)
- Twinkling stars with brightness cycling
- Weather icons with rotation effects
- WiFi signal strength bars
- Screen saver stars with collision detection

#### Modular Architecture
```
src/
├── main.cpp            # Application entry point
├── config.cpp          # Configuration values
├── display.cpp/h       # Screen rendering and mode management
├── animations.cpp/h    # Animation classes
├── message_queue.cpp/h # Message queue management
└── http_server.cpp/h   # Web server and API handlers
```

### Hardware Requirements

- **Board**: ESP32 DevKit (esp32dev)
- **Display**: SSD1306 128x64 OLED (Hardware I2C)
- **Wiring**: SDA, SCL, VCC (3.3V), GND

### Configuration

#### Wi-Fi Setup

Create a `.env` file from the template and fill in your credentials:

```bash
cp .env.example .env
```

Then edit `.env`:

```ini
WIFI_SSID=YourWiFiName
WIFI_PASSWORD=YourWiFiPassword
```

The credentials are injected at compile time via `platformio.ini` build flags. The `.env` file is **gitignored** and will not be committed.

#### Other Settings (`src/config.cpp`)

```cpp
// NTP Time Server
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 8 * 3600;   // GMT+8 for China
const int DAYLIGHT_OFFSET_SEC = 0;

// Display Settings
const int SCREEN_CYCLE_INTERVAL = 5000;  // Auto-switch interval (ms)
const int SCROLL_SPEED = 30;             // Text scroll speed (ms/pixel)
const int MSG_QUEUE_MAX_SIZE = 10;       // Max messages in queue

// Brightness Settings
const int BRIGHTNESS_DAY = 255;          // Daytime brightness
const int BRIGHTNESS_NIGHT = 50;         // Nighttime brightness
const int NIGHT_START_HOUR = 22;         // Night mode starts at 22:00
const int DAY_START_HOUR = 7;            // Day mode starts at 07:00

// Screen Saver
const unsigned long SCREENSAVER_TIMEOUT = 60000;  // Activate after 60s
```

### Dependencies

- **U8g2** (`olikraus/U8g2 @ ^2.36.18`) - Monochrome graphics library
- **WiFi.h** / **WebServer.h** / **time.h** - Built into ESP32 Arduino framework

### API Reference

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Web UI dashboard |
| `/msg` | POST | Add message (body: plain text) |
| `/status` | GET | JSON device status |
| `/messages` | GET | JSON message queue info |
| `/clear` | POST | Clear all messages |

```bash
# Send a message
curl -X POST http://192.168.1.100/msg -d "Hello World!"

# Check device status
curl http://192.168.1.100/status

# Clear message queue
curl -X POST http://192.168.1.100/clear
```

### Build & Upload

```bash
# Build
pio run

# Upload (with WiFi credentials)
source .env && pio run --target upload

# Serial monitor
pio device monitor -b 115200
```

Or use the convenience script:

```bash
chmod +x upload.sh
./upload.sh
```

### Troubleshooting

1. **Display not showing**: Check I2C wiring (SDA, SCL, VCC, GND)
2. **Garbled text**: Ensure `enableUTF8Print()` is called in setup
3. **Wi-Fi connection failed**: Verify credentials in `.env` file
4. **Compilation errors**: Run `pio lib install olikraus/U8g2`
5. **Screen too bright/dim**: Adjust `BRIGHTNESS_DAY` / `BRIGHTNESS_NIGHT` in `src/config.cpp`
6. **Messages not appearing**: Check `/messages` endpoint

---

### OpenClaw Integration

Push messages to the OLED display from OpenClaw or any automation:

```bash
curl -X POST http://<ESP32_IP>/msg \
  -H "Content-Type: text/plain" \
  -d "Hello from OpenClaw!"
```

**Use cases**: notification push, scheduled reminders, server status monitoring, smart home alerts.

---

## 中文

### 项目简介

功能丰富的 PlatformIO ESP32 项目，通过硬件 I2C 驱动 SSD1306 128x64 OLED 显示屏。支持多屏幕模式、NTP 时钟、消息队列、Web UI、自动调光、屏幕保护和丰富动画。

### 快速开始

```bash
# 1. 安装 PlatformIO
pip install platformio

# 2. 克隆并配置
git clone https://github.com/YOUR_USERNAME/NaoNao_OLED.git
cd NaoNao_OLED
cp .env.example .env
# 编辑 .env 填入 WiFi 信息

# 3. 编译上传
source .env && pio run --target upload

# 4. 串口监视
pio device monitor -b 115200
```

### 功能特性

#### 多屏幕显示
- **时钟模式**: NTP 同步实时时钟，带闪烁冒号
- **日期模式**: 完整日期 + 中文星期
- **通知模式**: 滚动文本消息，队列导航
- **天气模式**: 天气信息 + 动画图标（预留 API）
- **系统状态**: IP 地址、WiFi 信号、运行时间、可用内存

#### 自动屏幕切换
- 每 5 秒自动切换模式
- 新消息时重置计时器
- 支持 API 手动切换

#### 消息队列
- 最多 10 条消息循环存储
- 消息排队而非覆盖
- 可视化导航点显示位置

#### Web UI 控制台
- 响应式界面，访问 `http://<ESP32_IP>/`
- 浏览器发送消息
- 实时状态监控
- 预设消息和一键清除
- JSON API: `/status`、`/messages`、`/clear`

#### 智能亮度与屏保
- 根据时间自动调光（07:00-22:00 全亮度，夜间降低）
- 60 秒无操作激活屏保，带漂浮星星
- 防止 OLED 烧屏

#### 动画效果
- 小鸟吉祥物（3 状态翅膀扇动）
- 闪烁星星亮度循环
- 天气图标旋转动画
- WiFi 信号强度条
- 屏保星星碰撞检测

### 硬件要求

- **开发板**: ESP32 DevKit (esp32dev)
- **显示屏**: SSD1306 128x64 OLED (硬件 I2C)
- **接线**: SDA, SCL, VCC (3.3V), GND

### 配置说明

#### Wi-Fi 设置

从模板创建 `.env` 文件并填入你的 WiFi 信息：

```bash
cp .env.example .env
```

编辑 `.env`：

```ini
WIFI_SSID=你的WiFi名称
WIFI_PASSWORD=你的WiFi密码
```

凭证在编译时通过 `platformio.ini` 的 build flags 注入，`.env` 文件已加入 `.gitignore`，不会被提交。

#### 其他设置 (`src/config.cpp`)

```cpp
// NTP 时间服务器
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 8 * 3600;   // 中国时区 GMT+8
const int DAYLIGHT_OFFSET_SEC = 0;

// 显示设置
const int SCREEN_CYCLE_INTERVAL = 5000;  // 自动切换间隔（毫秒）
const int SCROLL_SPEED = 30;             // 滚动速度（毫秒/像素）
const int MSG_QUEUE_MAX_SIZE = 10;       // 队列最大消息数

// 亮度设置
const int BRIGHTNESS_DAY = 255;          // 白天亮度
const int BRIGHTNESS_NIGHT = 50;         // 夜间亮度
const int NIGHT_START_HOUR = 22;         // 夜间模式 22:00 开始
const int DAY_START_HOUR = 7;            // 白天模式 07:00 开始

// 屏幕保护
const unsigned long SCREENSAVER_TIMEOUT = 60000;  // 60 秒后激活
```

### API 接口

| 接口 | 方法 | 描述 |
|------|------|------|
| `/` | GET | Web UI 控制台 |
| `/msg` | POST | 添加消息（正文：纯文本） |
| `/status` | GET | JSON 设备状态 |
| `/messages` | GET | JSON 消息队列信息 |
| `/clear` | POST | 清除所有消息 |

```bash
# 发送消息
curl -X POST http://192.168.1.100/msg -d "你好世界！"

# 查看设备状态
curl http://192.168.1.100/status

# 清除消息队列
curl -X POST http://192.168.1.100/clear
```

### 编译与上传

```bash
# 编译
pio run

# 上传（需加载 WiFi 凭证）
source .env && pio run --target upload

# 串口监视器
pio device monitor -b 115200
```

或使用便捷脚本：

```bash
chmod +x upload.sh
./upload.sh
```

### 常见问题

1. **屏幕不显示**: 检查 I2C 接线 (SDA, SCL, VCC, GND)
2. **文字乱码**: 确保调用了 `enableUTF8Print()`
3. **Wi-Fi 连接失败**: 检查 `.env` 中的凭证
4. **编译错误**: 运行 `pio lib install olikraus/U8g2`
5. **屏幕太亮/太暗**: 调整 `src/config.cpp` 中的 `BRIGHTNESS_DAY` / `BRIGHTNESS_NIGHT`
6. **消息不显示**: 检查 `/messages` 端点

---

### OpenClaw 集成

通过 HTTP API 从 OpenClaw 或任何自动化工具推送消息到 OLED：

```bash
curl -X POST http://<ESP32_IP>/msg \
  -H "Content-Type: text/plain" \
  -d "来自 OpenClaw 的问候！"
```

**应用场景**: 通知推送、定时提醒、服务器状态监控、智能家居告警。

---

### 许可证

本项目仅供个人/教育用途。

### 参考资料

- [PlatformIO 文档](https://docs.platformio.org/)
- [U8g2 库](https://github.com/olikraus/u8g2)
- [ESP32 Arduino 框架](https://github.com/espressif/arduino-esp32)
- [NTP 时间服务器](https://www.ntppool.org/)
