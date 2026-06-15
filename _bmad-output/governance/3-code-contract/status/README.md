# status/ — تقارير الحالة (3-code-contract)

| الملف | الحالة |
|---|---|
| [implementation_status.md](implementation_status.md) | 🔴 **OUT-OF-DATE** — يدَّعي 156 ثابتاً + Epic 2 منجَز، لكن الفحص في 2026-05-30 وجد: `shared/include/sad_invariant.h` غير موجود، 0 مواضع `SAD_INVARIANT_DEF` في الكود، `scripts/lint/` غير موجود. انظر [../../1-policy/status/VERIFICATION_REPORT_2026-05-30.md](../../1-policy/status/VERIFICATION_REPORT_2026-05-30.md) القسم 2. |

## القاعدة

- أي ادعاء إنجاز في `implementation_status.md` يجب أن يكون مدعوماً بمسار ملف + رقم سطر.
- عند اكتشاف drift بين الادعاء والواقع، تُضاف شارة **OUT-OF-DATE** فوراً في رأس الملف، ولا يُعدَّل المحتوى الأصلي حتى لا يُفقد سياق الانحراف.
