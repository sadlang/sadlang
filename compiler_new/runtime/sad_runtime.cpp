// ========================================================================
// Sad Language Runtime Support Library - Implementation
// مكتبة دعم وقت التشغيل للغة ص - التنفيذ
// ========================================================================

#include "sad_runtime.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

extern "C" {

// Sleep implementation (platform-specific)
void sad_sleep(double seconds) {
    #ifdef _WIN32
        Sleep((DWORD)(seconds * 1000));
    #else
        usleep((useconds_t)(seconds * 1000000));
    #endif
}

} // extern "C"
