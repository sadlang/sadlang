# Tech Spec — ترحيل رسائل الأخطاء إلى YAML

**Epic**: EPIC-EM
**الإصدار**: 1.0.0
**التاريخ**: 2026-05-19
**المُعِد**: Amelia (bmad-agent-dev)
**المرجع PRD**: [prd-error-messages.md](prd-error-messages.md)

---

## 1. المعمارية الحالية

```
┌─────────────────────────────────────────────────────────────────┐
│                     قبل (Manual C++)                            │
├─────────────────────────────────────────────────────────────────┤
│  error_codes.h          ← enum ErrorCode (203 قيمة) — يدوي     │
│         │                                                       │
│         ↓                                                       │
│  error_codes.cpp        ← 843 سطر يدوي                          │
│    ├─ errorCodeStrings: unordered_map<ErrorCode, "SEM002">     │
│    ├─ errorDescriptions: unordered_map<ErrorCode, {ar, en}>    │
│    ├─ getErrorCodeString()                                      │
│    ├─ getErrorDescription()                                     │
│    └─ getErrorCategory() ← substr(0,3) من id                    │
│                                                                 │
│  error_catalog.h        ← ErrorTemplate struct + ErrorCatalog  │
│                                                                 │
│  error_catalog_init.cpp ← 213 سطر يدوي                          │
│    └─ registerDefaults(): 26 × registerTemplate(...)            │
└─────────────────────────────────────────────────────────────────┘
```

## 2. المعمارية المستهدفة

```
┌─────────────────────────────────────────────────────────────────┐
│                      بعد (YAML SOT)                             │
├─────────────────────────────────────────────────────────────────┤
│  data/language/                                                 │
│    ├─ error_messages.yaml              ← SOT (203 entry)        │
│    └─ error_messages.schema.json       ← تحقُّق هيكلي           │
│                                                                 │
│  scripts/codegen/                                               │
│    ├─ gen_error_messages.py            ← المولِّد               │
│    ├─ migrate_error_messages.py        ← مؤقت (يُحذف)           │
│    └─ test_gen_error_messages.py       ← اختبارات pytest        │
│                                                                 │
│  cmake/codegen.cmake                                            │
│    └─ sad_error_messages_codegen target                         │
│                                                                 │
│  shared/errors/generated/              ← مولَّد + baseline      │
│    ├─ error_messages_generated.h                                │
│    ├─ error_messages_generated.cpp                              │
│    └─ error_messages_generated.{h,cpp}.baseline                 │
│                                                                 │
│  error_codes.cpp (~200 سطر)            ← يستخدم findByCode      │
│  error_catalog_init.cpp (~60 سطر)      ← حلقة على kErrorMessages │
└─────────────────────────────────────────────────────────────────┘
```

---

## 3. تصميم ملف YAML

### 3.1 البنية الكاملة

