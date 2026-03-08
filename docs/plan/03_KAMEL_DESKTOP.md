# كامل — سطح المكتب (Desktop Platform)

> الدور: مهندس منصة سطح المكتب
> المسؤولية: Windows, Linux, macOS — رسوميات، واجهات، توزيع، مكتبات منصة
> الملف المرجعي: `docs/plan/01_STRATEGIC_PLAN.md`

---

## الملكية الحصرية

```
graphics/                    ← محرك الرسوميات
├── include/
│   ├── core/types.h         ← أنواع الرسوميات الأساسية
│   ├── rendering/           ← Renderer, Shader, Texture
│   ├── input/               ← مدير الإدخال
│   └── ui/                  ← Desktop UI widgets
├── src/
│   ├── sdl2/                ← SDL2 integration
│   ├── opengl/              ← OpenGL rendering
│   └── widgets/             ← Widget implementations
└── third_party/SDL2/        ← SDL2 مدمج

stdlib/platform/desktop/     ← مكتبات خاصة بسطح المكتب
├── filesystem_desktop.cpp
├── threads_desktop.cpp
├── window_desktop.cpp
├── clipboard_desktop.cpp
├── tray_desktop.cpp
└── notifications_desktop.cpp

stdlib/graphics/             ← واجهة رسوميات موحدة (تنفيذ Desktop)
stdlib/ui/                   ← واجهة UI موحدة (تنفيذ Desktop)

distribution/
├── windows/                 ← Windows MSI installer
│   ├── installer.wxs
│   └── build-msi.ps1
├── linux/                   ← deb + rpm
│   ├── build-deb.sh
│   └── sad.spec
└── macos/                   ← macOS pkg
    ├── build-pkg.sh
    └── Info.plist
```

### القاعدة الذهبية
> كامل لا يعدّل: `compiler_new/`، `tools/`، `platform/android/`، `platform/ios/`
> يقرأ `shared/` لفهم AST لكن لا يعدّله

---

## 2026 — التفصيل الشهري

---

### الربع الأول (مارس — مايو 2026): "إصلاح وبناء الأساس"

#### مارس 2026

| الأسبوع | المهمة | الملفات | المُخرج | معيار القبول |
|---------|--------|---------|---------|-------------|
| 1 | إصلاح graphics: types.h include | `graphics/include/*.h` (6 ملفات) | `#include "core/types.h"` في كل header | 0 compile errors في graphics |
| 2 | إصلاح SDL2 context + initialization | `graphics/src/sdl2/context.cpp` | نافذة SDL2 تفتح وتُعرض | نافذة فارغة + FPS counter |
| 3 | Renderer2D: رسم أساسي | `graphics/src/rendering/renderer2d.cpp` | خطوط + مستطيلات + دوائر + ألوان | رسم 5 أشكال أساسية |
| 4 | Texture + Image loading | `graphics/src/rendering/texture.cpp` | تحميل PNG/JPG + عرض | صورة تُعرض في نافذة |

**المُخرج الشهري**: مكتبة graphics تبني وتعمل — نافذة + رسم + صور

#### أبريل 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Desktop UI: Widget base class | `graphics/src/widgets/widget.h` | فئة أساسية لكل widget |
| 2 | UI: نص (Label) + زر (Button) | `graphics/src/widgets/label.cpp`, `button.cpp` | نص + زر قابل للنقر |
| 3 | UI: حقل إدخال (TextInput) + قائمة (List) | `graphics/src/widgets/input.cpp`, `list.cpp` | إدخال نص + قائمة عناصر |
| 4 | UI: تخطيط (Column/Row/Stack) | `graphics/src/widgets/layout.cpp` | Flexbox-like layout عربي (RTL) |
| | UI: صندوق اختيار + منزلق | `graphics/src/widgets/checkbox.cpp`, `slider.cpp` | 6 widgets إضافية |

**المُخرج الشهري**: 10 widgets أساسية تعمل مع RTL layout

