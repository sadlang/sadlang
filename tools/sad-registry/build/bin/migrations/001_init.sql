-- بسم الله الرحمن الرحيم
-- ═══════════════════════════════════════════════════════════════════════════════
-- مخطط قاعدة بيانات مستودع حزم لغة ص
-- الإصدار: ١.٠.٠
-- التاريخ: فبراير ٢٠٢٦
-- ═══════════════════════════════════════════════════════════════════════════════
--
-- هذا الملف يُنشئ الجداول والفهارس اللازمة لتشغيل مستودع الحزم.
-- يُنفَّذ تلقائياً عند أول تشغيل للخادم.
--
-- الجداول الرئيسية:
--   ١. المستخدمون (users)        - حسابات المطورين
--   ٢. الحزم (packages)          - الحزم المنشورة
--   ٣. الإصدارات (versions)      - إصدارات كل حزمة
--   ٤. التبعيات (dependencies)   - علاقات التبعية بين الحزم
--   ٥. التنزيلات (downloads)     - إحصائيات التنزيل
--   ٦. الكلمات_المفتاحية (keywords) - للبحث السريع
--   ٧. رموز_الوصول (api_tokens)  - رموز المصادقة
--   ٨. المستودعات_المتحدة (federated_registries) - شبكة الاتحاد
--   ٩. فهرس_البحث_العربي (arabic_search_index) - البحث العربي الذكي
-- ═══════════════════════════════════════════════════════════════════════════════

-- تفعيل المفاتيح الخارجية (ضروري في SQLite)
PRAGMA foreign_keys = ON;

-- تفعيل وضع WAL للأداء الأفضل مع القراءة المتزامنة
PRAGMA journal_mode = WAL;

-- ═══════════════════════════════════════════════════════════════════════════════
-- جدول ١: المستخدمون
-- ═══════════════════════════════════════════════════════════════════════════════
-- يحتوي على حسابات المطورين الذين ينشرون الحزم
-- كل مستخدم يمكنه نشر عدة حزم ولكل حزمة مالك واحد أساسي
-- ═══════════════════════════════════════════════════════════════════════════════
CREATE TABLE IF NOT EXISTS users (
    -- المعرف الفريد - يتزايد تلقائياً
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    -- اسم المستخدم - فريد، يُستخدم في تسجيل الدخول والعرض
    -- يدعم الحروف العربية واللاتينية والأرقام والشرطة السفلية
    username TEXT NOT NULL UNIQUE,

    -- البريد الإلكتروني - فريد، يُستخدم للتواصل واسترجاع الحساب
    email TEXT NOT NULL UNIQUE,

    -- كلمة المرور المُشفَّرة بخوارزمية bcrypt
    -- لا نخزن كلمة المرور الأصلية أبداً لأسباب أمنية
    password_hash TEXT NOT NULL,

    -- الاسم الكامل للعرض (يدعم العربية)
    display_name TEXT DEFAULT '',

    -- السيرة الذاتية المختصرة
    bio TEXT DEFAULT '',

    -- رابط الصورة الشخصية
    avatar_url TEXT DEFAULT '',

    -- هل الحساب مفعّل؟ (بعد تأكيد البريد الإلكتروني)
    is_active BOOLEAN DEFAULT 0,

    -- هل المستخدم مشرف على المستودع؟
    is_admin BOOLEAN DEFAULT 0,

    -- عدد الحزم المنشورة (يُحدَّث تلقائياً)
    package_count INTEGER DEFAULT 0,

    -- تاريخ إنشاء الحساب
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,

    -- تاريخ آخر تحديث
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,

    -- تاريخ آخر تسجيل دخول
    last_login_at DATETIME
);

-- فهرس للبحث السريع بالبريد الإلكتروني
CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);

-- فهرس للبحث بتاريخ الإنشاء (للإحصائيات)
CREATE INDEX IF NOT EXISTS idx_users_created ON users(created_at);


