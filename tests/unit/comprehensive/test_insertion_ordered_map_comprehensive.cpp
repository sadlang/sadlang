/**
 * @file test_insertion_ordered_map_comprehensive.cpp
 * @brief (AR) اختبارات عقدِ `InsertionOrderedMap` — الحاويةِ التي تحملُ خرائطَ لغةِ ص
 *        (EN) Contract tests for InsertionOrderedMap, the container behind every Sad map
 *
 * (AR) 🔑 **لماذا في هذه الطبقةِ لا عبرَ المفسّر.** هذه الحاويةُ أُنزِلَت في
 *      ٢٠٢٦-٠٨-١٩ بديلًا عن `std::unordered_map` (ISSUE-180)، وأُدخِلَ فيها أثناءَ
 *      إنزالِها عطبٌ **لم يرَه المُصرِّفُ ولا كانت مراجعةٌ لتراه**: الفهرسُ يحملُ
 *      مُكرِّراتٍ إلى عُقَدِ القائمة، والمُنشئُ الضمنيُّ للنسخِ ينسخُها حرفيًّا —
 *      فيُشيرُ فهرسُ **النسخةِ** إلى عُقَدِ **الأصل**. ولأنّ `خريطة_احذف` نقيّةٌ
 *      فتنسخ، تعلّقَ المفسّرُ **صامتًا بلا انهيارٍ يُقرَأ** على أوّلِ اختبارٍ يحذف.
 *      كشفَه التشغيلُ وحدَه بعد بناءٍ كامل؛ وأيُّ اختبارٍ من هذا الملفِّ كان
 *      سيمسكَه في ثانية. فهذا الملفُّ هو الدرسُ مُدوَّنًا حارسًا لا نثرًا.
 *
 * (EN) Why at this layer rather than through the interpreter: this container replaced
 *      std::unordered_map on 2026-08-19 (ISSUE-180), and while landing it a defect was
 *      introduced that neither the compiler nor a review would catch — the index holds
 *      iterators into the list, and the implicit copy constructor copies them verbatim,
 *      leaving the COPY's index pointing at the ORIGINAL's nodes. Because `خريطة_احذف`
 *      is pure and therefore copies, the interpreter hung SILENTLY, with no readable
 *      crash. Only running it found that, after a full build; any test in this file
 *      would have caught it in a second.
 */

#include "sad_test_framework.h"
#include "insertion_ordered_map.h"

#include <string>
#include <vector>

using Sad::Types::InsertionOrderedMap;

namespace {
    using OrderedMap = InsertionOrderedMap<std::string, int>;

    /// (AR) يجمعُ المفاتيحَ بترتيبِ المرور — فالمرورُ هو موضوعُ العقد.
    /// (EN) Collects keys in traversal order — traversal is what the contract is about.
    std::vector<std::string> keys_of(const OrderedMap &map)
    {
        std::vector<std::string> keys;
        for (const auto &entry : map)
        {
            keys.push_back(entry.first);
        }
        return keys;
    }

    std::string joined(const OrderedMap &map)
    {
        std::string out;
        for (const auto &key : keys_of(map))
        {
            if (!out.empty())
            {
                out += ",";
            }
            out += key;
        }
        return out;
    }
} // namespace

