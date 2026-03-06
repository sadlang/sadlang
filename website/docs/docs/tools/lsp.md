---
title: خادم LSP — دعم المحررات
---
# خادم LSP

يوفر `sad-lsp` بروتوكول Language Server Protocol لدعم:

- ✅ إكمال تلقائي عربي (Auto-complete)
- ✅ تشخيص الأخطاء في الوقت الفعلي
- ✅ الانتقال إلى التعريف (Go to Definition)
- ✅ معلومات عند التمرير (Hover)
- ✅ تنسيق الكود (Format Document)
- ✅ البحث عن المراجع (Find References)
- ✅ إعادة التسمية (Rename Symbol)

## الإعداد

### VS Code

ثبّت الإضافة — تُشغّل `sad-lsp` تلقائياً.

### Neovim (nvim-lspconfig)

```lua
require('lspconfig').sad_lsp.setup {
  cmd        = { 'sad-lsp' },
  filetypes  = { 'sad' },
  root_dir   = require('lspconfig.util').find_git_ancestor,
}
```

### تشغيل يدوي

```bash
sad-lsp --stdio   # وضع stdio للمحررات
sad-lsp --socket 6009  # وضع socket
```
