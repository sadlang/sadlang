---
name: grilling
description: Interview the user relentlessly about a plan or design. Use when the user wants to stress-test a plan before building, or uses any 'grill' trigger phrases (استجوبني، حاصرني، اختبر خطتي).
---

Interview me relentlessly about every aspect of this plan until we reach a shared understanding. Walk down each branch of the design tree, resolving dependencies between decisions one-by-one. For each question, provide your recommended answer.

Ask the questions one at a time, waiting for feedback on each question before continuing. Asking multiple questions at once is bewildering.

If a question can be answered by exploring the codebase, explore the codebase instead.

## مواءمة مشروع لغة ص

- تواصل بالعربية حصرًا (التزام المشروع).
- اطرح سؤالًا واحدًا في كل مرة عبر أداة `AskUserQuestion` بخيارات قابلة للنقر، مع خيار «شيء آخر...»
  وكتابة حرة — تطبيقًا لقاعدة المشروع، مع إرفاق توصيتك المقترحة كأول خيار.
- إن كان السؤال قابلًا للإجابة باستكشاف الشيفرة، استكشفها بدل السؤال.
