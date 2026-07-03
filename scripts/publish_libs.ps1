# publish_libs.ps1 - نشر المكتبات على خادم الحزم
# الاستخدام: عيّن SAD_TOKEN (إلزاميّ) وSAD_REGISTRY_URL (اختياريّ) قبل التشغيل:
#   $env:SAD_TOKEN = "sad_xxxx"; .\scripts\publish_libs.ps1
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$ErrorActionPreference = "Continue"

# (AR) PowerShell 5.1 قد يفاوض TLS 1.0 افتراضيًّا — نفرض TLS 1.2 لأنّ السجلّ HTTPS الآن
# (EN) PowerShell 5.1 may default to TLS 1.0 — force TLS 1.2 since the registry is HTTPS now
[Net.ServicePointManager]::SecurityProtocol = [Net.ServicePointManager]::SecurityProtocol -bor [Net.SecurityProtocolType]::Tls12

# (AR) الرمز من متغيّر البيئة حصرًا — لا أسرار مُصلَّبة في المستودع
# (EN) Token comes from the environment only — no hardcoded secrets in the repo
$token = $env:SAD_TOKEN
if ([string]::IsNullOrWhiteSpace($token)) {
    Write-Host "[X] SAD_TOKEN environment variable is required (sad_pkg API token)" -ForegroundColor Red
    exit 1
}

# (AR) عنوان السجلّ: SAD_REGISTRY_URL إن وُجد وإلّا الافتراضيّ الموحَّد https://sila-hub.dev
# (EN) Registry URL: SAD_REGISTRY_URL if set, otherwise the unified default https://sila-hub.dev
$registry = $env:SAD_REGISTRY_URL
if ([string]::IsNullOrWhiteSpace($registry)) { $registry = "https://sila-hub.dev" }
$server = "$($registry.TrimEnd('/'))/api/v1/packages/publish"
$base = "C:\s_lang\s-programming-language"

# Library data: folder_name, package_name, description, keywords, category
$libraries = @(
    @("بحث_ثنائي", "bahth_thunai", "Binary search and sorted insertion", "binary-search,bisect,sorted,algorithms", "algorithms"),
    @("رياضيات", "ryadiyat", "Comprehensive math library - trigonometry, logarithms, primes", "math,trigonometry,logarithm,prime", "math"),
    @("عشوائي", "ashwai", "Random number generation - uniform, normal, shuffle, sampling", "random,shuffle,sample,probability", "math"),
    @("نظام_ملفات", "nizam_malafat", "File system operations - paths, directories, reading, writing", "filesystem,files,directories,paths", "system"),
    @("جيسون", "json", "JSON serialization and deserialization", "json,serialization,parsing,data", "data"),
    @("نظام", "nizam", "System information - platform, environment variables, runtime", "system,platform,environment,runtime", "system"),
    @("تكرارات", "takararat", "Iterator tools - chain, zip, product, permutations, combinations", "itertools,permutations,combinations", "utilities"),
    @("برمجة_وظيفية", "barmaja_wazifiya", "Functional programming - map, filter, reduce, compose, memoize", "functional,map,filter,reduce", "functional"),
    @("تشفير", "tashfir", "Cryptographic hashing - SHA256, MD5, CRC32, Adler32, HMAC", "hash,sha256,md5,crc32,hmac", "security"),
    @("نسخ", "nuskh", "Deep and shallow copy utilities for all data types", "copy,deepcopy,clone,immutable", "utilities"),
    @("تعداد_قيم", "tadad_qiyam", "Enum-like enumeration types with names, values and iteration", "enum,enumeration,constants,types", "utilities"),
    @("نماذج_بيانات", "namazij_bayanat", "Data classes - auto constructors, equality, hashing, serialization", "dataclass,struct,record,serialization", "data"),
    @("طلبات_ويب", "talabat_web", "HTTP/HTTPS requests - GET, POST, PUT, DELETE, headers", "http,https,requests,web,api", "network"),
    @("مقابس", "maqabis", "Low-level socket programming - TCP, UDP servers and clients", "socket,tcp,udp,network,server", "network"),
    @("خيوط", "khuyut", "Thread management - create, join, synchronize, locks, events", "threading,concurrency,lock,sync", "system"),
    @("معالجة_متوازية", "mualja_mutawaziya", "Parallel processing - process pool, task distribution, shared memory", "multiprocessing,parallel,pool,process", "system"),
    @("ملفات_csv", "malafat_csv", "CSV file reading and writing - parse, format, headers", "csv,tabular,spreadsheet,data", "data"),
    @("إدخال_إخراج", "idkhal_ikhraj", "IO streams - text streams, binary streams, string buffers", "io,streams,buffer,text,binary", "io"),
    @("كومة_أولويات", "kawmat_awlawiyat", "Priority queue (min-heap) - insert, extract_min, heapify", "heap,priority-queue,min-heap,sorting", "data-structures")
)

