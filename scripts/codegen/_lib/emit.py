# Source Generated with Decompyle++
# File: emit.cpython-312.pyc (Python 3.12)

'''
============================================================================
(AR) emit.py — دوال توليد كود C++ والكتابة الذكية
(EN) emit.py — C++ codegen helpers and smart-write utilities
----------------------------------------------------------------------------
(AR) هذا الملف يحتوي على الدوال المشتركة المستخرجة من gen_keywords.py
     و gen_type_methods.py. كلتا الدالتين كانتا متطابقتين تماماً —
     استُخرجتا هنا لتطبيق مبدأ DRY (CW-19).

(EN) This file contains shared utilities extracted from gen_keywords.py
     and gen_type_methods.py. Both implementations were byte-for-byte
     identical — extracted here to apply DRY (CW-19).
----------------------------------------------------------------------------
(AR) مبدأ الـ diff = 0:
     يجب أن يكون ناتج المولِّدات المُعاد كتابتها مطابقاً بالكامل للناتج
     القديم. لذلك لم يُغيَّر أي سلوك — تُنقل الدوال فقط.

(EN) diff=0 contract:
     Rewritten generators MUST produce byte-identical output. No behavior
     was changed — functions were only moved, not altered.
============================================================================
'''
from __future__ import annotations
from pathlib import Path

def cpp_string_literal(s = None):
    '''
    (AR) يحوّل سلسلة Python إلى literal C++ بأمان.
         - UTF-8 يُحفظ كما هو (لا يُهرَّب الـ high bytes)
         - يُهرَّب: \\ → \\\\  |  " → \\"  |  \\n → \\\\n  |  \\r → \\\\r  |  \\t → \\\\t
         - مُستخرَجة حرفياً من gen_keywords.py و gen_type_methods.py (كانتا متطابقتين)

    (EN) Convert Python string to safe C++ literal.
         - UTF-8 preserved as-is (high bytes not escaped)
         - Escapes: \\ → \\\\  |  " → \\"  |  \\n → \\\\n  |  \\r → \\\\r  |  \\t → \\\\t
         - Extracted verbatim from gen_keywords.py + gen_type_methods.py (identical)
    '''
    out = []
    for ch in s:
        if ch == '\\':
            out.append('\\\\')
            continue
        if ch == '"':
            out.append('\\"')
            continue
        if ch == '\n':
            out.append('\\n')
            continue
        if ch == '\r':
            out.append('\\r')
            continue
        if ch == '\t':
            out.append('\\t')
            continue
        out.append(ch)
    return '"' + ''.join(out) + '"'


def vector_literal(items = None):
    '''
    (AR) يُنتج {"a","b","c"} أو {} للاستخدام في تهيئة std::vector بـ C++.
         مُستخرَجة حرفياً من gen_keywords.py و gen_type_methods.py.

    (EN) Produces {"a","b","c"} or {} for C++ std::vector initialization.
         Extracted verbatim from gen_keywords.py and gen_type_methods.py.
    '''
    if not items:
        return '{}'
    return '{' + ', '.join(cpp_string_literal(i) for i in items) + '}'


def write_if_changed(path = None, content = None):
    '''
    (AR) لا يُحدِّث الملف إن لم يتغير المحتوى — يحفظ ctime ويتجنب إعادة بناء CMake.
         يستخدم مقارنة نصية مباشرة (كافٍ لضمان diff=0 مع باقي الإصلاحات).
         الدليل: المحتوى المولَّد قطعي بالكامل (deterministic output).

         ملاحظة تحسين مستقبلي: يمكن استبدال المقارنة بـ sha256 لملفات كبيرة جداً،
         لكن ليس ضرورياً للحجم الحالي (< 100 KB).

    (EN) Does not update file if content unchanged — preserves ctime and
         avoids spurious CMake rebuilds. Uses direct text comparison (sufficient
         since all output is fully deterministic).

         Future note: could use sha256 for very large files, but not needed
         for current sizes (< 100 KB).

    @return True  إذا كُتب الملف فعلاً / if file was written
    @return False إذا لم يتغير المحتوى  / if content was unchanged
    '''
    if path.exists():
        existing = path.read_text(encoding = 'utf-8')
        if existing == content:
            return False
    path.parent.mkdir(parents = True, exist_ok = True)
    path.write_text(content, encoding = 'utf-8', newline = '\n')
    return True

