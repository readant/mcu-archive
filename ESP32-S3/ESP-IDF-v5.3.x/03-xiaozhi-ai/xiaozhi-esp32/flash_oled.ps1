# 小智 AI 一键烧录脚本（正点原子 DNESP32S3 + SSD1306 OLED）
# 用法：在 PowerShell 中执行  .\flash_oled.ps1
# 退出监视器：Ctrl + ]
$ErrorActionPreference = 'Stop'

# 1) 加载 ESP-IDF 5.4.4 环境（每次新 PowerShell 都要先加载）
$idf = 'D:\APP\Espressif\frameworks\esp-idf-v5.4.4'
Write-Host '==> 加载 ESP-IDF 环境 (v5.4.4)' -ForegroundColor Cyan
. "$idf\export.ps1"

# 2) 进入工程目录（相对本脚本所在位置，避免写死仓库路径）
$proj = $PSScriptRoot
Set-Location $proj
Write-Host "==> 工程目录: $proj" -ForegroundColor Cyan

# 3) 若已有非正点原子板的旧 sdkconfig，则备份后重新生成（确保板型为 ATK）
$sdk = Join-Path $proj 'sdkconfig'
if (Test-Path $sdk) {
    if (-not (Select-String -Path $sdk -Pattern 'CONFIG_BOARD_TYPE_ATK_DNESP32S3=y' -Quiet)) {
        Write-Host '==> 检测到旧 sdkconfig 非正点原子板，备份为 sdkconfig.bak 并重新生成' -ForegroundColor Yellow
        Move-Item $sdk (Join-Path $proj 'sdkconfig.bak') -Force
    }
}

# 4) 选择目标芯片（首次会生成 sdkconfig 并应用正点原子板默认）
Write-Host '==> set-target esp32s3' -ForegroundColor Cyan
idf.py set-target esp32s3

# 5) 编译
Write-Host '==> 编译 (idf.py build)' -ForegroundColor Cyan
idf.py build

# 6) 烧录 + 监视（COM7；想换口改下面这行）
$port = 'COM7'
Write-Host "==> 烧录并监视 ($port)" -ForegroundColor Cyan
idf.py -p $port flash monitor
