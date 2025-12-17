# 📚 توسيع المكتبة القياسية / Standard Library Expansion Plan

**المرحلة / Phase:** 3  
**المدة / Duration:** 3 أشهر / months  
**الأولوية / Priority:** P1 (مهمة / Important)

---

## 📋 نظرة عامة / Overview

### (AR) الوصف
توسيع شامل للمكتبة القياسية يشمل: الشبكات (HTTP/WebSocket)، معالجة الملفات المتقدمة، JSON/YAML، التشفير، قواعد البيانات، والمزيد. الهدف: توفير أدوات جاهزة للاستخدام للتطبيقات الحديثة.

### (EN) Description
Comprehensive expansion of the standard library including: networking (HTTP/WebSocket), advanced file operations, JSON/YAML, encryption, databases, and more. Goal: provide ready-to-use tools for modern applications.

---

## 🎯 المجالات / Categories

### 1. الشبكات / Networking

#### 1.1 HTTP Client

**الدوال المقترحة:**
```sad
# طلبات HTTP بسيطة
متغير استجابة = http_get("https://api.example.com/data")
متغير محتوى = http_post("https://api.example.com/submit", بيانات: {...})

# طلبات متقدمة
متغير استجابة = طلب_http(
    رابط: "https://api.example.com/data"،
    طريقة: "GET"،
    رؤوس: {
        "Authorization": "Bearer token123"،
        "Content-Type": "application/json"
    }،
    مهلة: 30  # ثانية
)

# فحص الاستجابة
إذا (استجابة.كود == 200) {
    متغير json = تحليل_json(استجابة.محتوى)
    اطبع(json)
} وإلا {
    اطبع("خطأ: " + استجابة.رسالة)
}
```

**التنفيذ C++:**
```cpp
// في include/stdlib/net/http_client.h
namespace Sad::Stdlib::Net {

struct HttpResponse {
    int statusCode;
    std::string body;
    std::map<std::string, std::string> headers;
    std::string error;
};

class HttpClient {
public:
    HttpResponse get(const std::string& url,
                     const std::map<std::string, std::string>& headers = {},
                     int timeout = 30);
    
    HttpResponse post(const std::string& url,
                      const std::string& body,
                      const std::map<std::string, std::string>& headers = {},
                      int timeout = 30);
    
    HttpResponse request(const std::string& method,
                         const std::string& url,
                         const std::string& body = "",
                         const std::map<std::string, std::string>& headers = {},
                         int timeout = 30);
};

} // namespace
```

**المهام:**
- [ ] libcurl integration
- [ ] Request building
- [ ] Response parsing
- [ ] Error handling
- [ ] Timeout support
- [ ] SSL/TLS support
- [ ] اختبارات

**المدة:** 2-3 أسابيع

---

#### 1.2 HTTP Server

**مثال:**
```sad
# خادم HTTP بسيط
متغير خادم = خادم_http(منفذ: 8080)

# تعريف نقاط النهاية
خادم.عند_طلب("/"، دالة(طلب، استجابة) {
    استجابة.أرسل("مرحباً بالعالم!")
})

خادم.عند_طلب("/api/data"، دالة(طلب، استجابة) {
    متغير بيانات = {
        "message": "Hello"،
        "timestamp": الوقت_الحالي()
    }
    استجابة.json(بيانات)
})

# بدء الخادم
خادم.استمع()
اطبع("الخادم يعمل على المنفذ 8080")
```

**التنفيذ:**
```cpp
// استخدام مكتبة خفيفة مثل cpp-httplib
namespace Sad::Stdlib::Net {

class HttpServer {
public:
    explicit HttpServer(int port);
    
    void on(const std::string& path, 
           std::function<void(Request&, Response&)> handler);
    
    void listen();
    void stop();
    
private:
    int port_;
    httplib::Server server_;
};

} // namespace
```

**المهام:**
- [ ] HTTP server implementation
- [ ] Routing system
- [ ] Request/Response handling
- [ ] Static file serving
- [ ] WebSocket support (optional)
- [ ] اختبارات

**المدة:** 2-3 أسابيع

---

#### 1.3 WebSocket Client/Server

**مثال:**
```sad
# WebSocket Client
متغير ws = websocket_client("wss://echo.websocket.org")

ws.عند_الاتصال(دالة() {
    اطبع("متصل!")
    ws.أرسل("مرحباً")
})

ws.عند_رسالة(دالة(رسالة) {
    اطبع("استلمت: " + رسالة)
})

ws.اتصل()
```

