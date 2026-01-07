# بسم الله الرحمن الرحيم
# 📋 تقرير Phase 1.5.3 - Project Templates
# Phase 1.5.3 Report - Project Template System

**التاريخ / Date:** 7 يناير 2026  
**المرحلة / Phase:** Phase 1.5.3 - Project Templates  
**الحالة / Status:** 🔴 **لم تبدأ / Not Started (0%)**

---

## 📋 ملخص تنفيذي / Executive Summary

Project Templates توفر قوالب جاهزة لإنشاء أنواع مختلفة من المشاريع بسرعة، مشابهة لـ:
- `cargo new --template` (Rust)
- `create-react-app` (React)
- `django-admin startproject` (Django)

---

## 🎯 الأهداف / Goals

### الهدف الرئيسي
توفير قوالب جاهزة للاستخدام لأنواع المشاريع الشائعة في لغة ص.

### أنواع القوالب المطلوبة

1. **CLI Application** - تطبيق سطر أوامر
2. **Web Application** - تطبيق ويب (Backend)
3. **Library** - مكتبة قابلة لإعادة الاستخدام
4. **REST API** - واجهة برمجية REST
5. **Game** - لعبة 2D
6. **Desktop App** - تطبيق سطح مكتب
7. **Blank** - مشروع فارغ

---

## 📂 بنية القوالب / Template Structure

### المجلد الأساسي

```
tools/templates/
├── cli/                    # قالب CLI
├── web/                    # قالب Web
├── library/                # قالب Library
├── rest-api/               # قالب REST API
├── game/                   # قالب Game
├── desktop/                # قالب Desktop
├── blank/                  # قالب Blank
│
├── generator.cpp           # Template generator
└── README.md
```

---

## 📋 القالب 1: CLI Application

### الهيكل

```
cli/
├── sad.toml
├── src/
│   └── main.s
├── tests/
│   └── test_main.s
├── README.md
└── .gitignore
```

### sad.toml

```toml
[package]
name = "{{PROJECT_NAME}}"
version = "0.1.0"
description = "A CLI application in Sad"
authors = ["{{AUTHOR_NAME}} <{{AUTHOR_EMAIL}}>"]

[build]
main = "src/main.s"
output = "build/"
optimization = "full"

[dependencies]
# Add your dependencies here

[dev-dependencies]
test_framework = "^0.5.0"

[tasks.build]
command = "sadc"
args = ["${main}", "-o", "${output}/{{PROJECT_NAME}}"]
description = "Build the CLI application"

[tasks.run]
command = "${output}/{{PROJECT_NAME}}"
description = "Run the application"
depends = ["build"]

[tasks.test]
command = "sadc"
args = ["tests/*.s", "--test"]
description = "Run tests"
```

### src/main.s

```sad
# بسم الله الرحمن الرحيم
# {{PROJECT_NAME}} - Main Entry Point

استورد نظام.args
استورد نظام.io

دالة رئيسية(وسائط: مصفوفة<نص>) -> رقم {
    إذا وسائط.طول() < 2 {
        اطبع("الاستخدام: {{PROJECT_NAME}} <command>")
        إرجاع 1
    }
    
    متغير أمر = وسائط[1]
    
    اختر أمر {
        حالة "مساعدة" {
            عرض_مساعدة()
        }
        حالة "إصدار" {
            عرض_إصدار()
        }
        افتراضي {
            اطبع("أمر غير معروف: " + أمر)
            اطبع("استخدم 'مساعدة' لعرض الأوامر المتاحة")
            إرجاع 1
        }
    }
    
    إرجاع 0
}

دالة عرض_مساعدة() {
    اطبع("{{PROJECT_NAME}} v0.1.0")
    اطبع("")
    اطبع("الاستخدام:")
    اطبع("  {{PROJECT_NAME}} <command> [options]")
    اطبع("")
    اطبع("الأوامر:")
    اطبع("  مساعدة    - عرض هذه الرسالة")
    اطبع("  إصدار    - عرض رقم الإصدار")
}

دالة عرض_إصدار() {
    اطبع("{{PROJECT_NAME}} v0.1.0")
}
```

### tests/test_main.s

```sad
# بسم الله الرحمن الرحيم
# Tests for {{PROJECT_NAME}}

استورد اختبار

@اختبار
دالة اختبار_رئيسي_بدون_وسائط() {
    متغير نتيجة = رئيسية([])
    تأكد.يساوي(نتيجة، 1)
}

@اختبار
دالة اختبار_أمر_مساعدة() {
    متغير نتيجة = رئيسية(["برنامج"، "مساعدة"])
    تأكد.يساوي(نتيجة، 0)
}
```

### README.md

```markdown
# {{PROJECT_NAME}}

{{PROJECT_DESCRIPTION}}

## Installation

```bash
sad install
```

## Usage

```bash
sad run -- [command]
```

## Development

```bash
# Build
sad build

# Run tests
sad test

# Run
sad run
```

## License

MIT
```

---

## 📋 القالب 2: Web Application

