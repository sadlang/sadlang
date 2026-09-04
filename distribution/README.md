# بسم الله الرحمن الرحيم

# توزيع لغة ص / Sad Language Distribution

هذا المجلد يحتوي جميع ملفات التوزيع والنشر لتسهيل تثبيت لغة ص على جميع المنصات.

## البنية / Structure

```
distribution/
├── install.ps1                     # سكريبت تثبيت Windows (PowerShell)
├── install.sh                      # سكريبت تثبيت Linux/macOS (Bash/sh)
├── README.md                       # هذا الملف
│
├── winget/                         # حزمة winget (Windows)
│   ├── SadLang.Sad.yaml            # إصدار
│   ├── SadLang.Sad.locale.ar.yaml  # وصف عربي
│   └── SadLang.Sad.installer.yaml  # معلومات التثبيت
│
├── scoop/                          # حزمة Scoop (Windows)
│   └── sad.json                    # الحزمة القياسية
│
├── homebrew/                       # حزمة Homebrew (Linux)
│   └── sad.rb                      # الحزمة القياسية
│
├── deb/                            # حزمة deb (Debian/Ubuntu)
│   └── build-deb.sh               # سكريبت بناء .deb
│
└── rpm/                            # حزمة rpm (Fedora/RHEL)
    └── sad.spec                    # ملف RPM spec
```

## مصدر التحميل / Download Source

جميع الملفات تُحمّل من **GitHub Releases**:
```
https://github.com/sad-lang/sad-language/releases
```

### أنواع الحزم لكل إصدار:

| الملف | المحتوى | مثال |
|-------|---------|------|
| `sad-v*.zip/.tar.gz` | **المترجم** + مكتبة قياسية + أدوات أساسية | `sad-v1.0.0-windows-x86_64.zip` |
| `sad-full-v*.zip/.tar.gz` | **المترجم** + كل الأدوات (LSP · REPL · مدير الحزم · المنسّق) | `sad-full-v1.0.0-linux-x86_64.tar.gz` |

<!-- (AR) 🔑 صنفان لا ثلاثة: زالَ `sadc-v*` («المترجم فقط») مع زوالِ المفسّر —
     لم يبقَ ما يميّزُه عن القياسيّة. وLLVM تُربَطُ ساكنًا فلا شرطَ على المستخدم. -->

### المنصات المدعومة:
- `windows-x86_64`
- `linux-x86_64`, `linux-aarch64` (القياسية) · `linux-x86_64` (الكاملة)

<!-- (AR) ⚠️ ماك غيرُ مبنيّةٍ في مجرى الإصدار (أُزيلت مدخلاتُها من المصفوفتَين). -->

---

## طرق التثبيت / Installation Methods

### 1. سكريبت تثبيت (الأسهل)

**Windows (PowerShell):**
```powershell
irm https://sad-lang.org/install.ps1 | iex
```

**Linux / macOS:**
```bash
curl -fsSL https://sad-lang.org/install.sh | sh
```

السكريبتات تعرض قائمة تفاعلية لاختيار:
- `[1]` الحزمة القياسية — الأفضل لمعظم المستخدمين
- `[2]` الحزمة الكاملة — + LSP و REPL ومدير الحزم والمنسّق

---

### 2. winget (Windows 10/11)
```powershell
winget install SadLang.Sad
```

**نشر الحزمة:**
1. Fork مستودع `microsoft/winget-pkgs`
2. أنشئ `manifests/s/SadLang/Sad/{VERSION}/`
3. انسخ ملفات `distribution/winget/` وحدّث الإصدار
4. افتح Pull Request

---

### 3. Scoop (Windows)
```powershell
# إضافة bucket لغة ص
scoop bucket add sad https://github.com/sad-lang/scoop-bucket

# الحزمة القياسية
scoop install sad
```

**نشر الحزمة:**
1. أنشئ مستودع `sad-lang/scoop-bucket`
2. ضع `sad.json` في الجذر

---

### 4. Homebrew (macOS / Linux)
```bash
# إضافة tap لغة ص
brew tap sad-lang/tap

# الحزمة القياسية
brew install sad
```

**نشر الحزمة:**
1. أنشئ مستودع `sad-lang/homebrew-tap`
2. ضع `sad.rb` في مجلد `Formula/`

---

### 5. deb (Debian / Ubuntu)
```bash
# تحميل وتثبيت
wget https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-lang_1.0.0_amd64.deb
sudo apt install ./sad-lang_1.0.0_amd64.deb
```

**بناء الحزمة محلياً:**
```bash
cd distribution/deb
./build-deb.sh 1.0.0 x86_64
```

---

### 6. rpm (Fedora / RHEL / openSUSE)
```bash
# تحميل وتثبيت
wget https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-lang-1.0.0-1.x86_64.rpm
sudo dnf install ./sad-lang-1.0.0-1.x86_64.rpm
```

**بناء الحزمة:**
```bash
rpmbuild -bb distribution/rpm/sad.spec
```

---

### 7. Docker (أي نظام)
```bash
docker pull sadlang/sad:latest
docker run --rm -v $(pwd):/app sadlang/sad /app/script.ص
```

---

## الفرق بين المفسر والمترجم

| | المفسر (`sad`) | المترجم (`sadc`) |
|---|---|---|
| **الوظيفة** | يشغل .ص مباشرة | يحوّل .ص إلى ملف تنفيذي |
| **السرعة** | سريع للتطوير | أسرع في التنفيذ |
| **التبعيات** | لا شيء | يتطلب LLVM 18 |
| **الاستخدام** | تطوير، تعلم، سكريبتات | إنتاج، أداء عالي |

---

## CI/CD

ملف `.github/workflows/release.yml` يبني تلقائياً عند وسم جديد:
1. يبني المفسر على 5 منصات
2. يبني المترجم (مع LLVM) على 3 منصات
3. ينشئ حزم `sad`, `sadc`, `sad-full` لكل منصة
4. ينشر GitHub Release مع SHA256 checksums
