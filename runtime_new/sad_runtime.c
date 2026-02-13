/* sad_runtime.c - مكتبة وقت التشغيل لبرامج لغة ص المُترجمة */
/* Sad Language Compiled Runtime Library */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

/* إعداد UTF-8 على ويندوز */
__attribute__((constructor))
void sad_runtime_init(void) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
}

/* قراءة نص من المستخدم / Read string from user */
const char* sad_llvm_input(void) {
    static char buffer[4096];
    fflush(stdout);
    if (fgets(buffer, sizeof(buffer), stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
        return buffer;
    }
    return "";
}

/* قراءة عدد صحيح / Read integer from user */
long long sad_llvm_input_int(void) {
    long long val = 0;
    fflush(stdout);
    scanf("%lld", &val);
    return val;
}

/* قراءة عدد عشري / Read float from user */
double sad_llvm_input_float(void) {
    double val = 0.0;
    fflush(stdout);
    scanf("%lf", &val);
    return val;
}