-- ═══════════════════════════════════════════════════════════════════════════════
-- جدول ٢: الحزم
-- ═══════════════════════════════════════════════════════════════════════════════
-- الجدول الأساسي - يحتوي على معلومات كل حزمة منشورة
-- كل حزمة لها اسم فريد (بالعربية أو الإنجليزية)
-- يمكن أن تحتوي على عدة إصدارات
-- ═══════════════════════════════════════════════════════════════════════════════
CREATE TABLE IF NOT EXISTS packages (
    -- المعرف الفريد
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    -- اسم الحزمة - فريد عالمياً
    -- يدعم الأسماء العربية مثل "شبكة" و"جيسون"
    -- والأسماء اللاتينية مثل "http" و"json"
    name TEXT NOT NULL UNIQUE,

    -- الاسم المعروض (قد يختلف عن اسم الحزمة)
    -- مثلاً: الحزمة "shabaka" تُعرض كـ "شبكة"
    display_name TEXT DEFAULT '',

    -- الاسم العربي البديل (للبحث ثنائي اللغة)
    -- مثلاً: الحزمة "http" لها اسم عربي "شبكة"
    arabic_name TEXT DEFAULT '',

    -- وصف الحزمة (يدعم العربية)
    description TEXT DEFAULT '',

    -- الوصف العربي
    description_ar TEXT DEFAULT '',

    -- معرف المالك (المطور الذي نشر الحزمة)
    owner_id INTEGER NOT NULL,

    -- ترخيص الحزمة (MIT, Apache-2.0, GPL-3.0, إلخ)
    license TEXT DEFAULT 'MIT',

    -- رابط مستودع الكود المصدري
    repository_url TEXT DEFAULT '',

    -- رابط الصفحة الرئيسية
    homepage_url TEXT DEFAULT '',

    -- رابط التوثيق
    documentation_url TEXT DEFAULT '',

    -- أحدث إصدار مستقر
    latest_version TEXT DEFAULT '0.0.0',

    -- إجمالي عدد التنزيلات (يُحدَّث دورياً)
    total_downloads INTEGER DEFAULT 0,

    -- عدد التنزيلات هذا الشهر
    monthly_downloads INTEGER DEFAULT 0,

    -- عدد التنزيلات هذا الأسبوع
    weekly_downloads INTEGER DEFAULT 0,

    -- تقييم الحزمة (من ٥)
    rating REAL DEFAULT 0.0,

    -- عدد التقييمات
    rating_count INTEGER DEFAULT 0,

    -- هل الحزمة موثّقة (تم التحقق من المطور)؟
    is_verified BOOLEAN DEFAULT 0,

    -- هل الحزمة مميزة (يختارها المشرفون)؟
    is_featured BOOLEAN DEFAULT 0,

    -- هل الحزمة مخفية/محذوفة؟
    is_yanked BOOLEAN DEFAULT 0,

    -- التصنيف الرئيسي
    -- القيم: شبكات، أدوات، واجهات، بيانات، أمان، رياضيات، نظام، اختبار، أخرى
    category TEXT DEFAULT 'أخرى',

    -- تاريخ الإنشاء
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,

    -- تاريخ آخر تحديث
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,

    -- المفتاح الخارجي: المالك
    FOREIGN KEY (owner_id) REFERENCES users(id) ON DELETE RESTRICT
);

-- فهرس للبحث بالاسم
CREATE INDEX IF NOT EXISTS idx_packages_name ON packages(name);

-- فهرس للبحث بالاسم العربي
CREATE INDEX IF NOT EXISTS idx_packages_arabic ON packages(arabic_name);

-- فهرس للتصنيف
CREATE INDEX IF NOT EXISTS idx_packages_category ON packages(category);

-- فهرس للترتيب بالتنزيلات
CREATE INDEX IF NOT EXISTS idx_packages_downloads ON packages(total_downloads DESC);

-- فهرس للترتيب بالتاريخ
CREATE INDEX IF NOT EXISTS idx_packages_created ON packages(created_at DESC);

-- فهرس للحزم المميزة
CREATE INDEX IF NOT EXISTS idx_packages_featured ON packages(is_featured) WHERE is_featured = 1;