#### مايو 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Windows Installer (MSI) | `distribution/windows/build-msi.ps1` | .msi يثبت sad + PATH |
| 2 | Windows installer: نقاط نهاية | `distribution/windows/installer.wxs` | اختصارات + إزالة نظيفة |
| 3 | stdlib/platform/desktop: filesystem | `stdlib/platform/desktop/filesystem_desktop.cpp` | قراءة/كتابة ملفات + مجلدات + مسارات |
| 4 | stdlib/platform/desktop: clipboard + tray | `stdlib/platform/desktop/clipboard.cpp`, `tray.cpp` | نسخ/لصق + أيقونة شريط المهام |

**المُخرج الشهري**: Windows installer + 2 وحدات stdlib desktop

**معايير القبول Q1:**
- [ ] Graphics تبني بـ 0 أخطاء (types.h fix)
- [ ] 10 widgets أساسية تعمل (RTL)
- [ ] Windows MSI installer عامل
- [ ] filesystem + clipboard + tray APIs

---

### الربع الثاني (يونيو — أغسطس 2026): "المنصات الثلاث"

#### يونيو 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Linux packaging: .deb builder | `distribution/linux/build-deb.sh` | .deb لـ Ubuntu/Debian |
| 2 | Linux packaging: .rpm builder | `distribution/linux/sad.spec` | .rpm لـ Fedora/RHEL |
| 3 | Linux testing: 10 برامج Desktop | `graphics/tests/linux/` | 10 برامج رسومية على Linux |
| 4 | stdlib/network: TCP/UDP sockets (Desktop) | `stdlib/network/network_desktop.cpp` | خادم/عميل TCP يعمل |

#### يوليو 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | macOS packaging: .pkg builder | `distribution/macos/build-pkg.sh` | .pkg لـ macOS |
| 2 | macOS: Homebrew formula | `distribution/macos/sad.rb` | `brew install sad` يعمل |
| 3 | macOS: كود توقيع (code signing) | `distribution/macos/codesign.sh` | توقيع رقمي لـ .pkg |
| 4 | cross-platform: اختبار 3 أنظمة | `graphics/tests/` | نفس البرنامج يعمل على Win+Linux+macOS |

#### أغسطس 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | UI: 10 widgets إضافية | `graphics/src/widgets/` | TabBar, Grid, Dialog, Menu, Dropdown, ScrollView, Image, Tooltip, ProgressBar, Table |
| 2 | UI: نظام أحداث (Events) | `graphics/src/widgets/event_system.cpp` | onClick, onHover, onKeyPress, onDrag |
| 3 | UI: نظام السمات (Theming) | `graphics/src/widgets/theme.cpp` | Light/Dark theme + ألوان مخصصة |
| 4 | تطبيق نموذجي: حاسبة Desktop | `examples/apps/calculator_desktop.ص` | تطبيق حاسبة كامل بلغة ص |

**معايير القبول Q2:**
- [ ] Linux .deb + .rpm يعملان
- [ ] macOS .pkg + Homebrew يعملان
- [ ] 20 widget يعمل مع RTL + theming
- [ ] TCP/UDP sockets تعمل
- [ ] نفس الكود يعمل على 3 أنظمة

---

### الربع الثالث (سبتمبر — نوفمبر 2026): "تطبيقات حقيقية"

#### سبتمبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | 3D أساسي: OpenGL 3D context | `graphics/src/rendering/renderer3d.cpp` | 3D viewport + camera |
| 2 | 3D: mesh loading + rendering | `graphics/src/rendering/mesh.cpp` | عرض OBJ/PLY models |
| 3 | 3D: lighting + materials | `graphics/src/rendering/lighting.cpp` | إضاءة Phong + مواد |
| 4 | stdlib/image: معالجة صور | `stdlib/image/image_desktop.cpp` | resize, crop, rotate, filters |

#### أكتوبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Desktop App Framework | `graphics/src/app/app_framework.cpp` | `تطبيق(عنوان, نافذة)` API |
| 2 | App Framework: menus + toolbar | `graphics/src/app/menu.cpp` | شريط قوائم + أدوات |
| 3 | App Framework: multi-window | `graphics/src/app/window_manager.cpp` | نوافذ متعددة + modals |
| 4 | تطبيق نموذجي: محرر نصوص | `examples/apps/text_editor.ص` | محرر نصوص بسيط بلغة ص |

#### نوفمبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Performance benchmarks: rendering | `graphics/benchmarks/` | FPS + draw calls + memory |
| 2 | Desktop: اختبار شامل 20 تطبيق | `graphics/tests/comprehensive/` | 20 تطبيق Desktop بدون crashes |
| 3 | stdlib/audio: تشغيل صوت | `stdlib/audio/audio_desktop.cpp` | WAV/MP3 playback |
| 4 | التثبيت النهائي: v1.0 preparation | `graphics/`, `distribution/` | 0 أخطاء P0 |

**معايير القبول Q3:**
- [ ] 3D أساسي (viewport + meshes + lighting)
- [ ] Desktop App Framework مع menus + multi-window
- [ ] معالجة صور (resize, crop, filters)
- [ ] صوت (WAV/MP3 playback)
- [ ] 20 تطبيق Desktop بدون crashes

---

### الربع الرابع (ديسمبر 2026): "v1.0"

#### ديسمبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | تثبيت Installers (4 منصات) | `distribution/` | Win MSI + macOS pkg + deb + rpm |
| 2 | Integration testing: كل منصة | `graphics/tests/` | Pass على Win + Linux + macOS |
| 3 | 3 تطبيقات نموذجية كاملة | `examples/apps/` | حاسبة + محرر + chat |
| 4 | v1.0 release + smoke tests | `graphics/`, `distribution/` | ✅ Desktop v1.0 |

**معايير القبول Q4:**
- [ ] 4 installers (Win, macOS, deb, rpm)
- [ ] نفس الكود يعمل على 3 أنظمة بدون تعديل
- [ ] 3 تطبيقات نموذجية كاملة

---

## 2027 — التفصيل الشهري

---

### الربع الأول (يناير — مارس 2027): "تحسين التجربة"

#### يناير 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Desktop++ : Native look-and-feel | `graphics/src/widgets/native/` | Widgets تبدو أصلية لكل نظام |
| 3-4 | Accessibility: قارئ شاشة | `graphics/src/accessibility/` | Screen reader support (ARIA-like) |

#### فبراير 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Widgets متقدمة: TreeView, DataGrid | `graphics/src/widgets/` | 5 widgets متقدمة |
| 3-4 | Drag & Drop + Resizable | `graphics/src/widgets/` | سحب + إسقاط + تغيير حجم |

#### مارس 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Accessibility: RTL 100% + High DPI | `graphics/src/accessibility/` | دعم شاشات عالية الكثافة |
| 3-4 | v1.3 Desktop release | `graphics/`, `distribution/` | 30+ widget + accessibility |

**معايير القبول Q1 2027:**
- [ ] Native look-and-feel لكل نظام
- [ ] Screen reader + High DPI
- [ ] 30+ widget مع accessibility

---

### الربع الثاني (أبريل — يونيو 2027): "3D + Animation"

#### أبريل 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | 3D Engine: scene graph | `graphics/src/3d/scene_graph.cpp` | هيكل شجرة المشهد |
| 3-4 | 3D Engine: physics أساسي | `graphics/src/3d/physics.cpp` | تصادم + جاذبية |

#### مايو 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Animation system | `graphics/src/animation/` | Keyframe + Tween + Easing |
| 3-4 | UI Transitions + Animations | `graphics/src/widgets/transitions.cpp` | Fade, Slide, Scale |

