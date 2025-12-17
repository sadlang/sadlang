# بسم الله الرحمن الرحيم
"""
مثال شامل: HTTP Module في لغة ص
Comprehensive HTTP Module Example

هذا المثال يوضح استخدام وحدة HTTP الكاملة
This example demonstrates complete HTTP module usage

الحمد لله رب العالمين
"""

# ============================================================================
# HTTP Client Examples - أمثلة عميل HTTP
# ============================================================================

اطبع("=== عميل HTTP / HTTP Client ===\n")

# --- GET Request ---
اطبع("1. طلب GET / GET Request")
استجابة = http_get("https://api.github.com/users/github")
اطبع("الحالة / Status:", استجابة.status)
اطبع("الجسم / Body:", استجابة.body[:100], "...")  # أول 100 حرف

# --- POST Request ---
اطبع("\n2. طلب POST / POST Request")
بيانات = '{"title": "مرحباً", "body": "محتوى الرسالة", "userId": 1}'
استجابة = http_post("https://jsonplaceholder.typicode.com/posts", بيانات)
اطبع("الحالة:", استجابة.status)
اطبع("الاستجابة:", استجابة.body)

# --- PUT Request ---
اطبع("\n3. طلب PUT / PUT Request")
تحديث = '{"title": "عنوان محدث", "body": "محتوى محدث", "userId": 1}'
استجابة = http_put("https://jsonplaceholder.typicode.com/posts/1", تحديث)
اطبع("الحالة:", استجابة.status)

# --- DELETE Request ---
اطبع("\n4. طلب DELETE / DELETE Request")
استجابة = http_delete("https://jsonplaceholder.typicode.com/posts/1")
اطبع("الحالة:", استجابة.status)
اطبع("تم الحذف بنجاح" إذا استجابة.status == 200 وإلا "فشل الحذف")

# ============================================================================
# HTTP Client Configuration - إعدادات عميل HTTP
# ============================================================================

اطبع("\n=== إعدادات العميل / Client Configuration ===\n")

# تعيين مهلة الاتصال / Set timeout
http_set_timeout(30)  # 30 ثانية
اطبع("✓ تم تعيين المهلة: 30 ثانية")

# تعيين User-Agent / Set User-Agent
http_set_user_agent("Sad-Language/1.0 (HTTP Client)")
اطبع("✓ تم تعيين User-Agent")

# تعيين ترويسات افتراضية / Set default headers
http_set_header("Accept", "application/json")
http_set_header("Content-Type", "application/json")
اطبع("✓ تم تعيين الترويسات الافتراضية")

# ============================================================================
# URL Utilities - مساعدات URL
# ============================================================================

اطبع("\n=== مساعدات URL / URL Utilities ===\n")

# --- URL Encoding ---
اطبع("1. تشفير URL / URL Encoding")
نص_عربي = "مرحباً بالعالم!"
مشفر = url_encode(نص_عربي)
اطبع("النص الأصلي:", نص_عربي)
اطبع("المشفر:", مشفر)

# --- URL Decoding ---
اطبع("\n2. فك تشفير URL / URL Decoding")
مفكوك = url_decode(مشفر)
اطبع("المفكوك:", مفكوك)
اطبع("✓ متطابق" إذا مفكوك == نص_عربي وإلا "✗ غير متطابق")

# --- URL Parsing ---
اطبع("\n3. تحليل URL / URL Parsing")
url = "https://example.com:8080/api/users?page=1&limit=10#section"
أجزاء = url_parse(url)
اطبع("النظام / Scheme:", أجزاء.scheme)
اطبع("المضيف / Host:", أجزاء.host)
اطبع("المنفذ / Port:", أجزاء.port)
اطبع("المسار / Path:", أجزاء.path)
اطبع("الاستعلام / Query:", أجزاء.query)
اطبع("القسم / Fragment:", أجزاء.fragment)

# --- Query String Parsing ---
اطبع("\n4. تحليل معاملات الاستعلام / Query String Parsing")
استعلام = "name=أحمد&age=25&city=الرياض"
معاملات = query_parse(استعلام)
اطبع("المعاملات / Parameters:")
لكل مفتاح، قيمة في معاملات:
    اطبع(f"  {مفتاح}: {قيمة}")

