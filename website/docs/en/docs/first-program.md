---
title: Your First Program
---
# Your First Program

Create a file `hello.ص` and write:

```sad
اطبع_سطر("Hello, World! 🌍")
```

Run it:
```bash
sad hello.ص
```

Output:
```
Hello, World! 🌍
```

## With Variables

```sad
متغير name = "World"
متغير year = 2025

اطبع_سطر("Hello, " + name + "! Year: " + year)
```

## With a Function

```sad
دالة greet(person, greeting = "Hello")
  اطبع_سطر(greeting + ", " + person + "!")
نهاية

greet("Ahmed")             # Hello, Ahmed!
greet("Sarah", "Welcome")  # Welcome, Sarah!
```

---

Next: [Variables & Types →](/en/docs/variables)
