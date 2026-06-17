# معمارية نظام SadUI IR — المرجع الشامل

> **تاريخ التوثيق:** أبريل 2026  
> **الإصدار:** 1.0  
> **الحالة:** مرجع تقني داخلي

---

## الفهرس

1. [نظرة عامة](#1-نظرة-عامة)
2. [بنية IRNode](#2-بنية-irnode)
3. [مسارات بناء شجرة IR](#3-مسارات-بناء-شجرة-ir)
4. [تدفق rebuildUI()](#4-تدفق-rebuildui)
5. [سلسلة المعدّلات في AST](#5-سلسلة-المعدّلات-في-ast)
6. [أنواع العناصر المدعومة](#6-أنواع-العناصر-المدعومة)
7. [التدفق الكامل من واجهة إلى العرض](#7-التدفق-الكامل-من-واجهة-إلى-العرض)
8. [الملفات المرجعية](#8-الملفات-المرجعية)

---

## 1. نظرة عامة

نظام SadUI يستخدم **تمثيلاً وسيطاً (IR)** بين كود لغة ص والعرض النهائي:

```
كود ص  →  Parser  →  AST (UIWidgetExprNode)
                        ↓ (المفسر يُنفذ بناء())
                    Data::Value (كائنات وقت التشغيل)
                        ↓ (UIBridge::convertToIR)
                    IRNode Tree
                        ↓
                    {SDL2, HTML, SwiftUI, AppKit, Freestanding}
```

### لماذا IR؟

1. **التجريد**: يُجرّد الفروقات بين المنصات — مولّدات الكود تتعامل مع IR موحد
2. **التوجيه الذكي**: كل عقدة تحمل `RenderingHint` (Native / Canvas / Hybrid / Auto)
3. **التحسين**: يمكن تطبيق تحسينات قبل العرض (دمج معدّلات، حذف عناصر مخفية)
4. **Tree Diffing**: مقارنة شجرتين IR لتخطي إعادة العرض غير الضرورية

---

## 2. بنية IRNode

**الملف:** `sad_ui/core/include/sad_ui/ir.h`

### الأعضاء الأساسية

| العضو | النوع | الوصف |
|---|---|---|
| `type_` | `UINodeType` (uint8_t enum) | نوع العنصر (Text, Button, Column, ...) |
| `id_` | `string` | معرّف فريد (عادةً: className + عنوان الكائن) |
| `hint_` | `RenderingHint` | طريقة العرض: Auto / Native / Canvas / Hybrid |
| `platform_` | `TargetPlatform` | المنصة المستهدفة |
| `properties_` | `vector<IRProperty>` | خصائص بصرية وهيكلية |
| `events_` | `vector<IREvent>` | معالجات الأحداث |
| `stateRefs_` | `vector<IRStateRef>` | إشارات حالة تفاعلية |
| `children_` | `vector<shared_ptr<IRNode>>` | عقد أبناء |

### IRProperty

```cpp
struct IRProperty {
    using Value = std::variant<int64_t, double, bool, std::string>;
    std::string key;    // اسم الخاصية (مثل: "نص", "حجم", "لون")
    Value value;        // القيمة
};
```

### IREvent

```cpp
struct IREvent {
    std::string eventType;                   // "عند_النقر", "عند_التغيير", ...
    std::string expression;                  // معرّف handler أو اسم دالة
    std::vector<std::string> modifiedStates; // متغيرات الحالة المتأثرة
};
```

### IRStateRef

```cpp
struct IRStateRef {
    std::string stateName;          // اسم المتغير
    std::string propertyBinding;    // الخاصية المرتبطة
    StateBindingType bindingType;   // نوع الربط
};
```

### إنشاء عقدة

```cpp
auto node = IRNode::create(UINodeType::Text);
node->setId("myText_1");
node->setProperty("text", std::string("مرحبا"));
node->setProperty("حجم", 32.0);
node->addEvent(IREvent{"عند_النقر", "__handler_0", {}});
node->addChild(childNode);
```

### IRModule

يمثل ملفاً أو شاشة كاملة:

```cpp
struct IRModule {
    std::string name;                                          // اسم الوحدة
    std::shared_ptr<IRNode> root;                              // الشجرة الجذرية
    std::unordered_map<std::string, std::string> stateDefinitions; // تعريفات الحالة
    std::string sourceFile;                                    // ملف المصدر
};
```

---

## 3. مسارات بناء شجرة IR

### المسار A — IRBuilder (مسار مولّدات الكود)

**الملفات:**
- `sad_ui/core/include/sad_ui/ir_builder.h`
- `sad_ui/core/src/ir_builder.cpp`

**التدفق:** `UINode Tree → IRBuilder::build() → IRModule`

يتعامل مع كائنات `UINode` (من نظام `sad_ui/core`) — وليس كائنات المفسر. يحوّل `Modifier` objects إلى `IRProperty` و `IREvent`. يستخدم `HybridRouter` للتوجيه الذكي.

```cpp
IRBuildOptions options;
options.targetPlatform = TargetPlatform::Desktop;
options.optimize = true;

IRBuilder builder(options);
auto irModule = builder.build(viewDef);
// أو من عقدة مباشرة:
auto irNode = builder.buildNode(uiNode);
```

**ملاحظة:** هذا المسار **غير مستخدم حالياً في المفسر** — مخصص لمولّدات الكود المستقبلية.

### المسار B — UIBridge::convertToIR() (المسار الفعلي)

**الملف:** `interpreter_new/src/ui/ui_bridge.cpp` (سطر 386+)

**التدفق:** `Data::Value (كائنات لغة ص) → convertNodeToIR() → IRNode Tree`

هذا هو المسار المستخدم فعلاً في المفسر. يقرأ كائنات `ObjectInstance` مباشرة:

**الخوارزمية:**
1. يقرأ `_نوع` من الكائن → `stringToNodeType()` → `UINodeType`
2. إذا `_نوع == "حالة"` (StatefulWidget): يستدعي `بناء()` على الكائن ويحوّل النتيجة
3. ينشئ `IRNode::create(nodeType)` مع معرّف فريد
4. يقرأ `_خصائص` (خريطة) ويحوّل كل زوج key/value إلى `IRProperty`:
   - الفصل بين خصائص الأحداث (`عند_النقر`, `عند_التغيير`, ...) → `IREvent`
   - خصائص `طريقة_عرض` → `RenderingHint`
   - باقي الخصائص → `IRProperty`
5. يقرأ `_أبناء` (مصفوفة) ويحوّل كل ابن تكرارياً (حد أقصى: 100 مستوى عمق)

```cpp
// مثال تحويل كائن ص إلى IR
std::shared_ptr<IRNode> UIBridge::convertToIR(const Data::Value& widget) {
    return convertNodeToIR(widget, 0);
}
```

### استخراج الأحداث

الأحداث المعروفة (يتعرف عليها `isEventProperty()`):
- `عند_الضغط`, `عند_النقر`, `عند_التغيير`, `عند_الإدخال`
- `عند_السحب`, `عند_التحرير`, `عند_التمرير`, `عند_التركيز`
- `onTap`, `onClick`, `onChange`, `onInput`, `onDrag`, `onRelease`, `onScroll`, `onFocus`

عند اكتشاف حدث:
- إذا كانت القيمة دالة: `registerHandler(val)` → يحفظها ويعيد معرّفاً `__handler_N`
- إذا كانت نصاً: يُحفظ كاسم دالة مباشرة

---

## 4. تدفق rebuildUI()

**الملف:** `interpreter_new/src/ui/ui_bridge_events.cpp`

### متى يُستدعى؟

- بعد كل `handleEvent()` (نقر، تغيير، ...)
- بعد `عيّن_الحالة()` / `setState()`
- عند تغيير `@حالة` عبر `UIStateManager::flush()`
- بعد `انتقل()` / `عودة()` / `استبدل()`
- بعد `تبديل_الثيم()` / `وضع_داكن()` / `وضع_فاتح()`

### الخطوات

```
rebuildUI()
│
├─ 1. هل builderFunc_ موجودة؟
│     نعم → استدعاء builderFunc_ → rootWidget_ = النتيجة
│     لا  → نستخدم rootWidget_ الحالي
│
├─ 2. مسح eventHandlers_ + إعادة تصفير العدّاد
│
├─ 3. convertToIR(rootWidget_) → newIR
│
├─ 4. Tree Diffing:
│     treesEqual(previousIR_, newIR)?
│     نعم → تخطي (الشجرة لم تتغير بصرياً)
│     لا  → متابعة
│
├─ 5. previousIR_ = newIR
│
├─ 6. window->setContent(newIR)
│     → يُعيد التخطيط والرسم
│
└─ 7. stateChanged_ = false
```

### Tree Diffing (`treesEqual`)

يقارن:
- نوع العنصر (`getType()`)
- عدد وقيم الخصائص (key + value)
- عدد وأنواع الأحداث (eventType + expression)
- عدد الأبناء + مقارنة تكرارية

---

## 5. سلسلة المعدّلات في AST

**الملف:** `shared/ast/include/ui_nodes.h`

### UIWidgetExprNode — تعبير العنصر

```cpp
class UIWidgetExprNode : public Expression {
public:
    std::string widgetName;                                  // "نص", "عمود", "زر"
    ExprList arguments;                                      // وسائط الاستدعاء
    std::vector<std::pair<std::string, ExprPtr>> namedArgs;  // وسائط مسماة
    std::vector<std::unique_ptr<UIModifierNode>> modifiers;  // سلسلة المعدّلات
    std::vector<std::unique_ptr<UIWidgetExprNode>> children; // العناصر الأبناء
    bool hasChildrenBlock;                                   // هل يحتوي كتلة أبناء؟
};
```

### UIModifierNode — المعدّل

```cpp
class UIModifierNode : public Expression {
public:
    std::string name;   // "حجم", "لون", "عرض", "عند_النقر"
    ExprList arguments; // وسائط المعدّل
    bool isEvent;       // هل هو حدث؟
    std::unique_ptr<UIEventHandlerNode> eventHandler; // معالج الحدث (إذا isEvent)
};
```

### UIEventHandlerNode — معالج الحدث

ثلاث صيغ:

```sad
# 1. ARROW — سطر واحد
.عند_النقر => عدد += 1

# 2. BLOCK — كتلة
.عند_النقر
    عدد += 1
    اطبع("تم")
نهاية

# 3. LAMBDA — لامدا صريحة
.عند_التغيير(لامدا(القيمة)
    نص_البحث = القيمة
نهاية)
```

### UIStateDecl — تصريحات الحالة

```cpp
enum class UIStateKind {
    STATE,       // @حالة — حالة محلية
    BINDING,     // @ربط — مرجع للأب
    ENVIRONMENT, // @بيئة — حالة عالمية
    COMPUTED     // @محسوب — قيمة مشتقة
};
```

### UIDeclarationNode — تعريف المكون

```cpp
class UIDeclarationNode : public Statement {
public:
    std::string name;                                     // اسم المكون
    std::string parentName;                               // وراثة (اختياري)
    std::vector<std::unique_ptr<UIStateDecl>> stateDecls; // تصريحات الحالة
    StmtList methods;                                     // دوال المكون (بما فيها بناء())
    bool isExported;
};
```

---

## 6. أنواع العناصر المدعومة

**الملف:** `sad_ui/core/include/sad_ui/types.h` (`UINodeType` enum)

### عرض أساسي
`Text` (نص), `Image` (صورة), `Icon` (أيقونة)

### إدخال
`Button` (زر), `TextField` (حقل_نص), `TextArea` (منطقة_نص), `Toggle` (تبديل/متاح), `Slider` (منزلق), `Picker` (منتقي/قائمة_منسدلة), `DatePicker` (منتقي_تاريخ), `ColorPicker` (منتقي_لون), `Checkbox` (مربع_اختيار/خانة_اختيار), `Radio` (زر_راديو)

### تخطيط
`Column` (عمود), `Row` (صف), `Stack` (رصة/تكدس), `Grid` (شبكة), `LazyColumn` (عمود_كسول), `LazyRow` (صف_كسول), `LazyGrid` (شبكة_كسولة), `Spacer` (فاصل), `Divider` (خط_فاصل), `Wrap` (التفاف)

### حاويات
`Container` (حاوية/توسط/مرن), `Card` (بطاقة), `Surface` (سطح), `Scaffold` (هيكل), `Box` (صندوق)

### تنقل
`NavigationView`, `NavigationLink`, `TabView`, `TabItem`

### قوائم
`List` (قائمة), `Section` (قسم_قائمة), `ForEach`

### حوارات
`Alert`, `Sheet`, `Dialog`, `Menu`

### متقدمة
`Canvas`, `MapView`, `WebView`, `VideoPlayer`, `ScrollView`

### خاصة
`CustomWidget`, `Conditional`, `Group`

### إضافات v2
`ProgressBar`, `Badge`, `Chip`, `Avatar`, `AppBar` (شريط_تطبيق), `BottomNav` (شريط_سفلي), `FAB` (زر_عائم), `Drawer` (درج), `Tabs` (شريط_تبويب), `SearchBar` (حقل_بحث), `RatingBar` (تقيم), `Tooltip` (تلميح), `SnackBar` (رسالة_منبثقة), `BottomSheet` (لوحة_سفلية), `Skeleton` (هيكل_تحميل), `CodeBlock` (كود), `Expandable` (أكورديون)

### ربط الأسماء العربية

`stringToNodeType()` في `ui_bridge.cpp` يربط أكثر من **80+ اسم عربي** بأنواع `UINodeType`، بما في ذلك أسماء بديلة ومتغيرات Material Design.

---

## 7. التدفق الكامل من واجهة إلى العرض

```
┌─────────────────────────────────────────────────┐
│  كود ص:                                          │
│  واجهة عداد                                       │
│      @حالة عدد: رقم = 0                           │
│      دالة بناء()                                   │
│          ارجع عمود                                 │
│              .تباعد(16)                            │
│              نص("العدد: " + عدد).حجم(48)           │
│              زر("زيادة").عند_النقر => عدد += 1     │
│          نهاية                                    │
│      نهاية                                        │
│  نهاية                                            │
└────────────────────┬────────────────────────────┘
                     │ 1. Parser
                     ▼
┌─────────────────────────────────────────────────┐
│ UIDeclarationNode("عداد")                         │
│   stateDecls: [UIStateDecl(STATE, "عدد", "رقم")] │
│   methods: [FunctionDecl("بناء")]                 │
│     body → ReturnStmt:                           │
│       UIWidgetExprNode("عمود")                    │
│         modifiers: [UIModifierNode("تباعد", [16])]│
│         children:                                │
│           ├─ UIWidgetExprNode("نص")               │
│           │   args: [BinaryExpr("العدد: " + عدد)] │
│           │   modifiers: [UIModifierNode("حجم",[48])] │
│           └─ UIWidgetExprNode("زر")               │
│               args: [StringLiteral("زيادة")]      │
│               modifiers:                         │
│                 UIModifierNode("عند_النقر")        │
│                   isEvent: true                   │
│                   eventHandler: ARROW(عدد += 1)   │
└────────────────────┬────────────────────────────┘
                     │ 2. المفسر: UIVisitor يُنفذ بناء()
                     │    - يُقيّم التعبيرات (عدد=0 → "العدد: 0")
                     │    - يُنشئ كائنات ObjectInstance
                     ▼
┌─────────────────────────────────────────────────┐
│ Data::Value (ObjectInstance)                      │
│   className: "عمود"                              │
│   _نوع: "عمود"                                   │
│   _خصائص: {"تباعد": 16}                          │
│   _أبناء: [                                      │
│     ObjectInstance{                               │
│       _نوع: "نص"                                 │
│       _خصائص: {"نص": "العدد: 0", "حجم": 48}     │
│     },                                           │
│     ObjectInstance{                               │
│       _نوع: "زر"                                 │
│       _خصائص: {"نص": "زيادة",                    │
│                "عند_النقر": <FunctionRef>}        │
│     }                                            │
│   ]                                              │
└────────────────────┬────────────────────────────┘
                     │ 3. UIBridge::convertToIR()
                     ▼
┌─────────────────────────────────────────────────┐
│ IRNode(Column)                                    │
│   id: "عمود_0x7FF..."                            │
│   props: [{key: "تباعد", value: (int64_t)16}]    │
│   children:                                      │
│     ├─ IRNode(Text)                              │
│     │    props: [{key: "text", value: "العدد: 0"},│
│     │           {key: "حجم", value: (int64_t)48}]│
│     └─ IRNode(Button)                            │
│          props: [{key: "text", value: "زيادة"}]  │
│          events: [{eventType: "عند_النقر",        │
│                    expression: "__handler_0"}]    │
└────────────────────┬────────────────────────────┘
                     │ 4. المنصة المستهدفة
                     ├──→ SDL2:  window.setContent(irRoot) → رسم مباشر
                     ├──→ HTML:  HtmlCodegen::generate(module) → صفحة ويب
                     ├──→ SwiftUI: SwiftUICodegen::generate(module)
                     ├──→ AppKit: AppKitCodegen::generate(module)
                     ├──→ Android: sad_native_ui_store(module) → JNI
                     └──→ Freestanding: FreestandingRenderer::renderTree()
```

---

## 8. الملفات المرجعية

| الملف | الدور |
|---|---|
| `sad_ui/core/include/sad_ui/ir.h` | تعريف IRNode, IRProperty, IREvent, IRStateRef, IRModule |
| `sad_ui/core/include/sad_ui/ir_builder.h` | IRBuilder — يحوّل UINode → IRNode (غير مستخدم في المفسر) |
| `sad_ui/core/src/ir_builder.cpp` | تنفيذ IRBuilder مع التوجيه الذكي |
| `sad_ui/core/include/sad_ui/types.h` | UINodeType enum (50+ نوع عنصر) |
| `interpreter_new/src/ui/ui_bridge.h` | UIBridge — الجسر بين المفسر وSadUI |
| `interpreter_new/src/ui/ui_bridge.cpp` | convertToIR(), run(), stringToNodeType() |
| `interpreter_new/src/ui/ui_bridge_events.cpp` | rebuildUI(), handleEvent(), treesEqual() |
| `interpreter_new/src/ui/ui_core_builtins.cpp` | _محرك_واجهات(), تشغيل_تطبيق(), توليد_ويب() |
| `shared/ast/include/ui_nodes.h` | UIWidgetExprNode, UIModifierNode, UIEventHandlerNode, UIStateDecl, UIDeclarationNode |

### ملاحظة: لا يوجد WidgetBuilder منفصل

لا يوجد أي صنف أو ملف باسم `WidgetBuilder`. المسار الفعلي هو `UIBridge::convertToIR()` الذي يحوّل كائنات `Data::Value` إلى `IRNode` مباشرة.

### الفجوة الرئيسية

لا يوجد مسار مباشر من AST (`UIWidgetExprNode`) إلى `IRNode`. المعدّلات تمر عبر:
```
AST (UIModifierNode) → المفسر → Data::Value (كائن بـ _خصائص) → convertToIR() → IRNode
```
بدلاً من تحويل مباشر. هذا مقبول للمفسر، لكن قد يحتاج تحسيناً للمترجم (sadc).