-- ═══════════════════════════════════════════════════════════════════════════════
-- جدول ٣: الإصدارات
-- ═══════════════════════════════════════════════════════════════════════════════
-- كل حزمة يمكن أن تحتوي على عدة إصدارات
-- كل إصدار له ملف مضغوط مرتبط به وبصمة تحقق SHA256
-- ═══════════════════════════════════════════════════════════════════════════════
CREATE TABLE IF NOT EXISTS versions (
    -- المعرف الفريد
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    -- معرف الحزمة
    package_id INTEGER NOT NULL,

    -- رقم الإصدار بصيغة الإصدار الدلالي (مثل 1.2.3)
    version TEXT NOT NULL,

    -- الإصدار الرئيسي (Major)
    version_major INTEGER DEFAULT 0,

    -- الإصدار الثانوي (Minor)
    version_minor INTEGER DEFAULT 0,

    -- إصدار الترقيع (Patch)
    version_patch INTEGER DEFAULT 0,

    -- تسمية ما قبل الإصدار (مثل alpha.1, beta.2, rc.1)
    prerelease TEXT DEFAULT '',

    -- ملاحظات الإصدار بالعربية
    release_notes TEXT DEFAULT '',

    -- ملاحظات الإصدار بالإنجليزية
    release_notes_en TEXT DEFAULT '',

    -- بصمة SHA256 لملف الحزمة المضغوط
    -- تُستخدم للتحقق من سلامة التنزيل
    checksum_sha256 TEXT NOT NULL DEFAULT '',

    -- مسار الملف المضغوط في مخزن الكائنات (blob store)
    blob_path TEXT DEFAULT '',

    -- حجم الملف المضغوط بالبايت
    file_size INTEGER DEFAULT 0,

    -- هل هذا الإصدار مسحوب (yanked)؟
    -- الإصدار المسحوب لا يظهر في البحث لكن يمكن تنزيله مباشرة
    is_yanked BOOLEAN DEFAULT 0,

    -- سبب سحب الإصدار (إن وُجد)
    yank_reason TEXT DEFAULT '',

    -- الحد الأدنى لإصدار مترجم لغة ص المطلوب
    min_sad_version TEXT DEFAULT '',

    -- عدد تنزيلات هذا الإصدار
    downloads INTEGER DEFAULT 0,

    -- معرف المستخدم الذي نشر هذا الإصدار
    published_by INTEGER,

    -- تاريخ النشر
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,

    -- المفاتيح الخارجية
    FOREIGN KEY (package_id) REFERENCES packages(id) ON DELETE CASCADE,
    FOREIGN KEY (published_by) REFERENCES users(id),

    -- قيد فريد: لا يمكن أن يكون لحزمة واحدة إصداران بنفس الرقم
    UNIQUE(package_id, version)
);

-- فهرس للبحث بمعرف الحزمة
CREATE INDEX IF NOT EXISTS idx_versions_package ON versions(package_id);

-- فهرس للترتيب بالإصدار
CREATE INDEX IF NOT EXISTS idx_versions_semver ON versions(version_major DESC, version_minor DESC, version_patch DESC);

-- فهرس للبحث بتاريخ النشر
CREATE INDEX IF NOT EXISTS idx_versions_created ON versions(created_at DESC);


-- ═══════════════════════════════════════════════════════════════════════════════
-- جدول ٤: التبعيات
-- ═══════════════════════════════════════════════════════════════════════════════
-- يربط كل إصدار بالحزم التي يعتمد عليها
-- مثلاً: حزمة "خادم_ويب" إصدار 1.0.0 تعتمد على "شبكة" ^2.0.0
-- ═══════════════════════════════════════════════════════════════════════════════
CREATE TABLE IF NOT EXISTS dependencies (
    -- المعرف الفريد
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    -- معرف الإصدار (الذي يعتمد على حزمة أخرى)
    version_id INTEGER NOT NULL,

    -- اسم الحزمة المطلوبة
    dependency_name TEXT NOT NULL,

    -- متطلب الإصدار (مثل ^1.0.0, ~2.3.0, >=1.5.0)
    version_requirement TEXT NOT NULL DEFAULT '*',

    -- نوع التبعية:
    --   'runtime'  = تبعية تشغيل (مطلوبة دائماً)
    --   'dev'      = تبعية تطوير (للاختبار فقط)
    --   'build'    = تبعية بناء (وقت الترجمة فقط)
    --   'optional' = تبعية اختيارية
    dependency_type TEXT DEFAULT 'runtime'
        CHECK(dependency_type IN ('runtime', 'dev', 'build', 'optional')),

    -- المفتاح الخارجي
    FOREIGN KEY (version_id) REFERENCES versions(id) ON DELETE CASCADE
);