#### يونيو 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Performance: GPU acceleration | `graphics/src/rendering/gpu_accel.cpp` | Batch rendering + instancing |
| 3-4 | تطبيق نموذجي: لعبة 2D | `examples/apps/game_2d.ص` | لعبة 2D كاملة بلغة ص |

**معايير القبول Q2 2027:**
- [ ] 3D scene graph + basic physics
- [ ] Animation system (keyframe + tween)
- [ ] GPU acceleration
- [ ] لعبة 2D كاملة

---

### الربع الثالث (يوليو — سبتمبر 2027): "نضج"

#### يوليو 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | GPU Compute: OpenGL Compute Shaders | `graphics/src/compute/` | حساب GPU عام |
| 3-4 | Offline rendering: SVG export | `graphics/src/export/svg_export.cpp` | تصدير رسومات لـ SVG/PNG |

#### أغسطس 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Offline mode: headless rendering | `graphics/src/headless/` | عمل بدون نافذة (server-side) |
| 3-4 | stdlib/printing: طباعة مستندات | `stdlib/printing/` | API طباعة مستندات (PDF) |

#### سبتمبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Cross-platform testing: كل نقطة اتصال | `graphics/tests/` | 100% API متوافق عبر 3 أنظمة |
| 3-4 | Desktop App: IDE بسيط بلغة ص | `examples/apps/mini_ide.ص` | محرر كود مع syntax highlighting |

**معايير القبول Q3 2027:**
- [ ] GPU compute عامل
- [ ] SVG/PNG export
- [ ] Headless rendering
- [ ] 100% API متوافق عبر Win/Linux/macOS

---

### الربع الرابع (أكتوبر — ديسمبر 2027): "v2.0"

#### أكتوبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Theme Engine: مخصص بالكامل | `graphics/src/widgets/theme_engine.cpp` | مواضيع مخصصة + محرر |
| 3-4 | Performance optimization: 60 FPS ثابت | `graphics/` | Benchmarks مرجعية |

#### نوفمبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Desktop v2.0: polish + bug fixes | `graphics/` | 0 أخطاء P0/P1 |
| 3-4 | Desktop v2.0: smoke tests | `graphics/tests/` | كل العناصر على 3 أنظمة |

#### ديسمبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Release preparation | `distribution/` | 4 installers محدثة |
| 3-4 | 🎉 Desktop v2.0 release | `graphics/`, `distribution/` | إطلاق رسمي |

**معايير القبول Q4 2027:**
- [ ] 40+ widget مع theming مخصص
- [ ] 60 FPS ثابت + GPU acceleration
- [ ] 5 تطبيقات نموذجية كاملة
- [ ] 4 installers (Win, macOS, deb, rpm)
- [ ] 100% cross-platform compatibility

---

## الأدوات والتقنيات

| الأداة | الغرض | الإصدار |
|--------|--------|---------|
| SDL2 | رسوميات + إدخال + صوت | 2.28.5 |
| OpenGL | رسم 3D | 3.3 Core (GLAD) |
| STB Image | تحميل صور | latest |
| FreeType | خطوط + عرض نص عربي | 2.13+ |
| WiX Toolset | Windows MSI | 4.x |
| rpmbuild | Linux RPM | latest |
| dpkg-deb | Linux DEB | latest |
| pkgbuild | macOS PKG | latest |

---

## ملخص المُخرجات

```
2026:
  ✅ Graphics module يبني ويعمل
  ✅ 20+ widget مع RTL + theming
  ✅ Desktop App Framework
  ✅ 3D أساسي + صور + صوت
  ✅ 4 installers (Win, macOS, deb, rpm)
  ✅ نفس الكود → 3 أنظمة

2027:
  ✅ 40+ widget + accessibility
  ✅ Native look-and-feel
  ✅ 3D engine + physics + animation
  ✅ GPU compute + headless rendering
  ✅ 5 تطبيقات نموذجية كاملة
  ✅ 60 FPS ثابت + optimization
```
