# بناء الكتاب (PDF/HTML)

## المسار المعتمد

- المصدر: `docs/book/*.md`
- ناتج التجميع: `build/book/book.md`
- ناتج HTML: `build/book/book.html`
- ناتج PDF: `build/book/book.pdf` (إذا توفر Pandoc + TeX)

## التنفيذ

```powershell
.\scripts\build_book.ps1
```

## المتطلبات

- PowerShell
- `pandoc` (اختياري لتحويل Markdown إلى HTML كامل)
- `xelatex` أو TeX engine (اختياري لإنتاج PDF عبر pandoc)
- أو `node + playwright` (مسار بديل لإنتاج PDF من HTML)

## ملاحظات

- عند عدم وجود pandoc، يولد السكربت نسخة HTML أولية قابلة للقراءة.
- السكربت يحاول إنتاج PDF أولاً عبر pandoc، ثم عبر Playwright كمسار بديل.
- عند فشل كل المسارات، ينشئ السكربت `build/book/PDF_REQUIRED.txt` بتعليمات الإكمال.
