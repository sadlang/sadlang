# سكريبت نشر خادم سجل الحزم على VPS
# Sad Registry Deployment Script to VPS

param(
    [string]$ServerIP = "185.47.174.39",
    [string]$User = "root",
    [string]$RemotePath = "/opt/sad-registry"
)

$ErrorActionPreference = "Stop"

Write-Host ""
Write-Host "╔══════════════════════════════════════════════════════════════╗" -ForegroundColor Cyan
Write-Host "║  نشر خادم سجل حزم لغة ص على $ServerIP              ║" -ForegroundColor Cyan
Write-Host "╚══════════════════════════════════════════════════════════════╝" -ForegroundColor Cyan
Write-Host ""

$RegistryDir = Split-Path -Parent $PSScriptRoot
if (-not (Test-Path "$RegistryDir\src\server.js")) {
    $RegistryDir = "c:\s\s-programming-language\tools\sad-registry"
}

# ─── 1. إنشاء أرشيف ───
Write-Host "▸ تجهيز الملفات..." -ForegroundColor Yellow
$TempDir = Join-Path $env:TEMP "sad-registry-deploy"
if (Test-Path $TempDir) { Remove-Item -Recurse -Force $TempDir }
New-Item -ItemType Directory -Path $TempDir -Force | Out-Null

# نسخ الملفات المطلوبة فقط
$filesToCopy = @(
    "package.json",
    "package-lock.json",
    "ecosystem.config.js",
    ".env.example"
)
$dirsToCopy = @(
    "src",
    "views",
    "web",
    "public",
    "migrations",
    "scripts"
)

foreach ($f in $filesToCopy) {
    $src = Join-Path $RegistryDir $f
    if (Test-Path $src) {
        Copy-Item $src (Join-Path $TempDir $f)
    }
}
foreach ($d in $dirsToCopy) {
    $src = Join-Path $RegistryDir $d
    if (Test-Path $src) {
        Copy-Item -Recurse $src (Join-Path $TempDir $d)
    }
}

# إنشاء tar.gz
$ArchivePath = Join-Path $env:TEMP "sad-registry.tar.gz"
if (Test-Path $ArchivePath) { Remove-Item $ArchivePath }

Push-Location $TempDir
tar -czf $ArchivePath *
Pop-Location

$size = [math]::Round((Get-Item $ArchivePath).Length / 1KB, 1)
Write-Host "  ✓ تم إنشاء الأرشيف ($size KB)" -ForegroundColor Green

# ─── 2. رفع الملفات ───
Write-Host ""
Write-Host "▸ رفع الملفات إلى ${User}@${ServerIP}..." -ForegroundColor Yellow
Write-Host "  (ستُطلب كلمة المرور)" -ForegroundColor DarkGray
scp -o StrictHostKeyChecking=no $ArchivePath "${User}@${ServerIP}:/tmp/sad-registry.tar.gz"
Write-Host "  ✓ تم رفع الملفات" -ForegroundColor Green

# ─── 3. إعداد وتشغيل الخادم ───
Write-Host ""
Write-Host "▸ إعداد الخادم عن بعد..." -ForegroundColor Yellow
Write-Host "  (ستُطلب كلمة المرور مرة أخرى)" -ForegroundColor DarkGray

$remoteScript = @"
set -e
echo '▸ تحديث النظام...'
apt-get update -qq 2>/dev/null
apt-get install -y -qq curl 2>/dev/null

# تثبيت Node.js 22 LTS
if ! command -v node &> /dev/null || [[ "\$(node --version | cut -d. -f1 | tr -d v)" -lt "22" ]]; then
    echo '▸ تثبيت Node.js 22...'
    curl -fsSL https://deb.nodesource.com/setup_22.x | bash - 2>/dev/null
    apt-get install -y -qq nodejs 2>/dev/null
fi
echo "  ✓ Node.js \$(node --version)"

# تثبيت PM2
if ! command -v pm2 &> /dev/null; then
    echo '▸ تثبيت PM2...'
    npm install -g pm2 2>/dev/null
fi
echo "  ✓ PM2 \$(pm2 --version)"

# إعداد مجلد التطبيق
echo '▸ إعداد التطبيق...'
mkdir -p $RemotePath/data $RemotePath/storage/packages $RemotePath/logs
cd $RemotePath
tar -xzf /tmp/sad-registry.tar.gz
rm /tmp/sad-registry.tar.gz

# إعداد .env
if [ ! -f .env ]; then
    JWT_SECRET=\$(openssl rand -hex 32 2>/dev/null || head -c 32 /dev/urandom | xxd -p)
    cat > .env << ENVEOF
PORT=3000
HOST=0.0.0.0
DB_PATH=./data/registry.db
STORAGE_PATH=./storage/packages
JWT_SECRET=\${JWT_SECRET}
NODE_ENV=production
REGISTRY_URL=http://$ServerIP:3000
ENVEOF
    echo '  ✓ تم إنشاء .env'
else
    echo '  ✓ .env موجود'
fi

# تثبيت التبعيات
echo '▸ تثبيت التبعيات...'
npm install --production 2>/dev/null

# فتح المنفذ
if command -v ufw &> /dev/null; then
    ufw allow 3000/tcp 2>/dev/null || true
fi

# إيقاف النسخة القديمة
pm2 delete sad-registry 2>/dev/null || true

# تشغيل الخادم
echo '▸ تشغيل الخادم...'
pm2 start ecosystem.config.js
pm2 save

# إعداد التشغيل التلقائي
pm2 startup systemd -u root --hp /root 2>/dev/null || true
pm2 save 2>/dev/null

echo ''
echo '══════════════════════════════════════════════════'
echo '  ✅ تم النشر بنجاح!'
echo "  http://$ServerIP:3000/api/v1/health"
echo '══════════════════════════════════════════════════'
pm2 status
"@

ssh -o StrictHostKeyChecking=no "${User}@${ServerIP}" $remoteScript

# ─── 4. التحقق ───
Write-Host ""
Write-Host "▸ التحقق من الوصول..." -ForegroundColor Yellow
Start-Sleep -Seconds 3
try {
    $r = Invoke-RestMethod -Uri "http://${ServerIP}:3000/api/v1/health" -TimeoutSec 10
    Write-Host ""
    Write-Host "╔══════════════════════════════════════════════════════════════╗" -ForegroundColor Green
    Write-Host "║  ✅ الخادم يعمل على الإنترنت!                              ║" -ForegroundColor Green
    Write-Host "║                                                            ║" -ForegroundColor Green
    Write-Host "║  http://${ServerIP}:3000                            ║" -ForegroundColor Green
    Write-Host "║  http://${ServerIP}:3000/api/v1/                    ║" -ForegroundColor Green
    Write-Host "║  http://${ServerIP}:3000/api/v1/health              ║" -ForegroundColor Green
    Write-Host "╚══════════════════════════════════════════════════════════════╝" -ForegroundColor Green
} catch {
    Write-Host "  ⚠ لم يتمكن من الوصول للخادم عبر الإنترنت" -ForegroundColor Red
    Write-Host "  تحقق من: جدار الحماية، سجلات PM2" -ForegroundColor Red
}

# تنظيف
Remove-Item -Recurse -Force $TempDir -ErrorAction SilentlyContinue
Remove-Item $ArchivePath -ErrorAction SilentlyContinue