-- فهرس للبحث السريع بالإصدار
CREATE INDEX IF NOT EXISTS idx_deps_version ON dependencies(version_id);

-- فهرس للبحث بالاسم (لمعرفة من يعتمد على حزمة معينة)
CREATE INDEX IF NOT EXISTS idx_deps_name ON dependencies(dependency_name);


-- ═══════════════════════════════════════════════════════════════════════════════
-- جدول ٥: التنزيلات
-- ═══════════════════════════════════════════════════════════════════════════════
-- يسجل كل عملية تنزيل لتتبع الإحصائيات
-- يُستخدم لحساب "الأكثر تحميلاً" و"الرائجة هذا الأسبوع"
-- ═══════════════════════════════════════════════════════════════════════════════
CREATE TABLE IF NOT EXISTS downloads (
    -- المعرف الفريد
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    -- معرف الإصدار الذي تم تنزيله
    version_id INTEGER NOT NULL,

    -- عنوان IP للمُنزِّل (مشفّر لحماية الخصوصية)
    ip_hash TEXT DEFAULT '',

    -- وكيل المستخدم (User-Agent) - لتحديد إصدار sad-pkg
    user_agent TEXT DEFAULT '',

    -- تاريخ التنزيل
    downloaded_at DATETIME DEFAULT CURRENT_TIMESTAMP,

    -- المفتاح الخارجي
    FOREIGN KEY (version_id) REFERENCES versions(id) ON DELETE CASCADE
);

-- فهرس زمني للإحصائيات اليومية/الأسبوعية/الشهرية
CREATE INDEX IF NOT EXISTS idx_downloads_date ON downloads(downloaded_at);

-- فهرس بالإصدار
CREATE INDEX IF NOT EXISTS idx_downloads_version ON downloads(version_id);


-- ═══════════════════════════════════════════════════════════════════════════════
-- جدول ٦: الكلمات المفتاحية
-- ═══════════════════════════════════════════════════════════════════════════════
-- كلمات مفتاحية لكل حزمة لتسهيل البحث
-- تدعم كلمات عربية وإنجليزية
-- ═══════════════════════════════════════════════════════════════════════════════
CREATE TABLE IF NOT EXISTS keywords (
    -- المعرف الفريد
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    -- معرف الحزمة
    package_id INTEGER NOT NULL,

    -- الكلمة المفتاحية (عربية أو إنجليزية أو مختلطة)
    keyword TEXT NOT NULL,

    -- المفتاح الخارجي
    FOREIGN KEY (package_id) REFERENCES packages(id) ON DELETE CASCADE
);

-- فهرس للبحث بالكلمة المفتاحية
CREATE INDEX IF NOT EXISTS idx_keywords_word ON keywords(keyword);

-- فهرس بمعرف الحزمة
CREATE INDEX IF NOT EXISTS idx_keywords_package ON keywords(package_id);


-- ═══════════════════════════════════════════════════════════════════════════════
-- جدول ٧: رموز الوصول (API Tokens)
-- ═══════════════════════════════════════════════════════════════════════════════
-- رموز المصادقة للوصول إلى API
-- كل مستخدم يمكنه إنشاء عدة رموز بصلاحيات مختلفة
-- ═══════════════════════════════════════════════════════════════════════════════
CREATE TABLE IF NOT EXISTS api_tokens (
    -- المعرف الفريد
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    -- معرف المستخدم
    user_id INTEGER NOT NULL,

    -- اسم الرمز (لتمييزه عن الرموز الأخرى)
    token_name TEXT NOT NULL DEFAULT 'default',

    -- بصمة الرمز المُشفَّرة (SHA256)
    -- لا نخزن الرمز الأصلي لأسباب أمنية
    token_hash TEXT NOT NULL UNIQUE,

    -- الصلاحيات (قائمة مفصولة بفاصلة)
    -- القيم الممكنة: publish, yank, admin
    scopes TEXT DEFAULT 'publish',

    -- هل الرمز مفعّل؟
    is_active BOOLEAN DEFAULT 1,

    -- تاريخ آخر استخدام
    last_used_at DATETIME,

    -- تاريخ انتهاء الصلاحية (فارغ = لا ينتهي)
    expires_at DATETIME,

    -- تاريخ الإنشاء
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,

    -- المفتاح الخارجي
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
);

