// بسم الله الرحمن الرحيم
/**
 * @file complete_crypto_example.s
 * @brief Complete Cryptography Examples - أمثلة شاملة للتشفير
 * 
 * الحمد لله رب العالمين - Examples covering all crypto features
 */

// ============================================================================
// Hash Function Examples - أمثلة دوال التجزئة
// ============================================================================

دالة example_hash_functions() {
    اطبع("=== Hash Functions Examples / أمثلة دوال التجزئة ===\n");
    
    // MD5 hash
    نص data = "Hello, World!";
    نص hash_md5 = md5(data);
    اطبع("MD5: " + hash_md5 + "\n");
    
    // SHA256 hash
    نص hash_sha256 = sha256(data);
    اطبع("SHA256: " + hash_sha256 + "\n");
    
    // SHA512 hash
    نص hash_sha512 = sha512(data);
    اطبع("SHA512: " + hash_sha512 + "\n");
    
    // Verify hash
    إذا (hash_verify("sha256", data, hash_sha256)) {
        اطبع("✓ Hash verification successful / التحقق من التجزئة ناجح\n");
    }
    
    // Arabic text hashing
    نص arabic_text = "بسم الله الرحمن الرحيم";
    نص arabic_hash = تجزئة("sha256", arabic_text);
    اطبع("Arabic text hash / تجزئة النص العربي: " + arabic_hash + "\n");
    
    اطبع("\n");
}

// ============================================================================
// HMAC Examples - أمثلة HMAC
// ============================================================================

دالة example_hmac() {
    اطبع("=== HMAC Examples / أمثلة HMAC ===\n");
    
    // Generate HMAC
    نص key = "secret_key_12345";
    نص message = "Important message";
    نص mac = hmac("sha256", key, message);
    
    اطبع("HMAC: " + mac + "\n");
    
    // Verify HMAC
    إذا (hmac_verify("sha256", key, message, mac)) {
        اطبع("✓ HMAC verification successful / التحقق من HMAC ناجح\n");
    }
    
    // Arabic HMAC
    نص مفتاح = "مفتاح_سري";
    نص رسالة = "رسالة مهمة";
    نص mac_ar = hmac("sha256", مفتاح, رسالة);
    اطبع("HMAC عربي: " + mac_ar + "\n");
    
    اطبع("\n");
}

// ============================================================================
// AES Encryption Examples - أمثلة تشفير AES
// ============================================================================

دالة example_aes_encryption() {
    اطبع("=== AES Encryption Examples / أمثلة تشفير AES ===\n");
    
    // Generate key and IV
    نص key = aes_generate_key(256);
    نص iv = aes_generate_iv();
    
    اطبع("Generated AES key / مفتاح AES المولد: " + key + "\n");
    اطبع("Generated IV: " + iv + "\n");
    
    // Encrypt
    نص plaintext = "This is a secret message الحمد لله";
    نص ciphertext = aes_encrypt(plaintext, key, iv, "cbc");
    
    اطبع("Encrypted / مشفر: " + ciphertext + "\n");
    
    // Decrypt
    نص decrypted = aes_decrypt(ciphertext, key, iv, "cbc");
    اطبع("Decrypted / مفكوك التشفير: " + decrypted + "\n");
    
    // Verify
    إذا (decrypted == plaintext) {
        اطبع("✓ Encryption/Decryption successful / التشفير/فك التشفير ناجح\n");
    }
    
    // Arabic encryption using Arabic functions
    نص نص_سري = "رسالة سرية للغاية";
    نص مشفر = شفر_aes(نص_سري, key, iv);
    نص مفكوك = فك_تشفير_aes(مشفر, key, iv);
    
    اطبع("نص عربي مفكوك: " + مفكوك + "\n");
    
    اطبع("\n");
}

// ============================================================================
// RSA Encryption Examples - أمثلة تشفير RSA
// ============================================================================

