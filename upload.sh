#!/bin/bash
# 加载 .env 环境变量并上传固件到 ESP32
# 用法: ./upload.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ENV_FILE="$SCRIPT_DIR/.env"

# 检查 .env 文件是否存在
if [ ! -f "$ENV_FILE" ]; then
    echo "❌ 错误: 找不到 .env 文件"
    echo "请先复制模板: cp .env.example .env"
    echo "然后编辑 .env 填入你的 WiFi 信息"
    exit 1
fi

# 加载 .env 文件（跳过注释和空行）
echo "📖 加载 .env 配置..."
set -a
source "$ENV_FILE"
set +a

# 验证必要变量
if [ -z "$WIFI_SSID" ] || [ "$WIFI_SSID" = "YOUR_WIFI_SSID" ]; then
    echo "⚠️  警告: .env 中的 WIFI_SSID 未配置或为默认值"
    echo "请编辑 .env 文件，设置正确的 WiFi 名称"
    exit 1
fi

echo "✅ WiFi SSID: $WIFI_SSID"
echo "🚀 开始编译并上传..."
echo ""

# 执行 PlatformIO 上传
pio run --target upload

echo ""
echo "✅ 上传完成！打开串口监视器查看状态: pio device monitor"
