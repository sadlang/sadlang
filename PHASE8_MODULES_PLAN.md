# Test Module System - Phase 8

## الهدف
تنفيذ نظام الوحدات (Modules) والاستيراد (Import) للغة ص.

## الميزات المطلوبة

### 8.1 Module Declaration
```sad
# ملف: math_utils.s
صدّر دالة جمع(رقم أ، رقم ب)
    إرجاع أ + ب;
نهاية

صدّر متغير PI = 3.14159;
```

### 8.2 Import Statement
```sad
# ملف: main.s
استورد "math_utils.s";
اطبع(جمع(5، 3));
اطبع(PI);
```

### 8.3 Selective Import
```sad
استورد جمع، PI من "math_utils.s";
```

### 8.4 Import with Alias
```sad
استورد "math_utils.s" كـ رياضيات;
اطبع(رياضيات.جمع(5، 3));
```

## الأولوية
- **P0**: Basic import/export
- **P1**: Selective import
- **P2**: Alias support
- **P3**: Package system

## الحالة
⏳ لم يبدأ - 0%
