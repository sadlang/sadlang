# بسم الله الرحمن الرحيم
# وحدة HTTP - HTTP Module
# مكتبة HTTP لإرسال واستقبال الطلبات

# ==========================================
# عميل HTTP / HTTP Client
# ==========================================

دالة عميل_http_جديد() {
    # إنشاء عميل HTTP جديد
    عودة __sad_http_client_new()
}

دالة عميل_http_get(عميل, رابط) {
    # إرسال طلب GET
    عودة __sad_http_client_get(عميل, رابط)
}

دالة عميل_http_post(عميل, رابط, جسم) {
    # إرسال طلب POST
    عودة __sad_http_client_post(عميل, رابط, جسم)
}

دالة عميل_http_put(عميل, رابط, جسم) {
    # إرسال طلب PUT
    عودة __sad_http_client_put(عميل, رابط, جسم)
}

دالة عميل_http_delete(عميل, رابط) {
    # إرسال طلب DELETE
    عودة __sad_http_client_delete(عميل, رابط)
}

دالة عميل_http_patch(عميل, رابط, جسم) {
    # إرسال طلب PATCH
    عودة __sad_http_client_patch(عميل, رابط, جسم)
}

دالة عميل_http_set_base_url(عميل, رابط_أساسي) {
    # تعيين الرابط الأساسي
    __sad_http_client_set_base_url(عميل, رابط_أساسي)
}

دالة عميل_http_set_header(عميل, مفتاح, قيمة) {
    # تعيين رأس افتراضي
    __sad_http_client_set_header(عميل, مفتاح, قيمة)
}

دالة عميل_http_set_timeout(عميل, مهلة) {
    # تعيين المهلة الزمنية
    __sad_http_client_set_timeout(عميل, مهلة)
}

دالة عميل_http_set_bearer_token(عميل, رمز) {
    # تعيين رمز المصادقة
    __sad_http_client_set_bearer_token(عميل, رمز)
}

دالة عميل_http_last_error(عميل) {
    # الحصول على آخر خطأ
    عودة __sad_http_client_last_error(عميل)
}

دالة عميل_http_is_ok(عميل) {
    # التحقق من نجاح آخر عملية
    عودة __sad_http_client_is_ok(عميل)
}

# ==========================================
# رد HTTP / HTTP Response
# ==========================================

دالة رد_http_status(رد) {
    # الحصول على رمز الحالة
    عودة __sad_http_response_status(رد)
}

دالة رد_http_body(رد) {
    # الحصول على جسم الرد
    عودة __sad_http_response_body(رد)
}

دالة رد_http_header(رد, مفتاح) {
    # الحصول على رأس محدد
    عودة __sad_http_response_header(رد, مفتاح)
}

دالة رد_http_is_success(رد) {
    # التحقق من نجاح الطلب
    عودة __sad_http_response_is_success(رد)
}

دالة رد_http_set_status(رد, حالة) {
    # تعيين رمز الحالة (للخادم)
    __sad_http_response_set_status(رد, حالة)
}

دالة رد_http_set_body(رد, جسم) {
    # تعيين جسم الرد (للخادم)
    __sad_http_response_set_body(رد, جسم)
}

دالة رد_http_set_json(رد, json) {
    # تعيين رد JSON (للخادم)
    __sad_http_response_set_json(رد, json)
}

دالة رد_http_set_html(رد, html) {
    # تعيين رد HTML (للخادم)
    __sad_http_response_set_html(رد, html)
}

# ==========================================
# خادم HTTP / HTTP Server
# ==========================================

دالة خادم_http_جديد(منفذ) {
    # إنشاء خادم HTTP جديد
    عودة __sad_http_server_new(منفذ)
}

دالة خادم_http_get(خادم, مسار, معالج) {
    # تسجيل مسار GET
    __sad_http_server_get(خادم, مسار, معالج)
}

دالة خادم_http_post(خادم, مسار, معالج) {
    # تسجيل مسار POST
    __sad_http_server_post(خادم, مسار, معالج)
}

