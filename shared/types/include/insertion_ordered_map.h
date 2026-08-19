#pragma once

/**
 * @file insertion_ordered_map.h
 * @brief (AR) خريطةٌ تحفظُ ترتيبَ الإدخال — بديلٌ مباشرٌ لـ`std::unordered_map`
 *        (EN) An insertion-ordered map — a drop-in replacement for std::unordered_map
 *
 * (AR) 🔑 **لماذا وُجِدَت.** كانت خريطةُ المفسّرِ `std::unordered_map`، فترتيبُ
 *      مرورِها ليس قرارَ لغةِ ص بل قرارَ مكتبةِ المنصّة — وقد قرّرَته الثلاثُ
 *      ثلاثةَ قراراتٍ مختلفة. القياس (٢٠٢٦-٠٨-١٨، مصفوفةُ القواعدِ على CI):
 *      ستّةُ اختباراتٍ خضراءُ سنتين على ويندوز وماك، حمراءُ على لينكس — لأنّ
 *      قائمةَ libstdc++ الأحاديّةَ تُدرِجُ في الرأسِ حين تكونُ السَّلّةُ خالية.
 *      وشكلُ الحمرةِ نفسُه كان البرهان: خرائطُ العنصرَين والثلاثةِ خرجت معكوسةً
 *      **تمامًا**، وذاتُ الأربعةِ خرجت مبعثرةً لا معكوسة — توقيعُ تصادُمِ سَلّةٍ
 *      لا توقيعُ عكس.
 *
 * (AR) ⚠️ **والترتيبُ هنا عقدٌ لا تفصيلُ تنفيذ.** الخلفيّةُ تحفظُ ترتيبَ الإدخالِ
 *      بالبناء (مصفوفةٌ خطّيّة)، فبقاءُ المفسّرِ بلا ترتيبٍ يجعلُ المحرّكَين
 *      يفترقان على برنامجٍ صحيحٍ — وهو ما تقيسُه بوّابةُ التكافؤ.
 *
 * (EN) Why this exists: the map behind every Sad value was a std::unordered_map, so
 *      its iteration order was the platform standard library decision, not the
 *      language one — and the three platforms decided differently. Measured on CI
 *      (2026-08-18): six rules-matrix tests green for two years on Windows and macOS,
 *      red on Linux. The shape of the red was itself the proof: two- and three-entry
 *      maps came out EXACTLY reversed while a four-entry map came out scattered — the
 *      signature of a bucket collision, not of a reversal.
 *      Order here is a contract, not an implementation detail: the compiled backend
 *      preserves insertion order by construction, so an unordered interpreter makes
 *      the two engines disagree on a correct program.
 *
 * (AR) التصميم: قائمةُ مدخلاتٍ بترتيبِ الإدخال + فهرسٌ مُهشَّرٌ من المفتاحِ إلى
 *      **مُكرِّرِ** مدخلتِه. فالبحثُ والإدراجُ والحذفُ O(1) وسطًا كما كانت كلُّها
 *      في `unordered_map`، ويبقى المرورُ بترتيبِ الإدخال. والقائمةُ لا المتّجهُ
 *      لأنّ الحاويةَ السابقةَ كانت تضمنُ **ثباتَ المراجع** والشيفرةُ مبنيّةٌ عليه.
 * (EN) Design: an insertion-ordered entry list plus a hash index from key to its
 *      ITERATOR. Lookup, insert and erase all stay average-O(1) exactly as they were
 *      in unordered_map, and iteration follows insertion order. A list rather than a
 *      vector because the previous container guaranteed reference stability and the
 *      surrounding code is written against that guarantee.
 */

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <list>
#include <string>
#include <unordered_map>
#include <utility>

namespace Sad
{
    namespace Types
    {
        /**
         * @brief (AR) خريطةٌ مرتّبةٌ بترتيبِ الإدخال / (EN) Insertion-ordered map
         * @tparam Key (AR) نوعُ المفتاح / (EN) key type
         * @tparam T (AR) نوعُ القيمة / (EN) mapped type
         *
         * (AR) ⚠️ `T` قد يكونُ نوعًا غيرَ مكتملٍ عند إنشاءِ القالبِ (تُستعمَلُ داخلَ
         *      `Value` نفسِها). ولذلك لا يُلمَسُ `T` إلّا داخلَ الدوالِّ الأعضاءِ التي
         *      لا تُستدعى إلّا بعدَ اكتمالِه — وهو ما تُجيزُه المعياريّةُ لـ`std::list`
         *      (وهي إحدى ثلاثِ حاوياتٍ فقط تُجيزُه: `vector` و`list` و`forward_list`).
         * (EN) T may be incomplete at instantiation (this is used inside Value itself),
         *      so T is only touched inside member functions, which are not called until
         *      it is complete — a guarantee std::list provides since C++17, one of only
         *      three containers that do (vector, list, forward_list).
         */
        template <typename Key, typename T>
        class InsertionOrderedMap
        {
        public:
            using key_type = Key;
            using mapped_type = T;
            using value_type = std::pair<Key, T>;

