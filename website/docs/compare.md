---
title: مقارنة لغة ص مع لغات البرمجة الأخرى
description: مقارنة شاملة بين لغة ص وPython وJavaScript وC++ وGo وRust
---

# مقارنة لغة ص مع اللغات الأخرى

<script setup>
import { ref } from 'vue'

const categories = ['الميزات العامة', 'الأداء والبناء', 'الأنواع والذاكرة', 'الأدوات والمنظومة', 'الاستخدام والتعلم']
const activeCategory = ref('الميزات العامة')

const comparisonData = {
  'الميزات العامة': [
    { feature: 'لغة عربية أصيلة', sad: '✅', python: '❌', js: '❌', cpp: '❌', go: '❌', rust: '❌' },
    { feature: 'كلمات مفتاحية عربية', sad: '✅', python: '❌ (إنجليزية)', js: '❌', cpp: '❌', go: '❌', rust: '❌' },
    { feature: 'مفسر فوري', sad: '✅', python: '✅', js: '✅', cpp: '❌', go: '❌', rust: '❌' },
    { feature: 'مترجم أصيل', sad: '✅ LLVM', python: '❌', js: '❌', cpp: '✅', go: '✅', rust: '✅' },
    { feature: 'البرمجة الكائنية (OOP)', sad: '✅', python: '✅', js: '✅', cpp: '✅', go: 'جزئي', rust: 'جزئي' },
    { feature: 'السمات (Traits/Interfaces)', sad: '✅', python: 'ABC', js: 'جزئي', cpp: '❌', go: 'Interfaces', rust: '✅' },
    { feature: 'البرمجة غير المتزامنة', sad: '✅ async/await', python: '✅', js: '✅', cpp: '✅ C++20', go: '✅ goroutines', rust: '✅' },
    { feature: 'المولدات', sad: '✅', python: '✅', js: '✅', cpp: 'C++23', go: '❌ جزئي', rust: '❌' },
    { feature: 'مطابقة الأنماط', sad: '✅', python: '✅ 3.10+', js: '❌', cpp: 'C++23', go: '❌', rust: '✅' },
    { feature: 'تعابير Lambda', sad: '✅', python: 'محدودة', js: '✅', cpp: '✅', go: '✅', rust: '✅' },
  ],
  'الأداء والبناء': [
    { feature: 'سرعة التنفيذ', sad: '⚡⚡⚡ مترجم / ⚡⚡ مفسر', python: '⚡', js: '⚡⚡', cpp: '⚡⚡⚡⚡', go: '⚡⚡⚡', rust: '⚡⚡⚡⚡' },
    { feature: 'وقت البناء', sad: 'سريع (LLVM)', python: 'لا يلزم', js: 'لا يلزم', cpp: 'بطيء', go: 'سريع', rust: 'بطيء' },
    { feature: 'حجم الملف التنفيذي', sad: 'صغير', python: 'كبير (مع runtime)', js: 'متوسط', cpp: 'صغير', go: 'متوسط', rust: 'صغير' },
    { feature: 'استخدام الذاكرة', sad: 'متوسط', python: 'مرتفع', js: 'مرتفع', cpp: 'منخفض', go: 'منخفض', rust: 'منخفض' },
    { feature: 'تحسين الأداء (-O3)', sad: '✅ LLVM O3', python: '❌', js: 'JIT', cpp: '✅', go: 'جزئي', rust: '✅' },
  ],
  'الأنواع والذاكرة': [
    { feature: 'نظام الأنواع', sad: 'ديناميكي + محدد', python: 'ديناميكي', js: 'ديناميكي', cpp: 'ثابت', go: 'ثابت', rust: 'ثابت' },
    { feature: 'إدارة الذاكرة', sad: 'مجمّع القمامة', python: 'GC', js: 'GC', cpp: 'يدوي', go: 'GC', rust: 'Ownership' },
    { feature: 'Type Inference', sad: '✅', python: '✅', js: '✅', cpp: '✅ auto', go: '✅', rust: '✅' },
    { feature: 'Null Safety', sad: 'جزئي', python: '❌', js: '❌', cpp: '❌', go: '❌', rust: '✅' },
    { feature: 'المؤشرات', sad: 'غير مباشر', python: '❌', js: '❌', cpp: '✅', go: '✅', rust: '✅' },
  ],
  'الأدوات والمنظومة': [
    { feature: 'مدير حزم مدمج', sad: '✅ sad-pkg', python: 'pip', js: 'npm/yarn', cpp: 'vcpkg/Conan', go: 'go modules', rust: 'Cargo' },
    { feature: 'منسّق كود مدمج', sad: '✅ sad-fmt', python: 'black (خارجي)', js: 'prettier (خارجي)', cpp: 'clang-format', go: 'gofmt ✅', rust: 'rustfmt ✅' },
    { feature: 'خادم LSP مدمج', sad: '✅ sad-lsp', python: 'pylsp (خارجي)', js: 'tsserver', cpp: 'clangd', go: 'gopls ✅', rust: 'rust-analyzer' },
    { feature: 'دعم VS Code رسمي', sad: '✅', python: '✅', js: '✅', cpp: '✅', go: '✅', rust: '✅' },
    { feature: 'REPL تفاعلي', sad: '✅', python: '✅', js: '✅', cpp: '❌', go: '❌', rust: '❌' },
    { feature: 'وثائق في الكود', sad: 'دوكمنت ## ', python: 'docstrings', js: 'JSDoc', cpp: 'Doxygen', go: 'godoc ✅', rust: 'rustdoc ✅' },
  ],
  'الاستخدام والتعلم': [
    { feature: 'منحنى التعلم', sad: 'سهل (عربي)', python: 'سهل', js: 'متوسط', cpp: 'صعب', go: 'سهل', rust: 'صعب' },
    { feature: 'مناسب للمبتدئين', sad: '✅✅ (عربي)', python: '✅✅', js: '✅', cpp: '❌', go: '✅', rust: '❌' },
    { feature: 'تطوير الويب', sad: 'مرتقب', python: '✅ Django/Flask', js: '✅✅', cpp: '❌', go: '✅', rust: 'جزئي' },
    { feature: 'تحليل البيانات', sad: 'مرتقب', python: '✅✅', js: 'جزئي', cpp: '❌', go: '❌', rust: '❌' },
    { feature: 'تطبيقات سطح المكتب', sad: '✅ (SDL2)', python: '✅ Tkinter', js: '✅ Electron', cpp: '✅✅', go: '✅', rust: '✅' },
    { feature: 'حجم مجتمع المطورين', sad: 'ناشئ 🌱', python: '🌍 ضخم', js: '🌍 ضخم', cpp: '🌍 كبير', go: '🌍 كبير', rust: '🌍 متوسط' },
  ],
}
</script>