### الهيكل

```
web/
├── sad.toml
├── src/
│   ├── main.s
│   ├── routes/
│   │   └── index.s
│   └── middleware/
│       └── logger.s
├── public/
│   └── index.html
├── tests/
└── README.md
```

### src/main.s

```sad
# بسم الله الرحمن الرحيم
# {{PROJECT_NAME}} - Web Server

استورد ويب
استورد ويب.خادم
استورد ويب.مسارات

دالة رئيسية() -> رقم {
    متغير تطبيق = ويب.تطبيق_جديد()
    
    # تكوين middleware
    تطبيق.استخدم(سجل_logger())
    
    # تكوين المسارات
    تطبيق.احصل("/", معالج_الرئيسية)
    تطبيق.احصل("/api/مرحبا", معالج_api_مرحبا)
    
    # بدء الخادم
    متغير منفذ = 3000
    اطبع("الخادم يعمل على http://localhost:" + منفذ)
    
    تطبيق.استمع(منفذ)
    إرجاع 0
}

دالة معالج_الرئيسية(طلب، استجابة) {
    استجابة.أرسل("<h1>مرحباً بكم في {{PROJECT_NAME}}</h1>")
}

دالة معالج_api_مرحبا(طلب، استجابة) {
    استجابة.json({
        "رسالة": "مرحباً من API",
        "الوقت": الآن()
    })
}
```

---

## 📋 القالب 3: Library

### الهيكل

```
library/
├── sad.toml
├── src/
│   └── lib.s
├── examples/
│   └── example.s
├── tests/
│   └── test_lib.s
├── README.md
└── LICENSE
```

### sad.toml

```toml
[package]
name = "{{LIBRARY_NAME}}"
version = "0.1.0"
description = "{{LIBRARY_DESCRIPTION}}"
authors = ["{{AUTHOR_NAME}} <{{AUTHOR_EMAIL}}>"]
license = "MIT"

[build]
type = "library"
main = "src/lib.s"
output = "build/"

[dependencies]

[dev-dependencies]
test_framework = "^0.5.0"
```

### src/lib.s

```sad
# بسم الله الرحمن الرحيم
# {{LIBRARY_NAME}} - Main Library File

# صدّر الدوال العامة
صدّر دالة مرحبا(اسم: نص) -> نص {
    إرجاع "مرحباً " + اسم
}

صدّر دالة جمع(أ: رقم، ب: رقم) -> رقم {
    إرجاع أ + ب
}

# دالة خاصة (غير مصدرة)
دالة مساعدة_داخلية() {
    # كود داخلي
}
```

---

## 📋 القالب 4: REST API

### الهيكل

```
rest-api/
├── sad.toml
├── src/
│   ├── main.s
│   ├── api/
│   │   ├── users.s
│   │   └── products.s
│   ├── models/
│   │   ├── user.s
│   │   └── product.s
│   └── database/
│       └── connection.s
├── tests/
└── README.md
```

### src/main.s

```sad
# بسم الله الرحمن الرحيم
# {{PROJECT_NAME}} - REST API Server

استورد ويب
استورد قاعدة_بيانات
استورد api.users
استورد api.products

دالة رئيسية() -> رقم {
    # اتصال قاعدة البيانات
    متغير قب = قاعدة_بيانات.اتصل("sqlite://data.db")
    
    # إنشاء تطبيق
    متغير تطبيق = ويب.تطبيق_جديد()
    
    # Middleware
    تطبيق.استخدم(ويب.json())
    تطبيق.استخدم(ويب.cors())
    
    # Routes
    تطبيق.احصل("/api/users", users.list)
    تطبيق.احصل("/api/users/:id", users.get)
    تطبيق.ضع("/api/users", users.create)
    تطبيق.عدّل("/api/users/:id", users.update)
    تطبيق.احذف("/api/users/:id", users.delete)
    
    تطبيق.احصل("/api/products", products.list)
    
    # بدء الخادم
    اطبع("REST API يعمل على http://localhost:8080")
    تطبيق.استمع(8080)
    
    إرجاع 0
}
```

---

## 📋 القالب 5: Game

### الهيكل

```
game/
├── sad.toml
├── src/
│   ├── main.s
│   ├── game.s
│   └── entities/
│       ├── player.s
│       └── enemy.s
├── assets/
│   ├── sprites/
│   ├── sounds/
│   └── fonts/
└── README.md
```

### src/main.s

```sad
# بسم الله الرحمن الرحيم
# {{GAME_NAME}} - Main Game File

استورد رسومات
استورد game

دالة رئيسية() -> رقم {
    # إنشاء نافذة
    متغير نافذة = رسومات.نافذة_جديدة(800, 600, "{{GAME_NAME}}")
    
    # إنشاء اللعبة
    متغير لعبة = game.لعبة_جديدة()
    
    # حلقة اللعبة
    بينما !نافذة.يجب_أن_تغلق() {
        # معالجة الإدخال
        لعبة.معالجة_إدخال()
        
        # تحديث اللعبة
        لعبة.تحديث(1.0/60.0)
        
        # رسم
        نافذة.امسح()
        لعبة.ارسم(نافذة)
        نافذة.اعرض()
    }
    
    إرجاع 0
}
```