// ======================================================================
int main() {
    SAD_TEST_INIT();

    // ══════════════════════════════════════════════════════════════════
    // (AR) المرورُ بترتيبِ الإدخال — جوهرُ العقد
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("IOM.Order / الترتيب");

    SAD_TEST("IOM01: المرورُ بترتيبِ الإدخالِ لا بترتيبِ المفاتيح", {
        OrderedMap map;
        map["ي"] = 1;
        map["أ"] = 2;
        map["م"] = 3;
        SAD_ASSERT_EQ(joined(map), std::string("ي,أ,م"));
    });

    SAD_TEST("IOM02: إعادةُ إسنادِ مفتاحٍ موجودٍ لا تُحرّكُ موضعَه", {
        // (AR) 🔑 إعادةُ الإسنادِ ليست إعادةَ إدخال. لو قفزَ المفتاحُ إلى الذيلِ
        //      لتغيّرَ ترتيبُ خريطةٍ لم يُضَفْ إليها شيءٌ أصلًا.
        OrderedMap map;
        map["أ"] = 1;
        map["ب"] = 2;
        map["ج"] = 3;
        map["أ"] = 99;
        SAD_ASSERT_EQ(joined(map), std::string("أ,ب,ج"));
        SAD_ASSERT_EQ(map.at("أ"), 99);
    });

    SAD_TEST("IOM03: الحذفُ يُطبِقُ الفجوةَ ولا يبتلعُ الإدراجَ التالي", {
        // (AR) نظيرُ ISSUE-181 في طبقةِ الحاوية: «د» يجبُ أن يلحقَ لا أن يتصدّر.
        OrderedMap map;
        map["أ"] = 1;
        map["ب"] = 2;
        map["ج"] = 3;
        SAD_ASSERT_EQ(map.erase("أ"), (size_t)1);
        map["د"] = 4;
        SAD_ASSERT_EQ(joined(map), std::string("ب,ج,د"));
    });

    SAD_TEST("IOM04: حذفُ مفتاحٍ غائبٍ لا يُغيّرُ شيئًا", {
        OrderedMap map;
        map["أ"] = 1;
        SAD_ASSERT_EQ(map.erase("غائب"), (size_t)0);
        SAD_ASSERT_EQ(map.size(), (size_t)1);
        SAD_ASSERT_EQ(joined(map), std::string("أ"));
    });

    // ══════════════════════════════════════════════════════════════════
    // (AR) النسخُ — العطبُ الذي وقعَ فعلًا
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("IOM.Copy / النسخ");

    SAD_TEST("IOM05: النسخةُ مستقلّةٌ عن الأصل — تعديلُ الأصلِ لا يمسُّها", {
        // (AR) 🔑 هذا بالضبط ما تعلّقَ عليه المفسّر: فهرسُ النسخةِ كان يُشيرُ إلى
        //      عُقَدِ الأصل. والاختبارُ يمسُّ **قيمةً** من النسخةِ بعدَ تحريكِ الأصل،
        //      فلو بقيَ الفهرسُ معلَّقًا لَقرأ عُقَدًا ليست له.
        OrderedMap original;
        original["أ"] = 1;
        original["ب"] = 2;

        OrderedMap copy = original;
        original["ج"] = 3;
        original.erase("أ");
        original["ب"] = 77;

        SAD_ASSERT_EQ(joined(copy), std::string("أ,ب"));
        SAD_ASSERT_EQ(copy.at("أ"), 1);
        SAD_ASSERT_EQ(copy.at("ب"), 2);
        SAD_ASSERT_EQ(copy.size(), (size_t)2);
    });

    SAD_TEST("IOM06: النسخةُ قابلةٌ للتعديلِ ولا تمسُّ الأصل", {
        OrderedMap original;
        original["أ"] = 1;
        original["ب"] = 2;

        OrderedMap copy = original;
        copy["ج"] = 3;
        copy.erase("أ");

        SAD_ASSERT_EQ(joined(original), std::string("أ,ب"));
        SAD_ASSERT_EQ(joined(copy), std::string("ب,ج"));
    });

    SAD_TEST("IOM07: الإسنادُ بالنسخِ يُعيدُ بناءَ الفهرسِ كذلك", {
        OrderedMap original;
        original["س"] = 5;
        original["ع"] = 6;

        OrderedMap target;
        target["قديم"] = 1;
        target = original;
        original["ن"] = 7;

        SAD_ASSERT_EQ(joined(target), std::string("س,ع"));
        SAD_ASSERT_TRUE(target.find("قديم") == target.end());
        SAD_ASSERT_TRUE(target.find("س") != target.end());
        SAD_ASSERT_EQ(target.at("ع"), 6);
    });

    SAD_TEST("IOM08: النقلُ يحفظُ الترتيبَ والبحث", {
        // (AR) `std::list` تنقلُ عُقَدَها فتبقى المُكرِّراتُ صالحة — والنقلُ
        //      مُصرَّحٌ به عمدًا لأنّ إعلانَ النسخِ يكبتُه فينحدرُ إلى نسخٍ صامت.
        OrderedMap source;
        source["أ"] = 1;
        source["ب"] = 2;
        source["ج"] = 3;

        OrderedMap moved = std::move(source);
        SAD_ASSERT_EQ(joined(moved), std::string("أ,ب,ج"));
        SAD_ASSERT_EQ(moved.at("ب"), 2);
        SAD_ASSERT_TRUE(moved.find("ج") != moved.end());
    });

    // ══════════════════════════════════════════════════════════════════
    // (AR) ثباتُ المراجع — سببُ اختيارِ القائمةِ على المتّجه
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("IOM.Refs / المراجع");

    SAD_TEST("IOM09: مرجعٌ مأخوذٌ يبقى صالحًا بعدَ إدراجٍ كثير", {
        // (AR) 🔑 `std::unordered_map` التي حلَّت هذه محلَّها تضمنُ ثباتَ المراجع،
        //      والشيفرةُ القائمةُ مبنيّةٌ عليه. ومتّجهٌ يُعيدُ التخصيصَ يُبطِلُها
        //      **صامتًا بلا انهيار** — فهذا هو الحارسُ على ذلك القرار.
        OrderedMap map;
        map["مرساة"] = 10;
        int &anchor = map["مرساة"];

        for (int i = 0; i < 500; ++i)
        {
            map["ح" + std::to_string(i)] = i;
        }

        SAD_ASSERT_EQ(anchor, 10);
        anchor = 42;
        SAD_ASSERT_EQ(map.at("مرساة"), 42);
    });

    SAD_TEST("IOM10: مرجعٌ مأخوذٌ يبقى صالحًا بعدَ حذفِ غيرِه", {
        OrderedMap map;
        map["أ"] = 1;
        map["ب"] = 2;
        map["ج"] = 3;
        int &kept = map["ب"];

        map.erase("أ");
        map.erase("ج");

        SAD_ASSERT_EQ(kept, 2);
        SAD_ASSERT_EQ(map.size(), (size_t)1);
        SAD_ASSERT_EQ(joined(map), std::string("ب"));
    });

    // ══════════════════════════════════════════════════════════════════
    // (AR) بقيّةُ عقدِ `unordered_map` التي يُعتمَدُ عليها
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("IOM.Api / الواجهة");

    SAD_TEST("IOM11: البحثُ والعدُّ والاحتواء", {
        OrderedMap map;
        map["أ"] = 1;
        SAD_ASSERT_TRUE(map.find("أ") != map.end());
        SAD_ASSERT_TRUE(map.find("غائب") == map.end());
        SAD_ASSERT_EQ(map.count("أ"), (size_t)1);
        SAD_ASSERT_EQ(map.count("غائب"), (size_t)0);
        SAD_ASSERT_TRUE(map.contains("أ"));
        SAD_ASSERT_TRUE(!map.contains("غائب"));
    });

    SAD_TEST("IOM12: `[]` يُنشئُ المدخلةَ بقيمةٍ افتراضيّةٍ عند الغياب", {
        OrderedMap map;
        SAD_ASSERT_EQ(map["جديد"], 0);
        SAD_ASSERT_EQ(map.size(), (size_t)1);
    });

    SAD_TEST("IOM13: `insert` لا يدهسُ الموجودَ ويُبلّغُ بذلك", {
        OrderedMap map;
        map["أ"] = 1;
        auto outcome = map.insert(std::make_pair(std::string("أ"), 99));
        SAD_ASSERT_TRUE(!outcome.second);
        SAD_ASSERT_EQ(map.at("أ"), 1);

        auto fresh = map.insert(std::make_pair(std::string("ب"), 2));
        SAD_ASSERT_TRUE(fresh.second);
        SAD_ASSERT_EQ(joined(map), std::string("أ,ب"));
    });

    SAD_TEST("IOM14: التساوي بالمحتوى لا بالترتيب", {
        // (AR) عمدًا: ترتيبُ المرورِ عقدٌ، أمّا تساوي خريطتَين فتساوي أزواجِهما.
        OrderedMap first;
        first["أ"] = 1;
        first["ب"] = 2;

        OrderedMap second;
        second["ب"] = 2;
        second["أ"] = 1;

        SAD_ASSERT_TRUE(first == second);
        SAD_ASSERT_TRUE(!(first != second));
        SAD_ASSERT_EQ(joined(first), std::string("أ,ب"));
        SAD_ASSERT_EQ(joined(second), std::string("ب,أ"));
    });

    SAD_TEST("IOM15: المسحُ يُفرِغُ الفهرسَ كما يُفرِغُ المدخلات", {
        // (AR) لو مُسِحت المدخلاتُ دونَ الفهرسِ لبقيَ `find` يجدُ مفتاحًا محذوفًا
        //      ويُعيدُ مُكرِّرًا معلَّقًا — وهذا سببُ الفحصِ بعد المسحِ لا قبلَه.
        OrderedMap map;
        map["أ"] = 1;
        map["ب"] = 2;
        map.clear();

        SAD_ASSERT_TRUE(map.empty());
        SAD_ASSERT_EQ(map.size(), (size_t)0);
        SAD_ASSERT_TRUE(map.find("أ") == map.end());
        map["ج"] = 3;
        SAD_ASSERT_EQ(joined(map), std::string("ج"));
    });

    SAD_TEST("IOM16: خريطةٌ فارغةٌ لا تمرُّ ولا تجدُ", {
        OrderedMap map;
        SAD_ASSERT_TRUE(map.empty());
        SAD_ASSERT_TRUE(map.begin() == map.end());
        SAD_ASSERT_EQ(keys_of(map).size(), (size_t)0);
    });

    SAD_SUMMARY();
}
