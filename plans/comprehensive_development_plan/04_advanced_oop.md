# 📝 المرحلة 4: البرمجة الكائنية المتقدمة
## Phase 4: Advanced OOP

**المدة:** أسبوعان (10 أيام عمل)  
**الأولوية:** 🟠 P1 - عالية  
**التبعيات:** المرحلة 1-3  
**التأثير:** قوة اللغة وقابلية التوسع

---

## 🎯 الأهداف

1. ✅ Multiple Inheritance
2. ✅ Interfaces (واجهة)
3. ✅ Abstract Classes (مجرد)
4. ✅ Method Resolution Order (MRO)
5. ✅ Properties & Descriptors
6. ✅ Metaclasses (متقدم)
7. ✅ Operator Overloading

---

## 📋 المهام

### المهمة 4.1: Multiple Inheritance (3 أيام)

```cpp
// Syntax
class Animal {
    function speak() { print("...") }
}

class Flyable {
    function fly() { print("Flying") }
}

class Bird : Animal, Flyable {
    function speak() { print("Tweet") }
}

// Arabic
صنف طائر : حيوان، قابل_للطيران {
    دالة تكلم() { اطبع("تغريد") }
}

// Implementation
class ClassDecl {
public:
    std::vector<std::string> base_classes;  // Multiple bases
    
    // Method Resolution Order (C3 linearization)
    std::vector<ClassDecl*> mro;
};

std::vector<ClassDecl*> computeMRO(ClassDecl* cls) {
    // C3 linearization algorithm
    std::vector<ClassDecl*> result;
    result.push_back(cls);
    
    // ... implementation
    
    return result;
}
```

### المهمة 4.2: Interfaces (يومان)

```cpp
// Syntax
interface Drawable {
    function draw()
    function getArea(): float
}

class Circle implements Drawable {
    function draw() { /* ... */ }
    function getArea(): float { return 3.14 * r * r }
}

// Arabic
واجهة قابل_للرسم {
    دالة ارسم()
    دالة احصل_على_المساحة(): عشري
}
```

### المهمة 4.3: Abstract Classes (يوم)

```cpp
// Syntax
abstract class Shape {
    abstract function getArea(): float
    
    function describe() {
        print("Area: " + this.getArea())
    }
}

// Arabic
مجرد صنف شكل {
    مجرد دالة احصل_على_المساحة(): عشري
}
```

### المهمة 4.4: Properties (يوم)

```cpp
// Syntax
class Person {
    private var _age: int
    
    property age {
        get { return this._age }
        set(value) { 
            if (value >= 0) this._age = value 
        }
    }
}

var p = Person()
p.age = 25  // Calls setter
print(p.age)  // Calls getter
```

### المهمة 4.5: Operator Overloading (يومان)

```cpp
// Syntax
class Vector {
    var x: float
    var y: float
    
    operator +(other: Vector): Vector {
        return Vector(this.x + other.x, this.y + other.y)
    }
    
    operator ==(other: Vector): bool {
        return this.x == other.x && this.y == other.y
    }
}

var v1 = Vector(1, 2)
var v2 = Vector(3, 4)
var v3 = v1 + v2  // Calls operator+
```

---

## 📅 الجدول الزمني

| اليوم | المهمة | الساعات |
|------|---------|----------|
| 1-3 | Multiple inheritance + MRO | 24 |
| 4-5 | Interfaces + tests | 16 |
| 6 | Abstract classes + tests | 8 |
| 7 | Properties + tests | 8 |
| 8-9 | Operator overloading | 16 |
| 10 | Integration testing | 8 |

**المجموع:** 80 ساعة

---

**الحالة:** 📝 جاهز للتنفيذ
