#!/bin/bash
# Debug test script - runs in a single WSL session
SRC="/mnt/c/s_lang/s-programming-language/distribution/output/sad-installer-1.0.0-linux-x64.sh"
INST="/tmp/sad_test_inst_$$.sh"
DEST="/tmp/sad_test_dest_$$"

echo "=== Step 1: Copy installer ==="
cp "$SRC" "$INST"
echo "File size: $(wc -c < "$INST") bytes"

echo "=== Step 2: Find ARCHIVE_LINE ==="
ALINE=$(awk '/^__ARCHIVE_BELOW__$/{print NR + 1; exit 0;}' "$INST")
echo "ARCHIVE_LINE=$ALINE"

echo "=== Step 3: Test extraction ==="
mkdir -p /tmp/sad_extract_debug_$$
tail -n +"$ALINE" "$INST" | tar xzf - -C /tmp/sad_extract_debug_$$
echo "Extracted contents:"
ls /tmp/sad_extract_debug_$$/
echo "Inner contents:"
ls /tmp/sad_extract_debug_$$/sad-1.0.0-linux-x64/
echo "Bin contents:"
ls /tmp/sad_extract_debug_$$/sad-1.0.0-linux-x64/bin/

echo "=== Step 4: Run installer with debug ==="
bash "$INST" --cli --dir "$DEST" <<< ""

echo "=== Step 5: Check result ==="
echo "Install dir exists: $(test -d "$DEST" && echo YES || echo NO)"
echo "Bin dir exists: $(test -d "$DEST/bin" && echo YES || echo NO)"
if [ -d "$DEST/bin" ]; then
    echo "Installed binaries:"
    ls -la "$DEST/bin/"
fi
echo "Lib dir exists: $(test -d "$DEST/lib" && echo YES || echo NO)"

echo "=== Cleanup ==="
rm -f "$INST"
rm -rf "/tmp/sad_extract_debug_$$"