-- فهرس للبحث ببصمة الرمز (تسجيل الدخول)
CREATE INDEX IF NOT EXISTS idx_tokens_hash ON api_tokens(token_hash);

-- فهرس بمعرف المستخدم
CREATE INDEX IF NOT EXISTS idx_tokens_user ON api_tokens(user_id);


-- ═══════════════════════════════════════════════════════════════════════════════
-- جدول ٨: المستودعات المتحدة (Federation)
-- ═══════════════════════════════════════════════════════════════════════════════
-- يحتوي على قائمة المستودعات الأخرى المتصلة عبر بروتوكول الاتحاد
-- يسمح بالبحث والتنزيل من مستودعات جامعية أو شركاتية
-- ═══════════════════════════════════════════════════════════════════════════════
CREATE TABLE IF NOT EXISTS federated_registries (
    -- المعرف الفريد
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    -- اسم المستودع المتحد
    name TEXT NOT NULL UNIQUE,

    -- عنوان URL الأساسي
    base_url TEXT NOT NULL,

    -- وصف المستودع
    description TEXT DEFAULT '',

    -- مفتاح التحقق العمومي (للتوقيع الرقمي)
    public_key TEXT DEFAULT '',

    -- مستوى الثقة (0-100)
    -- 100 = موثوق بالكامل (المستودع الرسمي)
    -- 50+ = موثوق (جامعات، شركات معروفة)
    -- أقل من 50 = غير موثوق (يحتاج مراجعة يدوية)
    trust_level INTEGER DEFAULT 0,

    -- هل المستودع متصل حالياً؟
    is_online BOOLEAN DEFAULT 0,

    -- تاريخ آخر مزامنة ناجحة
    last_sync_at DATETIME,

    -- عدد الحزم في هذا المستودع
    package_count INTEGER DEFAULT 0,

    -- تاريخ الإضافة
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);


-- ═══════════════════════════════════════════════════════════════════════════════
-- جدول ٩: فهرس البحث العربي
-- ═══════════════════════════════════════════════════════════════════════════════
-- جدول خاص بالبحث النصي العربي الذكي
-- يحتوي على الجذور والمترادفات والترجمات
-- يُستخدم لتحسين نتائج البحث للمستخدمين العرب
-- ═══════════════════════════════════════════════════════════════════════════════
CREATE TABLE IF NOT EXISTS arabic_search_index (
    -- المعرف الفريد
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    -- معرف الحزمة
    package_id INTEGER NOT NULL,

    -- الكلمة الأصلية
    original_word TEXT NOT NULL,

    -- الجذر العربي (مثل: "كتب" هو جذر "كاتب"، "مكتوب"، "كتاب")
    arabic_root TEXT DEFAULT '',

    -- الكلمة بدون تشكيل
    normalized_word TEXT NOT NULL,

    -- الترجمة الإنجليزية (إن وُجدت)
    english_equivalent TEXT DEFAULT '',

    -- المرادفات (مفصولة بفاصلة)
    synonyms TEXT DEFAULT '',

    -- نوع الكلمة: name, description, keyword, author
    word_source TEXT DEFAULT 'description',

    -- المفتاح الخارجي
    FOREIGN KEY (package_id) REFERENCES packages(id) ON DELETE CASCADE
);

-- فهرس للبحث بالكلمة المعيارية (بدون تشكيل)
CREATE INDEX IF NOT EXISTS idx_arabic_normalized ON arabic_search_index(normalized_word);

-- فهرس للبحث بالجذر العربي
CREATE INDEX IF NOT EXISTS idx_arabic_root ON arabic_search_index(arabic_root);

-- فهرس للبحث بالترجمة الإنجليزية
CREATE INDEX IF NOT EXISTS idx_arabic_english ON arabic_search_index(english_equivalent);