دالة example_rsa_encryption() {
    اطبع("=== RSA Encryption Examples / أمثلة تشفير RSA ===\n");
    
    // Generate key pair
    كائن keypair = rsa_generate_keypair(2048);
    
    اطبع("✓ RSA key pair generated / زوج مفاتيح RSA مولد\n");
    اطبع("Public key length / طول المفتاح العام: " + length(keypair.public) + "\n");
    
    // Encrypt with public key
    نص message = "Secret message الحمد لله";
    نص encrypted = rsa_encrypt(message, keypair.public);
    
    اطبع("Encrypted message / رسالة مشفرة: " + substring(encrypted, 0, 50) + "...\n");
    
    // Decrypt with private key
    نص decrypted = rsa_decrypt(encrypted, keypair.private);
    اطبع("Decrypted / مفكوك: " + decrypted + "\n");
    
    // Verify
    إذا (decrypted == message) {
        اطبع("✓ RSA Encryption/Decryption successful\n");
    }
    
    اطبع("\n");
}

// ============================================================================
// Digital Signature Examples - أمثلة التوقيع الرقمي
// ============================================================================

دالة example_digital_signatures() {
    اطبع("=== Digital Signature Examples / أمثلة التوقيع الرقمي ===\n");
    
    // Generate key pair
    كائن keypair = ولد_مفاتيح_rsa(2048);
    
    // Sign document
    نص document = "Important contract بسم الله الرحمن الرحيم";
    نص signature = rsa_sign(document, keypair.private, "sha256");
    
    اطبع("Document signed / وثيقة موقعة\n");
    اطبع("Signature length / طول التوقيع: " + length(signature) + "\n");
    
    // Verify signature
    إذا (rsa_verify(document, signature, keypair.public, "sha256")) {
        اطبع("✓ Signature valid / التوقيع صحيح\n");
    } وإلا {
        اطبع("✗ Signature invalid / التوقيع غير صحيح\n");
    }
    
    // Try with modified document
    نص modified_doc = document + " modified";
    إذا (!rsa_verify(modified_doc, signature, keypair.public, "sha256")) {
        اطبع("✓ Modified document detected / تم اكتشاف الوثيقة المعدلة\n");
    }
    
    // Arabic signing
    نص وثيقة = "عقد مهم";
    نص توقيع = وقع_rsa(وثيقة, keypair["مفتاح_خاص"], "sha256");
    
    إذا (تحقق_من_توقيع_rsa(وثيقة, توقيع, keypair["مفتاح_عام"], "sha256")) {
        اطبع("✓ توقيع عربي صحيح\n");
    }
    
    اطبع("\n");
}

// ============================================================================
// Password Hashing Examples - أمثلة تجزئة كلمات المرور
// ============================================================================

دالة example_password_hashing() {
    اطبع("=== Password Hashing Examples / أمثلة تجزئة كلمات المرور ===\n");
    
    // Hash password
    نص password = "MySecurePassword123!";
    كائن hashed = password_hash(password);
    
    اطبع("Password hashed / كلمة المرور مجزأة\n");
    اطبع("Hash: " + substring(hashed.hash, 0, 32) + "...\n");
    اطبع("Salt: " + substring(hashed.salt, 0, 32) + "...\n");
    
    // Verify correct password
    إذا (password_verify(password, hashed.hash, hashed.salt)) {
        اطبع("✓ Correct password / كلمة المرور صحيحة\n");
    }
    
    // Verify wrong password
    نص wrong_password = "WrongPassword";
    إذا (!password_verify(wrong_password, hashed.hash, hashed.salt)) {
        اطبع("✓ Wrong password rejected / كلمة مرور خاطئة مرفوضة\n");
    }
    
    // Arabic password
    نص كلمة_مرور = "كلمة_مرور_قوية_123";
    كائن مجزأ = جزئ_كلمة_مرور(كلمة_مرور);
    
    إذا (تحقق_من_كلمة_مرور(كلمة_مرور, مجزأ["تجزئة"], مجزأ["ملح"])) {
        اطبع("✓ كلمة المرور العربية صحيحة\n");
    }
    
    اطبع("\n");
}

