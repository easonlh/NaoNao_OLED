# NaoNao_OLED - ESP32 OLED Smart Display

[English](#english) | [中文](#中文)

---

## English

### Project Overview

A feature-rich PlatformIO ESP32 project that drives a 0.96" SSD1306 128x64 OLED display via hardware I2C. Features **9 display modes** including real-time weather, crypto prices, GitHub stars, countdown timer, MQTT smart home integration, OTA wireless updates, and **SG90 servo control** via HTTP API.

### Quick Start

```bash
# 1. Install PlatformIO
pip install platformio

# 2. Clone and configure
git clone https://github.com/YOUR_USERNAME/NaoNao_OLED.git
cd NaoNao_OLED
cp .env.example .env
# Edit .env with your credentials

# 3. Build and upload
source .env && pio run --target upload

# 4. Monitor serial output
pio device monitor -b 115200
```

### Features

#### 9 Display Modes
- **Clock**: NTP-synced real-time clock (HH:MM:SS) with blinking colon
- **Date**: Full date with Chinese weekday names
- **Notification**: Scrolling text messages with queue navigation
- **Weather**: Real weather from OpenWeatherMap API (temp, humidity, icon)
- **System Status**: IP, WiFi signal, uptime, free heap
- **Countdown Timer**: MM:SS countdown with progress bar, controllable via API
- **Crypto Price**: Bitcoin price + 24h change from CoinGecko (no API key needed)
- **GitHub Stars**: Repo star count and language from GitHub API
- **MQTT Monitor**: Smart home event messages from MQTT broker

#### Smart Screen Cycling
- Auto-cycles through enabled modes every 5 seconds
- Skips modes with no data (e.g., weather before first fetch)
- Activity timer resets on new messages
- Manual mode switching via `/mode` API

#### Message Queue
- Up to 10 messages in a circular queue
- Messages queue instead of overwriting
- Visual navigation dots show position

#### Web UI Dashboard
- Responsive web interface at `http://<ESP32_IP>/`
- Send messages, control countdown timer, view status
- Preset messages and clear button
- JSON API for programmatic access

#### Smart Brightness & Screen Saver
- Auto-dimming based on time of day
- Screen saver activates after 60s inactivity with floating stars
- OLED burn-in prevention

#### Animations
- Animated bird mascot (3-state wing flapping)
- Twinkling stars with brightness cycling
- Weather icons with rotation effects
- WiFi signal strength bars
- Screen saver stars with collision detection

#### OTA Wireless Updates
- Flash new firmware over WiFi, no USB needed
- Password-protected for security
- Disabled by default, enable via `.env`

#### Modular Architecture
```
src/
├── main.cpp                  # Application entry point
├── config.cpp                # Configuration values
├── display.cpp               # Screen rendering and mode management
├── http_server.cpp           # Web server and API handlers
├── weather_client.cpp        # OpenWeatherMap API client
├── price_client.cpp          # CoinGecko API client
├── github_client.cpp         # GitHub API client
├── countdown_timer.cpp       # Countdown timer state machine
├── mqtt_client_wrapper.cpp   # MQTT broker integration
├── message_queue.cpp         # Message queue management
├── animations.cpp            # Animation implementations
└── servo_control.cpp         # SG90 servo PWM control
```

### Hardware Requirements

- **Board**: ESP32 DevKit (esp32dev)
- **Display**: SSD1306 128x64 OLED (Hardware I2C)
- **Wiring**: SDA, SCL, VCC (3.3V), GND
- **Servo (optional)**: SG90 360° continuous rotation servo
  - Red wire → VIN
  - Brown wire → GND
  - Yellow wire → GPIO18 (D18)

### Configuration

#### Wi-Fi Setup

```bash
cp .env.example .env
```

Edit `.env`:

```ini
WIFI_SSID=YourWiFiName
WIFI_PASSWORD=YourWiFiPassword
```

#### API & Service Configuration

| Variable | Required | Default | Description |
|----------|----------|---------|-------------|
| `WEATHER_API_KEY` | No | "" | [OpenWeatherMap](https://openweathermap.org/api) free API key |
| `WEATHER_CITY` | No | Beijing | City name for weather |
| `MQTT_BROKER` | No | "" | MQTT broker address (leave empty to disable) |
| `MQTT_PORT` | No | 1883 | MQTT broker port |
| `MQTT_CLIENT_ID` | No | NaoNao_OLED | MQTT client identifier |
| `MQTT_USERNAME` | No | "" | MQTT username |
| `MQTT_PASSWORD` | No | "" | MQTT password |
| `MQTT_SUBSCRIBE_TOPIC` | No | naonao/cmd | Topic to subscribe |
| `MQTT_PUBLISH_TOPIC` | No | naonao/status | Topic to publish status |
| `GITHUB_REPO` | No | "" | GitHub repo (format: owner/repo) |
| `OTA_HOSTNAME` | No | naonao-oled | OTA hostname |
| `OTA_PASSWORD` | No | "" | OTA update password |

### Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| `olikraus/U8g2` | ^2.36.18 | OLED graphics |
| `bblanchon/ArduinoJson` | ^7 | JSON parsing for APIs |
| `knolleary/PubSubClient` | ^2.8 | MQTT client |
| `madhephaestus/ESP32Servo` | ^3.2.0 | SG90 servo PWM control |
| `WiFi.h` / `WebServer.h` / `time.h` | Built-in | ESP32 Arduino framework |
| `HTTPClient` / `ArduinoOTA` | Built-in | ESP32 Arduino framework |

### API Reference

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Web UI dashboard |
| `/msg` | POST | Add message (body: plain text) |
| `/status` | GET | JSON device status |
| `/messages` | GET | JSON message queue info |
| `/clear` | POST | Clear all messages |
| `/timer` | GET | Get timer state |
| `/timer` | POST | Control timer (start/pause/resume/reset) |
| `/weather` | GET | JSON weather data |
| `/price` | GET | JSON crypto price data |
| `/mode` | POST | Switch display mode |
| `/reboot` | POST | Reboot device |
| `/servo` | GET | Get servo status |
| `/servo` | POST | Control servo speed |

#### Timer Control

```bash
# Start 5-minute countdown
curl -X POST http://<ESP32_IP>/timer \
  -H "Content-Type: application/json" \
  -d '{"action":"start","duration":300}'

# Pause
curl -X POST http://<ESP32_IP>/timer \
  -H "Content-Type: application/json" \
  -d '{"action":"pause"}'

# Check status
curl http://<ESP32_IP>/timer
```

#### Servo Control

Servo: SG90 360° continuous rotation, wired to GPIO18.

```bash
# Get status
curl http://<ESP32_IP>/servo
# Response: {"speed":90,"attached":true,"stopped":true}

# Set speed (0=full reverse, 90=stop, 180=full forward)
curl -X POST http://<ESP32_IP>/servo \
  -H "Content-Type: application/json" \
  -d '{"speed":120}'

# Stop servo
curl -X POST http://<ESP32_IP>/servo \
  -H "Content-Type: application/json" \
  -d '{"action":"stop"}'

# Rotate for N milliseconds then auto-stop
curl -X POST http://<ESP32_IP>/servo \
  -H "Content-Type: application/json" \
  -d '{"action":"rotate","speed":180,"duration":2000}'

# Quick pulse in a direction (then auto-stop)
curl -X POST http://<ESP32_IP>/servo \
  -H "Content-Type: application/json" \
  -d '{"action":"pulse","direction":"cw","duration":500}'

# Direct PWM microsecond control (500-2400us)
curl -X POST http://<ESP32_IP>/servo \
  -H "Content-Type: application/json" \
  -d '{"us":2000}'
```

#### Hardware Wiring

#### Examples

```bash
# Send a message
curl -X POST http://192.168.1.100/msg -d "Hello World!"

# Check device status
curl http://192.168.1.100/status

# Get weather data
curl http://192.168.1.100/weather

# Switch to clock mode
curl -X POST http://192.168.1.100/mode \
  -H "Content-Type: application/json" \
  -d '{"mode":0}'
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

### OTA Wireless Updates

After configuring `OTA_HOSTNAME` and `OTA_PASSWORD` in `.env` and flashing once via USB:

```bash
# Upload over WiFi (replace with your ESP32 IP)
pio run --target upload --upload-port <ESP32_IP>
```

### Troubleshooting

1. **Display not showing**: Check I2C wiring (SDA, SCL, VCC, GND)
2. **Garbled text**: Ensure `enableUTF8Print()` is called in setup
3. **Wi-Fi connection failed**: Verify credentials in `.env`
4. **Weather not working**: Set `WEATHER_API_KEY` in `.env` (free at openweathermap.org)
5. **MQTT not connecting**: Set `MQTT_BROKER` to your broker address
6. **Compilation errors**: Run `pio lib install` to fetch dependencies
7. **Screen too bright/dim**: Adjust `BRIGHTNESS_DAY` / `BRIGHTNESS_NIGHT` in `src/config.cpp`

---

### OpenClaw Integration

Push messages to the OLED display from OpenClaw or any automation:

```bash
curl -X POST http://<ESP32_IP>/msg \
  -H "Content-Type: text/plain" \
  -d "Hello from OpenClaw!"
```

**Servo control via OpenClaw:**

```bash
# Full speed forward
curl -X POST http://<ESP32_IP>/servo \
  -H "Content-Type: application/json" \
  -d '{"speed":180}'

# Rotate 2 seconds then auto-stop
curl -X POST http://<ESP32_IP>/servo \
  -H "Content-Type: application/json" \
  -d '{"action":"rotate","speed":180,"duration":2000}'

# Quick 500ms pulse clockwise then stop
curl -X POST http://<ESP32_IP>/servo \
  -H "Content-Type: application/json" \
  -d '{"action":"pulse","direction":"cw","duration":500}'

# Stop
curl -X POST http://<ESP32_IP>/servo \
  -H "Content-Type: application/json" \
  -d '{"action":"stop"}'

# Check status
curl http://<ESP32_IP>/servo
```

**Use cases**: notification push, scheduled reminders, server status monitoring, smart home alerts, servo control (robot arm, door lock, pan-tilt, flag waving).

---

## 中文

### 项目简介

功能丰富的 PlatformIO ESP32 项目，通过硬件 I2C 驱动 0.96 寸 SSD1306 128x64 OLED 显示屏。支持 **9 种显示模式**，包括实时天气、加密货币价格、GitHub Star、倒计时、MQTT 智能家居、OTA 无线升级，以及 **SG90 舵机 HTTP API 控制**。

### 快速开始

```bash
# 1. 安装 PlatformIO
pip install platformio

# 2. 克隆并配置
git clone https://github.com/YOUR_USERNAME/NaoNao_OLED.git
cd NaoNao_OLED
cp .env.example .env
# 编辑 .env 填入你的凭证

# 3. 编译上传
source .env && pio run --target upload

# 4. 串口监视
pio device monitor -b 115200
```

### 功能特性

#### 9 种显示模式
- **时钟**: NTP 同步实时时钟 (HH:MM:SS)，冒号闪烁
- **日期**: 完整日期 + 中文星期
- **通知**: 滚动文本消息，队列导航
- **天气**: OpenWeatherMap 实时数据（温度、湿度、图标）
- **系统状态**: IP、WiFi 信号、运行时间、可用内存
- **倒计时**: MM:SS 倒计时 + 进度条，API 可控
- **加密货币**: CoinGecko BTC 价格 + 24h 涨跌（无需 API Key）
- **GitHub Star**: 仓库 Star 数 + 编程语言
- **MQTT 监控**: MQTT Broker 智能家居事件

#### 智能屏幕切换
- 每 5 秒自动切换已启用的模式
- 跳过无数据的模式（如天气首次拉取前）
- 新消息时重置计时器
- 支持 `/mode` API 手动切换

#### 消息队列
- 最多 10 条消息循环存储
- 消息排队而非覆盖
- 可视化导航点显示位置

#### Web UI 控制台
- 响应式界面，访问 `http://<ESP32_IP>/`
- 发送消息、控制倒计时、查看状态
- 预设消息和一键清除
- JSON API 供编程访问

#### 智能亮度与屏保
- 根据时间自动调光
- 60 秒无操作激活屏保，带漂浮星星
- 防止 OLED 烧屏

#### 动画效果
- 小鸟吉祥物（3 状态翅膀扇动）
- 闪烁星星亮度循环
- 天气图标旋转动画
- WiFi 信号强度条
- 屏保星星碰撞检测

#### OTA 无线升级
- WiFi 刷写固件，无需 USB
- 密码保护
- 默认禁用，通过 `.env` 开启

### 硬件要求

- **开发板**: ESP32 DevKit (esp32dev)
- **显示屏**: SSD1306 128x64 OLED (硬件 I2C)
- **接线**: SDA, SCL, VCC (3.3V), GND
- **舵机（可选）**: SG90 360° 连续旋转舵机
  - 红线 → VIN
  - 棕线 → GND
  - 黄线 → GPIO18 (D18)

### 配置说明

#### Wi-Fi 设置

```bash
cp .env.example .env
```

编辑 `.env`：

```ini
WIFI_SSID=你的WiFi名称
WIFI_PASSWORD=你的WiFi密码
```

#### API 与服务配置

| 变量 | 必需 | 默认值 | 说明 |
|------|------|--------|------|
| `WEATHER_API_KEY` | 否 | "" | [OpenWeatherMap](https://openweathermap.org/api) 免费 API Key |
| `WEATHER_CITY` | 否 | Beijing | 天气查询城市 |
| `MQTT_BROKER` | 否 | "" | MQTT Broker 地址，留空禁用 |
| `MQTT_PORT` | 否 | 1883 | MQTT Broker 端口 |
| `MQTT_CLIENT_ID` | 否 | NaoNao_OLED | MQTT 客户端 ID |
| `MQTT_USERNAME` | 否 | "" | MQTT 用户名 |
| `MQTT_PASSWORD` | 否 | "" | MQTT 密码 |
| `MQTT_SUBSCRIBE_TOPIC` | 否 | naonao/cmd | 订阅主题 |
| `MQTT_PUBLISH_TOPIC` | 否 | naonao/status | 发布主题 |
| `GITHUB_REPO` | 否 | "" | GitHub 仓库 (格式: owner/repo) |
| `OTA_HOSTNAME` | 否 | naonao-oled | OTA 主机名 |
| `OTA_PASSWORD` | 否 | "" | OTA 升级密码 |

### API 接口

| 接口 | 方法 | 描述 |
|------|------|------|
| `/` | GET | Web UI 控制台 |
| `/msg` | POST | 添加消息（正文：纯文本） |
| `/status` | GET | JSON 设备状态 |
| `/messages` | GET | JSON 消息队列信息 |
| `/clear` | POST | 清除所有消息 |
| `/timer` | GET | 获取倒计时状态 |
| `/timer` | POST | 控制倒计时 (start/pause/resume/reset) |
| `/weather` | GET | JSON 天气数据 |
| `/price` | GET | JSON 加密货币价格 |
| `/mode` | POST | 切换显示模式 |
| `/reboot` | POST | 重启设备 |
| `/servo` | GET | 获取舵机状态 |
| `/servo` | POST | 控制舵机速度 |

#### 倒计时控制

```bash
# 开始 5 分钟倒计时
curl -X POST http://<ESP32_IP>/timer \
  -H "Content-Type: application/json" \
  -d '{"action":"start","duration":300}'

# 暂停
curl -X POST http://<ESP32_IP>/timer \
  -H "Content-Type: application/json" \
  -d '{"action":"pause"}'

# 查看状态
curl http://<ESP32_IP>/timer
```

#### 舵机控制

舵机：SG90 360° 连续旋转，接 GPIO18。

```bash
# 查看状态
curl http://<ESP32_IP>/servo

# 设置速度 (0=全速反转, 90=停止, 180=全速正转)
curl -X POST http://<ESP32_IP>/servo \
  -H "Content-Type: application/json" \
  -d '{"speed":120}'

# 停止舵机
curl -X POST http://<ESP32_IP>/servo \
  -H "Content-Type: application/json" \
  -d '{"action":"stop"}'

# 旋转指定时间后自动停止
curl -X POST http://<ESP32_IP>/servo \
  -H "Content-Type: application/json" \
  -d '{"action":"rotate","speed":180,"duration":2000}'

# 短促脉冲（指定方向，然后自动停止）
curl -X POST http://<ESP32_IP>/servo \
  -H "Content-Type: application/json" \
  -d '{"action":"pulse","direction":"cw","duration":500}'

# 直接 PWM 微秒控制 (500-2400us)
curl -X POST http://<ESP32_IP>/servo \
  -H "Content-Type: application/json" \
  -d '{"us":2000}'
```

#### 硬件接线
  -H "Content-Type: application/json" \
  -d '{"action":"stop"}'

# 查看状态
curl http://<ESP32_IP>/servo
```

#### 示例

```bash
# 发送消息
curl -X POST http://192.168.1.100/msg -d "你好世界！"

# 查看设备状态
curl http://192.168.1.100/status

# 获取天气数据
curl http://192.168.1.100/weather

# 切换到时钟模式
curl -X POST http://192.168.1.100/mode \
  -H "Content-Type: application/json" \
  -d '{"mode":0}'
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

### OTA 无线升级

配置 `.env` 中的 `OTA_HOSTNAME` 和 `OTA_PASSWORD` 并通过 USB 刷入一次后：

```bash
# 通过 WiFi 上传固件（替换为你的 ESP32 IP）
pio run --target upload --upload-port <ESP32_IP>
```

### 常见问题

1. **屏幕不显示**: 检查 I2C 接线 (SDA, SCL, VCC, GND)
2. **文字乱码**: 确保调用了 `enableUTF8Print()`
3. **Wi-Fi 连接失败**: 检查 `.env` 中的凭证
4. **天气不工作**: 在 `.env` 设置 `WEATHER_API_KEY`（openweathermap.org 免费注册）
5. **MQTT 不连接**: 设置 `MQTT_BROKER` 为你的 Broker 地址
6. **编译错误**: 运行 `pio lib install` 拉取依赖
7. **屏幕太亮/太暗**: 调整 `src/config.cpp` 中的 `BRIGHTNESS_DAY` / `BRIGHTNESS_NIGHT`

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
- [OpenWeatherMap API](https://openweathermap.org/api)
- [CoinGecko API](https://www.coingecko.com/api/documentation)
- [GitHub API](https://docs.github.com/en/rest)
