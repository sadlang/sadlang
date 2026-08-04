"""
============================================================================
(AR) test_ui_props_guard.py — اختباراتُ ماسحِ السلاسلِ في حارسِ اتّساقِ الواجهة.

     الحارسُ يمنعُ وجودَ اسمِ عقدةٍ/حدثٍ **حرفيًّا** في مستهلِكي المسارِ التصريحيّ
     (قرار مالك: كلُّ اسمٍ يراه الكودُ يأتي مولَّدًا من مصدرِ الحقيقة). وقيمةُ
     مِثلِ هذا الحارسِ في ما **لا** يُخدَع به، لا في مرورِه على الشجرةِ النظيفة —
     فهذه الاختباراتُ تُثبّت الطرقَ التي كان يُخدَع بها قبلَ تقويتِه.
(EN) Tests for the guard's C++ string scanner: what it must NOT be fooled by.
============================================================================
"""
from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
_GUARD = Path(__file__).with_name("check_ui_props_consistency.py")


def _load_guard():
    spec = importlib.util.spec_from_file_location("ui_props_guard", _GUARD)
    assert spec and spec.loader, "تعذّر تحميل حارس اتّساق الواجهة"
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


guard = _load_guard()

_B = chr(92)  # (AR) شرطةٌ مائلةٌ خلفيّة — تُبنى برمزها كي لا تلتبس بهروبِ بايثون
_HEX_BUTTON = f'"{_B}xd8{_B}xb2{_B}xd8{_B}xb1"'   # (AR) «زر» بترميز البايتات
_BUTTON = "زر"


@pytest.mark.parametrize(
    "source",
    [
        pytest.param(f"const char *p = {_HEX_BUTTON};", id="hex-escaped-bytes"),
        pytest.param('const char *p = R"(زر)";', id="raw-string"),
        pytest.param('const char *p = "ز" "ر";', id="adjacent-concat"),
        pytest.param('const char *p = "زر";', id="plain"),
        pytest.param(f'const char *p = u8{_HEX_BUTTON};', id="u8-prefix"),
    ],
)
def test_scanner_is_not_fooled(source: str):
    # (AR) الطرقُ الأربعُ التي كانت تُمرِّر الاسمَ من تحتِ التعبيرِ النمطيّ القديم.
    assert _BUTTON in guard._arabic_literals(source)  # noqa: SLF001


def test_comment_is_not_a_literal():
    # (AR) اسمٌ في تعليقٍ ليس نصًّا في الكود — لا يُبلَّغ عنه.
    assert guard._arabic_literals("// زر\n/* زر */") == []  # noqa: SLF001


def test_slashes_inside_a_string_do_not_start_a_comment():
    # (AR) كنسُ التعليقاتِ بتعبيرٍ نمطيٍّ كان يبتر بقيّةَ السطرِ الحيّ بعد `//`
    #      الواقعةِ داخلَ سلسلة، فيعمى الحارسُ عمّا بعدها.
    src = 'const char *a = "x//y"; const char *b = "زر";'
    assert _BUTTON in guard._arabic_literals(src)  # noqa: SLF001


def test_escaped_backslash_char_literal_keeps_the_scanner_in_sync():
    # (AR) `'\\\\'` أربعةُ محارف؛ قفزٌ ثابتٌ بثلاثةٍ يقع على علامةِ الإغلاقِ فيفتح
    #      حرفًا وهميًّا ويبتلع بقيّةَ الملفّ صامتًا — وهو أخطرُ فشلٍ ممكنٍ لحارس.
    src = "char c = '" + _B + _B + "'; const char *p = \"زر\";"
    assert _BUTTON in guard._arabic_literals(src)  # noqa: SLF001


def test_unterminated_quote_does_not_swallow_the_rest_of_the_file():
    # (AR) اقتباسٌ يتيمٌ في سطرٍ لا يُخرِس ما بعده.
    src = 'int x; // "\nconst char *p = "زر";'
    assert _BUTTON in guard._arabic_literals(src)  # noqa: SLF001


def test_macro_names_are_exact_not_substrings():
    # (AR) «قائمة» رمزُها جزءٌ من رمزِ «عند_القائمة_السياقية»؛ المطابقةُ بالاحتواء
    #      تُبلّغ عن عقدةٍ غيرِ موجودةٍ في القائمة. الاستخراجُ يجب أن يكون بالمدخلات.
    header = (
        "#define SAD_UI_PARSER_EVENT_LIST(X) " + _B + "\n"
        '    X(OnContextMenu, "عند_القائمة_السياقية")\n'
        "#define SAD_UI_PARSER_EVENT_COUNT 1\n"
    )
    names = guard._macro_names(header, "SAD_UI_PARSER_EVENT_LIST")  # noqa: SLF001
    assert names == {"عند_القائمة_السياقية"}
    assert "قائمة" not in names


def test_primary_prop_list_covers_the_widgets_that_carry_an_argument():
    """(AR) جدولُ الخاصّيّةِ الأولى يغطّي العناصرَ التي تأخذ وسيطًا موضعيًّا.

    (AR) هذه المدخلاتُ الأربعُ هي عينُ العيب: شرطٌ حرفيٌّ في مُقيِّمِ المفسّر كان
         يقارن بـ«نص» بينما الاسمُ القانونيُّ «نص_عنصر»، فسقط وسيطُ العنصرِ في
         مفتاحٍ احتياطيٍّ لا قارئَ له ⇒ نصٌّ فارغٌ على الشاشةِ بلا خطأ. إفراغُ
         الجدولِ أو إسقاطُ مدخلةٍ منه يُعيد العيبَ صامتًا، فنُثبّته هنا.
    (EN) Pin the four entries whose loss produced a silently empty widget.
    """
    header = (
        ROOT / "features" / "graphics" / "core" / "include" / "sad_ui"
        / "generated" / "node_types_generated.h"
    ).read_text(encoding="utf-8")
    body = header[header.index("SAD_UI_NODE_PRIMARY_PROP_LIST(X)"):]
    body = body[: body.index("#define SAD_UI_NODE_PRIMARY_PROP_COUNT")]
    for node_id, prop_key in (
        ("Text", "CONTENT"),
        ("Button", "TITLE"),
        ("Image", "SOURCE"),
        ("TextField", "HINT"),
    ):
        assert f"X({node_id}, {prop_key})" in body, (
            f"العقدة {node_id} فقدت مفتاحَ خاصّيّتِها الأولى ({prop_key}) — "
            "وسيطُها سيسقط في مفتاحٍ لا قارئَ له"
        )


def test_guard_passes_on_the_current_tree():
    # (AR) تكامل: الشجرةُ الحاليّةُ يجب أن تمرّ (لا تنبيهاتٍ كاذبة).
    assert guard.main() == 0


if __name__ == "__main__":
    sys.exit(pytest.main([__file__, "-v"]))