```yaml
version: "1.0"
language: "sad"

# تعريف الفئات
categories:
  lexical:   { prefix: "LEX", description_ar: "أخطاء معجمية",        description_en: "Lexical errors" }
  syntax:    { prefix: "SYN", description_ar: "أخطاء نحوية",         description_en: "Syntax errors" }
  semantic:  { prefix: "SEM", description_ar: "أخطاء دلالية",        description_en: "Semantic errors" }
  runtime:   { prefix: "RUN", description_ar: "أخطاء وقت التشغيل",   description_en: "Runtime errors" }
  ownership: { prefix: "OWN", description_ar: "أخطاء الملكية",       description_en: "Ownership errors" }
  import:    { prefix: "IMP", description_ar: "أخطاء الاستيراد",     description_en: "Import errors" }
  io:        { prefix: "IO",  description_ar: "أخطاء الإدخال/الإخراج", description_en: "I/O errors" }
  internal:  { prefix: "INT", description_ar: "أخطاء داخلية",        description_en: "Internal errors" }

# قائمة الأخطاء — مرتبة بـcategory ثم id
errors:
  # ═══════════ Lexical ═══════════
  - code: LEX_INVALID_CHARACTER
    id: "LEX001"
    category: lexical
    title:    { ar: "حرف غير صالح", en: "Invalid character" }
    brief:    { ar: "الحرف '{char}' غير مسموح به في هذا السياق",
                en: "Character '{char}' is not allowed here" }
    fix_hint: { ar: "احذف الحرف أو تأكد من ترميز الملف (UTF-8)",
                en: "Remove the character or check file encoding (UTF-8)" }
    placeholders: [char]

  # ═══════════ Semantic ═══════════
  - code: SEM_TYPE_MISMATCH
    id: "SEM002"
    category: semantic
    title:    { ar: "عدم تطابق الأنواع", en: "Type mismatch" }
    brief:    { ar: "متوقع '{expected}' لكن وُجد '{found}'",
                en: "Expected '{expected}' but found '{found}'" }
    fix_hint: { ar: "حوّل القيمة بـ {expected}(...) أو غيّر النوع المتوقع",
                en: "Convert with {expected}(...) or change the expected type" }
    detailed:
      ar: "النظام يفرض تطابق الأنواع لمنع أخطاء وقت التشغيل..."
      en: "The type system enforces type matching to prevent runtime errors..."
    code_example: |
      # خاطئ
      متغير ن: رقم = "12"
      # صحيح
      متغير ن: رقم = رقم("12")
    placeholders: [expected, found]
  # ... باقي الـ201 خطأ
```

### 3.2 قواعد الـSchema

```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "Sad Error Messages",
  "type": "object",
  "required": ["version", "language", "categories", "errors"],
  "properties": {
    "version":  { "type": "string", "pattern": "^\\d+\\.\\d+$" },
    "language": { "type": "string", "enum": ["sad"] },
    "categories": {
      "type": "object",
      "patternProperties": {
        "^[a-z]+$": {
          "type": "object",
          "required": ["prefix", "description_ar", "description_en"],
          "properties": {
            "prefix":         { "type": "string", "pattern": "^[A-Z]{2,3}$" },
            "description_ar": { "type": "string", "minLength": 1 },
            "description_en": { "type": "string", "minLength": 1 }
          }
        }
      }
    },
    "errors": {
      "type": "array",
      "items": {
        "type": "object",
        "required": ["code", "id", "category", "title", "brief"],
        "properties": {
          "code":     { "type": "string", "pattern": "^[A-Z][A-Z_]+$" },
          "id":       { "type": "string", "pattern": "^[A-Z]{2,3}\\d{3}$" },
          "category": { "type": "string" },
          "title":    { "$ref": "#/definitions/bilingual" },
          "brief":    { "$ref": "#/definitions/bilingual" },
          "fix_hint": { "$ref": "#/definitions/bilingual" },
          "detailed": { "$ref": "#/definitions/bilingual" },
          "teacher":  { "$ref": "#/definitions/bilingual" },
          "code_example": { "type": "string" },
          "placeholders": {
            "type": "array",
            "items": { "type": "string", "pattern": "^[a-z][a-z_]*$" }
          }
        }
      }
    }
  },
  "definitions": {
    "bilingual": {
      "type": "object",
      "required": ["ar", "en"],
      "properties": {
        "ar": { "type": "string", "minLength": 1 },
        "en": { "type": "string", "minLength": 1 }
      }
    }
  }
}
```

### 3.3 تحقُّقات دلالية (في المولِّد، خارج Schema)

1. **تفرّد**: كل `code` فريد، كل `id` فريد.
2. **مطابقة enum**: كل `code` موجود في `ErrorCode` enum (يقرأ `error_codes.h` بـregex).
3. **مطابقة prefix**: كل `id` يبدأ بـ `categories[category].prefix`.
4. **تحقُّق placeholders**: كل `{name}` في النصوص (`brief`, `fix_hint`, `detailed`, `teacher`) مذكور في `placeholders` array.
5. **ترتيب**: الإدخالات مرتبة حسب `category` (بالترتيب المعرَّف في `categories`) ثم `id` تصاعدياً.
6. **اكتمال**: كل ErrorCode في enum له entry (لا حذف صامت).

