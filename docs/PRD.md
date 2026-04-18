# NaoNao_OLED 产品需求文档 (PRD)

## 1. 产品概述

NaoNao_OLED 是一款基于 ESP32 的智能桌面显示屏，通过 0.96 寸 SSD1306 OLED 屏幕展示时间、通知、天气、金融数据和智能家居状态。支持 Web UI 和 HTTP API 远程交互，可集成 OpenClaw AI 助手和 MQTT 智能家居平台。

## 2. 功能规格

### 2.1 时钟模式
- NTP 同步实时时钟 (HH:MM:SS)，冒号每秒闪烁
- 中文星期显示（周日～周六）
- 状态栏：WiFi 信号、消息计数、时间

### 2.2 日期模式
- 完整日期显示（YYYY 年 MM 月 DD 日）
- 中文星期名称
- 小号时间显示

### 2.3 通知模式
- 滚动文本消息展示
- 消息队列（最多 10 条）
- 底部导航圆点指示当前位置
- 长文本自动滚动

### 2.4 天气模式（实时数据）
- 数据源：OpenWeatherMap API
- 刷新间隔：15 分钟
- 显示内容：城市名、温度（大字体）、湿度、天气描述、天气图标
- 天气图标：晴天（太阳）、多云（云朵）、雨天（雨滴）
- 未配置 API Key 时显示提示信息
- API 失败时显示加载中或上次缓存数据

### 2.5 系统状态模式
- IP 地址
- WiFi 信号强度 (RSSI)
- 运行时间
- 可用堆内存

### 2.6 倒计时模式
- 通过 `/timer` API 设置倒计时时长（秒）
- 操作：start（开始）、pause（暂停）、resume（恢复）、reset（重置）
- 显示：MM:SS 倒计时 + 底部进度条
- 倒计时结束时闪烁 "DONE!"
- Web UI 提供快捷控制按钮

### 2.7 加密货币价格模式
- 数据源：CoinGecko 免费 API（无需 API Key）
- 刷新间隔：5 分钟
- 显示：BTC 价格（自动缩写，如 $67.5K）、24h 涨跌幅百分比
- 价格 >= 10000 时自动缩写为 K/M 格式

### 2.8 GitHub Star 模式
- 数据源：GitHub REST API（无需认证，60 次/小时）
- 刷新间隔：10 分钟
- 显示：仓库名称、Star 数量、主要编程语言
- 需通过 `GITHUB_REPO` 环境变量配置仓库（格式：owner/repo）

### 2.9 MQTT 智能家居模式
- 数据源：MQTT Broker
- 订阅主题：接收智能家居事件（门铃、传感器告警等）
- 发布主题：定期发送设备状态（IP、信号、运行时间、内存）
- 收到消息自动推送到通知队列
- 连接状态指示（Connected / Disconnected）
- 指数退避重连机制（1s → 2s → 4s → ... → 30s）

### 2.10 屏保与防烧屏
- 60 秒无操作自动激活
- 简约时钟 + 漂浮星星动画
- 新消息或交互时自动退出

### 2.11 自动亮度
- 白天模式 (07:00-22:00)：全亮度 (255)
- 夜间模式 (22:00-07:00)：降低亮度 (50)
- 防止 OLED 烧屏

### 2.12 OTA 无线升级
- 无需 USB 连接即可更新固件
- 通过 `.env` 配置主机名和密码
- 未配置时自动禁用

## 3. API 参考

### HTTP 端点

| 端点 | 方法 | 请求体 | 响应 |
|------|------|--------|------|
| `/` | GET | - | Web UI 控制台 |
| `/msg` | POST | 纯文本 | `OK: Message added to queue` |
| `/status` | GET | - | JSON 设备状态 |
| `/messages` | GET | - | JSON 消息队列 |
| `/clear` | POST | - | `OK: Messages cleared` |
| `/timer` | GET | - | `{"state":0,"remaining":300,"total":300}` |
| `/timer` | POST | `{"action":"start","duration":300}` | `{"ok":true,"state":"running"}` |
| `/weather` | GET | - | `{"valid":true,"temperature":25.3,"humidity":65,"city":"Beijing"}` |
| `/price` | GET | - | `{"valid":true,"symbol":"BTC","price_usd":67500,"change_24h":2.3}` |
| `/mode` | POST | `{"mode":3}` | `{"ok":true,"mode":3}` |
| `/reboot` | POST | - | `{"ok":true}` 然后重启 |

### Timer Actions
- `start` - 开始倒计时，需指定 `duration`（秒）
- `pause` - 暂停倒计时
- `resume` - 恢复倒计时
- `reset` - 重置为空闲状态

## 4. 配置参考

### 环境变量 (.env)

| 变量 | 必需 | 默认值 | 说明 |
|------|------|--------|------|
| `WIFI_SSID` | 是 | - | WiFi 网络名称 |
| `WIFI_PASSWORD` | 是 | - | WiFi 密码 |
| `WEATHER_API_KEY` | 否 | "" | OpenWeatherMap API Key |
| `WEATHER_CITY` | 否 | Beijing | 天气查询城市 |
| `MQTT_BROKER` | 否 | "" | MQTT Broker 地址，留空禁用 |
| `MQTT_PORT` | 否 | 1883 | MQTT Broker 端口 |
| `MQTT_CLIENT_ID` | 否 | NaoNao_OLED | MQTT 客户端 ID |
| `MQTT_USERNAME` | 否 | "" | MQTT 用户名 |
| `MQTT_PASSWORD` | 否 | "" | MQTT 密码 |
| `MQTT_SUBSCRIBE_TOPIC` | 否 | naonao/cmd | 订阅主题 |
| `MQTT_PUBLISH_TOPIC` | 否 | naonao/status | 发布主题 |
| `GITHUB_REPO` | 否 | "" | GitHub 仓库 (owner/repo)，留空禁用 |
| `OTA_HOSTNAME` | 否 | naonao-oled | OTA 主机名 |
| `OTA_PASSWORD` | 否 | - | OTA 升级密码 |

## 5. 技术约束

| 项目 | 规格 |
|------|------|
| 开发板 | ESP32 DevKit (esp32dev) |
| 显示屏 | SSD1306 128x64 单色 OLED |
| Flash 空间 | ~3MB (huge_app.csv 分区) |
| 堆内存 | ~300KB 可用 |
| 连接方式 | WiFi (STA 模式) |
| 框架 | Arduino (PlatformIO) |

### API 频率限制

| API | 免费额度 | 刷新间隔 | 安全裕度 |
|-----|----------|----------|----------|
| OpenWeatherMap | 1000 次/天 | 15 分钟 (96 次/天) | 安全 |
| CoinGecko | 10-30 次/分钟 | 5 分钟 | 安全 |
| GitHub 未认证 | 60 次/小时 | 10 分钟 (6 次/小时) | 安全 |

## 6. 非功能需求

- **网络韧性**：WiFi 断开时自动重连，API 请求失败不阻塞主循环
- **数据新鲜度**：各 API 按配置间隔自动刷新，显示上次缓存数据
- **功耗**：OLED 持续运行，夜间自动降低亮度减少功耗
- **启动时间**：WiFi 连接 + NTP 同步 < 20 秒
- **安全**：WiFi 凭证和 API Key 不写入版本控制，通过 `.env` 注入