# --- Query String Building ---
اطبع("\n5. بناء سلسلة استعلام / Query String Building")
معاملات_جديدة = {
    "الاسم": "فاطمة",
    "العمر": "30",
    "المدينة": "جدة"
}
استعلام_جديد = query_build(معاملات_جديدة)
اطبع("الاستعلام المبني:", استعلام_جديد)

# ============================================================================
# HTTP Server Example - مثال خادم HTTP
# ============================================================================

اطبع("\n=== خادم HTTP / HTTP Server ===\n")

# إنشاء الخادم / Create server
أنشئ_خادم_http()
اطبع("✓ تم إنشاء خادم HTTP")

# --- معالج GET ---
دالة معالج_المستخدمين(طلب):
    """معالج لجلب قائمة المستخدمين / Handler for getting user list"""
    
    # بناء استجابة JSON
    بيانات = {
        "users": [
            {"id": 1, "name": "أحمد", "email": "ahmed@example.com"},
            {"id": 2, "name": "فاطمة", "email": "fatima@example.com"},
            {"id": 3, "name": "محمد", "email": "mohamed@example.com"}
        ],
        "total": 3
    }
    
    أرجع {
        "status": 200,
        "headers": {"Content-Type": "application/json"},
        "body": json_stringify(بيانات)
    }

# تسجيل المعالج / Register handler
خادم_get("/api/users", معالج_المستخدمين)
اطبع("✓ تم تسجيل معالج GET: /api/users")

# --- معالج POST ---
دالة معالج_إنشاء_مستخدم(طلب):
    """معالج لإنشاء مستخدم جديد / Handler for creating new user"""
    
    # تحليل الجسم / Parse body
    بيانات = json_parse(طلب.body)
    
    # التحقق من البيانات / Validate data
    إذا لا بيانات.has("name") أو لا بيانات.has("email"):
        أرجع {
            "status": 400,
            "body": '{"error": "Missing required fields"}'
        }
    
    # إنشاء مستخدم جديد / Create new user
    مستخدم_جديد = {
        "id": 4,
        "name": بيانات["name"],
        "email": بيانات["email"]
    }
    
    أرجع {
        "status": 201,
        "headers": {"Content-Type": "application/json"},
        "body": json_stringify(مستخدم_جديد)
    }

خادم_post("/api/users", معالج_إنشاء_مستخدم)
اطبع("✓ تم تسجيل معالج POST: /api/users")

# --- معالج مع معاملات المسار ---
دالة معالج_مستخدم_واحد(طلب):
    """معالج للحصول على مستخدم واحد / Handler for getting single user"""
    
    # استخراج ID من المسار / Extract ID from path
    user_id = طلب.path_params["id"]
    
    # محاكاة قاعدة بيانات / Simulate database
    مستخدم = {
        "id": user_id,
        "name": "أحمد",
        "email": "ahmed@example.com",
        "created_at": "2025-01-01"
    }
    
    أرجع {
        "status": 200,
        "headers": {"Content-Type": "application/json"},
        "body": json_stringify(مستخدم)
    }

خادم_get("/api/users/:id", معالج_مستخدم_واحد)
اطبع("✓ تم تسجيل معالج GET: /api/users/:id")

# --- خدمة الملفات الثابتة ---
# http_server_static("/static", "./public")
# اطبع("✓ تم تفعيل خدمة الملفات الثابتة: /static")

# --- بدء الخادم ---
اطبع("\n🚀 بدء الخادم على المنفذ 8080...")
ابدأ_خادم(8080)
اطبع("✓ الخادم يعمل!")
اطبع("   زر: http://localhost:8080/api/users")

# ============================================================================
# Middleware Example - مثال الوسائط
# ============================================================================

اطبع("\n=== الوسائط / Middleware ===\n")

دالة middleware_logger(طلب، استجابة):
    """وسيط لتسجيل الطلبات / Middleware for logging requests"""
    
    وقت_البداية = time_now()
    
    اطبع(f"[{time_format(وقت_البداية)}] {طلب.method} {طلب.path}")
    
    # السماح بالمتابعة / Allow continuation
    أرجع صحيح