---

## 4. تصميم الكود المُولَّد

### 4.1 `error_messages_generated.h`

```cpp
/**
 * @file error_messages_generated.h
 * @brief (AR) كتالوج رسائل الأخطاء المولَّد تلقائياً من error_messages.yaml
 *        (EN) Auto-generated error messages catalog from error_messages.yaml
 *
 * @warning (AR) ⚠️ لا تُعدِّل هذا الملف يدوياً — عدِّل error_messages.yaml بدلاً منه
 *          (EN) ⚠️ DO NOT EDIT — modify error_messages.yaml instead
 */
#pragma once

#include "../include/error_codes.h"
#include <string_view>

namespace Sad::Errors::Generated {

    struct GeneratedErrorEntry {
        ErrorCode    code;
        const char*  id;
        const char*  category;
        const char*  titleAr;     const char* titleEn;
        const char*  briefAr;     const char* briefEn;
        const char*  detailedAr;  const char* detailedEn;
        const char*  teacherAr;   const char* teacherEn;
        const char*  fixHintAr;   const char* fixHintEn;
        const char*  codeExample;
    };

    /// (AR) مصفوفة كل الأخطاء المولَّدة (مرتبة بـcategory ثم id)
    extern const GeneratedErrorEntry kErrorMessages[];
    extern const std::size_t kErrorMessagesCount;

    /// (AR) بحث O(1) — يُرجع nullptr إذا الكود غير موجود
    const GeneratedErrorEntry* findByCode(ErrorCode code);

    /// (AR) بحث O(1) بـid نصي
    const GeneratedErrorEntry* findById(std::string_view id);

}  // namespace Sad::Errors::Generated
```

### 4.2 `error_messages_generated.cpp` (مختصر)

```cpp
#include "error_messages_generated.h"
#include <unordered_map>
#include <string>

namespace Sad::Errors::Generated {

    const GeneratedErrorEntry kErrorMessages[] = {
        { ErrorCode::LEX_INVALID_CHARACTER, "LEX001", "lexical",
          "حرف غير صالح", "Invalid character",
          "الحرف '{char}' غير مسموح به في هذا السياق",
          "Character '{char}' is not allowed here",
          nullptr, nullptr,   // detailed
          nullptr, nullptr,   // teacher
          "احذف الحرف أو تأكد من ترميز الملف (UTF-8)",
          "Remove the character or check file encoding (UTF-8)",
          nullptr             // code_example
        },
        // ... 202 entry آخر
    };

    const std::size_t kErrorMessagesCount =
        sizeof(kErrorMessages) / sizeof(kErrorMessages[0]);

    // lookup مبني مرة واحدة (CW-26)
    static const std::unordered_map<ErrorCode, const GeneratedErrorEntry*>& byCodeMap() {
        static const auto map = []{
            std::unordered_map<ErrorCode, const GeneratedErrorEntry*> m;
            m.reserve(kErrorMessagesCount);
            for (std::size_t i = 0; i < kErrorMessagesCount; ++i)
                m.emplace(kErrorMessages[i].code, &kErrorMessages[i]);
            return m;
        }();
        return map;
    }

    const GeneratedErrorEntry* findByCode(ErrorCode code) {
        const auto& m = byCodeMap();
        auto it = m.find(code);
        return (it != m.end()) ? it->second : nullptr;
    }

    const GeneratedErrorEntry* findById(std::string_view id) {
        for (std::size_t i = 0; i < kErrorMessagesCount; ++i)
            if (id == kErrorMessages[i].id) return &kErrorMessages[i];
        return nullptr;
    }

}  // namespace Sad::Errors::Generated
```