---

## 🛠️ Template Generator / مولد القوالب

### generator.cpp

```cpp
// بسم الله الرحمن الرحيم
/**
 * @file generator.cpp
 * @brief Project Template Generator
 */

#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

namespace sad {
namespace templates {

class TemplateGenerator {
public:
    /**
     * @brief Generate project from template
     */
    bool generate(const std::string& template_name,
                 const std::string& project_name,
                 const std::map<std::string, std::string>& variables) {
        
        std::string template_dir = get_template_dir(template_name);
        if (!std::filesystem::exists(template_dir)) {
            std::cerr << "Template not found: " << template_name << "\n";
            return false;
        }
        
        // Create project directory
        std::filesystem::create_directories(project_name);
        
        // Copy template files
        copy_template(template_dir, project_name, variables);
        
        std::cout << "✓ Created " << project_name << " from template '" 
                  << template_name << "'\n";
        
        return true;
    }

private:
    std::string get_template_dir(const std::string& name) {
        return "tools/templates/" + name + "/";
    }
    
    void copy_template(const std::string& src, 
                      const std::string& dest,
                      const std::map<std::string, std::string>& vars) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(src)) {
            if (entry.is_regular_file()) {
                std::string rel_path = std::filesystem::relative(entry.path(), src).string();
                std::string dest_path = dest + "/" + rel_path;
                
                // Create parent directory
                std::filesystem::create_directories(
                    std::filesystem::path(dest_path).parent_path()
                );
                
                // Copy file with variable substitution
                copy_file_with_substitution(entry.path(), dest_path, vars);
            }
        }
    }
    
    void copy_file_with_substitution(const std::filesystem::path& src,
                                    const std::filesystem::path& dest,
                                    const std::map<std::string, std::string>& vars) {
        std::ifstream in(src);
        std::ofstream out(dest);
        
        std::string line;
        while (std::getline(in, line)) {
            // Replace variables like {{VAR_NAME}}
            for (const auto& [key, value] : vars) {
                std::string pattern = "{{" + key + "}}";
                size_t pos = 0;
                while ((pos = line.find(pattern, pos)) != std::string::npos) {
                    line.replace(pos, pattern.length(), value);
                    pos += value.length();
                }
            }
            out << line << "\n";
        }
    }
};

} // namespace templates
} // namespace sad
```

---

## 💻 الأوامر / Commands

### sad new

```bash
# إنشاء مشروع من قالب
sad new <project_name> --template <template>

# أمثلة:
sad new my_cli --template cli
sad new my_api --template rest-api
sad new my_game --template game

# مع معلومات إضافية:
sad new my_project --template web \
    --author "Ahmed" \
    --email "ahmed@example.com" \
    --description "My web application"

# سرد القوالب المتاحة:
sad templates list

# عرض معلومات قالب:
sad templates show cli
```

---

## 📊 خطة التنفيذ / Implementation Plan

### الأسبوع 1: القوالب الأساسية

**يوم 1:** CLI Template
- إنشاء الهيكل
- sad.toml
- main.s
- tests

**يوم 2:** Web Template
- هيكل web app
- خادم أساسي
- مسارات

**يوم 3:** Library Template
- هيكل library
- exports
- examples

**يوم 4:** REST API Template
- API structure
- models
- database

**يوم 5:** Game Template
- game structure
- assets
- game loop

### الأسبوع 2: Generator & Testing

**يوم 1-2:** Template Generator
- copy_template()
- variable substitution
- CLI integration

**يوم 3-4:** Testing
- اختبار كل قالب
- generation tests
- integration tests

**يوم 5:** Documentation
- README لكل قالب
- Usage guide
- Examples

---

## 🎯 معايير الإكمال / Completion Criteria

Phase 1.5.3 مكتملة عندما:

### Templates ✅
- [ ] 7 قوالب جاهزة
- [ ] كل قالب يبنى وينفذ
- [ ] كل قالب له README
- [ ] كل قالب له tests

### Generator ✅
- [ ] `sad new` يعمل
- [ ] Variable substitution يعمل
- [ ] `sad templates list` يعمل
- [ ] `sad templates show` يعمل

### Documentation ✅
- [ ] README.md شامل
- [ ] دليل لكل قالب
- [ ] أمثلة استخدام
- [ ] Best practices

---

## 🏆 النتيجة المتوقعة / Expected Outcome

نظام قوالب احترافي يسمح بإنشاء مشاريع بسرعة:

```bash
# بدلاً من:
mkdir my_project
cd my_project
# ... إنشاء ملفات يدوياً ...

# الآن:
sad new my_project --template cli
cd my_project
sad build
sad run
```

---

**الحالة:** 0%  
**الهدف:** 100%  
**المدة المتوقعة:** 2 أسبوع

**الحمد لله رب العالمين** 🤲