// ============================================================================
// Random Generation Examples - أمثلة التوليد العشوائي
// ============================================================================

دالة example_random_generation() {
    اطبع("=== Random Generation Examples / أمثلة التوليد العشوائي ===\n");
    
    // Random bytes (hex encoded)
    نص random_hex = random_bytes(16);
    اطبع("Random 16 bytes / 16 بايت عشوائي: " + random_hex + "\n");
    
    // Random integer
    رقم rand_int = random_int(1, 100);
    اطبع("Random integer (1-100) / عدد عشوائي: " + rand_int + "\n");
    
    // Random number
    رقم rand_num = random_number(0, 1);
    اطبع("Random number (0-1) / رقم عشوائي: " + rand_num + "\n");
    
    // Random hex string
    نص hex_string = random_hex(32);
    اطبع("Random hex (32 bytes) / hex عشوائي: " + substring(hex_string, 0, 40) + "...\n");
    
    // Random alphanumeric string
    نص alphanum = random_string(20);
    اطبع("Random alphanumeric / أبجدي رقمي عشوائي: " + alphanum + "\n");
    
    // UUID
    نص uuid_value = uuid();
    اطبع("UUID: " + uuid_value + "\n");
    
    // Arabic random functions
    نص بايتات_عشوائية = بايتات_عشوائية(8);
    رقم عدد_عشوائي = عدد_صحيح_عشوائي(1, 50);
    نص نص_عشوائي = نص_عشوائي(15);
    
    اطبع("بايتات عشوائية: " + بايتات_عشوائية + "\n");
    اطبع("عدد عشوائي: " + عدد_عشوائي + "\n");
    اطبع("نص عشوائي: " + نص_عشوائي + "\n");
    
    اطبع("\n");
}

// ============================================================================
// Encoding Examples - أمثلة الترميز
// ============================================================================

دالة example_encoding() {
    اطبع("=== Encoding Examples / أمثلة الترميز ===\n");
    
    نص original = "Hello, World! مرحبا بالعالم";
    
    // Base64
    نص base64_encoded = base64_encode(original);
    نص base64_decoded = base64_decode(base64_encoded);
    
    اطبع("Original / الأصلي: " + original + "\n");
    اطبع("Base64 encoded / مرمز: " + base64_encoded + "\n");
    اطبع("Base64 decoded / مفكوك: " + base64_decoded + "\n");
    
    // Hex
    نص hex_encoded = hex_encode(original);
    نص hex_decoded = hex_decode(hex_encoded);
    
    اطبع("Hex encoded / hex مرمز: " + hex_encoded + "\n");
    اطبع("Hex decoded / hex مفكوك: " + hex_decoded + "\n");
    
    // Arabic encoding functions
    نص نص_عربي = "بسم الله الرحمن الرحيم";
    نص مرمز = رمز_base64(نص_عربي);
    نص مفكوك = فك_ترميز_base64(مرمز);
    
    اطبع("نص عربي مرمز base64: " + مرمز + "\n");
    اطبع("نص عربي مفكوك: " + مفكوك + "\n");
    
    اطبع("\n");
}

// ============================================================================
// Practical Use Cases - حالات استخدام عملية
// ============================================================================

دالة example_secure_file_transfer() {
    اطبع("=== Secure File Transfer Simulation / محاكاة نقل ملف آمن ===\n");
    
    // Sender: Generate RSA key pair
    كائن sender_keys = rsa_generate_keypair(2048);
    
    // Sender: Generate AES key for file encryption
    نص aes_key = aes_generate_key(256);
    نص aes_iv = aes_generate_iv();
    
    // Sender: Encrypt file content
    نص file_content = "Confidential document الحمد لله\nLine 2\nLine 3";
    نص encrypted_content = aes_encrypt(file_content, aes_key, aes_iv);
    
    اطبع("1. File encrypted with AES / الملف مشفر بـ AES\n");
    
    // Sender: Sign the encrypted content
    نص signature = rsa_sign(encrypted_content, sender_keys.private);
    
    اطبع("2. Content signed / المحتوى موقع\n");
    
    // Receiver: Verify signature
    إذا (rsa_verify(encrypted_content, signature, sender_keys.public)) {
        اطبع("3. ✓ Signature verified / التوقيع محقق\n");
        
        // Receiver: Decrypt content
        نص decrypted_content = aes_decrypt(encrypted_content, aes_key, aes_iv);
        
        إذا (decrypted_content == file_content) {
            اطبع("4. ✓ File decrypted successfully / الملف فك تشفيره بنجاح\n");
            اطبع("Original content: " + substring(file_content, 0, 30) + "...\n");
        }
    }
    
    اطبع("\n");
}

