# sad-math — مكتبة الرياضيات المتقدمة
# Advanced Mathematics Library for لغة ص

## الوصف / Description
مكتبة رياضيات شاملة تحتوي على أكثر من 30 دالة تغطي: الجبر، الهندسة، نظرية الأعداد، المتتاليات، والخوارزميات العددية.

A comprehensive math library with 30+ functions covering: algebra, geometry, number theory, sequences, and numerical algorithms.

## الدوال المتوفرة / Available Functions

### الأساسيات / Basics
- `القيمة_المطلقة(قيمة)` — Absolute value
- `الاكبر(أ، ب)` — Maximum
- `الاصغر(أ، ب)` — Minimum
- `حصر(قيمة، أدنى، أقصى)` — Clamp

### القوى واللوغاريتمات / Powers & Logarithms
- `القوة(أساس، أس)` — Power (iterative)
- `القوة_السريعة(أساس، أس)` — Fast power O(log n)
- `الجذر_التربيعي(ع)` — Square root (Newton's method)
- `لوغاريتم2(ع)` / `لوغاريتم10(ع)` — Log base 2/10

### التوافقيات / Combinatorics
- `العاملي(ع)` — Factorial
- `التوافيق(ع، ك)` — Combinations C(n,k)
- `التباديل(ع، ك)` — Permutations P(n,k)

### نظرية الأعداد / Number Theory
- `هل_اولي(ع)` — Primality test
- `ق_م_أ(أ، ب)` — GCD (Euclidean)
- `م_م_أ(أ، ب)` — LCM
- `هل_كامل(ع)` — Perfect number test

### المتتاليات / Sequences
- `فيبوناتشي(ع)` — Fibonacci
- `مجموع_المتتالية(ع)` — Sum 1..n
- `مجموع_المربعات(ع)` — Sum of squares

### الهندسة / Geometry
- `مساحة_المستطيل` / `مساحة_المثلث` / `محيط_الدائرة` / `حجم_المكعب`

### الأعداد العشوائية / Random Numbers
- `عشوائي()` / `عشوائي_بين(أدنى، أقصى)` — LCG generator

## التثبيت / Installation
```
sad-pkg add sad-math ^1.0.0
```

## الترخيص / License
MIT
