# ─────────────────────────────────────────────────────────────────────────────
# (AR) تنزيلٌ محدودُ الزمنِ ومُعادُ المحاولة — أخو `net_retry.sh` على الويندوز.
#
#      🔑 الدافعُ مقيسٌ لا مُفترَض: خطوةُ «تحميل LLVM 18» في وظيفةِ
#      🪟 Windows (Release) علقت على `Invoke-WebRequest` **٥:٠١:٢١** في
#      الشوطَين 32066236587 و32066237695 (١٧–١٨ آب ٢٠٢٦)، ولم يقطعْهما حدُّ
#      زمنٍ البتّةَ — بل إلغاءُ التزامنِ حين دُفِعَ تغييرٌ جديد؛ أي أنّهما كانا
#      سيمضيان. ووسيطُ زمنِ الشوطِ في المستودعِ نحوُ ٢٠ دقيقة.
#
#      و`Invoke-WebRequest` بلا `-TimeoutSec` لا حدَّ له، وبلا إعادةٍ لا يتعافى
#      من انقطاعٍ عابر. فهذا يعطيه الحدَّين معًا، ويطبعُ حجمَ المُنزَّلِ إذ إنّ
#      ملفًّا مبتورًا يمضي صامتًا حتّى يُخفِقَ المثبّتُ بعدَه بسببٍ لا يدلُّ عليه.
#
# (EN) Bounded, retried download — the Windows sibling of net_retry.sh.
#      Measured motivation: the "Download LLVM 18" step hung 5h01m21s on
#      Invoke-WebRequest in runs 32066236587 / 32066237695, stopped only by
#      concurrency cancellation — never by a timeout.
#
# الاستعمال / usage:
#      pwsh scripts/ci/net_retry.ps1 -Uri $url -OutFile $path
#      pwsh scripts/ci/net_retry.ps1 -Uri $url -OutFile $path -Attempts 5 -TimeoutSec 900
# ─────────────────────────────────────────────────────────────────────────────
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$Uri,
    [Parameter(Mandatory = $true)][string]$OutFile,
    [int]$Attempts = 3,
    [int]$TimeoutSec = 600,
    [int]$BackoffSec = 10
)

$ErrorActionPreference = 'Stop'
$الوسم = Split-Path -Leaf $OutFile

for ($محاولة = 1; $محاولة -le $Attempts; $محاولة++) {
    Write-Host "▶️ تنزيل $الوسم — المحاولة $محاولة من $Attempts (حدّ $TimeoutSec ث)"
    try {
        # (AR) بقيّةُ محاولةٍ مبتورةٍ تُحذَف — لئلّا يُبنى على نصفِ ملفّ.
        # (EN) Drop any truncated leftover so we never build on half a file.
        if (Test-Path -LiteralPath $OutFile) { Remove-Item -LiteralPath $OutFile -Force }

        Invoke-WebRequest -Uri $Uri -OutFile $OutFile -UseBasicParsing -TimeoutSec $TimeoutSec

        $حجم = (Get-Item -LiteralPath $OutFile).Length
        if ($حجم -le 0) { throw "الملفُّ المُنزَّلُ فارغ" }
        # (AR) يُطبَعُ الحجمُ لأنّ ملفًّا مبتورًا يمضي صامتًا حتّى يُخفِقَ ما بعدَه.
        $الحجم_المقروء = if ($حجم -ge 1MB) { "$([math]::Round($حجم / 1MB, 1)) مب" }
                          else { "$حجم بايتًا" }
        Write-Host "✅ تنزيل $الوسم — نجح في المحاولة $محاولة ($الحجم_المقروء)"
        exit 0
    }
    catch {
        Write-Host "⚠️ المحاولة $محاولة أخفقت: $($_.Exception.Message)"
        if ($محاولة -lt $Attempts) { Start-Sleep -Seconds $BackoffSec }
    }
}

# (AR) الإخفاقُ بعدَ استنفادِ المحاولاتِ يُعلَنُ ولا يُبتلَع.
# (EN) Exhausted retries are reported, never swallowed.
Write-Host "::error::تنزيل $الوسم أخفق بعد $Attempts محاولات بحدّ $TimeoutSec ث لكلٍّ"
exit 1