---

## 5. تصميم المولِّد (Python)

### 5.1 بنية `gen_error_messages.py`

```python
#!/usr/bin/env python3
"""
مولِّد كتالوج رسائل الأخطاء — يقرأ error_messages.yaml ويُنتج C++.
"""
import argparse
import re
import sys
from pathlib import Path
from _lib.loader import load_yaml, load_schema, validate_schema
from _lib.emit import cpp_string_literal, write_if_changed

# Map: category → prefix (مُستخرَج من YAML)
CATEGORY_TO_ENUM_PREFIX = {}

def extract_enum_values(error_codes_h: Path) -> set[str]:
    """استخراج كل قيم ErrorCode من error_codes.h بـregex."""
    text = error_codes_h.read_text(encoding="utf-8")
    # نمط: داخل enum class ErrorCode { ... } يلتقط أسماء القيم
    pattern = r"^\s*([A-Z][A-Z0-9_]+)\s*(?:=|,|$)"
    in_enum = False
    values = set()
    for line in text.splitlines():
        if "enum class ErrorCode" in line: in_enum = True
        elif in_enum and "}" in line: break
        elif in_enum:
            m = re.match(pattern, line)
            if m: values.add(m.group(1))
    return values

def validate_semantically(data: dict, enum_values: set[str]) -> list[str]:
    """تحقُّقات دلالية تتجاوز Schema."""
    errors = []
    seen_codes, seen_ids = set(), set()
    for entry in data["errors"]:
        code, eid = entry["code"], entry["id"]
        if code in seen_codes: errors.append(f"تكرار code: {code}")
        if eid in seen_ids:    errors.append(f"تكرار id: {eid}")
        seen_codes.add(code); seen_ids.add(eid)

        if code not in enum_values:
            errors.append(f"code '{code}' غير موجود في ErrorCode enum")

        cat = entry["category"]
        prefix = data["categories"].get(cat, {}).get("prefix")
        if prefix and not eid.startswith(prefix):
            errors.append(f"id '{eid}' لا يبدأ بـ prefix '{prefix}' للفئة '{cat}'")

        # placeholders dans النصوص
        declared = set(entry.get("placeholders", []))
        for field in ("brief", "fix_hint", "detailed", "teacher"):
            for lang in ("ar", "en"):
                text = (entry.get(field) or {}).get(lang, "")
                used = set(re.findall(r"\{([a-z_]+)\}", text))
                missing = used - declared
                if missing:
                    errors.append(
                        f"{code}.{field}.{lang}: placeholders غير مُعرَّفة: {missing}"
                    )

    # اكتمال
    yaml_codes = {e["code"] for e in data["errors"]}
    missing_codes = enum_values - yaml_codes
    if missing_codes:
        errors.append(f"ErrorCodes في enum بلا entry في YAML: {sorted(missing_codes)[:5]}...")

    return errors

def sort_entries(data: dict) -> list[dict]:
    """ترتيب: حسب category (بالترتيب المعرَّف) ثم id."""
    cat_order = list(data["categories"].keys())
    def key(e):
        return (cat_order.index(e["category"]) if e["category"] in cat_order else 999, e["id"])
    return sorted(data["errors"], key=key)

def emit_header(data: dict, out_path: Path) -> str:
    # ... توليد .h
    pass

def emit_source(data: dict, out_path: Path) -> str:
    # ... توليد .cpp مع lookup map
    pass

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--yaml", required=True, type=Path)
    parser.add_argument("--schema", required=True, type=Path)
    parser.add_argument("--enum-header", required=True, type=Path)
    parser.add_argument("--header", required=True, type=Path)
    parser.add_argument("--source", required=True, type=Path)
    parser.add_argument("--quiet", action="store_true")
    args = parser.parse_args()

    data = load_yaml(args.yaml)
    schema = load_schema(args.schema)
    validate_schema(data, schema)

    enum_values = extract_enum_values(args.enum_header)
    sem_errors = validate_semantically(data, enum_values)
    if sem_errors:
        for e in sem_errors: print(f"❌ {e}", file=sys.stderr)
        sys.exit(1)

    data["errors"] = sort_entries(data)
    h_text = emit_header(data, args.header)
    c_text = emit_source(data, args.source)
    write_if_changed(args.header, h_text)
    write_if_changed(args.source, c_text)
    if not args.quiet:
        print(f"✅ مولَّد {len(data['errors'])} رسالة خطأ")

if __name__ == "__main__":
    main()
```

