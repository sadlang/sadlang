# =============================================================================
# سكريبت نشر تلقائي — لغة ص
# ينشر موقع اللغة (sad-lang.org) وخادم الوحدات (sila-hub.dev)
# =============================================================================
# الاستخدام:
#   .\deployment\deploy.ps1 -Target all      # نشر كل شيء
#   .\deployment\deploy.ps1 -Target website   # نشر موقع اللغة فقط
#   .\deployment\deploy.ps1 -Target registry  # نشر خادم الوحدات فقط
#   .\deployment\deploy.ps1 -Target nginx     # تحديث إعدادات Nginx فقط
# =============================================================================

param(
    [Parameter(Mandatory=$true)]
    [ValidateSet("all", "website", "registry", "nginx")]
    [string]$Target
)

$SERVER = "root@185.47.174.39"
$PROJECT_ROOT = "c:\s_lang\s-programming-language"
$TEMP_DIR = "$env:TEMP\sad-deploy-$(Get-Date -Format 'yyyyMMdd-HHmmss')"

function Write-Step($num, $msg) {
    Write-Host "`n▶ [$num] $msg" -ForegroundColor Cyan
}

function Deploy-Website {
    Write-Step "W1" "بناء موقع اللغة (VitePress)..."
    
    $distDir = "$PROJECT_ROOT\website\.vitepress\dist"
    if (-not (Test-Path $distDir)) {
        Write-Host "   ⚠ مجلد dist غير موجود — جارٍ البناء..." -ForegroundColor Yellow
        Push-Location "$PROJECT_ROOT\website"
        npm run build
        Pop-Location
    }
    
    Write-Step "W2" "ضغط ملفات الموقع..."
    $archivePath = "$TEMP_DIR\sad-lang-website.tar.gz"
    New-Item -ItemType Directory -Path $TEMP_DIR -Force | Out-Null
    Push-Location $distDir
    tar -czf $archivePath *
    Pop-Location
    $size = [math]::Round((Get-Item $archivePath).Length / 1KB, 1)
    Write-Host "   ✓ الأرشيف: $size KB" -ForegroundColor Green
    
    Write-Step "W3" "رفع الموقع إلى السيرفر..."
    scp $archivePath "${SERVER}:/tmp/sad-lang-website.tar.gz"
    
    Write-Step "W4" "نشر الملفات على السيرفر..."
    ssh $SERVER 'rm -rf /opt/sad-lang-website/*; tar -xzf /tmp/sad-lang-website.tar.gz -C /opt/sad-lang-website/; chown -R www-data:www-data /opt/sad-lang-website/; rm /tmp/sad-lang-website.tar.gz; echo WEBSITE_DEPLOYED_OK'
    
    Write-Host "   ✓ موقع اللغة منشور بنجاح — sad-lang.org" -ForegroundColor Green
}

function Deploy-Registry {
    Write-Step "R1" "تحضير حزمة خادم الوحدات..."
    
    $registryDir = "$PROJECT_ROOT\tools\sad-registry"
    $archivePath = "$TEMP_DIR\sad-registry.tar.gz"
    New-Item -ItemType Directory -Path $TEMP_DIR -Force | Out-Null
    
    $tempPkg = "$TEMP_DIR\registry-pkg"
    New-Item -ItemType Directory -Path $tempPkg -Force | Out-Null
    
    @("src","web","public","views","migrations","scripts") | ForEach-Object {
        $src = "$registryDir\$_"
        if (Test-Path $src) { Copy-Item -Recurse $src "$tempPkg\$_" }
    }
    @("package.json","package-lock.json","ecosystem.config.js",".env.example") | ForEach-Object {
        $src = "$registryDir\$_"
        if (Test-Path $src) { Copy-Item $src "$tempPkg\$_" }
    }
    
    Push-Location $tempPkg
    tar -czf $archivePath *
    Pop-Location
    $size = [math]::Round((Get-Item $archivePath).Length / 1KB, 1)
    Write-Host "   ✓ الأرشيف: $size KB" -ForegroundColor Green
    
    Write-Step "R2" "رفع الحزمة إلى السيرفر..."
    scp $archivePath "${SERVER}:/tmp/sad-registry.tar.gz"
    
    Write-Step "R3" "نشر وإعادة تشغيل الخادم..."
    ssh $SERVER 'cd /opt/sad-registry; tar -xzf /tmp/sad-registry.tar.gz; rm /tmp/sad-registry.tar.gz; npm install --production 2>/dev/null; pm2 restart sad-registry; pm2 status; echo REGISTRY_DEPLOYED_OK'
    
    Write-Host "   ✓ خادم الوحدات منشور بنجاح — sila-hub.dev" -ForegroundColor Green
}

function Deploy-Nginx {
    Write-Step "N1" "رفع إعدادات Nginx..."
    
    $nginxDir = "$PROJECT_ROOT\deployment\nginx"
    scp "$nginxDir\sila-hub.dev.conf" "${SERVER}:/etc/nginx/sites-available/sila-hub.dev"
    scp "$nginxDir\sad-lang.org.conf" "${SERVER}:/etc/nginx/sites-available/sad-lang.org"
    
    Write-Step "N2" "تفعيل المواقع وإعادة تشغيل Nginx..."
    ssh $SERVER "ln -sf /etc/nginx/sites-available/sila-hub.dev /etc/nginx/sites-enabled/ && ln -sf /etc/nginx/sites-available/sad-lang.org /etc/nginx/sites-enabled/ && nginx -t && systemctl reload nginx && echo 'NGINX RELOADED OK'"
    
    Write-Host "   ✓ إعدادات Nginx محدّثة" -ForegroundColor Green
}

# === التنفيذ ===
Write-Host ""
Write-Host "╔══════════════════════════════════════╗" -ForegroundColor Magenta
Write-Host "║   نشر لغة ص — $Target               ║" -ForegroundColor Magenta
Write-Host "╚══════════════════════════════════════╝" -ForegroundColor Magenta

$startTime = Get-Date

switch ($Target) {
    "all" {
        Deploy-Website
        Deploy-Registry
        Deploy-Nginx
    }
    "website"  { Deploy-Website }
    "registry" { Deploy-Registry }
    "nginx"    { Deploy-Nginx }
}

# تنظيف
if (Test-Path $TEMP_DIR) { Remove-Item -Recurse -Force $TEMP_DIR }

$elapsed = (Get-Date) - $startTime
Write-Host ""
Write-Host "══════════════════════════════════════" -ForegroundColor Green
Write-Host "✓ النشر مكتمل! ($([math]::Round($elapsed.TotalSeconds))s)" -ForegroundColor Green
Write-Host "  🌐 https://sad-lang.org" -ForegroundColor White
Write-Host "  📦 https://sila-hub.dev" -ForegroundColor White
Write-Host "══════════════════════════════════════" -ForegroundColor Green
