# وثائق API — وحدة async

> المسار: `stdlib/async/`
> مساحة الأسماء: `sad::stdlib::async`

## نظرة عامة
الوحدة تقدم قناة ثنائية الاتجاه thread-safe مع مهلات واسترجاع نتائج محكومة بحالة نجاح/فشل.

## الملفات الأساسية
- `async/src/channels.cpp` (تنفيذ القنوات)

## الأنواع الأساسية
- `template<typename T> struct ChannelResult`
  - `success`
  - `value` (`std::optional<T>`)
  - `error_ar`
  - `ok(value)`, `empty()`, `error(message)`
- `template<typename T> class BidirectionalChannel`

## أهم API
- `أرسل_أ(value)` / `أرسل_ب(value)`
- `استقبل_أ()` / `استقبل_ب()`
- `استقبل_أ_مهلة(duration)` / `استقبل_ب_مهلة(duration)`
- `أغلق()`
- `مفتوحة()`
- `حجم_أ()` / `حجم_ب()`

## مثال سريع
```cpp
sad::stdlib::async::BidirectionalChannel<int> chan(10);
chan.أرسل_أ(42);
auto r = chan.استقبل_ب();
if (r.success) {
    int x = r.value.value();
}
```

## ملاحظات
- القناة تحتفظ برسائل buffer بعد الإغلاق حتى يتم سحبها.
- اختبارات الوحدة تغطي thread safety وtimeout وسيناريوهات الإغلاق.
