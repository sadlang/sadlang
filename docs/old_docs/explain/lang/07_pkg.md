# مدير الحزم (Package Manager - PKG)

## ما هو مدير الحزم؟

مدير الحزم (sad-pkg) هو أداة سطر الأوامر المسؤولة عن إدارة حزم ومكتبات لغة ص. يسمح للمطورين بمشاركة الكود واستخدام مكتبات كتبها آخرون بسهولة. بدلاً من نسخ الملفات يدوياً، يمكن للمطور ببساطة كتابة أمر واحد لتثبيت أي مكتبة من السجل المركزي. مدير الحزم يتعامل تلقائياً مع التبعيات، أي المكتبات التي تحتاجها المكتبة التي تريد استخدامها.

يشبه sad-pkg أدوات إدارة الحزم الشهيرة مثل npm في JavaScript أو pip في Python أو Cargo في Rust. الهدف هو توفير بيئة غنية بالمكتبات الجاهزة التي يمكن للمطورين استخدامها فوراً، مما يسرع عملية التطوير ويتجنب إعادة اختراع العجلة.

## بنية مدير الحزم

يتواجد مدير الحزم في المجلد `tools/pkg/` ويتكون من:

```
tools/pkg/
├── cli.cpp                 # واجهة سطر الأوامر
├── package.h               # تعريف بنية الحزمة
├── toml_parser.h           # محلل ملفات التكوين
├── dependency_resolver.h   # حل التبعيات
├── registry_client.h       # الاتصال بالسجل المركزي
├── tests/                  # اختبارات
└── README.md               # التوثيق
```

## الأوامر المتاحة

### تهيئة حزمة جديدة (init)

```bash
sad-pkg init
```

يُنشئ مشروعاً جديداً بالهيكل التالي:

```
my_package/
├── sad.toml          # ملف تكوين الحزمة
└── src/
    └── main.s        # الملف الرئيسي
```

```cpp
// من cli.cpp
int cmd_init() {
    print_header("Initializing new Sad package...");

    // التحقق من وجود sad.toml
    if (std::filesystem::exists("sad.toml")) {
        print_error("sad.toml already exists in current directory");
        return 1;
    }

    // الحصول على معلومات الحزمة
    std::string name, version_str, description, author;
    
    std::cout << "Package name (default: my_package): ";
    std::getline(std::cin, name);
    if (name.empty()) name = "my_package";

    std::cout << "Version (default: 0.1.0): ";
    std::getline(std::cin, version_str);
    if (version_str.empty()) version_str = "0.1.0";

    // إنشاء الحزمة
    Package pkg;
    pkg.name = name;
    pkg.version = Version::parse(version_str);
    pkg.main_file = "src/main.s";
    
    // حفظ التكوين
    pkg.save_to_file("sad.toml");

    // إنشاء هيكل المجلدات
    std::filesystem::create_directories("src");
    
    // إنشاء main.s
    std::ofstream main_file("src/main.s");
    main_file << "// بسم الله الرحمن الرحيم\n";
    main_file << "دالة رئيسية() {\n";
    main_file << "    اطبع(\"مرحباً من " << name << "!\");\n";
    main_file << "    أرجع 0;\n";
    main_file << "}\n";
    
    print_success("Package initialized successfully!");
    return 0;
}
```

### تثبيت التبعيات (install)

```bash
# تثبيت جميع التبعيات من sad.toml
sad-pkg install

# تثبيت حزمة محددة
sad-pkg install http
```

### إضافة تبعية (add)

```bash
sad-pkg add http ^1.0.0
sad-pkg add json ">=2.0.0"
```

يُضيف التبعية إلى sad.toml ويُثبِّتها:

```toml
[dependencies]
http = "^1.0.0"
json = ">=2.0.0"
```

### إزالة تبعية (remove)

```bash
sad-pkg remove http
```

### تحديث التبعيات (update)

```bash
sad-pkg update
```

### نشر حزمة (publish)

```bash
sad-pkg publish
```

### البحث عن حزم (search)

```bash
sad-pkg search http
```

### معلومات حزمة (info)

```bash
sad-pkg info http
```

### قائمة الحزم المُثبَّتة (list)

```bash
sad-pkg list
```

## ملف التكوين (sad.toml)

كل مشروع لغة ص يحتوي على ملف `sad.toml` يصف الحزمة:

```toml
[package]
name = "my_app"
version = "1.0.0"
description = "تطبيقي الأول بلغة ص"
authors = ["أحمد محمد <ahmed@example.com>"]
license = "MIT"
main = "src/main.s"

[dependencies]
http = "^1.0.0"
json = "^2.0.0"
database = { version = "3.0.0", features = ["async"] }

[dev-dependencies]
test-utils = "^1.0.0"

[build]
source_dirs = ["src"]
output_dir = "build"
```

## بنية الحزمة (Package Structure)