**المدة:** 2 أسابيع

---

### 2. معالجة البيانات / Data Processing

#### 2.1 JSON Parser & Generator

**الدوال:**
```sad
# تحليل JSON
متغير نص_json = '{"name": "أحمد"، "age": 25}'
متغير كائن = تحليل_json(نص_json)
اطبع(كائن["name"])  # أحمد

# توليد JSON
متغير بيانات = {
    "اسم": "فاطمة"،
    "عمر": 30،
    "هوايات": ["قراءة"، "رياضة"]
}
متغير json = إلى_json(بيانات، مسافة: 2)
اطبع(json)

# JSON متقدم
متغير parser = محلل_json()
parser.تعيين_خيار("allow_comments"، صحيح)
parser.تعيين_خيار("allow_trailing_commas"، صحيح)
متغير نتيجة = parser.تحليل(نص)
```

**التنفيذ:**
```cpp
// استخدام مكتبة مثل nlohmann/json
namespace Sad::Stdlib::Data {

class JsonParser {
public:
    Value parse(const std::string& text);
    std::string stringify(const Value& value, int indent = 0);
    
    void setOption(const std::string& key, bool value);
};

// دوال مساعدة
Value parseJson(const std::string& text);
std::string toJson(const Value& value, int indent = 0);

} // namespace
```

**المهام:**
- [ ] JSON parser integration
- [ ] Stringify implementation
- [ ] Error handling
- [ ] Unicode support
- [ ] Streaming parser (للملفات الكبيرة)
- [ ] اختبارات

**المدة:** 1-2 أسبوع

---

#### 2.2 YAML Parser

**مثال:**
```sad
# تحليل YAML
متغير نص_yaml = """
name: أحمد
age: 25
hobbies:
  - قراءة
  - برمجة
"""
متغير كائن = تحليل_yaml(نص_yaml)
```

**المدة:** 1-2 أسبوع

---

#### 2.3 CSV Parser

**مثال:**
```sad
# قراءة CSV
متغير بيانات = قراءة_csv("data.csv")
لكل صف في بيانات {
    اطبع(صف["name"] + ": " + صف["age"])
}

# كتابة CSV
متغير بيانات = [
    {"name": "أحمد"، "age": "25"}،
    {"name": "فاطمة"، "age": "30"}
]
كتابة_csv("output.csv"، بيانات)
```

**المدة:** 1 أسبوع

---

### 3. عمليات الملفات المتقدمة / Advanced File Operations

#### 3.1 Path Manipulation

**الدوال:**
```sad
# معالجة المسارات
متغير مسار = مسار_دمج("home"، "user"، "documents"، "file.txt")
# → home/user/documents/file.txt

متغير اسم = اسم_الملف(مسار)  # file.txt
متغير امتداد = امتداد_الملف(مسار)  # .txt
متغير مجلد = مجلد_الملف(مسار)  # home/user/documents

متغير مطلق = مسار_مطلق("./file.txt")
متغير نسبي = مسار_نسبي("/home/user/file.txt"، "/home")
# → user/file.txt
```

---

#### 3.2 Directory Operations

**الدوال:**
```sad
# عمليات المجلدات
أنشئ_مجلد("new_folder")
أنشئ_مجلدات("path/to/nested/folder")  # إنشاء متداخل

متغير ملفات = قائمة_ملفات("."، نمط: "*.s")
لكل ملف في ملفات {
    اطبع(ملف)
}

# البحث المتداخل
متغير كل_الملفات = قائمة_ملفات_متداخلة("src"، امتداد: ".s")

# معلومات الملف
متغير معلومات = معلومات_ملف("file.txt")
اطبع("الحجم: " + نص(معلومات.حجم) + " بايت")
اطبع("تاريخ التعديل: " + معلومات.تاريخ_تعديل)
```

---

#### 3.3 File Watching

**مثال:**
```sad
# مراقبة الملفات
متغير مراقب = مراقب_ملفات()

مراقب.مراقبة("src/"، دالة(حدث) {
    إذا (حدث.نوع == "modified") {
        اطبع("تم تعديل: " + حدث.مسار)
    }
})

مراقب.ابدأ()
```

**المدة:** 2 أسابيع

---

### 4. التاريخ والوقت / Date & Time

#### 4.1 Date/Time Parsing & Formatting

