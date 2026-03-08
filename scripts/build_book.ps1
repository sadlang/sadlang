param(
    [string]$BookDir = "docs/book",
    [string]$OutDir = "build/book"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if (-not (Test-Path $BookDir)) {
    throw "Book directory not found: $BookDir"
}

if (-not (Test-Path $OutDir)) {
    New-Item -ItemType Directory -Path $OutDir | Out-Null
}

$chapters = Get-ChildItem $BookDir -File -Filter "*.md" |
    Where-Object { $_.Name -notin @("README.md", "BUILD.md") } |
    Sort-Object Name

if ($chapters.Count -eq 0) {
    throw "No chapter markdown files found in $BookDir"
}

$bookMdPath = Join-Path $OutDir "book.md"
$bookHtmlPath = Join-Path $OutDir "book.html"
$bookPdfPath = Join-Path $OutDir "book.pdf"

$allLines = @()
$allLines += "# Sad Language Book"
$allLines += ""
$allLines += "Build Date: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
$allLines += ""
$allLines += "## Table of Contents"
$allLines += ""

$chapterNumber = 1
foreach ($ch in $chapters) {
    $chapterTitle = [System.IO.Path]::GetFileNameWithoutExtension($ch.Name)
    $allLines += "$chapterNumber. $chapterTitle"
    $chapterNumber++
}

foreach ($ch in $chapters) {
    $allLines += ""
    $allLines += "---"
    $allLines += ""
    $allLines += (Get-Content $ch.FullName -Encoding UTF8)
}

Set-Content -Path $bookMdPath -Value $allLines -Encoding UTF8

$pandoc = Get-Command pandoc -ErrorAction SilentlyContinue
if ($pandoc) {
    & pandoc $bookMdPath -o $bookHtmlPath --standalone --metadata title="Sad Language Book"
} else {
    $rawMd = Get-Content $bookMdPath -Raw -Encoding UTF8
    $escaped = [System.Net.WebUtility]::HtmlEncode($rawMd)
    $html = @"
<!doctype html>
<html lang="ar" dir="rtl">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>Sad Language Book</title>
  <style>
    body { font-family: Tahoma, Arial, sans-serif; line-height: 1.8; max-width: 980px; margin: 0 auto; padding: 24px; }
    pre { white-space: pre-wrap; word-wrap: break-word; background: #f7f7f7; border: 1px solid #ddd; padding: 16px; }
  </style>
</head>
<body>
  <h1>Sad Language Book (HTML fallback)</h1>
  <p>Pandoc was not found. This is a readable fallback output.</p>
  <pre>$escaped</pre>
</body>
</html>
"@
    Set-Content -Path $bookHtmlPath -Value $html -Encoding UTF8
}

if ($pandoc) {
    try {
        & pandoc $bookMdPath -o $bookPdfPath --metadata title="Sad Language Book"
    } catch {
        $note = "Pandoc found but PDF generation failed. Install a TeX engine (for example xelatex) and rerun scripts/build_book.ps1."
        Set-Content -Path (Join-Path $OutDir "PDF_REQUIRED.txt") -Value $note -Encoding UTF8
    }
} else {
    # Fallback PDF path via Playwright if available in website workspace.
    $node = Get-Command node -ErrorAction SilentlyContinue
    $playwrightModule = "website/node_modules/playwright"
    if ($node -and (Test-Path $playwrightModule)) {
        try {
            Push-Location website
            $script = "const path=require('path'); const { chromium }=require('playwright'); (async()=>{ const browser=await chromium.launch(); const page=await browser.newPage(); const file='file:///'+path.resolve('../build/book/book.html').replace(/\\\\/g,'/'); await page.goto(file,{waitUntil:'networkidle'}); await page.pdf({path:path.resolve('../build/book/book.pdf'), format:'A4', printBackground:true, margin:{top:'20mm',bottom:'20mm',left:'15mm',right:'15mm'}}); await browser.close(); console.log('PDF generated via Playwright'); })();"
            node -e $script | Out-Null
            Pop-Location
        } catch {
            if ((Get-Location).Path -like '*website') { Pop-Location }
            $note = "Pandoc not found and Playwright PDF fallback failed. Install pandoc+TeX or setup website Playwright dependency."
            Set-Content -Path (Join-Path $OutDir "PDF_REQUIRED.txt") -Value $note -Encoding UTF8
        }
    } else {
        $note = "Pandoc not found. For PDF: either install pandoc+TeX, or install Playwright under website/ then rerun scripts/build_book.ps1."
        Set-Content -Path (Join-Path $OutDir "PDF_REQUIRED.txt") -Value $note -Encoding UTF8
    }
}

if (Test-Path $bookPdfPath) {
    $pdfRequiredPath = Join-Path $OutDir "PDF_REQUIRED.txt"
    if (Test-Path $pdfRequiredPath) { Remove-Item $pdfRequiredPath -Force }
}

$websiteBookDir = "website/docs/book"
if (-not (Test-Path $websiteBookDir)) {
    New-Item -ItemType Directory -Path $websiteBookDir | Out-Null
}
Copy-Item $bookMdPath (Join-Path $websiteBookDir "book.md") -Force

$indexMd = @"
---
title: Sad Language Book
description: Combined book build output
---

# Sad Language Book

- Combined markdown: [book.md](./book.md)
- Build outputs: `build/book/`
"@
Set-Content -Path (Join-Path $websiteBookDir "index.md") -Value $indexMd -Encoding UTF8

Write-Host "Book build complete: $bookMdPath"
Write-Host "HTML output: $bookHtmlPath"
if (Test-Path $bookPdfPath) {
    Write-Host "PDF output: $bookPdfPath"
} else {
    Write-Host "PDF not generated. See build/book/PDF_REQUIRED.txt"
}