دالة خادم_http_put(خادم, مسار, معالج) {
    # تسجيل مسار PUT
    __sad_http_server_put(خادم, مسار, معالج)
}

دالة خادم_http_delete(خادم, مسار, معالج) {
    # تسجيل مسار DELETE
    __sad_http_server_delete(خادم, مسار, معالج)
}

دالة خادم_http_listen(خادم) {
    # تشغيل الخادم
    __sad_http_server_listen(خادم)
}

دالة خادم_http_stop(خادم) {
    # إيقاف الخادم
    __sad_http_server_stop(خادم)
}

دالة خادم_http_enable_cors(خادم, أصل) {
    # تفعيل CORS
    __sad_http_server_enable_cors(خادم, أصل)
}

# ==========================================
# طلب HTTP (للمعالجات) / HTTP Request
# ==========================================

دالة طلب_http_method(طلب) {
    # الحصول على طريقة الطلب
    عودة __sad_http_request_method(طلب)
}

دالة طلب_http_path(طلب) {
    # الحصول على مسار الطلب
    عودة __sad_http_request_path(طلب)
}

دالة طلب_http_body(طلب) {
    # الحصول على جسم الطلب
    عودة __sad_http_request_body(طلب)
}

دالة طلب_http_header(طلب, مفتاح) {
    # الحصول على رأس من الطلب
    عودة __sad_http_request_header(طلب, مفتاح)
}

دالة طلب_http_query_param(طلب, مفتاح) {
    # الحصول على معامل استعلام
    عودة __sad_http_request_query_param(طلب, مفتاح)
}

# ==========================================
# دوال مساعدة / Helper Functions
# ==========================================

# HTTP Status Codes
متغير HTTP_OK = 200
متغير HTTP_CREATED = 201
متغير HTTP_NO_CONTENT = 204
متغير HTTP_BAD_REQUEST = 400
متغير HTTP_UNAUTHORIZED = 401
متغير HTTP_FORBIDDEN = 403
متغير HTTP_NOT_FOUND = 404
متغير HTTP_INTERNAL_ERROR = 500

# دوال مختصرة / Shorthand functions
دالة http_get(رابط) {
    # GET مختصر
    عميل = عميل_http_جديد()
    رد = عميل_http_get(عميل, رابط)
    عودة رد
}

دالة http_post(رابط, بيانات) {
    # POST مختصر
    عميل = عميل_http_جديد()
    رد = عميل_http_post(عميل, رابط, بيانات)
    عودة رد
}

# تصدير الدوال
تصدير عميل_http_جديد
تصدير عميل_http_get
تصدير عميل_http_post
تصدير عميل_http_put
تصدير عميل_http_delete
تصدير عميل_http_patch
تصدير عميل_http_set_base_url
تصدير عميل_http_set_header
تصدير عميل_http_set_timeout
تصدير عميل_http_set_bearer_token

تصدير رد_http_status
تصدير رد_http_body
تصدير رد_http_header
تصدير رد_http_is_success
تصدير رد_http_set_status
تصدير رد_http_set_body
تصدير رد_http_set_json
تصدير رد_http_set_html

تصدير خادم_http_جديد
تصدير خادم_http_get
تصدير خادم_http_post
تصدير خادم_http_put
تصدير خادم_http_delete
تصدير خادم_http_listen
تصدير خادم_http_stop
تصدير خادم_http_enable_cors

تصدير طلب_http_method
تصدير طلب_http_path
تصدير طلب_http_body
تصدير طلب_http_header
تصدير طلب_http_query_param

تصدير http_get
تصدير http_post

تصدير HTTP_OK
تصدير HTTP_CREATED
تصدير HTTP_NO_CONTENT
تصدير HTTP_BAD_REQUEST
تصدير HTTP_UNAUTHORIZED
تصدير HTTP_FORBIDDEN
تصدير HTTP_NOT_FOUND
تصدير HTTP_INTERNAL_ERROR