            // ══════════════════════════════
            // (AR) 🔑 قائمةٌ لا متّجه — والسببُ ليس الأداءَ بل **ثباتَ المراجع**.
            //      `std::unordered_map` التي حلَّت هذه محلَّها تضمنُ ألّا يُبطِلَ
            //      إدراجٌ مرجعًا قائمًا، والشيفرةُ القائمةُ كُتِبت على ذلك الضمان.
            //      ومتّجهٌ يُعيدُ التخصيصَ عند النموِّ فيُبطِلُ كلَّ مرجعٍ سابق —
            //      ⚠️ وهذا **سلوكٌ غيرُ معرَّفٍ صامتٌ** لا انهيارٌ يُقاس، من أخبثِ
            //      ما يُدخَلُ في حاويةٍ يمسُّها ٢٥٤ موضعًا.
            //      وربحٌ ثانٍ: الفهرسُ يحملُ مُكرِّراتٍ لا مواضعَ، فالحذفُ O(1)
            //      ولا يحتاجُ إعادةَ فهرسةٍ أصلًا.
            //      و`std::list` من الحاوياتِ الثلاثِ التي تُجيزُ المعياريّةُ
            //      إنشاءَها بنوعٍ غيرِ مكتمل — و`T` هنا غيرُ مكتملٍ فعلًا.
            // (EN) A list, not a vector — for REFERENCE STABILITY, not speed. The
            //      std::unordered_map this replaced guaranteed that an insert never
            //      invalidates an existing reference, and the existing code was
            //      written against that guarantee. A vector reallocates on growth and
            //      invalidates every prior reference: silent undefined behaviour, not
            //      a measurable crash — the worst thing to introduce into a container
            //      touched from 254 sites. Bonus: the index holds iterators rather
            //      than positions, so erase is O(1) with no reindexing. And list is
            //      one of the three containers the standard permits with an incomplete
            //      element type, which T genuinely is here.
            // ══════════════════════════════
            using container_type = std::list<value_type>;
            using iterator = typename container_type::iterator;
            using const_iterator = typename container_type::const_iterator;
            using size_type = std::size_t;

            InsertionOrderedMap() = default;

            // ══════════════════════════════
            // (AR) 🔴 **النسخُ يُعيدُ بناءَ الفهرسِ ولا ينسخُه.** الفهرسُ يحملُ
            //      مُكرِّراتٍ إلى عُقَدِ `entries_`، والمُنشئُ الضمنيُّ ينسخُها كما هي
            //      — فيُشيرُ فهرسُ **النسخةِ** إلى عُقَدِ **الأصل**. وهذا ليس نظريًّا:
            //      قِيس أنّ `خريطة_احذف` نقيّةٌ فتنسخ، فتعلّقَ المفسّرُ دونما
            //      انهيارٍ يُقرَأ. ⚙️ والنقلُ سليمٌ بلا عمل: `std::list` تنقلُ
            //      عُقَدَها فتبقى المُكرّراتُ صالحةً — ولذلك يُصرَّحُ بهما
            //      معًا: إعلانُ النسخِ يكبتُ النقلَ الضمنيَّ فينحدرُ إلى نسخٍ صامت.
            // (EN) Copying REBUILDS the index rather than copying it: the index holds
            //      iterators into entries_, and the implicit copy would leave the COPY's
            //      index pointing at the ORIGINAL's nodes. Not theoretical — measured:
            //      `خريطة_احذف` is pure and therefore copies, and the interpreter hung
            //      with no readable crash. Moves are correct with no work because
            //      std::list transfers its nodes, so iterators stay valid — and both are
            //      declared explicitly because declaring a copy suppresses the implicit
            //      move, which would silently degrade every move into a copy.
            // ══════════════════════════════
            InsertionOrderedMap(const InsertionOrderedMap &other) : entries_(other.entries_)
            {
                reindex();
            }