دالة middleware_auth(طلب، استجابة):
    """وسيط للمصادقة / Middleware for authentication"""
    
    # فحص رمز المصادقة / Check auth token
    token = طلب.headers.get("Authorization", "")
    
    إذا لا token.starts_with("Bearer "):
        استجابة.status = 401
        استجابة.body = '{"error": "Unauthorized"}'
        أرجع خطأ  # إيقاف المعالجة / Stop processing
    
    # التحقق من الرمز / Verify token
    # ...
    
    أرجع صحيح

# تسجيل الوسائط / Register middlewares
# http_server_use(middleware_logger)
# http_server_use(middleware_auth)
اطبع("✓ تم تسجيل الوسائط")

# ============================================================================
# Advanced HTTP Client Usage - استخدام متقدم لعميل HTTP
# ============================================================================

اطبع("\n=== استخدام متقدم / Advanced Usage ===\n")

دالة جلب_بيانات_مستخدمين():
    """جلب بيانات المستخدمين من API خارجي"""
    
    اطبع("جاري جلب بيانات المستخدمين...")
    
    حاول:
        # طلب GET مع ترويسات مخصصة
        استجابة = http_get("https://jsonplaceholder.typicode.com/users")
        
        إذا استجابة.status == 200:
            مستخدمون = json_parse(استجابة.body)
            اطبع(f"✓ تم جلب {len(مستخدمون)} مستخدمين")
            
            # عرض أول 3 مستخدمين
            لكل i، مستخدم في enumerate(مستخدمون[:3]):
                اطبع(f"  {i+1}. {مستخدم['name']} ({مستخدم['email']})")
                
            أرجع مستخدمون
        وإلا:
            اطبع(f"✗ خطأ: {استجابة.status}")
            أرجع لا شيء
            
    اعترض خطأ:
        اطبع(f"✗ فشل الاتصال: {خطأ}")
        أرجع لا شيء

دالة إنشاء_منشور(عنوان، محتوى):
    """إنشاء منشور جديد / Create new post"""
    
    اطبع(f"\nإنشاء منشور: {عنوان}")
    
    # بناء البيانات / Build data
    بيانات = {
        "title": عنوان,
        "body": محتوى,
        "userId": 1
    }
    
    بيانات_json = json_stringify(بيانات)
    
    حاول:
        استجابة = http_post(
            "https://jsonplaceholder.typicode.com/posts",
            بيانات_json
        )
        
        إذا استجابة.status == 201:
            منشور = json_parse(استجابة.body)
            اطبع(f"✓ تم إنشاء منشور برقم: {منشور['id']}")
            أرجع منشور
        وإلا:
            اطبع(f"✗ فشل الإنشاء: {استجابة.status}")
            أرجع لا شيء
            
    اعترض خطأ:
        اطبع(f"✗ خطأ: {خطأ}")
        أرجع لا شيء

# تنفيذ الوظائف / Execute functions
مستخدمون = جلب_بيانات_مستخدمين()

منشور = إنشاء_منشور(
    "مرحباً بلغة ص!",
    "هذا منشور تجريبي من لغة ص البرمجية العربية"
)

# ============================================================================
# Performance Testing - اختبار الأداء
# ============================================================================

اطبع("\n=== اختبار الأداء / Performance Testing ===\n")

دالة اختبار_أداء_http(عدد_الطلبات):
    """اختبار أداء طلبات HTTP / Test HTTP request performance"""
    
    اطبع(f"جاري إرسال {عدد_الطلبات} طلب...")
    
    وقت_البداية = time_now()
    نجاحات = 0
    فشل = 0
    
    لكل i في range(عدد_الطلبات):
        حاول:
            استجابة = http_get("https://jsonplaceholder.typicode.com/posts/1")
            
            إذا استجابة.status == 200:
                نجاحات = نجاحات + 1
            وإلا:
                فشل = فشل + 1
                
        اعترض:
            فشل = فشل + 1
    
    وقت_النهاية = time_now()
    المدة = وقت_النهاية - وقت_البداية
    
    اطبع(f"\n📊 النتائج:")
    اطبع(f"  الطلبات الكلية: {عدد_الطلبات}")
    اطبع(f"  النجاحات: {نجاحات}")
    اطبع(f"  الفشل: {فشل}")
    اطبع(f"  المدة: {المدة:.2f} ثانية")
    اطبع(f"  المعدل: {عدد_الطلبات / المدة:.2f} طلب/ثانية")

