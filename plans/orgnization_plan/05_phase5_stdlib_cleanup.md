# المرحلة 5: تنظيف وتنظيم stdlib

## 🎯 الهدف

تنظيف مكتبة stdlib وإزالة التكرار وتوحيد الهيكل.

---

## 📍 الوضع الحالي

```
stdlib/
├── core/                        # ✅ جيد
│   ├── array_functions.cpp/h
│   ├── builtins.cpp/h
│   ├── other_functions.cpp/h
│   ├── stdlib_manager.cpp/h
│   └── type_functions.cpp/h
├── crypto/                      # ✅ جيد
├── database/                    # ✅ جيد
├── filesystem/                  # ✅ جيد
├── graphics/                    # ✅ جيد
├── http/                        # ⚠️ تكرار مع network/http
│   ├── http_builtins.cpp
│   ├── http_module.cpp/h
├── image/                       # ✅ جيد
├── io/                          # ✅ جيد
├── json/                        # ✅ جيد
├── math/                        # ✅ جيد
├── modules/                     # ⚠️ غير واضح الغرض
├── network/                     # ⚠️ معقد
│   ├── http/                    # تكرار مع stdlib/http
│   ├── http.s
│   ├── http_bindings.cpp
│   ├── network_bindings.cpp
│   ├── network_error.cpp
│   ├── socket.s
│   ├── socket_address.cpp
│   ├── socket_base.cpp
│   ├── tcp_socket.cpp
│   ├── udp_socket.cpp
│   └── websocket/
├── string/                      # ✅ جيد
└── xml/                         # ✅ جيد
```

### المشاكل:

1. **تكرار HTTP**: `stdlib/http/` و `stdlib/network/http/`
2. **مجلد modules غير واضح**: ما الغرض منه؟
3. **include/network**: يوجد أيضاً في include/
4. **خلط .h و .cpp**: بعض المجلدات تخلط بينهما

---

## 📁 الهيكل الجديد المقترح

```
stdlib/
├── core/                        # الدوال الأساسية
│   ├── builtins.h
│   ├── builtins.cpp
│   ├── array_functions.h
│   ├── array_functions.cpp
│   ├── type_functions.h
│   ├── type_functions.cpp
│   ├── other_functions.h
│   ├── other_functions.cpp
│   ├── stdlib_manager.h
│   └── stdlib_manager.cpp
│
├── io/                          # الإدخال/الإخراج
│   ├── io_functions.h
│   └── io_functions.cpp
│
├── math/                        # الرياضيات
│   ├── math_functions.h
│   └── math_functions.cpp
│
├── string/                      # النصوص
│   ├── string_functions.h
│   └── string_functions.cpp
│
├── filesystem/                  # نظام الملفات
│   ├── filesystem_module.h
│   ├── filesystem_module.cpp
│   └── filesystem_builtins.cpp
│
├── network/                     # الشبكات (موحد)
│   ├── core/                    # النواة
│   │   ├── socket_base.h
│   │   ├── socket_base.cpp
│   │   ├── socket_address.h
│   │   ├── socket_address.cpp
│   │   ├── network_error.h
│   │   └── network_error.cpp
│   │
│   ├── tcp/                     # TCP
│   │   ├── tcp_socket.h
│   │   └── tcp_socket.cpp
│   │
│   ├── udp/                     # UDP
│   │   ├── udp_socket.h
│   │   └── udp_socket.cpp
│   │
│   ├── http/                    # HTTP (موحد)
│   │   ├── http_module.h
│   │   ├── http_module.cpp
│   │   ├── http_client.h
│   │   ├── http_client.cpp
│   │   ├── http_server.h
│   │   ├── http_server.cpp
│   │   └── http_builtins.cpp
│   │
│   ├── websocket/               # WebSocket
│   │   ├── websocket_client.h
│   │   ├── websocket_client.cpp
│   │   └── websocket_server.h
│   │
│   └── bindings/                # روابط اللغة
│       ├── network_bindings.h
│       ├── network_bindings.cpp
│       ├── http_bindings.h
│       └── http_bindings.cpp
│
├── json/                        # JSON
│   ├── json_module.h
│   ├── json_module.cpp
│   └── json_builtins.cpp
│
├── xml/                         # XML
│   ├── xml_module.h
│   ├── xml_module.cpp
│   └── xml_builtins.cpp
│
├── database/                    # قواعد البيانات
│   ├── database_module.h
│   ├── database_module.cpp
│   └── database_builtins.cpp
│
├── crypto/                      # التشفير
│   ├── crypto_module.h
│   ├── crypto_module.cpp
│   └── crypto_builtins.cpp
│
├── image/                       # الصور
│   ├── image_module.h
│   ├── image_module.cpp
│   └── image_builtins.cpp
│
└── graphics/                    # الرسومات
    ├── graphics_module.h
    ├── graphics_module.cpp
    └── graphics_builtins.cpp
```

---

## 📋 خطوات التنفيذ

### الخطوة 1: دمج HTTP

```powershell
$networkHttp = "C:\s\s_language\stdlib\network\http"
$stdlibHttp = "C:\s\s_language\stdlib\http"

# إنشاء مجلد موحد جديد
New-Item -ItemType Directory -Path "C:\s\s_language\stdlib\network_new\http" -Force

# نقل ملفات من stdlib/http
Copy-Item "$stdlibHttp\*" -Destination "C:\s\s_language\stdlib\network_new\http" -Recurse

# دمج ملفات فريدة من network/http (إذا وجدت)
Get-ChildItem "$networkHttp" | ForEach-Object {
    $destPath = "C:\s\s_language\stdlib\network_new\http\$($_.Name)"
    if (-not (Test-Path $destPath)) {
        Copy-Item $_.FullName -Destination $destPath
    }
}
```