دالة example_user_authentication() {
    اطبع("=== User Authentication System / نظام مصادقة المستخدم ===\n");
    
    // Registration
    نص username = "ahmad";
    نص password = "SecurePass123!";
    
    كائن hashed_password = password_hash(password);
    
    اطبع("User registered / المستخدم مسجل: " + username + "\n");
    اطبع("Password hash stored / تجزئة كلمة المرور مخزنة\n");
    
    // Login - correct password
    اطبع("\n--- Login Attempt 1 / محاولة تسجيل دخول 1 ---\n");
    نص login_password = "SecurePass123!";
    
    إذا (password_verify(login_password, hashed_password.hash, hashed_password.salt)) {
        اطبع("✓ Login successful / تسجيل الدخول ناجح\n");
        
        // Generate session token
        نص session_token = uuid();
        اطبع("Session token: " + session_token + "\n");
    } وإلا {
        اطبع("✗ Login failed / فشل تسجيل الدخول\n");
    }
    
    // Login - wrong password
    اطبع("\n--- Login Attempt 2 / محاولة تسجيل دخول 2 ---\n");
    نص wrong_password = "WrongPass";
    
    إذا (password_verify(wrong_password, hashed_password.hash, hashed_password.salt)) {
        اطبع("✓ Login successful\n");
    } وإلا {
        اطبع("✗ Login failed - Wrong password / كلمة مرور خاطئة\n");
    }
    
    اطبع("\n");
}

دالة example_api_authentication() {
    اطبع("=== API Authentication with HMAC / مصادقة API بـ HMAC ===\n");
    
    // API credentials
    نص api_key = "user_api_key_12345";
    نص api_secret = "user_api_secret_67890";
    
    // Create request
    نص request_body = '{"action":"get_user","user_id":123}';
    نص timestamp = "1234567890";
    نص request_data = request_body + timestamp;
    
    // Generate HMAC signature
    نص signature = hmac("sha256", api_secret, request_data);
    
    اطبع("API Request / طلب API:\n");
    اطبع("  Key: " + api_key + "\n");
    اطبع("  Data: " + request_data + "\n");
    اطبع("  Signature: " + substring(signature, 0, 32) + "...\n");
    
    // Server verifies request
    اطبع("\nServer verification / التحقق من الخادم:\n");
    إذا (hmac_verify("sha256", api_secret, request_data, signature)) {
        اطبع("✓ Request authenticated / الطلب موثق\n");
        اطبع("✓ Processing request... / معالجة الطلب...\n");
    } وإلا {
        اطبع("✗ Authentication failed / فشلت المصادقة\n");
    }
    
    اطبع("\n");
}

دالة example_data_integrity() {
    اطبع("=== Data Integrity Check / فحص سلامة البيانات ===\n");
    
    // Original data
    نص important_data = "Critical system data الحمد لله";
    
    // Compute hash for integrity
    نص original_hash = sha256(important_data);
    
    اطبع("Original data / البيانات الأصلية: " + important_data + "\n");
    اطبع("Hash stored / التجزئة مخزنة: " + original_hash + "\n");
    
    // Check 1: Data unchanged
    اطبع("\n--- Check 1: Verify unchanged data ---\n");
    نص current_data = important_data;
    نص current_hash = sha256(current_data);
    
    إذا (current_hash == original_hash) {
        اطبع("✓ Data integrity verified / سلامة البيانات محققة\n");
    }
    
    // Check 2: Data modified
    اطبع("\n--- Check 2: Verify modified data ---\n");
    نص modified_data = important_data + " modified";
    نص modified_hash = sha256(modified_data);
    
    إذا (modified_hash != original_hash) {
        اطبع("✗ Data integrity compromised / سلامة البيانات مخترقة\n");
        اطبع("⚠️  Data has been modified / البيانات تم تعديلها\n");
    }
    
    اطبع("\n");
}

