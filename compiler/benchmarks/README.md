# قياسات الأداء — مترجم ص / Compiler Benchmarks

مجموعة من 10 برامج قياس أداء مكتوبة بلغة ص لاختبار المترجم والمفسر.

## البرامج / Programs

| # | الملف | الموضوع | ما يُختبر |
|---|-------|---------|----------|
| 01 | `01_فيبوناتشي.ص` | Recursive Fibonacci | استدعاء تكراري، إدارة المكدس |
| 02 | `02_الترتيب.ص` | Sorting (Bubble/Selection) | حلقات متداخلة، مقارنات، تبديل |
| 03 | `03_البحث.ص` | Search (Linear/Binary) | فهرسة، شروط، حلقات |
| 04 | `04_البرمجة_الكائنية.ص` | OOP (inheritance, polymorphism) | أصناف، وراثة، `هذا`، `الأساس` |
| 05 | `05_النصوص.ص` | String Processing | جمع نصوص، بحث، تكرار |
| 06 | `06_المصفوفات.ص` | Array Operations | map/filter/reduce، تسطيح |
| 07 | `07_الرياضيات.ص` | Math (primes, GCD, factorial) | عمليات حسابية، تكرار |
| 08 | `08_الاستثناءات.ص` | Exception Handling | حاول/امسك/ارمي |
| 09 | `09_مطابقة_الأنماط.ص` | Pattern Matching | طابق/عندما، نطاقات |
| 10 | `10_اختبار_شامل.ص` | Integration Test | جميع الميزات معاً |

## التشغيل / Running

```powershell
# تشغيل برنامج واحد
.\build\bin\Debug\sad.exe compiler_new\benchmarks\01_فيبوناتشي.ص

# تشغيل جميع البرامج
Get-ChildItem compiler_new\benchmarks\*.ص | ForEach-Object {
    Write-Host "--- $($_.Name) ---"
    .\build\bin\Debug\sad.exe $_.FullName
}
```