**الدوال:**
```sad
# الوقت الحالي
متغير الآن = الآن()
اطبع(الآن)  # 2025-11-29 15:30:45

# تنسيق
متغير منسق = تنسيق_تاريخ(الآن، "DD/MM/YYYY HH:mm:ss")
اطبع(منسق)  # 29/11/2025 15:30:45

# تنسيق عربي
متغير عربي = تنسيق_تاريخ_عربي(الآن)
اطبع(عربي)  # الجمعة، ٢٩ نوفمبر ٢٠٢٥

# التحليل
متغير تاريخ = تحليل_تاريخ("2025-11-29"، "YYYY-MM-DD")

# العمليات
متغير غداً = الآن + أيام(1)
متغير الأسبوع_القادم = الآن + أسابيع(1)
متغير الفرق = غداً - الآن
اطبع("الفرق: " + نص(الفرق.أيام) + " يوم")
```

**التنفيذ:**
```cpp
// استخدام مكتبة مثل date/chrono
namespace Sad::Stdlib::Time {

class DateTime {
public:
    static DateTime now();
    static DateTime parse(const std::string& str, const std::string& format);
    
    std::string format(const std::string& format) const;
    std::string formatArabic() const;
    
    DateTime operator+(const Duration& duration) const;
    DateTime operator-(const Duration& duration) const;
    Duration operator-(const DateTime& other) const;
    
private:
    std::chrono::system_clock::time_point time_;
};

class Duration {
public:
    static Duration days(int n);
    static Duration hours(int n);
    static Duration minutes(int n);
    
    int totalDays() const;
    int totalHours() const;
};

} // namespace
```

**المهام:**
- [ ] DateTime class
- [ ] Parsing/formatting
- [ ] Arabic formatting
- [ ] Duration calculations
- [ ] Timezone support
- [ ] اختبارات

**المدة:** 2 أسابيع

---

### 5. التشفير / Cryptography

#### 5.1 Hashing

**الدوال:**
```sad
# Hash functions
متغير نص = "مرحباً بالعالم"
متغير md5 = hash_md5(نص)
متغير sha256 = hash_sha256(نص)
متغير sha512 = hash_sha512(نص)

اطبع("MD5: " + md5)
اطبع("SHA256: " + sha256)

# Hash ملف
متغير hash_ملف = hash_sha256_file("large_file.dat")
```

---

#### 5.2 Encryption/Decryption

**مثال:**
```sad
# AES encryption
متغير مفتاح = "my_secret_key_32_bytes_long!!!"
متغير نص = "رسالة سرية"

متغير مشفر = aes_encrypt(نص، مفتاح)
متغير مفكوك = aes_decrypt(مشفر، مفتاح)

اطبع("الأصلي: " + نص)
اطبع("المشفر: " + مشفر)
اطبع("المفكوك: " + مفكوك)

# RSA (asymmetric)
متغير keys = rsa_generate_keys(حجم: 2048)
متغير عام = keys.مفتاح_عام
متغير خاص = keys.مفتاح_خاص

متغير مشفر = rsa_encrypt(نص، عام)
متغير مفكوك = rsa_decrypt(مشفر، خاص)
```

**المدة:** 2-3 أسابيع

---

### 6. قواعد البيانات / Databases

#### 6.1 SQLite

**مثال:**
```sad
# فتح قاعدة بيانات
متغير db = sqlite_open("mydatabase.db")

# إنشاء جدول
db.تنفيذ("""
    CREATE TABLE IF NOT EXISTS users (
        id INTEGER PRIMARY KEY،
        name TEXT NOT NULL،
        age INTEGER
    )
""")

# إدراج بيانات
db.تنفيذ("INSERT INTO users (name، age) VALUES (?, ?)"، ["أحمد"، 25])
db.تنفيذ("INSERT INTO users (name، age) VALUES (?, ?)"، ["فاطمة"، 30])

# استعلام
متغير نتائج = db.استعلام("SELECT * FROM users WHERE age > ?"، [20])
لكل صف في نتائج {
    اطبع(صف["name"] + ": " + نص(صف["age"]))
}

# إغلاق
db.أغلق()
```

**التنفيذ:**
```cpp
// استخدام sqlite3
namespace Sad::Stdlib::DB {

class SQLiteConnection {
public:
    explicit SQLiteConnection(const std::string& path);
    ~SQLiteConnection();
    
    void execute(const std::string& sql, 
                const std::vector<Value>& params = {});
    
    std::vector<std::map<std::string, Value>> 
        query(const std::string& sql,
              const std::vector<Value>& params = {});
    
    void close();
    
private:
    sqlite3* db_;
};

} // namespace
```