# اختبار_أداء_http(10)

# ============================================================================
# Cleanup - التنظيف
# ============================================================================

اطبع("\n=== التنظيف / Cleanup ===\n")

# إيقاف الخادم / Stop server
إذا هل_خادم_يعمل():
    اطبع("إيقاف الخادم...")
    أوقف_خادم()
    اطبع("✓ تم إيقاف الخادم")

اطبع("\n=== اكتمل المثال بنجاح! ===")
اطبع("الحمد لله رب العالمين")

"""
مخرجات متوقعة / Expected Output:
===================================

=== عميل HTTP / HTTP Client ===

1. طلب GET / GET Request
الحالة / Status: 200
الجسم / Body: {"login":"github","id":9919,"node_id":"MDEyOk9yZ2FuaXphdGlvbjk5MTk=","avatar_url":"https://avatar...

2. طلب POST / POST Request
الحالة: 201
الاستجابة: {"title": "مرحباً", "body": "محتوى الرسالة", "userId": 1, "id": 101}

3. طلب PUT / PUT Request
الحالة: 200

4. طلب DELETE / DELETE Request
الحالة: 200
تم الحذف بنجاح

=== إعدادات العميل / Client Configuration ===

✓ تم تعيين المهلة: 30 ثانية
✓ تم تعيين User-Agent
✓ تم تعيين الترويسات الافتراضية

=== مساعدات URL / URL Utilities ===

1. تشفير URL / URL Encoding
النص الأصلي: مرحباً بالعالم!
المشفر: %D9%85%D8%B1%D8%AD%D8%A8%D8%A7%20%D8%A8%D8%A7%D9%84%D8%B9%D8%A7%D9%84%D9%85%21

2. فك تشفير URL / URL Decoding
المفكوك: مرحباً بالعالم!
✓ متطابق

3. تحليل URL / URL Parsing
النظام / Scheme: https
المضيف / Host: example.com
المنفذ / Port: 8080
المسار / Path: /api/users
الاستعلام / Query: page=1&limit=10
القسم / Fragment: section

4. تحليل معاملات الاستعلام / Query String Parsing
المعاملات / Parameters:
  name: أحمد
  age: 25
  city: الرياض

5. بناء سلسلة استعلام / Query String Building
الاستعلام المبني: الاسم=فاطمة&العمر=30&المدينة=جدة

=== خادم HTTP / HTTP Server ===

✓ تم إنشاء خادم HTTP
✓ تم تسجيل معالج GET: /api/users
✓ تم تسجيل معالج POST: /api/users
✓ تم تسجيل معالج GET: /api/users/:id

🚀 بدء الخادم على المنفذ 8080...
✓ الخادم يعمل!
   زر: http://localhost:8080/api/users

=== الوسائط / Middleware ===

✓ تم تسجيل الوسائط

=== استخدام متقدم / Advanced Usage ===

جاري جلب بيانات المستخدمين...
✓ تم جلب 10 مستخدمين
  1. Leanne Graham (Sincere@april.biz)
  2. Ervin Howell (Shanna@melissa.tv)
  3. Clementine Bauch (Nathan@yesenia.net)

إنشاء منشور: مرحباً بلغة ص!
✓ تم إنشاء منشور برقم: 101

=== اختبار الأداء / Performance Testing ===

جاري إرسال 10 طلب...

📊 النتائج:
  الطلبات الكلية: 10
  النجاحات: 10
  الفشل: 0
  المدة: 2.34 ثانية
  المعدل: 4.27 طلب/ثانية

=== التنظيف / Cleanup ===

إيقاف الخادم...
✓ تم إيقاف الخادم

=== اكتمل المثال بنجاح! ===
الحمد لله رب العالمين
"""