---

## 6. تكامل CMake

في [cmake/codegen.cmake](../../../cmake/codegen.cmake) — إضافة بلوك جديد بعد `sad_keywords_codegen`:

```cmake
# ═════════════ Error Messages Codegen ═════════════
set(SAD_ERROR_MESSAGES_YAML   "${CMAKE_SOURCE_DIR}/data/language/error_messages.yaml" CACHE INTERNAL "")
set(SAD_ERROR_MESSAGES_SCHEMA "${CMAKE_SOURCE_DIR}/data/language/error_messages.schema.json" CACHE INTERNAL "")
set(SAD_ERROR_MESSAGES_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/codegen/gen_error_messages.py" CACHE INTERNAL "")
set(SAD_ERROR_MESSAGES_ENUM_H "${CMAKE_SOURCE_DIR}/shared/errors/include/error_codes.h" CACHE INTERNAL "")
set(SAD_ERROR_MESSAGES_GEN_H  "${CMAKE_SOURCE_DIR}/shared/errors/generated/error_messages_generated.h" CACHE INTERNAL "")
set(SAD_ERROR_MESSAGES_GEN_C  "${CMAKE_SOURCE_DIR}/shared/errors/generated/error_messages_generated.cpp" CACHE INTERNAL "")

add_custom_command(
    OUTPUT ${SAD_ERROR_MESSAGES_GEN_H} ${SAD_ERROR_MESSAGES_GEN_C}
    COMMAND ${Python3_EXECUTABLE} ${SAD_ERROR_MESSAGES_SCRIPT}
        --yaml ${SAD_ERROR_MESSAGES_YAML}
        --schema ${SAD_ERROR_MESSAGES_SCHEMA}
        --enum-header ${SAD_ERROR_MESSAGES_ENUM_H}
        --header ${SAD_ERROR_MESSAGES_GEN_H}
        --source ${SAD_ERROR_MESSAGES_GEN_C}
        --quiet
    DEPENDS ${SAD_ERROR_MESSAGES_YAML}
            ${SAD_ERROR_MESSAGES_SCHEMA}
            ${SAD_ERROR_MESSAGES_SCRIPT}
            ${SAD_ERROR_MESSAGES_ENUM_H}
    COMMENT "[codegen] توليد كتالوج رسائل الأخطاء"
    VERBATIM
)

add_custom_target(sad_error_messages_codegen
    DEPENDS ${SAD_ERROR_MESSAGES_GEN_H} ${SAD_ERROR_MESSAGES_GEN_C}
)
add_dependencies(sad_error_messages_codegen sad_check_codegen_env)
```

في [shared/CMakeLists.txt](../../../shared/CMakeLists.txt) — إضافة الملف المولَّد إلى مصادر `sad_shared`:

```cmake
target_sources(sad_shared PRIVATE
    ${SAD_ERROR_MESSAGES_GEN_C}
)
add_dependencies(sad_shared sad_error_messages_codegen)
```

---

## 7. التغييرات في C++ الموجود

### 7.1 `error_codes.cpp` — تقليص ≥ 70%

**قبل (843 سطر)**:
- `errorCodeStrings` map (203 إدخال يدوي).
- `errorDescriptions` map (203 إدخال يدوي).

