// ═══════════════════════════════════════════════════════════════════
// صNet — طبقة Freestanding — التنفيذ
// (AR) تعريف المتغيرات العالمية لبيئة بدون نظام تشغيل
// المكون: network/sadnet (توافق Freestanding)
// المسار: network/sadnet/src/freestanding/freestanding.cpp
// ═══════════════════════════════════════════════════════════════════

#ifdef SADNET_FREESTANDING

#include "sadnet/freestanding/freestanding.h"

namespace sad::net::freestanding
{

    // (AR) مؤشرات الكولباك العالمية — تُهيَّأ عبر init()
    NetworkCallbacks g_callbacks = {};

} // namespace sad::net::freestanding

#endif // SADNET_FREESTANDING
