#!/bin/bash
# (AR) برهانُ النقل المتوازي + الانسكاب الحيّ: ثنائيّان مُخفَّضان من ص —
#      (١) نداءٌ متداخل «اجمع(اجمع(40،0)،2)» (وسيطٌ مؤقّت) ، (٢) مؤقّتٌ حيٌّ عبر نداء
#      «اجمع(40،0)+اجمع(1،1)» — كلاهما يخرج ٤٢. يُثبت انسكابَ المؤقّتات عبر النداء
#      وتحميلَ وسائطها من خانات الانسكاب. بلا clang/lld/as.
set -u
NESTED="${1:?usage: prove_sir_spill.sh <nested> <liveacross>}"
LIVE="${2:?usage: prove_sir_spill.sh <nested> <liveacross>}"

run_case()
{
    local label="$1" bin="$2"
    chmod +x "$bin" 2>/dev/null || true
    "$bin"
    local code=$?
    echo "$label EXIT_CODE=$code"
    [ "$code" -eq 42 ] || { echo "FAIL: $label توقّعنا ٤٢ ووجدنا $code"; return 1; }
    return 0
}

ok=0
run_case "NESTED(temp-arg)"   "$NESTED" || ok=1
run_case "LIVE-ACROSS(spill)" "$LIVE"   || ok=1
echo
if [ "$ok" -eq 0 ]; then
    echo "PASS: النقلُ المتوازي + الانسكابُ السياديّ (نداءٌ متداخل + مؤقّتٌ حيٌّ عبر نداء) خرجا ٤٢ / sovereign parallel-move + spill exited 42"
    exit 0
fi
echo "FAIL: أخفق أحدُ النمطين"
exit 1
