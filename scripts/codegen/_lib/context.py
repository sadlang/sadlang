# Source Generated with Decompyle++
# File: context.cpython-312.pyc (Python 3.12)

'''
============================================================================
(AR) context.py — سياق التوليد بدون حالة عامة (GenerationContext)
(EN) context.py — Stateless generation context (GenerationContext)
----------------------------------------------------------------------------
(AR) يحمل البيانات المُحمَّلة والمُتحقَّق منها ويمررها للمولِّدات.
     لا يحتوي على أي حالة عامة (global state) — نسختان مستقلتان لا
     تتعارضان أبداً.

     يُمرَّر هذا الـ context للمولِّدات الحالية وسيُمرَّر للمولِّدات
     الجديدة في UTM-6.2+ عند دعم data/language/index.yaml.

(EN) Carries loaded and validated data, passed to generators.
     Contains NO global state — two instances are fully independent.

     Passed to current generators and will be used by new generators
     in UTM-6.2+ when data/language/index.yaml is introduced.
----------------------------------------------------------------------------
(AR) التصميم: بسيط مقصود (YAGNI) — لا نُضيف خصائص لا تُستخدم الآن.
     الامتداد يكون عبر إضافة حقول جديدة (additive, non-breaking).
(EN) Design: intentionally simple (YAGNI) — no properties added before use.
     Extension via new fields (additive, non-breaking).
============================================================================
'''
from __future__ import annotations
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any
GenerationContext = <NODE:12>()
