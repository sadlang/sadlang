<#
.SYNOPSIS
    سكريبت نشر إضافة VS Code للغة ص
    Publish script for Sad Language VS Code Extension

.DESCRIPTION
    يقوم بـ:
    1. تثبيت التبعيات
    2. بناء الإضافة (TypeScript → JavaScript)
    3. تعبئة ملف VSIX
    4. (اختيارياً) نشر على VS Code Marketplace

.NOTES
    المتطلبات:
    - Node.js 18+ مثبت
    - Personal Access Token (PAT) من Azure DevOps (للنشر فقط)
    
    إنشاء PAT:
    1. اذهب إلى https://dev.azure.com
    2. User Settings → Personal Access Tokens → New Token
    3. Name: أي اسم
    4. Organization: All accessible organizations
    5. Scopes: Marketplace → Manage
    6. انسخ التوكن

    إنشاء Publisher:
    1. اذهب إلى https://marketplace.visualstudio.com/manage
    2. اختر "Create Publisher"
    3. Publisher ID: sad-language (يجب أن يطابق package.json)
    4. Display Name: Sad Language Team
#>

param(
    [switch]$PublishOnly,
    [switch]$PackageOnly,
    [string]$Token
)

$ErrorActionPreference = "Stop"
$ExtDir = Split-Path -Parent $MyInvocation.MyCommand.Path

Write-Host ""
Write-Host "╔══════════════════════════════════════════╗" -ForegroundColor Cyan
Write-Host "║   نشر إضافة لغة ص — Sad Language Ext   ║" -ForegroundColor Cyan
Write-Host "╚══════════════════════════════════════════╝" -ForegroundColor Cyan
Write-Host ""

Push-Location $ExtDir

try {
    # === Step 1: Check prerequisites ===
    Write-Host "[1/5] فحص المتطلبات..." -ForegroundColor Yellow
    
    $nodeVersion = & node --version 2>$null
    if (-not $nodeVersion) {
        Write-Host "خطأ: Node.js غير مثبت. ثبته من https://nodejs.org" -ForegroundColor Red
        exit 1
    }
    Write-Host "  Node.js: $nodeVersion" -ForegroundColor Green
    
    $npmVersion = & npm --version 2>$null
    Write-Host "  npm: $npmVersion" -ForegroundColor Green

    # === Step 2: Install dependencies ===
    if (-not $PublishOnly) {
        Write-Host ""
        Write-Host "[2/5] تثبيت التبعيات..." -ForegroundColor Yellow
        npm install --no-audit --no-fund 2>&1 | Out-Null
        Write-Host "  تم تثبيت التبعيات" -ForegroundColor Green
    }

    # === Step 3: Compile TypeScript ===
    if (-not $PublishOnly) {
        Write-Host ""
        Write-Host "[3/5] بناء TypeScript..." -ForegroundColor Yellow
        npm run compile
        if ($LASTEXITCODE -ne 0) {
            Write-Host "خطأ: فشل البناء!" -ForegroundColor Red
            exit 1
        }
        Write-Host "  تم البناء بنجاح" -ForegroundColor Green
    }

    # === Step 4: Package VSIX ===
    Write-Host ""
    Write-Host "[4/5] تعبئة VSIX..." -ForegroundColor Yellow
    
    # Install/update vsce
    $vsce = & npx @vscode/vsce --version 2>$null
    if (-not $vsce) {
        Write-Host "  تثبيت @vscode/vsce..." -ForegroundColor Gray
        npm install -g @vscode/vsce 2>&1 | Out-Null
    }
    
    npx @vscode/vsce package --no-dependencies
    if ($LASTEXITCODE -ne 0) {
        Write-Host "خطأ: فشلت التعبئة!" -ForegroundColor Red
        exit 1
    }
    
    $vsix = Get-ChildItem -Path $ExtDir -Filter "*.vsix" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    Write-Host "  تم إنشاء: $($vsix.Name) ($([math]::Round($vsix.Length / 1KB, 1)) KB)" -ForegroundColor Green

    if ($PackageOnly) {
        Write-Host ""
        Write-Host "تم التعبئة فقط. للتثبيت محلياً:" -ForegroundColor Cyan
        Write-Host "  code --install-extension $($vsix.Name)" -ForegroundColor White
        exit 0
    }

    # === Step 5: Publish ===
    Write-Host ""
    Write-Host "[5/5] النشر على Marketplace..." -ForegroundColor Yellow

    if (-not $Token) {
        Write-Host ""
        Write-Host "  لنشر الإضافة، تحتاج Personal Access Token (PAT):" -ForegroundColor Yellow
        Write-Host "  1. اذهب إلى https://dev.azure.com" -ForegroundColor Gray
        Write-Host "  2. User Settings → Personal Access Tokens → New Token" -ForegroundColor Gray
        Write-Host "  3. Scopes: Marketplace → Manage" -ForegroundColor Gray
        Write-Host ""
        $Token = Read-Host "أدخل PAT (أو اضغط Enter للتخطي)"
    }

    if ($Token) {
        npx @vscode/vsce publish --pat $Token
        if ($LASTEXITCODE -eq 0) {
            Write-Host ""
            Write-Host "╔══════════════════════════════════════════╗" -ForegroundColor Green
            Write-Host "║   تم النشر بنجاح! الحمد لله            ║" -ForegroundColor Green
            Write-Host "╚══════════════════════════════════════════╝" -ForegroundColor Green
            Write-Host ""
            Write-Host "ستظهر الإضافة في Marketplace خلال دقائق:" -ForegroundColor Cyan
            Write-Host "  https://marketplace.visualstudio.com/items?itemName=sad-language.sad-language" -ForegroundColor White
        } else {
            Write-Host "خطأ: فشل النشر!" -ForegroundColor Red
            exit 1
        }
    } else {
        Write-Host "  تم تخطي النشر." -ForegroundColor Yellow
        Write-Host ""
        Write-Host "  للتثبيت محلياً:" -ForegroundColor Cyan
        Write-Host "    code --install-extension $($vsix.Name)" -ForegroundColor White
        Write-Host ""
        Write-Host "  للنشر لاحقاً:" -ForegroundColor Cyan
        Write-Host "    .\publish.ps1 -PublishOnly -Token '<PAT>'" -ForegroundColor White
    }

} finally {
    Pop-Location
}

Write-Host ""
Write-Host "الحمد لله رب العالمين" -ForegroundColor Cyan