**المهام:**
- [ ] SQLite integration
- [ ] Query execution
- [ ] Prepared statements
- [ ] Transaction support
- [ ] Error handling
- [ ] اختبارات

**المدة:** 2-3 أسابيع

---

#### 6.2 MongoDB (Optional)

**مثال:**
```sad
# الاتصال
متغير client = mongo_connect("mongodb://localhost:27017")
متغير db = client.قاعدة("mydb")
متغير collection = db.مجموعة("users")

# إدراج
collection.أدرج_واحد({
    "name": "أحمد"،
    "age": 25،
    "city": "الرياض"
})

# استعلام
متغير نتائج = collection.ابحث({"age": {"$gt": 20}})
لكل مستند في نتائج {
    اطبع(مستند["name"])
}
```

**المدة:** 2-3 أسابيع (اختياري)

---

### 7. معالجة الصور / Image Processing

#### 7.1 Basic Operations

**مثال:**
```sad
# تحميل صورة
متغير صورة = تحميل_صورة("photo.jpg")

# معلومات الصورة
اطبع("العرض: " + نص(صورة.عرض))
اطبع("الارتفاع: " + نص(صورة.ارتفاع))

# تحويلات
متغير صغيرة = تصغير_صورة(صورة، عرض: 200، ارتفاع: 200)
متغير مقصوصة = قص_صورة(صورة، س: 100، ص: 100، عرض: 300، ارتفاع: 300)
متغير مدورة = تدوير_صورة(صورة، زاوية: 90)

# فلاتر
متغير رمادية = إلى_رمادي(صورة)
متغير غير_واضحة = تمويه(صورة، قوة: 5)
متغير حادة = زيادة_حدة(صورة)

# حفظ
حفظ_صورة(صغيرة، "thumbnail.jpg"، جودة: 85)
```

**المدة:** 2-3 أسابيع (باستخدام stb_image)

---

### 8. Compression

**مثال:**
```sad
# ضغط/فك ضغط
متغير نص = "نص طويل جداً..." * 1000
متغير مضغوط = ضغط_gzip(نص)
متغير مفكوك = فك_ضغط_gzip(مضغوط)

اطبع("الحجم الأصلي: " + نص(طول(نص)))
اطبع("بعد الضغط: " + نص(طول(مضغوط)))

# ضغط ملف
ضغط_ملف("large_file.txt"، "large_file.txt.gz")
فك_ضغط_ملف("large_file.txt.gz"، "extracted.txt")

# ZIP
أنشئ_zip("archive.zip"، ["file1.txt"، "file2.txt"، "folder/"])
فك_zip("archive.zip"، "extracted/")
```

**المدة:** 1-2 أسبوع

---

## 📊 خطة التنفيذ / Implementation Plan

### الشهر 1: الأساسيات

**الأسبوع 1-2:**
- [ ] HTTP Client
- [ ] JSON Parser

**الأسبوع 3-4:**
- [ ] Date/Time utilities
- [ ] Path manipulation
- [ ] Directory operations

### الشهر 2: المتقدمة

**الأسبوع 1-2:**
- [ ] HTTP Server
- [ ] WebSocket
- [ ] YAML Parser

**الأسبوع 3-4:**
- [ ] Cryptography (hashing, AES)
- [ ] SQLite integration

### الشهر 3: التحسينات والاختبار

**الأسبوع 1-2:**
- [ ] Image processing
- [ ] Compression
- [ ] CSV parser

**الأسبوع 3-4:**
- [ ] اختبارات شاملة
- [ ] توثيق كامل
- [ ] أمثلة

---

## ✅ معايير القبول / Acceptance Criteria

### الوظائف (Features)
- ✅ 50+ دالة جديدة
- ✅ تغطية المجالات الأساسية
- ✅ توافق عبر المنصات

### الجودة (Quality)
- ✅ Test coverage > 80%
- ✅ لا memory leaks
- ✅ معالجة أخطاء شاملة

### التوثيق (Documentation)
- ✅ API reference كامل
- ✅ 100+ أمثلة
- ✅ دروس تعليمية

---

**آخر تحديث:** 29 نوفمبر 2025  
**الحالة:** ✅ خطة جاهزة للتنفيذ
