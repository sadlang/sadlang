"""
يستبدل النصوص العربية المشفرة hex في builtin_module_async.cpp بثوابت مُولَّدة
"""
import pathlib, sys
sys.stdout.reconfigure(encoding='utf-8')

f = pathlib.Path("interpreter/src/builtins/builtin_module_async.cpp")
text = f.read_text(encoding='utf-8')

NAME_TO_CONST = {
    "أنشئ_مهمة":      "std::string(Basync::ASYNC_SPAWN)",
    "انتظر_مهمة":     "std::string(Basync::ASYNC_AWAIT_TASK)",
    "نوم_غير_متزامن": "std::string(Basync::ASYNC_SLEEP)",
    "أنشئ_مستقبل":    "std::string(Basync::CREATE_FUTURE_ALT)",
    "أوف_مستقبل":     "std::string(Basync::FUTURE_OFF)",
    "حقق_مستقبل":     "std::string(Basync::FUTURE_RESOLVE)",
    "احصل_مستقبل":    "std::string(Basync::FUTURE_GET)",
    "أنشئ_قناة":      "std::string(Basync::CREATE_CHANNEL)",
    "أرسل_قناة":      "std::string(Basync::CHANNEL_SEND)",
    "استقبل_قناة":    "std::string(Basync::CHANNEL_RECV)",
    "أغلق_قناة":      "std::string(Basync::CHANNEL_CLOSE)",
    "أنشئ_قفل":       "std::string(Basync::CREATE_MUTEX)",
    "افتح_قفل":       "std::string(Basync::MUTEX_UNLOCK)",
    "أنشئ_خيط":       "std::string(Basync::CREATE_THREAD)",
    "انضم_خيط":       "std::string(Basync::THREAD_JOIN)",
    "أنشئ_ذري":       "std::string(Basync::CREATE_ATOMIC)",
    "حمل_ذري":         "std::string(Basync::ATOMIC_LOAD)",
    "خزن_ذري":         "std::string(Basync::ATOMIC_STORE)",
    "أضف_ذري":         "std::string(Basync::ATOMIC_ADD)",
    "قارن_وبدل":       "std::string(Basync::ATOMIC_CAS)",
    "انتظر_أي":        "std::string(Basync::WAIT_ANY)",
    "اختر_قناة":      "std::string(Basync::CHANNEL_SELECT)",
    "هو_مثيل":        "std::string(Basync::IS_INSTANCE)",
    "مثيل_من":         "std::string(Basync::INSTANCE_OF)",
    "نوع_الكائن":     "std::string(Basync::OBJECT_TYPE)",
    "حقول_الكائن":    "std::string(Basync::OBJECT_FIELDS)",
    "انتظر_الكل":     "std::string(Basync::WAIT_ALL_ALT)",
}


def to_hex_escaped(s: str) -> str:
    # (AR) الأحرف ASCII (مثل _) تبقى حرفية، غير ASCII تُشفَّر hex
    parts = []
    for ch in s:
        if ord(ch) < 128:
            parts.append(ch)
        else:
            for b in ch.encode('utf-8'):
                parts.append('\\' + 'x' + f'{b:02x}')
    return ''.join(parts)


count = 0
for arabic, const in NAME_TO_CONST.items():
    hex_str = to_hex_escaped(arabic)
    old = 'registerBuiltinFunction("' + hex_str + '",'
    new = f'registerBuiltinFunction({const},'
    n = text.count(old)
    if n > 0:
        text = text.replace(old, new)
        count += n
        print(f"  '{arabic}': {n}")

f.write_text(text, encoding='utf-8')
print(f"\nTotal: {count}")