### الخطوة 2: تنظيم Network

```powershell
$oldNetwork = "C:\s\s_language\stdlib\network"
$newNetwork = "C:\s\s_language\stdlib\network_new"

# إنشاء الهيكل
New-Item -ItemType Directory -Path "$newNetwork\core" -Force
New-Item -ItemType Directory -Path "$newNetwork\tcp" -Force
New-Item -ItemType Directory -Path "$newNetwork\udp" -Force
New-Item -ItemType Directory -Path "$newNetwork\websocket" -Force
New-Item -ItemType Directory -Path "$newNetwork\bindings" -Force

# نقل Core
Copy-Item "$oldNetwork\socket_base.cpp" -Destination "$newNetwork\core"
Copy-Item "$oldNetwork\socket_address.cpp" -Destination "$newNetwork\core"
Copy-Item "$oldNetwork\network_error.cpp" -Destination "$newNetwork\core"

# نقل TCP
Copy-Item "$oldNetwork\tcp_socket.cpp" -Destination "$newNetwork\tcp"

# نقل UDP
Copy-Item "$oldNetwork\udp_socket.cpp" -Destination "$newNetwork\udp"

# نقل WebSocket
Copy-Item "$oldNetwork\websocket\*" -Destination "$newNetwork\websocket" -Recurse

# نقل Bindings
Copy-Item "$oldNetwork\network_bindings.cpp" -Destination "$newNetwork\bindings"
Copy-Item "$oldNetwork\http_bindings.cpp" -Destination "$newNetwork\bindings"
```

### الخطوة 3: نقل ملفات الرأس من include/network

```powershell
$includeNetwork = "C:\s\s_language\include\network"
$newNetwork = "C:\s\s_language\stdlib\network_new"

# Core headers
Copy-Item "$includeNetwork\socket_base.h" -Destination "$newNetwork\core"
Copy-Item "$includeNetwork\socket_address.h" -Destination "$newNetwork\core"
Copy-Item "$includeNetwork\network_error.h" -Destination "$newNetwork\core"

# TCP headers
Copy-Item "$includeNetwork\tcp_socket.h" -Destination "$newNetwork\tcp"

# UDP headers
Copy-Item "$includeNetwork\udp_socket.h" -Destination "$newNetwork\udp"

# WebSocket headers
Copy-Item "$includeNetwork\websocket\*" -Destination "$newNetwork\websocket" -Recurse

# Bindings headers
Copy-Item "$includeNetwork\network_bindings.h" -Destination "$newNetwork\bindings"
Copy-Item "$includeNetwork\http_bindings.h" -Destination "$newNetwork\bindings"
```

### الخطوة 4: استبدال المجلد القديم

```powershell
# حذف القديم
Remove-Item -Recurse -Force "C:\s\s_language\stdlib\network"
Remove-Item -Recurse -Force "C:\s\s_language\stdlib\http"

# إعادة التسمية
Rename-Item "C:\s\s_language\stdlib\network_new" "C:\s\s_language\stdlib\network"
```

### الخطوة 5: التحقق من مجلد modules

```powershell
# فحص محتوى modules
Get-ChildItem -Recurse "C:\s\s_language\stdlib\modules"

# إذا كان فارغاً أو غير ضروري، حذفه
# Remove-Item -Recurse -Force "C:\s\s_language\stdlib\modules"
```

---

## 🔧 تحديث مسارات Include

### أمثلة:

```cpp
// قبل
#include "network/tcp_socket.h"
#include "network/http_bindings.h"
#include "http/http_module.h"

// بعد
#include "stdlib/network/tcp/tcp_socket.h"
#include "stdlib/network/bindings/http_bindings.h"
#include "stdlib/network/http/http_module.h"
```

---

## 📝 تحديث CMakeLists.txt

```cmake
# Stdlib Network sources
set(STDLIB_NETWORK_SOURCES
    # Core
    stdlib/network/core/socket_base.cpp
    stdlib/network/core/socket_address.cpp
    stdlib/network/core/network_error.cpp
    
    # TCP
    stdlib/network/tcp/tcp_socket.cpp
    
    # UDP
    stdlib/network/udp/udp_socket.cpp
    
    # HTTP
    stdlib/network/http/http_module.cpp
    stdlib/network/http/http_builtins.cpp
    
    # WebSocket
    stdlib/network/websocket/websocket_client.cpp
    
    # Bindings
    stdlib/network/bindings/network_bindings.cpp
    stdlib/network/bindings/http_bindings.cpp
)
```

---

## ✅ قائمة التحقق

- [ ] دمج HTTP من المصدرين
- [ ] تنظيم Network بالهيكل الجديد
- [ ] نقل ملفات الرأس من include/network
- [ ] التحقق من مجلد modules
- [ ] تحديث مسارات #include
- [ ] تحديث CMakeLists.txt
- [ ] بناء المشروع
- [ ] تشغيل الاختبارات
- [ ] حذف المجلدات القديمة

---

## 🗑️ ما سيتم حذفه

```powershell
# بعد النقل الناجح
Remove-Item -Recurse -Force "C:\s\s_language\include\network"
Remove-Item -Recurse -Force "C:\s\s_language\stdlib\modules"  # إذا كان غير ضروري
```

---

## ⚠️ ملاحظات

1. **ملفات .s**: الملفات مثل `http.s` و `socket.s` هي أمثلة بلغة ص - تُنقل لمجلد examples
2. **التوافقية**: تأكد من عدم كسر الكود الموجود
3. **الاختبارات**: قد تحتاج tests/stdlib و tests/network لتحديث

---

## 🔗 المرحلة التالية

[المرحلة 6: تنظيم الاختبارات](06_phase6_tests_organization.md)
