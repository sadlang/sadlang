#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
مهاجِر آمن: تحويل البادئة `جديد X(...)` إلى اللاحقة `X(...) جديد` في ملفّات .ص.

(AR) القاعدة الرسميّة: NewExpr = ClassName '(' ArgList ')' [ 'جديد' ] ;
     «جديد» **لاحقة اختياريّة** (الصفة بعد الموصوف، نظمٌ عربيّ)؛ بادئةً تُعامَل مُعرّفًا
     فتُنتج SEM001. هذا المهاجِر ينقل الكلمة من البادئة إلى اللاحقة:
         جديد مخزن()            →  مخزن() جديد
         جديد متجه(أ، ب)        →  متجه(أ، ب) جديد
     يتجاهل **السلاسل والتعليقات** (لا يلمس `جديد` داخلها).

(EN) Moves the optional `جديد` (new) keyword from deprecated prefix to canonical
     postfix position, matching balanced parentheses. Skips strings + comments.

الاستعمال:  python scripts/migrate_new_prefix_to_postfix.py [--apply] [مسارات...]
"""
import sys
import pathlib

KW = "جديد"


def _skip_string(s, i, out):
    n = len(s)
    is_raw = (len(out) >= 1 and out[-1] == 'r')
    out.append(s[i]); i += 1
    while i < n:
        ch = s[i]; out.append(ch); i += 1
        if ch == '\\' and not is_raw and i < n:
            out.append(s[i]); i += 1; continue
        if ch == '"':
            break
    return i


def _match_call(s, i):
    """من موضع بداية معرّف، طابِق `Name(<أقواس متوازنة>)`. يعيد (call_text, end) أو (None, i)."""
    n = len(s)
    start = i
    # المعرّف: حتى '(' أو فراغ
    while i < n and s[i] not in '( \t\r\n':
        i += 1
    if i >= n or s[i] != '(':
        return None, start
    name = s[start:i]
    if not name:
        return None, start
    # أقواس متوازنة
    depth = 0
    j = i
    while j < n:
        c = s[j]
        if c == '"':
            tmp = []
            j = _skip_string(s, j, tmp)
            continue
        if c == '(':
            depth += 1
        elif c == ')':
            depth -= 1
            if depth == 0:
                j += 1
                return s[start:j], j
        j += 1
    return None, start


def migrate_text(s):
    out = []
    i, n = 0, len(s)
    changed = 0
    while i < n:
        c = s[i]
        if c == '"':
            i = _skip_string(s, i, out); continue
        if c == '#' and i + 1 < n and s[i + 1] == '*':
            end = s.find('*#', i + 2); end = (end + 2) if end != -1 else n
            out.append(s[i:end]); i = end; continue
        if c == '#':
            nl = s.find('\n', i); nl = nl if nl != -1 else n
            out.append(s[i:nl]); i = nl; continue
        # كلمة «جديد» بادئةً: محاطة بحدود غير حرفيّة + يتبعها استدعاء
        if s.startswith(KW, i):
            before_ok = (i == 0) or (not s[i - 1].isalnum() and s[i - 1] not in '_ًٌٍَُِّْ')
            after = i + len(KW)
            if before_ok and after < n and s[after] in ' \t':
                k = after
                while k < n and s[k] in ' \t':
                    k += 1
                call, end = _match_call(s, k)
                if call is not None:
                    out.append(call + ' ' + KW)
                    i = end
                    changed += 1
                    continue
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
    tf, tc = 0, 0
    for f in files:
        try:
            text = f.read_text(encoding='utf-8')
        except Exception as e:
            print(f"  تخطّي (قراءة): {f} — {e}"); continue
        new, changed = migrate_text(text)
        if changed:
            tf += 1; tc += changed
            print(f"  {'✏️' if apply else '👁'} {f}  (+{changed})")
            if apply:
                f.write_text(new, encoding='utf-8')
    mode = "طُبِّق" if apply else "عرض فقط (dry-run)"
    print(f"\n[{mode}] ملفّات متأثّرة: {tf} — استبدالات: {tc}")
    if not apply and tf:
        print("لإجراء التغيير فعليًّا: أضِف --apply")


if __name__ == '__main__':
    main()
