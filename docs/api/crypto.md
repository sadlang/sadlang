# وثائق API — وحدة crypto

> المسار: `stdlib/crypto/`
> مساحة الأسماء: `sad::stdlib::crypto`

## نظرة عامة
توفر الوحدة عمليات التشفير الأساسية: hashing, HMAC, AES, وتوليد مفاتيح.

## الملفات الأساسية
- `crypto/crypto_module.h`
- `crypto/crypto_module.cpp`

## API رئيسية
- `enum class HashAlgorithm`
  - `MD5`, `SHA256`, `SHA512`, ...
- `class Hash`
  - `compute_hex(algorithm, data)`
  - `verify(algorithm, data, expectedHash)`
- `class HMAC`
  - `compute_hex(algorithm, key, data)`
  - `verify(algorithm, key, data, mac)`
- `class AES`
  - `encrypt_string(plaintext, key, iv)`
  - `decrypt_string(ciphertext, key, iv)`
  - `generate_key(KeySize)`

## مثال سريع
```cpp
using namespace sad::stdlib::crypto;
std::string hash = Hash::compute_hex(HashAlgorithm::SHA256, "hello");
```

## ملاحظات
- اختبارات الوحدة تغطي نصوص عربية، أحجام بيانات كبيرة، والتحقق من سلامة hash/HMAC.