            InsertionOrderedMap &operator=(const InsertionOrderedMap &other)
            {
                if (this != &other)
                {
                    entries_ = other.entries_;
                    reindex();
                }
                return *this;
            }

            InsertionOrderedMap(InsertionOrderedMap &&) = default;
            InsertionOrderedMap &operator=(InsertionOrderedMap &&) = default;

            InsertionOrderedMap(std::initializer_list<value_type> init)
            {
                for (const auto &entry : init)
                {
                    insertOrAssign(entry.first, entry.second);
                }
            }

            template <typename InputIt>
            InsertionOrderedMap(InputIt first, InputIt last)
            {
                for (; first != last; ++first)
                {
                    insertOrAssign(first->first, first->second);
                }
            }

            // ══════════════════════════════════════════════════════════════
            // (AR) المرور — بترتيبِ الإدخالِ دائمًا / (EN) Iteration — always insertion order
            // ══════════════════════════════════════════════════════════════
            iterator begin() { return entries_.begin(); }
            iterator end() { return entries_.end(); }
            const_iterator begin() const { return entries_.begin(); }
            const_iterator end() const { return entries_.end(); }
            const_iterator cbegin() const { return entries_.cbegin(); }
            const_iterator cend() const { return entries_.cend(); }

            size_type size() const { return entries_.size(); }
            bool empty() const { return entries_.empty(); }

            void clear()
            {
                entries_.clear();
                index_.clear();
            }

            /// (AR) القائمةُ لا تحجز — والفهرسُ وحدَه ينتفعُ بالحجز.
            /// (EN) A list has nothing to reserve; only the index benefits.
            void reserve(size_type n) { index_.reserve(n); }

            // ══════════════════════════════════════════════════════════════
            // (AR) البحث / (EN) Lookup
            // ══════════════════════════════════════════════════════════════
            iterator find(const Key &key)
            {
                auto slot = index_.find(key);
                if (slot == index_.end())
                {
                    return entries_.end();
                }
                return slot->second;
            }

            const_iterator find(const Key &key) const
            {
                auto slot = index_.find(key);
                if (slot == index_.end())
                {
                    return entries_.end();
                }
                return const_iterator(slot->second);
            }

            size_type count(const Key &key) const { return index_.count(key); }

            bool contains(const Key &key) const { return index_.find(key) != index_.end(); }

            T &at(const Key &key) { return index_.at(key)->second; }
            const T &at(const Key &key) const { return index_.at(key)->second; }

            /// (AR) يُنشئُ المدخلةَ بقيمةٍ افتراضيّةٍ إن غابت — كـ`unordered_map` تمامًا.
            /// (EN) Default-constructs the entry when absent, exactly like unordered_map.
            T &operator[](const Key &key)
            {
                auto slot = index_.find(key);
                if (slot != index_.end())
                {
                    return slot->second->second;
                }
                entries_.emplace_back(key, T());
                iterator placed = std::prev(entries_.end());
                index_.emplace(key, placed);
                return placed->second;
            }

            // ══════════════════════════════════════════════════════════════
            // (AR) الإدراج — الموجودُ يُبقي **موضعَه** الأصليّ
            // (EN) Insert — an existing key keeps its ORIGINAL position
            //
            // (AR) 🔑 وهذا عقدُ `unordered_map` نفسُه ولا يجوزُ خرقُه: إعادةُ إسنادِ
            //      مفتاحٍ ليست إعادةَ إدخالِه، فلو قفزَ إلى الذيلِ لَتغيّرَ ترتيبُ
            //      خريطةٍ لم يُضَفْ إليها شيء.
            // (EN) Re-assigning a key is not re-inserting it; moving it to the tail
            //      would reorder a map nothing was added to.
            // ══════════════════════════════════════════════════════════════
            std::pair<iterator, bool> insert(const value_type &entry)
            {
                auto slot = index_.find(entry.first);
                if (slot != index_.end())
                {
                    return {slot->second, false};
                }
                entries_.push_back(entry);
                iterator placed = std::prev(entries_.end());
                index_.emplace(entry.first, placed);
                return {placed, true};
            }

            template <typename InputIt>
            void insert(InputIt first, InputIt last)
            {
                for (; first != last; ++first)
                {
                    insert(*first);
                }
            }

            template <typename... Args>
            std::pair<iterator, bool> emplace(Args &&...args)
            {
                return insert(value_type(std::forward<Args>(args)...));
            }

