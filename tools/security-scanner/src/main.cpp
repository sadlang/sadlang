// ============================================================================
// tools/security-scanner/src/main.cpp
// (AR) نقطة دخول رفيعة لـ sad-guard. تستدعي Sad::Security::guard_main.
// (EN) Thin entry point for sad-guard. Delegates to Sad::Security::guard_main.
// ============================================================================
//
// (AR) لماذا ملف main منفصل؟
//   - guard_main معرّف داخل namespace Sad::Security ليبقى قابلاً للاستدعاء
//     من اختبارات unit (test_security_scanner) ومن أدوات أخرى مستقبلاً.
//   - main العمومي مطلوب من المُجمِّع لإنشاء التنفيذي.
// (EN) Why a separate main file?
//   - guard_main lives in namespace Sad::Security so it remains reachable from
//     unit tests (test_security_scanner) and any future host tool.
//   - The linker still needs a global ::main to produce the executable.
// ============================================================================

#include "security_scanner.h"

int main(int argc, char* argv[]) {
    return Sad::Security::guard_main(argc, argv);
}
