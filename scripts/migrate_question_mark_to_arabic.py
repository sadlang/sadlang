#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
مهاجِر آمن: تحويل عوامل ASCII `?` إلى العربية `؟` في ملفّات .ص [ADR-NS-002].

(AR) يحوّل في **الكود والتعليقات فقط**:
       ?.  →  ؟.   (وصول آمن)
       ??  →  ؟؟   (اندماج فارغ)
       ?   →  ؟    (الثلاثيّ + لاحقة الاختياريّة)
     ويتجنّب **السلاسل النصّية** ("..."، f"..."، r"...") حفاظًا على مخرجات البرنامج
     (تغيير `?` داخل سلسلة يكسر اختبارات الإخراج المتوقَّع).

(EN) Tokenizer-aware migrator: rewrites ASCII `?` operators to Arabic `؟` in code +
     comments, while preserving string literals (changing `?` inside a string would
     alter program output and break expected-output tests).

الاستعمال:  python scripts/migrate_question_mark_to_arabic.py [--apply] [مسارات...]
            بلا --apply: عرض فقط (dry-run).
"""
import sys
import pathlib

def migrate_text(s: str):
    """يهاجر `?`→`؟` في الكود فقط؛ يحفظ السلاسل والتعليقات كما هي (preserve)."""
    out = []
    i, n = 0, len(s)
    changed = 0
    while i < n:
        c = s[i]
        # سلسلة نصّية: "..." (raw إن سُبِقت بـ r) — تُحفظ كما هي
        if c == '"':
            is_raw = (len(out) >= 1 and out[-1] == 'r')
            out.append(c); i += 1
            while i < n:
                ch = s[i]
                out.append(ch); i += 1
                if ch == '\\' and not is_raw and i < n:
                    out.append(s[i]); i += 1  # هروب
                    continue
                if ch == '"':
                    break
            continue
        # تعليق كتليّ: #* ... *#  أو  #** ... **#  — يُحفظ كما هو
        if c == '#' and i + 1 < n and s[i + 1] == '*':
            start = i
            end = s.find('*#', i + 2)
            end = (end + 2) if end != -1 else n
            out.append(s[start:end]); i = end
            continue
        # تعليق سطريّ: # ... حتى نهاية السطر — يُحفظ كما هو
        if c == '#':
            nl = s.find('\n', i)
            nl = nl if nl != -1 else n
            out.append(s[i:nl]); i = nl
            continue
        # كود فعليّ: هاجِر العوامل (؟. ثم ؟؟ ثم ؟)
        if c == '?':
            if i + 1 < n and s[i + 1] == '.':
                out.append('؟.'); i += 2; changed += 1; continue
            if i + 1 < n and s[i + 1] == '?':
                out.append('؟؟'); i += 2; changed += 1; continue
            out.append('؟'); i += 1; changed += 1; continue
        out.append(c); i += 1
    return ''.join(out), changed

def main():
    args = sys.argv[1:]
    apply = '--apply' in args
    paths = [a for a in args if a != '--apply'] or ['tests', 'examples', 'stdlib']
    files = []
    for p in paths:
        pp = pathlib.Path(p)
        if pp.is_file() and pp.suffix == '.ص':
            files.append(pp)
        elif pp.is_dir():
            files.extend(pp.rglob('*.ص'))
    total_files, total_changes = 0, 0
    for f in files:
        try:
            text = f.read_text(encoding='utf-8')
        except Exception as e:
            print(f"  تخطّي (قراءة): {f} — {e}")
            continue
        new, changed = migrate_text(text)
        if changed:
            total_files += 1
            total_changes += changed
            print(f"  {'✏️' if apply else '👁'} {f}  (+{changed})")
            if apply:
                f.write_text(new, encoding='utf-8')
    mode = "طُبِّق" if apply else "عرض فقط (dry-run)"
    print(f"\n[{mode}] ملفّات متأثّرة: {total_files} — استبدالات: {total_changes}")
    if not apply and total_files:
        print("لإجراء التغيير فعليًّا: أضِف --apply")

if __name__ == '__main__':
    main()