$success = 0
$failed = 0

foreach ($lib in $libraries) {
    $folder = $lib[0]
    $name = $lib[1]
    $desc = $lib[2]
    $kw = $lib[3] -split ","
    $cat = $lib[4]
    
    $libPath = Join-Path "$base\مكتبات_خارجية" $folder
    
    if (-not (Test-Path $libPath)) {
        Write-Host "[SKIP] $folder - not found" -ForegroundColor Yellow
        $failed++
        continue
    }
    
    # Create zip
    $zipPath = Join-Path $env:TEMP "$name-1.0.0.zip"
    if (Test-Path $zipPath) { Remove-Item $zipPath -Force }
    Compress-Archive -Path "$libPath\*" -DestinationPath $zipPath -Force
    
    # Build keywords JSON array
    $kwJson = ($kw | ForEach-Object { "`"$_`"" }) -join ","
    
    $metaJson = "{`"name`":`"$name`",`"version`":`"1.0.0`",`"description`":`"$desc`",`"authors`":[`"Sad Language Team`"],`"license`":`"MIT`",`"keywords`":[$kwJson],`"category`":`"$cat`",`"dependencies`":{}}"
    
    # Build multipart body
    $boundary = "----Boundary" + [System.Guid]::NewGuid().ToString("N").Substring(0, 16)
    $CRLF = "`r`n"
    
    $headerPart = "--$boundary${CRLF}Content-Disposition: form-data; name=`"metadata`"${CRLF}Content-Type: application/json${CRLF}${CRLF}${metaJson}${CRLF}--$boundary${CRLF}Content-Disposition: form-data; name=`"package`"; filename=`"$name-1.0.0.zip`"${CRLF}Content-Type: application/zip${CRLF}${CRLF}"
    $footerPart = "${CRLF}--$boundary--${CRLF}"
    
    $headerBytes = [System.Text.Encoding]::UTF8.GetBytes($headerPart)
    $fileBytes = [System.IO.File]::ReadAllBytes($zipPath)
    $footerBytes = [System.Text.Encoding]::UTF8.GetBytes($footerPart)
    
    $bodyBytes = New-Object byte[] ($headerBytes.Length + $fileBytes.Length + $footerBytes.Length)
    [System.Array]::Copy($headerBytes, 0, $bodyBytes, 0, $headerBytes.Length)
    [System.Array]::Copy($fileBytes, 0, $bodyBytes, $headerBytes.Length, $fileBytes.Length)
    [System.Array]::Copy($footerBytes, 0, $bodyBytes, $headerBytes.Length + $fileBytes.Length, $footerBytes.Length)
    
    try {
        $result = Invoke-RestMethod -Uri $server -Method POST `
            -ContentType "multipart/form-data; boundary=$boundary" `
            -Body $bodyBytes `
            -Headers @{Authorization = "Bearer $token"} `
            -TimeoutSec 30
        
        if ($result.success) {
            Write-Host "[OK] $folder ($name v1.0.0)" -ForegroundColor Green
            $success++
        } else {
            Write-Host "[FAIL] $folder : $($result.message)" -ForegroundColor Red
            $failed++
        }
    } catch {
        $errMsg = $_.Exception.Message
        try {
            if ($_.Exception.Response) {
                $sr = New-Object System.IO.StreamReader($_.Exception.Response.GetResponseStream())
                $errBody = $sr.ReadToEnd() | ConvertFrom-Json
                $errMsg = $errBody.message
            }
        } catch {}
        Write-Host "[FAIL] $folder : $errMsg" -ForegroundColor Red
        $failed++
    }
    
    # Cleanup
    if (Test-Path $zipPath) { Remove-Item $zipPath -Force }
}

Write-Host ""
Write-Host "=== Result: $success OK / $failed FAIL from $($libraries.Count) ===" -ForegroundColor Cyan