<div class="compare-tabs">
  <button
    v-for="cat in categories"
    :key="cat"
    class="compare-tab"
    :class="{ active: activeCategory === cat }"
    @click="activeCategory = cat"
  >{{ cat }}</button>
</div>

<div class="compare-table-wrap">
  <table class="compare-table">
    <thead>
      <tr>
        <th>الميزة</th>
        <th class="lang sad-col">🇸🇦 لغة ص</th>
        <th class="lang">🐍 Python</th>
        <th class="lang">🌐 JavaScript</th>
        <th class="lang">⚙️ C++</th>
        <th class="lang">🐹 Go</th>
        <th class="lang">🦀 Rust</th>
      </tr>
    </thead>
    <tbody>
      <tr v-for="row in comparisonData[activeCategory]" :key="row.feature">
        <td class="feature-name">{{ row.feature }}</td>
        <td class="sad-col" :class="{ 'best': row.sad.startsWith('✅') }">{{ row.sad }}</td>
        <td>{{ row.python }}</td>
        <td>{{ row.js }}</td>
        <td>{{ row.cpp }}</td>
        <td>{{ row.go }}</td>
        <td>{{ row.rust }}</td>
      </tr>
    </tbody>
  </table>
</div>

---

## مقارنة الكود — نفس البرنامج بلغات مختلفة

### مرحبا بالعالم (Hello World)

::: code-group

```sad [لغة ص 🟢]
اطبع_سطر("مرحباً يا عالم!")
```

```python [Python]
print("Hello, World!")
```

```javascript [JavaScript]
console.log("Hello, World!")
```

```cpp [C++]
#include <iostream>
int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
```

```go [Go]
package main
import "fmt"
func main() {
    fmt.Println("Hello, World!")
}
```

```rust [Rust]
fn main() {
    println!("Hello, World!");
}
```

:::

### الفيبوناتشي

::: code-group

```sad [لغة ص 🟢]
دالة فيبو(ن)
  إذا (ن <= 1)  ارجع ن  نهاية
  ارجع فيبو(ن - 1) + فيبو(ن - 2)
نهاية

لكل ي في مدى(0، 10)
  اطبع(فيبو(ي) + " ")
نهاية
```

```python [Python]
def fib(n):
    if n <= 1: return n
    return fib(n-1) + fib(n-2)

for i in range(10):
    print(fib(i), end=" ")
```

```javascript [JavaScript]
function fib(n) {
    if (n <= 1) return n;
    return fib(n-1) + fib(n-2);
}
Array.from({length:10}, (_,i) => i)
  .forEach(i => process.stdout.write(fib(i) + " "));
```

```cpp [C++]
#include <iostream>
int fib(int n) {
    if (n <= 1) return n;
    return fib(n-1) + fib(n-2);
}
int main() {
    for (int i = 0; i < 10; i++)
        std::cout << fib(i) << " ";
}
```

```go [Go]
package main
import "fmt"
func fib(n int) int {
    if n <= 1 { return n }
    return fib(n-1) + fib(n-2)
}
func main() {
    for i := 0; i < 10; i++ {
        fmt.Printf("%d ", fib(i))
    }
}
```

