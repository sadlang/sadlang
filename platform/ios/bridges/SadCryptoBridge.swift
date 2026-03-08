// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadCryptoBridge.swift
// الوصف: جسر التشفير — iOS (CryptoKit + CommonCrypto)
// ═══════════════════════════════════════════════════════════════════════════════

import Foundation
import CryptoKit
import CommonCrypto

/// جسر التشفير لغة ص — خوارزميات التشفير والتجزئة
public final class SadCryptoBridge {
    
    public static let shared = SadCryptoBridge()
    private init() {}
    
    // ═════════════════════════════════════════════════════════════════════
    // التجزئة (Hashing)
    // ═════════════════════════════════════════════════════════════════════
    
    /// تجزئة SHA-256
    public func sha256(_ input: String) -> String {
        let data = Data(input.utf8)
        let hash = SHA256.hash(data: data)
        return hash.compactMap { String(format: "%02x", $0) }.joined()
    }
    
    /// تجزئة SHA-384
    public func sha384(_ input: String) -> String {
        let data = Data(input.utf8)
        let hash = SHA384.hash(data: data)
        return hash.compactMap { String(format: "%02x", $0) }.joined()
    }
    
    /// تجزئة SHA-512
    public func sha512(_ input: String) -> String {
        let data = Data(input.utf8)
        let hash = SHA512.hash(data: data)
        return hash.compactMap { String(format: "%02x", $0) }.joined()
    }
    
    /// تجزئة MD5 (للتوافقية فقط — غير آمنة)
    public func md5(_ input: String) -> String {
        let data = Data(input.utf8)
        var digest = [UInt8](repeating: 0, count: Int(CC_MD5_DIGEST_LENGTH))
        data.withUnsafeBytes {
            _ = CC_MD5($0.baseAddress, CC_LONG(data.count), &digest)
        }
        return digest.map { String(format: "%02x", $0) }.joined()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // HMAC
    // ═════════════════════════════════════════════════════════════════════
    
    /// HMAC-SHA256
    public func hmacSHA256(message: String, key: String) -> String {
        let symmetricKey = SymmetricKey(data: Data(key.utf8))
        let mac = HMAC<SHA256>.authenticationCode(for: Data(message.utf8), using: symmetricKey)
        return Data(mac).map { String(format: "%02x", $0) }.joined()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // التشفير المتماثل (AES-GCM)
    // ═════════════════════════════════════════════════════════════════════
    
    /// تشفير AES-GCM
    public func encrypt(plainText: String, key: String) -> String? {
        guard let keyData = key.data(using: .utf8) else { return nil }
        let symmetricKey = SymmetricKey(data: SHA256.hash(data: keyData))
        
        do {
            let sealedBox = try AES.GCM.seal(Data(plainText.utf8), using: symmetricKey)
            return sealedBox.combined?.base64EncodedString()
        } catch {
            print("خطأ في التشفير: \(error)")
            return nil
        }
    }
    
    /// فك التشفير AES-GCM
    public func decrypt(cipherText: String, key: String) -> String? {
        guard let keyData = key.data(using: .utf8),
              let data = Data(base64Encoded: cipherText) else { return nil }
        let symmetricKey = SymmetricKey(data: SHA256.hash(data: keyData))
        
        do {
            let sealedBox = try AES.GCM.SealedBox(combined: data)
            let decrypted = try AES.GCM.open(sealedBox, using: symmetricKey)
            return String(data: decrypted, encoding: .utf8)
        } catch {
            print("خطأ في فك التشفير: \(error)")
            return nil
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // Base64
    // ═════════════════════════════════════════════════════════════════════
    
    /// ترميز Base64
    public func base64Encode(_ input: String) -> String {
        return Data(input.utf8).base64EncodedString()
    }
    
    /// فك ترميز Base64
    public func base64Decode(_ input: String) -> String? {
        guard let data = Data(base64Encoded: input) else { return nil }
        return String(data: data, encoding: .utf8)
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // توليد عشوائي آمن
    // ═════════════════════════════════════════════════════════════════════
    
    /// توليد بايتات عشوائية آمنة
    public func randomBytes(count: Int) -> [UInt8] {
        var bytes = [UInt8](repeating: 0, count: count)
        _ = SecRandomCopyBytes(kSecRandomDefault, count, &bytes)
        return bytes
    }
    
    /// توليد نص عشوائي آمن (hex)
    public func randomHex(length: Int) -> String {
        let bytes = randomBytes(count: length / 2 + 1)
        return bytes.map { String(format: "%02x", $0) }.prefix(length).joined()
    }
    
    /// توليد UUID
    public func generateUUID() -> String {
        return UUID().uuidString
    }
}
