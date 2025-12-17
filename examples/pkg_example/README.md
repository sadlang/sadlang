# بسم الله الرحمن الرحيم

# Math Utils - أدوات الرياضيات

دوال رياضية مساعدة للغة ص  
Mathematical utility functions for Sad Language

## المزايا / Features

- ✅ دوال رياضية أساسية (Basic math functions)
- ✅ حسابات هندسية (Geometric calculations)
- ✅ معالجة الأرقام (Number processing)
- ✅ دوال إحصائية (Statistical functions)

## التثبيت / Installation

```bash
sad-pkg add math-utils ^1.0.0
```

## الاستخدام / Usage

```sad
# استيراد المكتبة
استورد math من "math-utils"

# استخدام الدوال
دالة main() {
    # حساب القوة
    متغير result = math.power(2, 10)
    اطبع("2^10 = " + result)  # 1024
    
    # حساب الجذر التربيعي
    متغير sqrt = math.sqrt(144)
    اطبع("√144 = " + sqrt)  # 12
    
    # حساب المتوسط
    متغير avg = math.average([10, 20, 30, 40, 50])
    اطبع("المتوسط = " + avg)  # 30
    
    # محيط الدائرة
    متغير circumference = math.circle_circumference(5)
    اطبع("محيط دائرة نصف قطرها 5 = " + circumference)
}
```

## الدوال المتاحة / Available Functions

### دوال القوى والجذور / Power & Root Functions

- `power(base, exponent)` - رفع عدد لقوة
- `sqrt(number)` - الجذر التربيعي
- `cbrt(number)` - الجذر التكعيبي

### دوال هندسية / Geometric Functions

- `circle_area(radius)` - مساحة الدائرة
- `circle_circumference(radius)` - محيط الدائرة
- `rectangle_area(width, height)` - مساحة المستطيل
- `triangle_area(base, height)` - مساحة المثلث

### دوال إحصائية / Statistical Functions

- `average(numbers)` - المتوسط الحسابي
- `sum(numbers)` - مجموع الأرقام
- `min(numbers)` - أصغر رقم
- `max(numbers)` - أكبر رقم

## الترخيص / License

MIT License - رخصة MIT

## المساهمة / Contributing

نرحب بمساهماتكم! يرجى زيارة المستودع على GitHub.

---

**الحمد لله رب العالمين**