```rust [Rust]
fn fib(n: u64) -> u64 {
    if n <= 1 { return n; }
    fib(n-1) + fib(n-2)
}
fn main() {
    for i in 0..10 { print!("{} ", fib(i)); }
}
```

:::

### صنف وكائن (OOP)

::: code-group

```sad [لغة ص 🟢]
صنف حيوان
  باني(اسم، صوت)
    هذا.اسم = اسم
    هذا.صوت = صوت
  نهاية
  دالة تحدث()
    اطبع_سطر(هذا.اسم + ": " + هذا.صوت + "!")
  نهاية
نهاية

صنف كلب يرث حيوان
  باني(اسم)  الأساس(اسم، "هاو")  نهاية
نهاية

متغير ك = جديد كلب("ريكس")
ك.تحدث()  # ريكس: هاو!
```

```python [Python]
class Animal:
    def __init__(self, name, sound):
        self.name = name
        self.sound = sound
    def speak(self):
        print(f"{self.name}: {self.sound}!")

class Dog(Animal):
    def __init__(self, name):
        super().__init__(name, "Woof")

d = Dog("Rex")
d.speak()  # Rex: Woof!
```

```javascript [JavaScript]
class Animal {
    constructor(name, sound) {
        this.name  = name;
        this.sound = sound;
    }
    speak() { console.log(`${this.name}: ${this.sound}!`); }
}
class Dog extends Animal {
    constructor(name) { super(name, "Woof"); }
}
new Dog("Rex").speak(); // Rex: Woof!
```

```cpp [C++]
#include <iostream>
#include <string>
class Animal {
  std::string name, sound;
public:
  Animal(std::string n, std::string s): name(n), sound(s) {}
  virtual void speak() { std::cout << name << ": " << sound << "!\n"; }
};
class Dog : public Animal {
public:
  Dog(std::string n): Animal(n, "Woof") {}
};
int main() { Dog d("Rex"); d.speak(); }
```

```go [Go]
package main
import "fmt"
type Animal struct { Name, Sound string }
func (a Animal) Speak() { fmt.Printf("%s: %s!\n", a.Name, a.Sound) }
type Dog struct { Animal }
func NewDog(name string) Dog { return Dog{Animal{name, "Woof"}} }
func main() { NewDog("Rex").Speak() }
```

```rust [Rust]
struct Animal { name: String, sound: String }
impl Animal {
    fn speak(&self) { println!("{}: {}!", self.name, self.sound); }
}
struct Dog(Animal);
impl Dog {
    fn new(name: &str) -> Self { Dog(Animal { name: name.into(), sound: "Woof".into() }) }
    fn speak(&self) { self.0.speak(); }
}
fn main() { Dog::new("Rex").speak(); }
```

:::

---

## الخلاصة

::: tip لغة ص — نقاط القوة الفريدة
- **الميزة الوحيدة في العالم**: لغة برمجة عربية كاملة بكلمات مفتاحية عربية
- **مزدوجة التنفيذ**: مفسر فوري + مترجم أصيل LLVM في حزمة واحدة
- **أدوات مدمجة**: مدير حزم + منسّق + LSP دون تثبيت إضافي
- **منحنى تعلم منخفض**: خاصة للعرب الذين لا يتقنون الإنجليزية
:::

::: warning ما زال في طور النمو
- مجتمع المطورين ناشئ مقارنة باللغات الأخرى
- المنظومة البيئية (المكتبات) في طور البناء
- بعض ميزات المكتبة القياسية قيد التطوير
:::

<style scoped>
.compare-tabs {
  display:   flex;
  flex-wrap: wrap;
  gap:       8px;
  margin:    24px 0;
}
.compare-tab {
  padding:       7px 16px;
  border:        1px solid var(--vp-c-border);
  border-radius: 20px;
  background:    var(--vp-c-bg-soft);
  cursor:        pointer;
  font-size:     0.85rem;
  color:         var(--vp-c-text-2);
  transition:    all 0.15s;
}
.compare-tab:hover,
.compare-tab.active {
  background:   var(--vp-c-brand-soft);
  border-color: var(--vp-c-brand-1);
  color:        var(--vp-c-brand-1);
  font-weight:  700;
}
.compare-table-wrap { overflow-x: auto; margin: 0 -16px; }
.compare-table {
  min-width:   700px;
  width:       100%;
  border-collapse: collapse;
}
.compare-table th,
.compare-table td {
  padding:    10px 14px;
  border:     1px solid var(--vp-c-border);
  text-align: center;
  font-size:  0.85rem;
  white-space: nowrap;
}
.compare-table th.lang { font-weight: 700; background: var(--vp-c-bg-soft); }
.compare-table .feature-name { text-align: right; font-weight: 600; white-space: normal; min-width: 180px; }
.compare-table .sad-col { background: rgba(14,165,233,0.06); font-weight: 600; }
.compare-table .best     { color: var(--sad-green); }
</style>