            template <typename... Args>
            std::pair<iterator, bool> try_emplace(const Key &key, Args &&...args)
            {
                auto slot = index_.find(key);
                if (slot != index_.end())
                {
                    return {slot->second, false};
                }
                entries_.emplace_back(key, T(std::forward<Args>(args)...));
                iterator placed = std::prev(entries_.end());
                index_.emplace(key, placed);
                return {placed, true};
            }

            std::pair<iterator, bool> insert_or_assign(const Key &key, const T &value)
            {
                return insertOrAssign(key, value);
            }

            // ══════════════════════════════════════════════════════════════
            // (AR) الحذف — يُطبِقُ الفجوةَ ويُعيدُ الفهرسة
            // (EN) Erase — closes the gap and reindexes
            //
            // (AR) ⚠️ الفهرسُ يحملُ **مواضعَ** لا مؤشّرات، فكلُّ موضعٍ بعدَ المحذوفِ
            //      ينقصُ واحدًا. إغفالُ هذا يُبقي فهرسًا يشيرُ إلى جارِ المدخلةِ لا
            //      إليها — جوابٌ خاطئٌ صامتٌ لا انهيار.
            // (EN) The index stores POSITIONS, so every position after the erased one
            //      decreases by one. Skipping that leaves the index pointing at the
            //      neighbour: a silent wrong answer, not a crash.
            // ══════════════════════════════════════════════════════════════
            size_type erase(const Key &key)
            {
                auto slot = index_.find(key);
                if (slot == index_.end())
                {
                    return 0;
                }
                entries_.erase(slot->second);
                index_.erase(slot);
                return 1;
            }

            iterator erase(const_iterator position)
            {
                if (position == entries_.cend())
                {
                    return entries_.end();
                }
                auto slot = index_.find(position->first);
                iterator following = entries_.erase(position);
                if (slot != index_.end())
                {
                    index_.erase(slot);
                }
                return following;
            }

            // ══════════════════════════════════════════════════════════════
            // (AR) المساواة — بالمحتوى لا بالترتيب، كـ`unordered_map`
            // (EN) Equality — by content, not by order, like unordered_map
            //
            // (AR) 🔑 عمدًا: ترتيبُ المرورِ عقدٌ، أمّا تساوي خريطتَين فمعناه تساوي
            //      أزواجِهما. ولو رُبِط التساوي بالترتيبِ لَصارت `{أ:1، ب:2}` مخالفةً
            //      لـ`{ب:2، أ:1}` وهو ليس ما تَعِدُ به اللغة.
            // (EN) Deliberate: iteration order is a contract, but two maps are equal
            //      when their pairs are equal. Tying equality to order would make
            //      {أ:1، ب:2} differ from {ب:2، أ:1}, which the language does not promise.
            // ══════════════════════════════════════════════════════════════
            bool operator==(const InsertionOrderedMap &other) const
            {
                if (entries_.size() != other.entries_.size())
                {
                    return false;
                }
                for (const auto &entry : entries_)
                {
                    auto found = other.find(entry.first);
                    if (found == other.end() || !(found->second == entry.second))
                    {
                        return false;
                    }
                }
                return true;
            }

            bool operator!=(const InsertionOrderedMap &other) const { return !(*this == other); }

        private:
            /// (AR) يُعيدُ بناءَ الفهرسِ من `entries_` — بعدَ كلِّ نسخٍ يُنشئُ عُقَدًا جديدة.
            /// (EN) Rebuilds the index from entries_ after any copy creates fresh nodes.
            void reindex()
            {
                index_.clear();
                index_.reserve(entries_.size());
                for (iterator it = entries_.begin(); it != entries_.end(); ++it)
                {
                    index_.emplace(it->first, it);
                }
            }

            std::pair<iterator, bool> insertOrAssign(const Key &key, const T &value)
            {
                auto slot = index_.find(key);
                if (slot != index_.end())
                {
                    slot->second->second = value;
                    return {slot->second, false};
                }
                entries_.emplace_back(key, value);
                iterator placed = std::prev(entries_.end());
                index_.emplace(key, placed);
                return {placed, true};
            }

            container_type entries_;                  ///< (AR) المدخلاتُ بترتيبِ الإدخال / (EN) entries in insertion order
            std::unordered_map<Key, iterator> index_; ///< (AR) المفتاحُ ← مُكرِّرُ مدخلتِه / (EN) key to its entry iterator
        };

    } // namespace Types
} // namespace Sad