```cpp
// من package.h
struct Package {
    std::string name;              // اسم الحزمة
    Version version;               // الإصدار
    std::string description;       // الوصف
    std::vector<std::string> authors;  // المؤلفون
    std::string license;           // الرخصة
    std::string main_file;         // الملف الرئيسي
    
    std::vector<std::string> source_dirs;  // مجلدات المصدر
    
    std::map<std::string, Dependency> dependencies;      // التبعيات
    std::map<std::string, Dependency> dev_dependencies;  // تبعيات التطوير
    
    // حفظ وتحميل
    void save_to_file(const std::string& path);
    static Package load_from_file(const std::string& path);
};
```

## نظام الإصدارات (Semantic Versioning)

مدير الحزم يستخدم Semantic Versioning:

```
MAJOR.MINOR.PATCH
مثال: 2.1.3
```

- **MAJOR:** تغييرات غير متوافقة
- **MINOR:** ميزات جديدة متوافقة
- **PATCH:** إصلاحات متوافقة

### محددات الإصدار

```toml
# إصدار محدد
http = "1.0.0"

# أي إصدار متوافق مع 1.x.x
http = "^1.0.0"

# أي إصدار 1.0.x
http = "~1.0.0"

# أي إصدار >= 1.0.0 و < 2.0.0
http = ">=1.0.0, <2.0.0"

# أحدث إصدار
http = "*"
```

## حل التبعيات (Dependency Resolution)

عند تثبيت الحزم، يجب حل تعارضات التبعيات:

```cpp
// من dependency_resolver.h
class DependencyResolver {
public:
    // حل التبعيات
    DependencyGraph resolve(const Package& package);
    
private:
    // خوارزمية SAT لحل التعارضات
    std::vector<PackageVersion> solveDependencies(
        const std::vector<Dependency>& deps
    );
    
    // التحقق من التوافق
    bool isCompatible(const Version& v1, const Version& v2);
};
```

### مثال على حل التعارضات

إذا كان:
- الحزمة A تحتاج http ^1.0.0
- الحزمة B تحتاج http ^1.5.0

الحل: تثبيت http 1.5.x (يُلبي كلا المتطلبين)

## السجل المركزي (Registry)

```cpp
// من registry_client.h
class RegistryClient {
public:
    // البحث عن حزم
    std::vector<PackageInfo> search(const std::string& query);
    
    // جلب معلومات حزمة
    PackageInfo getPackageInfo(const std::string& name);
    
    // تنزيل حزمة
    bool downloadPackage(const std::string& name, 
                        const Version& version,
                        const std::string& dest);
    
    // نشر حزمة
    bool publishPackage(const Package& package,
                       const std::string& token);
                       
private:
    std::string registry_url_ = "https://registry.sadlang.org";
};
```

## الذاكرة المؤقتة (Cache)

الحزم المُحمَّلة تُخزَّن محلياً:

```
~/.sad/
├── cache/           # الحزم المُحمَّلة
│   └── http/
│       ├── 1.0.0/
│       └── 1.5.0/
├── registry/        # فهرس السجل
└── config.toml      # إعدادات عامة
```

```bash
# تنظيف الذاكرة المؤقتة
sad-pkg clean
```

## مجلد التبعيات (sad_modules)

التبعيات تُثبَّت في مجلد `sad_modules/`:

```
my_project/
├── sad.toml
├── sad.lock           # ملف القفل (الإصدارات الدقيقة)
├── sad_modules/       # التبعيات المُثبَّتة
│   ├── http/
│   └── json/
└── src/
```

## ملف القفل (sad.lock)

يحتوي على الإصدارات الدقيقة المُثبَّتة:

```toml
# sad.lock - لا تعدل يدوياً
[[package]]
name = "http"
version = "1.5.3"
checksum = "sha256:abc123..."

[[package]]
name = "json"
version = "2.1.0"
checksum = "sha256:def456..."
dependencies = ["http ^1.0.0"]
```

## النصوص (Scripts)

يمكن تعريف نصوص مخصصة:

```toml
[scripts]
build = "sad compile -o app src/main.s"
test = "sad test tests/"
lint = "sad-lint src/"
```

```bash
sad-pkg run build
sad-pkg run test
```

## مساحات العمل (Workspaces)

للمشاريع الكبيرة متعددة الحزم:

```toml
[workspace]
members = [
    "core",
    "cli",
    "gui"
]
```

## الأمان

### التحقق من الشفرات

```cpp
bool verifyChecksum(const std::string& file, 
                   const std::string& expected) {
    std::string actual = sha256sum(file);
    return actual == expected;
}
```

### توقيع الحزم

الحزم المنشورة تُوقَّع للتحقق من المصدر:

```bash
sad-pkg publish --sign
```

## الخلاصة

مدير الحزم sad-pkg هو أداة أساسية في نظام لغة ص البيئي. يسهل مشاركة واستخدام المكتبات، ويدير التبعيات تلقائياً، ويوفر نظام إصدارات واضح. من خلال السجل المركزي، يمكن للمطورين الوصول لآلاف المكتبات الجاهزة. نظام القفل يضمن تكرارية البناء، والتحقق من الشفرات يضمن الأمان. مدير الحزم يجعل التطوير بلغة ص تجربة سلسة وممتعة.