دالة example_encryption_best_practices() {
    اطبع("=== Encryption Best Practices / أفضل ممارسات التشفير ===\n");
    
    // 1. Use strong random keys
    اطبع("1. Always use cryptographically secure random keys:\n");
    نص strong_key = aes_generate_key(256);
    اطبع("   ✓ AES-256 key generated / مفتاح AES-256 مولد\n");
    
    // 2. Use unique IVs
    اطبع("\n2. Always use unique IVs for each encryption:\n");
    نص iv1 = aes_generate_iv();
    نص iv2 = aes_generate_iv();
    اطبع("   ✓ IV 1: " + substring(iv1, 0, 20) + "...\n");
    اطبع("   ✓ IV 2: " + substring(iv2, 0, 20) + "...\n");
    
    // 3. Use strong password hashing
    اطبع("\n3. Use PBKDF2 for password hashing:\n");
    نص password = "user_password";
    كائن hashed = password_hash(password);
    اطبع("   ✓ Password hashed with salt / كلمة المرور مجزأة مع ملح\n");
    
    // 4. Verify signatures
    اطبع("\n4. Always verify digital signatures:\n");
    كائن keys = rsa_generate_keypair(2048);
    نص doc = "Important document";
    نص sig = rsa_sign(doc, keys.private);
    إذا (rsa_verify(doc, sig, keys.public)) {
        اطبع("   ✓ Signature verified / التوقيع محقق\n");
    }
    
    // 5. Use HMAC for message authentication
    اطبع("\n5. Use HMAC for API authentication:\n");
    نص api_secret = "secret_key";
    نص message = "api_request_data";
    نص mac = hmac("sha256", api_secret, message);
    اطبع("   ✓ HMAC generated for message authentication\n");
    
    اطبع("\n");
}

// ============================================================================
// Main Function - الدالة الرئيسية
// ============================================================================

دالة main() {
    اطبع("\n");
    اطبع("╔════════════════════════════════════════════════════════════╗\n");
    اطبع("║   Sad Language Cryptography Module - Complete Examples    ║\n");
    اطبع("║   وحدة التشفير للغة الحزينة - أمثلة شاملة                ║\n");
    اطبع("║   بسم الله الرحمن الرحيم                                 ║\n");
    اطبع("╚════════════════════════════════════════════════════════════╝\n");
    اطبع("\n");
    
    // Run all examples
    example_hash_functions();
    example_hmac();
    example_aes_encryption();
    example_rsa_encryption();
    example_digital_signatures();
    example_password_hashing();
    example_random_generation();
    example_encoding();
    
    // Practical use cases
    اطبع("╔════════════════════════════════════════════════════════════╗\n");
    اطبع("║              Practical Use Cases / حالات عملية            ║\n");
    اطبع("╚════════════════════════════════════════════════════════════╝\n");
    اطبع("\n");
    
    example_secure_file_transfer();
    example_user_authentication();
    example_api_authentication();
    example_data_integrity();
    example_encryption_best_practices();
    
    اطبع("╔════════════════════════════════════════════════════════════╗\n");
    اطبع("║   All cryptography examples completed successfully! ✅    ║\n");
    اطبع("║   جميع أمثلة التشفير اكتملت بنجاح! ✅                    ║\n");
    اطبع("║   الحمد لله رب العالمين                                  ║\n");
    اطبع("╚════════════════════════════════════════════════════════════╝\n");
    
    ارجع 0;
}

// الحمد لله رب العالمين
