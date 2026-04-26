# أنواع تجريبية مُؤرشفة (Experimental Types — Archived)

## السبب

هذه الملفات الثلاثة كانت مرشحة كـ **🔬 تجريبي** في
`docs/وثيقة_تتبع_ميزات_المشروع_الشاملة.md`
وثبت عبر فحص شامل بـ `grep_search` لـ `*.cpp/*.h/CMakeLists.txt` أنها **معزولة كلياً**:
لا أحد يستوردها (`#include`)، لا أحد يستدعي رموزها، ولا أي اختبار يغطيها.

| الملف | الحجم | السطور | الحالة الموثقة |
|------|-------|--------|----------------|
| `qubit_type.cpp` | 16 KB | 378 | 🔬 — يحتاج `SadTypeKind::Qubit` + `stdlib/كمومي.ص` |
| `actor_type.cpp` | 7 KB | 227 | 🔬 — `ActorRef::send()` يطبع `cerr` فقط (لا MessageRouter) |
| `evm_codegen.cpp` | 15 KB | 511 | 🔬 — لم يُربط بـ AST، `keccak256` يرجع `0x12345678` ثابت |

## التحقق قبل الأرشفة

```powershell
# لم يُرجع أي ملف مرجع لأي من الرموز التالية خارج الملفات نفسها:
Select-String -Pattern 'QubitType|QubitState|ActorType|ActorRef|evm_codegen|EVM_Opcode' -Path *.cpp,*.h
# النتيجة: 0 مراجع خارجية
```

## التغييرات في `compiler/CMakeLists.txt`

- حُذف `src/backend/evm_codegen.cpp` من `BACKEND_COMMON_SOURCES`
- `qubit_type.cpp` و `actor_type.cpp` كانا داخل `file(GLOB src/types/*.cpp)` —
  تختفي تلقائياً بعد النقل (لا حاجة لتعديل CMake).

## استعادة الملفات

```powershell
Move-Item archived/experimental_types/qubit_type.cpp     compiler/src/types/
Move-Item archived/experimental_types/actor_type.cpp     compiler/src/types/
Move-Item archived/experimental_types/evm_codegen.cpp    compiler/src/backend/
# ثم استعادة سطر evm_codegen.cpp في BACKEND_COMMON_SOURCES
```

## ما يحتاجه كل ملف ليعود حياً

### Qubit
1. `SadTypeKind::Qubit` في `shared/types/include/type.h`
2. كلمة `كيوبت` كنوع مدمج
3. `stdlib/كمومي.ص` (غير موجود)

### Actor
1. بناء `MessageRouter` فوق `SadChannel`
2. كلمتا `ممثل` و `رسالة` في lexer + parser
3. توليد SIR لـ Actor dispatch loop

### EVM
1. `keccak256` حقيقي (عبر OpenSSL المدمج)
2. ربط `ولّد_عقد()` بـ `ContractDecl` في AST
3. تنفيذ `ولّد_كود_النشر()`
4. CLI: `ص بناء --هدف=ethereum`

## التحقق بعد الأرشفة

- `cmake --build build --config Release --target sad sadc` → ✅ نجح
- اختبارات `tests/compiler_features/` → `PASS=32 XFAIL=18 FAIL=0` ✅

تاريخ الأرشفة: ضمن جلسة Fix #5 (تنظيف الميزات غير المُكتملة).