-- ═══════════════════════════════════════════════════════════════════════════════
-- جدول ١٠: سجل المراجعة (Audit Log)
-- ═══════════════════════════════════════════════════════════════════════════════
-- يسجل جميع العمليات الحساسة للمراجعة الأمنية
-- ═══════════════════════════════════════════════════════════════════════════════
CREATE TABLE IF NOT EXISTS audit_log (
    -- المعرف الفريد
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    -- معرف المستخدم (فارغ للعمليات المجهولة)
    user_id INTEGER,

    -- نوع العملية: publish, yank, login, register, delete, admin
    action TEXT NOT NULL,

    -- تفاصيل العملية بصيغة JSON
    details TEXT DEFAULT '{}',

    -- عنوان IP
    ip_address TEXT DEFAULT '',

    -- وكيل المستخدم
    user_agent TEXT DEFAULT '',

    -- التاريخ
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- فهرس زمني
CREATE INDEX IF NOT EXISTS idx_audit_date ON audit_log(created_at DESC);

-- فهرس بالمستخدم
CREATE INDEX IF NOT EXISTS idx_audit_user ON audit_log(user_id);


-- ═══════════════════════════════════════════════════════════════════════════════
-- جدول البحث النصي الكامل (FTS5)
-- ═══════════════════════════════════════════════════════════════════════════════
-- يستخدم ميزة FTS5 في SQLite للبحث النصي الكامل السريع
-- يدعم البحث بالعربية والإنجليزية
-- ═══════════════════════════════════════════════════════════════════════════════
CREATE VIRTUAL TABLE IF NOT EXISTS packages_fts USING fts5(
    -- اسم الحزمة
    name,
    -- الاسم العربي
    arabic_name,
    -- الوصف
    description,
    -- الوصف العربي
    description_ar,
    -- الكلمات المفتاحية (مفصولة بمسافات)
    keywords,
    -- ربط بجدول الحزم
    content='packages',
    content_rowid='id'
);

-- مشغّلات لتحديث فهرس FTS تلقائياً
-- ═══════════════════════════════════════════════════════════════════════════════

-- عند إضافة حزمة جديدة → إضافتها للفهرس
CREATE TRIGGER IF NOT EXISTS packages_fts_insert AFTER INSERT ON packages BEGIN
    INSERT INTO packages_fts(rowid, name, arabic_name, description, description_ar, keywords)
    VALUES (new.id, new.name, new.arabic_name, new.description, new.description_ar, '');
END;

-- عند حذف حزمة → إزالتها من الفهرس
CREATE TRIGGER IF NOT EXISTS packages_fts_delete AFTER DELETE ON packages BEGIN
    INSERT INTO packages_fts(packages_fts, rowid, name, arabic_name, description, description_ar, keywords)
    VALUES ('delete', old.id, old.name, old.arabic_name, old.description, old.description_ar, '');
END;

-- عند تحديث حزمة → تحديث الفهرس
CREATE TRIGGER IF NOT EXISTS packages_fts_update AFTER UPDATE ON packages BEGIN
    INSERT INTO packages_fts(packages_fts, rowid, name, arabic_name, description, description_ar, keywords)
    VALUES ('delete', old.id, old.name, old.arabic_name, old.description, old.description_ar, '');
    INSERT INTO packages_fts(rowid, name, arabic_name, description, description_ar, keywords)
    VALUES (new.id, new.name, new.arabic_name, new.description, new.description_ar, '');
END;


-- ═══════════════════════════════════════════════════════════════════════════════
-- بيانات أولية
-- ═══════════════════════════════════════════════════════════════════════════════

-- إنشاء حساب المشرف الافتراضي
-- كلمة المرور: admin (يجب تغييرها فوراً!)
INSERT OR IGNORE INTO users (id, username, email, password_hash, display_name, is_active, is_admin)
VALUES (1, 'admin', 'admin@sadlang.org',
        '$2b$12$placeholder_hash_change_immediately',
        'مشرف المستودع', 1, 1);

-- إضافة المستودع الرسمي كمستودع متحد
INSERT OR IGNORE INTO federated_registries (name, base_url, description, trust_level, is_online)
VALUES ('الرسمي', 'https://packages.sadlang.org', 'المستودع الرسمي لحزم لغة ص', 100, 1);

-- ═══════════════════════════════════════════════════════════════════════════════
-- إصدار المخطط
-- ═══════════════════════════════════════════════════════════════════════════════
CREATE TABLE IF NOT EXISTS schema_version (
    version INTEGER PRIMARY KEY,
    applied_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    description TEXT
);

INSERT OR IGNORE INTO schema_version (version, description)
VALUES (1, 'المخطط الأولي - إنشاء جميع الجداول');
