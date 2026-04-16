# complete-setup.ps1 - PowerShell 5.1 compatible
$SERVER = "root@185.47.174.39"
$PROJ   = "c:\s_lang\s-programming-language"
$TEMP   = "$env:TEMP\sad-cs"
New-Item -ItemType Directory -Path $TEMP -Force | Out-Null

Write-Host ""
Write-Host "=== sad-lang.org + sila-hub.dev Setup ==="
Write-Host ""

# --- Step 1: SSH Key ---
Write-Host "[1/4] Setting up SSH key..."
Write-Host "      Enter password when prompted"
$pubKey = (Get-Content "$env:USERPROFILE\.ssh\id_rsa.pub" -Raw).Trim()
$ks = "mkdir -p ~/.ssh; echo $pubKey >> ~/.ssh/authorized_keys; sort -u ~/.ssh/authorized_keys -o ~/.ssh/authorized_keys; chmod 700 ~/.ssh; chmod 600 ~/.ssh/authorized_keys; echo SSH_KEY_OK"
$r = ssh $SERVER $ks 2>&1
if ($r -match "SSH_KEY_OK") {
    Write-Host "      [OK] SSH key added" -ForegroundColor Green
} else {
    Write-Host "      Result: $r" -ForegroundColor Yellow
}

# --- Step 2: Upload ---
Write-Host ""
Write-Host "[2/4] Uploading setup script..."
scp "$PROJ\deployment\complete-server-setup.sh" "${SERVER}:/tmp/complete-server-setup.sh"
if ($LASTEXITCODE -ne 0) { Write-Host "[FAIL] SCP failed" -ForegroundColor Red; exit 1 }
Write-Host "      [OK] Uploaded" -ForegroundColor Green

# --- Step 3: Run ---
Write-Host ""
Write-Host "[3/4] Running server setup..."
$out = ssh $SERVER "bash /tmp/complete-server-setup.sh; rm -f /tmp/complete-server-setup.sh"
foreach ($ln in ($out -split "`n")) {
    $ln = $ln.Trim()
    if ($ln -match "^(DNS_READY|SSL_STATUS|NGINX|HTTP_|SSL_SILA|SSL_SAD|PM2|WEBSITE|SILA_IP|SAD_IP)") {
        Write-Host "      $ln"
    }
}
Write-Host "      [OK] Done" -ForegroundColor Green

# --- Step 4: Summary ---
Write-Host ""
Write-Host "[4/4] Status:"
Write-Host "==========================================="
$keys = @{
    "NGINX_STATUS"  = "Nginx"
    "WEBSITE_FILES" = "Website files"
    "PM2_ONLINE"    = "PM2 services"
    "HTTP_WEB"      = "Website HTTP"
    "HTTP_API"      = "Registry API"
    "SSL_SILA"      = "SSL sila-hub.dev"
    "SSL_SAD"       = "SSL sad-lang.org"
    "DNS_READY"     = "DNS"
}
foreach ($ln in ($out -split "`n")) {
    $ln = $ln.Trim()
    foreach ($k in $keys.Keys) {
        if ($ln -match "^${k}=(.+)") {
            $v   = $Matches[1]
            $ok  = $v -match "active|200|YES|[1-9]"
            $sym = if ($ok) { "[OK]" } else { "[??]" }
            $col = if ($ok) { "Green" } else { "Yellow" }
            Write-Host ("  {0} {1,-20}: {2}" -f $sym, $keys[$k], $v) -ForegroundColor $col
        }
    }
}
Write-Host "==========================================="
$ssl = $out -match "SSL_STATUS=COMPLETE"
$dns = $out -match "DNS_READY=YES"
Write-Host ""
if ($ssl)       { Write-Host "  https://sad-lang.org" -ForegroundColor Cyan; Write-Host "  https://sila-hub.dev" -ForegroundColor Cyan }
elseif ($dns)  { Write-Host "  DNS OK but SSL partial - re-run in a few minutes" -ForegroundColor Yellow }
else           { Write-Host "  DNS not pointing to server yet - add A records" -ForegroundColor Yellow }
Write-Host ""
Remove-Item -Recurse -Force $TEMP -ErrorAction SilentlyContinue
