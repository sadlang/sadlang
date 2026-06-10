# Source Generated with Decompyle++
# File: validator.cpython-312.pyc (Python 3.12)

'''
============================================================================
(AR) validator.py — فحوصات دلالية مشتركة بين المولِّدات
(EN) validator.py — Shared semantic validation across generators
----------------------------------------------------------------------------
(AR) تحتوي على:
       - validate_no_duplicates: يفحص عدم تكرار قيمة مفتاح معين في قائمة
       - validate_canonical_utf8: يتحقق أن الاسم مطابق لـ NFC ومقبول

     ملاحظة: الفحوصات الخاصة بكل مولِّد (مثل اشتراط shadda في بعض
     الأسماء في gen_type_methods) تبقى في المولِّد نفسه لأنها منطق خاص
     وليس منطقاً مشتركاً.

(EN) Contains:
       - validate_no_duplicates: checks uniqueness of a key across a list
       - validate_canonical_utf8: verifies NFC compliance and acceptability

     Note: generator-specific checks (e.g. shadda requirement for certain
     method names in gen_type_methods) remain in that generator, not here,
     because they are domain-specific, not shared logic.
============================================================================
'''
from __future__ import annotations
import unicodedata
from typing import Any

def validate_no_duplicates(items = None, key = None, context = None):
    '''
    (AR) يفحص أن قيمة المفتاح `key` فريدة عبر كل عناصر القائمة.
         يُرفع ValueError عند أول تكرار مع ذكر السياق.

    (EN) Asserts that the value of `key` is unique across all items.
         Raises ValueError on first duplicate, mentioning context.

    @param items   : قائمة القواميس المراد فحصها / List of dicts to check
    @param key     : اسم المفتاح للمقارنة / Key name to compare
    @param context : وصف اختياري للسياق في رسالة الخطأ / Optional context for error
    '''
    seen = { }
    ctx = f''' (in {context})''' if context else ''
    for idx, item in enumerate(items):
        val = item[key]
        if val in seen:
            raise ValueError(f'''Duplicate value \'{val}\' for key \'{key}\'{ctx}: positions {seen[val]} and {idx}.''')
        seen[val] = idx


def validate_canonical_utf8(name = None, context = None, require_nfc = None, allow_shadda = ('', True, True)):
    '''
    (AR) يتحقق من أن الاسم القانوني مقبول:
         - require_nfc=True:  يجب أن يكون بصيغة NFC (المعيار المختار للمشروع)
         - allow_shadda=False: يمنع وجود U+0651 (شدّة) في الاسم
           (مفيد للأسماء التي يُراد تجنُّب الشدّة فيها لأسباب تقنية)

    (EN) Verifies a canonical name is acceptable:
         - require_nfc=True:  name must be in NFC form (project standard)
         - allow_shadda=False: forbids U+0651 (shadda) in the name
           (useful for names that must avoid shadda for technical reasons)

    @param name         : الاسم المراد فحصه / Name to verify
    @param context      : وصف السياق للخطأ / Context description for error
    @param require_nfc  : هل يُشترط NFC؟ (الافتراضي: True) / Require NFC? (default True)
    @param allow_shadda : هل يُسمح بالشدّة U+0651؟ (الافتراضي: True) / Allow shadda? (default True)
    '''
    ctx = f''' [{context}]''' if context else ''
    if require_nfc:
        nfc_name = unicodedata.normalize('NFC', name)
        if nfc_name != name:
            raise ValueError(f'''Canonical name \'{name}\'{ctx} is not in NFC form. Expected NFC: \'{nfc_name}\'.''')
    if not allow_shadda:
        if 'ّ' in name:
            raise ValueError(f'''Canonical name \'{name}\'{ctx} contains shadda U+0651, which is not allowed in this context.''')
        return None