**بعد (≤ 250 سطر)**:
```cpp
#include "error_codes.h"
#include "../generated/error_messages_generated.h"

namespace Sad::Errors {

std::string getErrorCodeString(ErrorCode code) {
    auto* e = Generated::findByCode(code);
    return e ? e->id : "UNKNOWN";
}

std::string getErrorDescription(ErrorCode code, Language lang) {
    auto* e = Generated::findByCode(code);
    if (!e) return (lang == Language::ARABIC) ? "خطأ غير معروف" : "Unknown error";
    switch (lang) {
        case Language::ARABIC:  return e->briefAr ? e->briefAr : "";
        case Language::ENGLISH: return e->briefEn ? e->briefEn : "";
        case Language::BOTH:    return std::string(e->briefAr ? e->briefAr : "") + " / "
                                       + std::string(e->briefEn ? e->briefEn : "");
    }
    return "";
}

std::string getErrorCategory(ErrorCode code) {
    auto* e = Generated::findByCode(code);
    return e ? e->category : "Unknown";
}

// ... باقي الدوال المساعدة (formatError, etc.) تبقى
}
```

### 7.2 `error_catalog_init.cpp` — تقليص ≥ 65%

**قبل (213 سطر)**: 26 × `registerTemplate(...)` يدوياً.

**بعد (≤ 80 سطر)**:
```cpp
#include "error_catalog.h"
#include "../generated/error_messages_generated.h"

namespace Sad::Errors {

void ErrorCatalog::registerDefaults() {
    using G = Generated::GeneratedErrorEntry;
    for (std::size_t i = 0; i < Generated::kErrorMessagesCount; ++i) {
        const G& e = Generated::kErrorMessages[i];
        ErrorTemplate t;
        t.id        = e.id;
        t.titleAr   = e.titleAr   ? e.titleAr   : "";
        t.titleEn   = e.titleEn   ? e.titleEn   : "";
        t.briefAr   = e.briefAr   ? e.briefAr   : "";
        t.briefEn   = e.briefEn   ? e.briefEn   : "";
        t.detailedAr= e.detailedAr? e.detailedAr: "";
        t.detailedEn= e.detailedEn? e.detailedEn: "";
        t.teacherAr = e.teacherAr ? e.teacherAr : "";
        t.teacherEn = e.teacherEn ? e.teacherEn : "";
        t.fixHintAr = e.fixHintAr ? e.fixHintAr : "";
        t.fixHintEn = e.fixHintEn ? e.fixHintEn : "";
        t.codeExample = e.codeExample ? e.codeExample : "";
        registerTemplate(e.code, std::move(t));
    }
}

}
```

---

## 8. خطة الاختبار

### 8.1 اختبارات Python (pytest)

`scripts/codegen/test_gen_error_messages.py`:
- ✅ `test_yaml_validates_against_schema`
- ✅ `test_all_codes_unique`
- ✅ `test_all_ids_unique`
- ✅ `test_all_codes_in_enum`
- ✅ `test_all_placeholders_declared`
- ✅ `test_ids_match_category_prefix`
- ✅ `test_entries_sorted_correctly`
- ✅ `test_generator_produces_compilable_cpp`
- ✅ `test_generator_idempotent` (تشغيل مرتين = نفس الإخراج)

### 8.2 اختبارات C++

`tests/errors/test_generated_messages.cpp`:
- ✅ `test_count_matches_enum_size`
- ✅ `test_findByCode_returns_valid_entry_for_each_code`
- ✅ `test_findByCode_returns_nullptr_for_invalid`
- ✅ `test_findById_works_for_all_ids`
- ✅ `test_specific_entry_SEM_TYPE_MISMATCH_content`

### 8.3 Golden Round-Trip (BF-22)

`scripts/codegen/golden_render_test.py`:
1. **قبل التغيير**: تشغيل برنامج C++ صغير يطبع لكل ErrorCode:
   ```
   <code> | <getErrorCodeString> | <getErrorDescription AR> | <getErrorDescription EN> | <getErrorCategory> | <ErrorCatalog::render(BRIEF, AR)> | <render(BRIEF, EN)>
   ```
   ← يُحفظ كـ `tests/errors/golden_before.txt`.
