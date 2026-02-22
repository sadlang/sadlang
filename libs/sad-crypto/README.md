# sad-crypto — مكتبة التشفير والأمان
# Cryptography & Security Library for لغة ص

## الوصف / Description
مكتبة تشفير وأمان شاملة تحتوي على خوارزميات التشفير، التجزئة، التحقق، وتوليد الأرقام العشوائية الآمنة.

A comprehensive cryptography library with encryption algorithms, hash functions, verification, and secure random number generation.

## الدوال المتوفرة / Available Functions

### التشفير / Encryption
- `قيصر_شفر(قيمة، مفتاح)` — Caesar cipher encrypt
- `قيصر_فك(قيمة، مفتاح)` — Caesar cipher decrypt
- `xor_شفر(قيمة، مفتاح)` — XOR encryption (self-reversing)
- `xor_كتلة(ق1، ق2، ق3، ق4، مفتاح)` — Block XOR
- `spn_شفر(نص، مفتاح)` — 4-round SPN cipher
- `spn_فك(نص، مفتاح)` — SPN decrypt

### التجزئة / Hashing
- `تجزئة(قيمة)` — DJB2 hash
- `تجزئة_مجمعة(ق1، ق2، ق3)` — Combined hash
- `تجزئة_fnv(قيمة)` — FNV-1a hash

### التحقق / Verification
- `لون_تحقق(رقم_البطاقة)` — Luhn algorithm
- `مجموع_التحقق(ق1، ق2، ق3، ق4)` — CRC-like checksum
- `تحقق_تكامل(ق1، ق2، ق3، ق4، مجموع)` — Integrity check

### الأمان / Security
- `عشوائي_آمن()` — Secure RNG (xorshift32)
- `عشوائي_آمن_بين(أدنى، أقصى)` — Secure random in range
- `ولد_رمز(طول)` — Generate PIN code
- `قوة_كلمة_المرور(كلمة_سر)` — Password strength (0-100)
- `تعيين_بذرة_أمان(بذرة)` — Set RNG seed

### الاستبدال / Substitution
- `_sbox(قيمة)` / `_sbox_عكسي(قيمة)` — S-Box tables

## التثبيت / Installation
```
sad-pkg add sad-crypto ^1.0.0
```

## الترخيص / License
MIT
