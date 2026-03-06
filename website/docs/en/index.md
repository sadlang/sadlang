---
layout: home
title: Sad Language — The First Modern Arabic Programming Language

hero:
  name: "Sad Language"
  text: "لغة ص — The First Modern Arabic Programming Language"
  tagline: "Interpreter · Native Compiler via LLVM · Full LSP · Arabic Standard Library"
  image:
    src: /sad-hero.svg
    alt: Sad Language Logo
  actions:
    - theme: brand
      text: Get Started
      link: /en/docs/introduction
    - theme: alt
      text: Download v1.0.0
      link: /en/download
    - theme: alt
      text: Try Playground ▶
      link: /playground

features:
  - icon: ⚡
    title: Instant Interpreter
    details: Run .ص files immediately without any build step. Perfect for learning and rapid prototyping.
  - icon: 🚀
    title: Native Compiler (LLVM)
    details: Compile to native executables with peak performance using LLVM 18 backend.
  - icon: 🔤
    title: Fully Arabic
    details: Arabic keywords, comments, variable names. Write code in your native language.
  - icon: 🧩
    title: Full OOP Support
    details: Classes, inheritance, traits, operator overloading — all with elegant Arabic syntax.
  - icon: 🛠️
    title: Integrated Tooling
    details: Package manager (sad-pkg), formatter (sad-fmt), and a full LSP server.
  - icon: 🌐
    title: Rich Standard Library
    details: io · math · string · network · graphics · database — everything you need.
---

## Why Sad Language?

Arabic has over 400 million native speakers, yet all major programming languages use English keywords. Sad removes this barrier so Arabic speakers can code in their native language.

## Quick Example

```sad
# برنامج حساب الفيبوناتشي — Fibonacci Calculator

دالة فيبوناتشي(ن)
  إذا (ن <= 1)
    ارجع ن
  نهاية
  ارجع فيبوناتشي(ن - 1) + فيبوناتشي(ن - 2)
نهاية

لكل ي في [0، 1، 2، 3، 4، 5، 6، 7]
  اطبع_سطر("فيبو(" + ي + ") = " + فيبوناتشي(ي))
نهاية
```

## Quick Install

::: code-group

```powershell [Windows]
irm https://sad-lang.org/install.ps1 | iex
```

```bash [Linux/macOS]
curl -fsSL https://sad-lang.org/install.sh | bash
```

:::

[Full installation guide →](/en/docs/getting-started)

<SadVersion />