2. **بعد التغيير**: نفس البرنامج → `golden_after.txt`.
3. **التحقُّق**: `diff golden_before.txt golden_after.txt` = 0.

### 8.4 اختبار الإصلاح (BF-29)

- `cmake --build build --target sad`
- `cmake --build build --target sadc`
- `.\run_tests.ps1` → `PASS:137 / FAIL:20` بالضبط.

---

## 9. ملف الترحيل المؤقت

`scripts/codegen/migrate_error_messages.py` (يُستخدم مرة واحدة ثم يُحذف):

```python
"""
ترحيل تلقائي: يستخرج من error_codes.cpp + error_catalog_init.cpp
ويُولِّد مسوّدة error_messages.yaml للمراجعة البشرية.
"""
# 1. يقرأ errorCodeStrings → يستخرج (code, id)
# 2. يقرأ errorDescriptions → يستخرج (code, briefAr, briefEn)
# 3. يقرأ registerTemplate(...) → يستخرج template الكامل (26 خطأ)
# 4. يدمج البيانات في entries
# 5. يستنتج category من id prefix
# 6. يكتب error_messages.yaml.draft
# 7. يكتب migration_report.txt يُعدِّد ما تم استخراجه
```

---

## 10. ملفات الإخراج

| الملف | الإجراء | الحجم المتوقع |
|---|---|---|
| `data/language/error_messages.yaml` | **جديد** | ~1500 سطر |
| `data/language/error_messages.schema.json` | **جديد** | ~80 سطر |
| `scripts/codegen/gen_error_messages.py` | **جديد** | ~250 سطر |
| `scripts/codegen/migrate_error_messages.py` | **جديد (مؤقت)** | ~150 سطر |
| `scripts/codegen/test_gen_error_messages.py` | **جديد** | ~120 سطر |
| `shared/errors/generated/error_messages_generated.h` | **مولَّد** | ~60 سطر |
| `shared/errors/generated/error_messages_generated.cpp` | **مولَّد** | ~700 سطر |
| `shared/errors/generated/*.baseline` | **مولَّد + git** | نسخة من المُولَّد |
| `tests/errors/test_generated_messages.cpp` | **جديد** | ~150 سطر |
| `cmake/codegen.cmake` | **معدَّل** | +30 سطر |
| `shared/CMakeLists.txt` | **معدَّل** | +5 أسطر |
| [shared/errors/src/error_codes.cpp](../../../shared/errors/src/error_codes.cpp) | **معدَّل (تقليص)** | 843 → ≤250 سطر |
| [shared/errors/src/error_catalog_init.cpp](../../../shared/errors/src/error_catalog_init.cpp) | **معدَّل (تقليص)** | 213 → ≤80 سطر |
| `tests/errors/golden_before.txt` و `golden_after.txt` | **مولَّد (مؤقت)** | ~600 سطر كل منها |

---

## 11. التحقق النهائي

```powershell
# 1. توليد الـbaseline يدوياً
python scripts/codegen/gen_error_messages.py `
  --yaml data/language/error_messages.yaml `
  --schema data/language/error_messages.schema.json `
  --enum-header shared/errors/include/error_codes.h `
  --header shared/errors/generated/error_messages_generated.h `
  --source shared/errors/generated/error_messages_generated.cpp

# 2. اختبارات Python
pytest scripts/codegen/test_gen_error_messages.py -v

# 3. بناء كامل
cmake --build build --config Debug --target sad
cmake --build build --config Debug --target sadc

# 4. اختبارات C++ + golden
ctest --test-dir build -R Errors -V

# 5. لا تراجع
.\run_tests.ps1   # = PASS:137 / FAIL:20
```

---

**حالة Tech Spec**: جاهز للتنفيذ.
