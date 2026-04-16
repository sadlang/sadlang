// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// عرض مرئي لعناصر واجهة لغة ص — 87 عنصر
// Visual Demo: Sad Language UI Widgets (87 widgets)
// ═══════════════════════════════════════════════════════════════════════════════

import 'package:flutter/material.dart';

void main() {
  runApp(const SadUIDemo());
}

class SadUIDemo extends StatelessWidget {
  const SadUIDemo({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'عناصر واجهة لغة ص',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        colorSchemeSeed: const Color(0xFF1565C0),
        useMaterial3: true,
      ),
      home: const WidgetShowcase(),
    );
  }
}

// ═══════════════════════════════════════════════════════════════════════════════
// الصفحة الرئيسية
// ═══════════════════════════════════════════════════════════════════════════════

class WidgetShowcase extends StatefulWidget {
  const WidgetShowcase({super.key});
  @override
  State<WidgetShowcase> createState() => _WidgetShowcaseState();
}

class _WidgetShowcaseState extends State<WidgetShowcase> {
  int _selectedCategory = 0;

  static const categories = [
    _Cat('🏠', 'الكل', null),
    _Cat('📝', 'نصوص', 'نصوص'),
    _Cat('🖼️', 'وسائط', 'وسائط'),
    _Cat('🔘', 'أزرار', 'أزرار'),
    _Cat('📥', 'إدخال', 'إدخال'),
    _Cat('🎚️', 'منتقيات', 'منتقيات'),
    _Cat('⏳', 'تحميل', 'تحميل'),
    _Cat('📐', 'تخطيط', 'تخطيط'),
    _Cat('🏗️', 'هيكل', 'هيكل'),
    _Cat('📊', 'بيانات', 'بيانات'),
    _Cat('💬', 'حوارات', 'حوارات'),
    _Cat('🎬', 'وسائط_م', 'وسائط_متعددة'),
    _Cat('⚙️', 'متقدم', 'متقدم'),
    _Cat('🎞️', 'حركة', 'حركة'),
  ];

  @override
  Widget build(BuildContext context) {
    final filter = categories[_selectedCategory].filter;
    final widgets = filter == null
        ? allWidgets
        : allWidgets.where((w) => w.category == filter).toList();

    return Directionality(
      textDirection: TextDirection.rtl,
      child: Scaffold(
        appBar: AppBar(
          title: const Text('🧩 عناصر واجهة لغة ص — 87 عنصر'),
          centerTitle: true,
        ),
        body: Column(
          children: [
            SizedBox(
              height: 50,
              child: ListView.builder(
                scrollDirection: Axis.horizontal,
                reverse: true,
                itemCount: categories.length,
                itemBuilder: (ctx, i) {
                  final cat = categories[i];
                  return Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 4, vertical: 6),
                    child: ChoiceChip(
                      label: Text('${cat.icon} ${cat.name}'),
                      selected: i == _selectedCategory,
                      onSelected: (_) => setState(() => _selectedCategory = i),
                    ),
                  );
                },
              ),
            ),
            const Divider(height: 1),
            Padding(
              padding: const EdgeInsets.all(8),
              child: Text('${widgets.length} عنصر',
                  style: Theme.of(context).textTheme.bodySmall),
            ),
            Expanded(
              child: GridView.builder(
                padding: const EdgeInsets.all(12),
                gridDelegate: const SliverGridDelegateWithMaxCrossAxisExtent(
                  maxCrossAxisExtent: 400,
                  childAspectRatio: 1.3,
                  crossAxisSpacing: 12,
                  mainAxisSpacing: 12,
                ),
                itemCount: widgets.length,
                itemBuilder: (ctx, i) => _WCard(info: widgets[i]),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

// ═══════════════════════════════════════════════════════════════════════════════
// بطاقة عنصر واحد
// ═══════════════════════════════════════════════════════════════════════════════

class _WCard extends StatelessWidget {
  final _WI info;
  const _WCard({required this.info});

  @override
  Widget build(BuildContext context) {
    final cs = Theme.of(context).colorScheme;
    return Card(
      elevation: 2,
      clipBehavior: Clip.antiAlias,
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          Container(
            padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
            color: cs.primaryContainer,
            child: Row(
              children: [
                Text(info.icon, style: const TextStyle(fontSize: 18)),
                const SizedBox(width: 8),
                Text(info.name,
                    style: TextStyle(
                        fontWeight: FontWeight.bold,
                        color: cs.onPrimaryContainer)),
                const Spacer(),
                Container(
                  padding:
                      const EdgeInsets.symmetric(horizontal: 8, vertical: 2),
                  decoration: BoxDecoration(
                      color: cs.surface,
                      borderRadius: BorderRadius.circular(12)),
                  child: Text(info.category,
                      style: TextStyle(fontSize: 10, color: cs.onSurface)),
                ),
              ],
            ),
          ),
          Expanded(
            child: Padding(
              padding: const EdgeInsets.all(12),
              child: Center(child: info.builder(context)),
            ),
          ),
        ],
      ),
    );
  }
}

// ═══════════════════════════════════════════════════════════════════════════════
// أنواع مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

class _Cat {
  final String icon, name;
  final String? filter;
  const _Cat(this.icon, this.name, this.filter);
}

class _WI {
  final String name, category, icon;
  final Widget Function(BuildContext) builder;
  const _WI(this.name, this.category, this.icon, this.builder);
}

// ═══════════════════════════════════════════════════════════════════════════════
// جميع عناصر الواجهة الـ 87
// ═══════════════════════════════════════════════════════════════════════════════

final allWidgets = <_WI>[
  // نصوص (7)
  _WI('نص', 'نصوص', '📝', (_) => const Text('مرحباً بك في لغة ص!', style: TextStyle(fontSize: 16))),
  _WI('عنوان', 'نصوص', '🔤', (_) => const Text('عنوان رئيسي', style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold))),
  _WI('تسمية', 'نصوص', '🏷️', (_) => const Text('تسمية', style: TextStyle(fontSize: 12, color: Colors.grey))),
  _WI('فقرة', 'نصوص', '📄', (_) => const Text('هذه فقرة نصية طويلة نوعاً ما.', textAlign: TextAlign.justify)),
  _WI('رابط', 'نصوص', '🔗', (_) => const Text('اضغط هنا', style: TextStyle(color: Colors.blue, decoration: TextDecoration.underline))),
  _WI('كود', 'نصوص', '💻', (_) => Container(padding: const EdgeInsets.all(8), color: Colors.grey[200], child: const Text('اطبع("مرحبا")', style: TextStyle(fontFamily: 'monospace')))),
  _WI('اقتباس', 'نصوص', '💬', (_) => Container(padding: const EdgeInsets.all(8), decoration: const BoxDecoration(border: Border(right: BorderSide(color: Colors.blue, width: 3))), child: const Text('العلم نور', style: TextStyle(fontStyle: FontStyle.italic)))),

  // وسائط (5)
  _WI('صورة', 'وسائط', '🖼️', (_) => Container(width: 80, height: 80, color: Colors.grey[300], child: const Icon(Icons.image, size: 40, color: Colors.grey))),
  _WI('ايقونة', 'وسائط', '⭐', (_) => const Icon(Icons.star, size: 40, color: Colors.amber)),
  _WI('شارة', 'وسائط', '🔴', (_) => const Badge(label: Text('3'), child: Icon(Icons.mail, size: 32))),
  _WI('رقاقة', 'وسائط', '🏷️', (_) => const Chip(avatar: CircleAvatar(child: Text('ص')), label: Text('لغة ص'))),
  _WI('صورة_رمزية', 'وسائط', '👤', (_) => const CircleAvatar(radius: 30, backgroundColor: Colors.blue, child: Text('أ', style: TextStyle(fontSize: 24, color: Colors.white)))),

  // أزرار (6)
  _WI('زر', 'أزرار', '🔵', (_) => ElevatedButton(onPressed: () {}, child: const Text('اضغط هنا'))),
  _WI('زر_محيط', 'أزرار', '⭕', (_) => OutlinedButton(onPressed: () {}, child: const Text('زر بحدود'))),
  _WI('زر_نصي', 'أزرار', '📝', (_) => TextButton(onPressed: () {}, child: const Text('زر نصي'))),
  _WI('زر_ايقونة', 'أزرار', '⚙️', (_) => IconButton(onPressed: () {}, icon: const Icon(Icons.settings))),
  _WI('زر_عائم', 'أزرار', '➕', (_) => FloatingActionButton.small(onPressed: () {}, child: const Icon(Icons.add))),
  _WI('زر_تبديل', 'أزرار', '🔄', (_) => _ToggleBtn()),

  // إدخال (9)
  _WI('حقل', 'إدخال', '📥', (_) => const SizedBox(width: 200, child: TextField(decoration: InputDecoration(labelText: 'أدخل اسمك', border: OutlineInputBorder())))),
  _WI('حقل_بحث', 'إدخال', '🔍', (_) => const SizedBox(width: 200, child: SearchBar(hintText: 'ابحث هنا...', leading: Icon(Icons.search)))),
  _WI('حقل_سر', 'إدخال', '🔒', (_) => const SizedBox(width: 200, child: TextField(obscureText: true, decoration: InputDecoration(labelText: 'كلمة المرور', border: OutlineInputBorder())))),
  _WI('منطقة_نص', 'إدخال', '📑', (_) => const SizedBox(width: 200, child: TextField(maxLines: 3, decoration: InputDecoration(labelText: 'ملاحظات', border: OutlineInputBorder())))),
  _WI('مفتاح', 'إدخال', '🔛', (_) => _SwitchD()),
  _WI('منزلق', 'إدخال', '🎚️', (_) => _SliderD()),
  _WI('مربع_اختيار', 'إدخال', '☑️', (_) => _CheckD()),
  _WI('قائمة_منسدلة', 'إدخال', '📋', (_) => _DropD()),
  _WI('تقييم', 'إدخال', '⭐', (_) => _RatingD()),

  // منتقيات (8)
  _WI('منتقي_تاريخ', 'منتقيات', '📅', (ctx) => ElevatedButton.icon(onPressed: () => showDatePicker(context: ctx, firstDate: DateTime(2020), lastDate: DateTime(2030)), icon: const Icon(Icons.calendar_today), label: const Text('اختر تاريخ'))),
  _WI('منتقي_وقت', 'منتقيات', '🕐', (ctx) => ElevatedButton.icon(onPressed: () => showTimePicker(context: ctx, initialTime: TimeOfDay.now()), icon: const Icon(Icons.access_time), label: const Text('اختر وقت'))),
  _WI('منتقي_لون', 'منتقيات', '🎨', (_) => Wrap(spacing: 4, children: [Colors.red, Colors.blue, Colors.green, Colors.orange].map((c) => Container(width: 30, height: 30, decoration: BoxDecoration(color: c, shape: BoxShape.circle))).toList())),
  _WI('عجلة_ألوان', 'منتقيات', '🌈', (_) => const Icon(Icons.color_lens, size: 48, color: Colors.purple)),
  _WI('تقويم', 'منتقيات', '📆', (_) => const Icon(Icons.calendar_month, size: 48, color: Colors.teal)),
  _WI('زر_راديو', 'منتقيات', '🔘', (_) => _RadioD()),
  _WI('خطوات', 'منتقيات', '📶', (_) => _StepD()),
  _WI('تحكم_مقسم', 'منتقيات', '🔀', (_) => _SegD()),

  // تحميل (4)
  _WI('شريط_تقدم', 'تحميل', '📊', (_) => const SizedBox(width: 200, child: LinearProgressIndicator(value: 0.65))),
  _WI('تقدم_دائري', 'تحميل', '🔄', (_) => const SizedBox(width: 40, height: 40, child: CircularProgressIndicator(value: 0.7))),
  _WI('تحميل', 'تحميل', '⏳', (_) => const SizedBox(width: 40, height: 40, child: CircularProgressIndicator())),
  _WI('هيكل_تحميل', 'تحميل', '💀', (_) => Column(mainAxisSize: MainAxisSize.min, children: [Container(width: 180, height: 12, margin: const EdgeInsets.only(bottom: 8), decoration: BoxDecoration(color: Colors.grey[300], borderRadius: BorderRadius.circular(6))), Container(width: 120, height: 12, decoration: BoxDecoration(color: Colors.grey[300], borderRadius: BorderRadius.circular(6)))])),

  // تخطيط (18)
  _WI('عمود', 'تخطيط', '⬇️', (_) => const Column(mainAxisSize: MainAxisSize.min, children: [Text('عنصر 1'), Text('عنصر 2'), Text('عنصر 3')])),
  _WI('صف', 'تخطيط', '➡️', (_) => const Row(mainAxisSize: MainAxisSize.min, children: [Icon(Icons.star), SizedBox(width: 4), Text('صف')])),
  _WI('تكديس', 'تخطيط', '📚', (_) => SizedBox(width: 80, height: 60, child: Stack(children: [Positioned(left: 0, top: 0, child: Container(width: 50, height: 40, color: Colors.blue.withAlpha(128))), Positioned(left: 20, top: 15, child: Container(width: 50, height: 40, color: Colors.red.withAlpha(128)))]))),
  _WI('شبكة', 'تخطيط', '🔲', (_) => SizedBox(width: 120, height: 60, child: GridView.count(crossAxisCount: 3, shrinkWrap: true, physics: const NeverScrollableScrollPhysics(), children: List.generate(6, (i) => Container(margin: const EdgeInsets.all(2), color: Colors.blue[(i + 1) * 100]))))),
  _WI('حاوية', 'تخطيط', '📦', (_) => Container(width: 80, height: 50, decoration: BoxDecoration(color: Colors.blue[100], borderRadius: BorderRadius.circular(8), border: Border.all(color: Colors.blue)), child: const Center(child: Text('حاوية')))),
  _WI('توسيط', 'تخطيط', '🎯', (_) => Container(width: 80, height: 50, color: Colors.grey[200], child: const Center(child: Text('متوسط')))),
  _WI('مرن', 'تخطيط', '↔️', (_) => Row(mainAxisSize: MainAxisSize.min, children: [Container(width: 40, height: 30, color: Colors.red[200]), Container(width: 60, height: 30, color: Colors.blue[200])])),
  _WI('موسّع', 'تخطيط', '⬛', (_) => const Icon(Icons.open_in_full, size: 32)),
  _WI('فاصل', 'تخطيط', '↕️', (_) => const SizedBox(height: 20, child: Center(child: Text('↕️ فاصل')))),
  _WI('خط_فاصل', 'تخطيط', '➖', (_) => const SizedBox(width: 150, child: Divider(thickness: 2))),
  _WI('التفاف', 'تخطيط', '🔁', (_) => Wrap(spacing: 4, runSpacing: 4, children: ['أ', 'ب', 'ج', 'د'].map((c) => Chip(label: Text(c))).toList())),
  _WI('صندوق', 'تخطيط', '📥', (_) => Container(width: 60, height: 40, decoration: BoxDecoration(border: Border.all()), child: const Center(child: Text('صندوق')))),
  _WI('سطح', 'تخطيط', '🧱', (_) => const Card(elevation: 4, child: Padding(padding: EdgeInsets.all(12), child: Text('سطح')))),
  _WI('عمود_كسول', 'تخطيط', '⬇️', (_) => const Icon(Icons.view_agenda, size: 32, color: Colors.indigo)),
  _WI('صف_كسول', 'تخطيط', '➡️', (_) => const Icon(Icons.view_column, size: 32, color: Colors.indigo)),
  _WI('شبكة_كسولة', 'تخطيط', '🔲', (_) => const Icon(Icons.grid_view, size: 32, color: Colors.indigo)),
  _WI('عرض_تمرير', 'تخطيط', '📜', (_) => const Icon(Icons.unfold_more, size: 32, color: Colors.teal)),
  _WI('عرض_مقسم', 'تخطيط', '🔀', (_) => const Icon(Icons.view_sidebar, size: 32, color: Colors.brown)),

  // هيكل (14)
  _WI('هيكل', 'هيكل', '🏗️', (_) => Container(width: 150, height: 80, decoration: BoxDecoration(border: Border.all(color: Colors.blue), borderRadius: BorderRadius.circular(4)), child: Column(mainAxisSize: MainAxisSize.min, children: [Container(width: double.infinity, height: 24, color: const Color(0xFF1565C0), child: const Center(child: Text('AppBar', style: TextStyle(color: Colors.white, fontSize: 10)))), const Expanded(child: Center(child: Text('Body', style: TextStyle(fontSize: 10))))]))),
  _WI('شريط_تطبيق', 'هيكل', '📱', (_) => Container(width: 150, height: 30, color: const Color(0xFF1565C0), child: const Row(children: [SizedBox(width: 8), Icon(Icons.menu, color: Colors.white, size: 16), Spacer(), Text('العنوان', style: TextStyle(color: Colors.white, fontSize: 12)), Spacer()]))),
  _WI('شريط_سفلي', 'هيكل', '🔽', (_) => Container(width: 150, height: 35, decoration: BoxDecoration(color: Colors.white, boxShadow: [BoxShadow(color: Colors.grey.withAlpha(77), blurRadius: 4)]), child: const Row(mainAxisAlignment: MainAxisAlignment.spaceAround, children: [Icon(Icons.home, size: 18), Icon(Icons.search, size: 18), Icon(Icons.person, size: 18)]))),
  _WI('عنصر_تنقل', 'هيكل', '🧭', (_) => const Column(mainAxisSize: MainAxisSize.min, children: [Icon(Icons.home, size: 20), Text('الرئيسية', style: TextStyle(fontSize: 10))])),
  _WI('درج', 'هيكل', '📂', (_) => Container(width: 100, height: 80, decoration: BoxDecoration(color: Colors.white, border: Border.all(color: Colors.grey)), child: const Column(mainAxisSize: MainAxisSize.min, crossAxisAlignment: CrossAxisAlignment.start, children: [Padding(padding: EdgeInsets.all(4), child: Text('☰ القائمة', style: TextStyle(fontSize: 10, fontWeight: FontWeight.bold))), Divider(height: 1), Padding(padding: EdgeInsets.all(4), child: Text('الرئيسية', style: TextStyle(fontSize: 10))), Padding(padding: EdgeInsets.all(4), child: Text('الإعدادات', style: TextStyle(fontSize: 10)))]))),
  _WI('عنصر_درج', 'هيكل', '📋', (_) => const ListTile(dense: true, leading: Icon(Icons.home, size: 18), title: Text('عنصر'))),
  _WI('شريط_تبويب', 'هيكل', '📑', (_) => Container(width: 150, height: 30, color: const Color(0xFF1565C0), child: const Row(mainAxisAlignment: MainAxisAlignment.spaceAround, children: [Text('تبويب1', style: TextStyle(color: Colors.white, fontSize: 10)), Text('تبويب2', style: TextStyle(color: Colors.white70, fontSize: 10))]))),
  _WI('تبويب', 'هيكل', '📌', (_) => const Tab(icon: Icon(Icons.home, size: 16), text: 'الرئيسية')),
  _WI('محتوى_تبويب', 'هيكل', '📄', (_) => const Text('محتوى التبويب', style: TextStyle(color: Colors.grey))),
  _WI('عرض_تنقل', 'هيكل', '🧭', (_) => const Icon(Icons.navigation, size: 32, color: Colors.indigo)),
  _WI('رابط_تنقل', 'هيكل', '🔗', (_) => const Row(mainAxisSize: MainAxisSize.min, children: [Text('الرئيسية', style: TextStyle(color: Colors.blue, fontSize: 12)), Text(' > ', style: TextStyle(fontSize: 12)), Text('الصفحة', style: TextStyle(fontSize: 12))])),
  _WI('تنقل_جانبي', 'هيكل', '📍', (_) => const Icon(Icons.view_sidebar, size: 32, color: Colors.teal)),
  _WI('شريط_أدوات', 'هيكل', '🔧', (_) => Container(padding: const EdgeInsets.all(4), decoration: BoxDecoration(border: Border.all(color: Colors.grey), borderRadius: BorderRadius.circular(4)), child: const Row(mainAxisSize: MainAxisSize.min, children: [Icon(Icons.format_bold, size: 16), Icon(Icons.format_italic, size: 16), Icon(Icons.format_underline, size: 16)]))),
  _WI('قائمة_ضخمة', 'هيكل', '📋', (_) => const Icon(Icons.table_rows, size: 32, color: Colors.deepPurple)),

  // بيانات (6)
  _WI('قائمة', 'بيانات', '📋', (_) => Column(mainAxisSize: MainAxisSize.min, children: [Container(padding: const EdgeInsets.all(4), decoration: BoxDecoration(border: Border(bottom: BorderSide(color: Colors.grey[300]!))), child: const Row(children: [Icon(Icons.circle, size: 8), SizedBox(width: 4), Text('عنصر 1', style: TextStyle(fontSize: 12))])), Container(padding: const EdgeInsets.all(4), child: const Row(children: [Icon(Icons.circle, size: 8), SizedBox(width: 4), Text('عنصر 2', style: TextStyle(fontSize: 12))]))])),
  _WI('عنصر_قائمة_بيانات', 'بيانات', '📄', (_) => const ListTile(dense: true, leading: CircleAvatar(radius: 14, child: Text('أ')), title: Text('عنصر بيانات'), subtitle: Text('وصف', style: TextStyle(fontSize: 10)))),
  _WI('بطاقة', 'بيانات', '🃏', (_) => Card(child: Padding(padding: const EdgeInsets.all(12), child: Column(mainAxisSize: MainAxisSize.min, children: [const Text('بطاقة', style: TextStyle(fontWeight: FontWeight.bold)), Text('محتوى', style: TextStyle(color: Colors.grey[600], fontSize: 12))])))),
  _WI('قائمة_شبكية', 'بيانات', '🔲', (_) => const Icon(Icons.grid_on, size: 32, color: Colors.orange)),
  _WI('قائمة_تمرير', 'بيانات', '📜', (_) => const Icon(Icons.view_list, size: 32, color: Colors.green)),
  _WI('أكورديون', 'بيانات', '🪗', (_) => Container(width: 150, decoration: BoxDecoration(border: Border.all(color: Colors.grey), borderRadius: BorderRadius.circular(4)), child: const Column(mainAxisSize: MainAxisSize.min, children: [Padding(padding: EdgeInsets.all(6), child: Row(children: [Text('القسم ▼', style: TextStyle(fontSize: 12, fontWeight: FontWeight.bold))])), Divider(height: 1), Padding(padding: EdgeInsets.all(6), child: Text('المحتوى هنا', style: TextStyle(fontSize: 10)))]))),

  // حوارات (7)
  _WI('حوار', 'حوارات', '💬', (ctx) => ElevatedButton.icon(onPressed: () => showDialog(context: ctx, builder: (_) => AlertDialog(title: const Text('حوار'), content: const Text('محتوى'), actions: [TextButton(onPressed: () => Navigator.pop(ctx), child: const Text('حسناً'))])), icon: const Icon(Icons.open_in_new), label: const Text('افتح حوار'))),
  _WI('حوار_تأكيد', 'حوارات', '❓', (ctx) => ElevatedButton.icon(onPressed: () => showDialog(context: ctx, builder: (_) => AlertDialog(title: const Text('تأكيد'), content: const Text('هل أنت متأكد؟'), actions: [TextButton(onPressed: () => Navigator.pop(ctx), child: const Text('لا')), ElevatedButton(onPressed: () => Navigator.pop(ctx), child: const Text('نعم'))])), icon: const Icon(Icons.help), label: const Text('تأكيد'))),
  _WI('لوحة_سفلية', 'حوارات', '⬆️', (ctx) => ElevatedButton.icon(onPressed: () => showModalBottomSheet(context: ctx, builder: (_) => const SizedBox(height: 150, child: Center(child: Text('لوحة سفلية')))), icon: const Icon(Icons.expand_less), label: const Text('لوحة سفلية'))),
  _WI('رسالة_منبثقة', 'حوارات', '📢', (ctx) => ElevatedButton.icon(onPressed: () => ScaffoldMessenger.of(ctx).showSnackBar(const SnackBar(content: Text('رسالة منبثقة!'))), icon: const Icon(Icons.announcement), label: const Text('رسالة'))),
  _WI('تلميح', 'حوارات', '💡', (_) => const Tooltip(message: 'هذا تلميح', child: Icon(Icons.info, color: Colors.blue))),
  _WI('قائمة_منبثقة', 'حوارات', '📂', (_) => PopupMenuButton<String>(itemBuilder: (_) => [const PopupMenuItem(value: '1', child: Text('خيار 1')), const PopupMenuItem(value: '2', child: Text('خيار 2'))], child: const Row(mainAxisSize: MainAxisSize.min, children: [Text('القائمة'), Icon(Icons.arrow_drop_down)]))),
  _WI('عنصر_قائمة', 'حوارات', '📋', (_) => const ListTile(dense: true, leading: Icon(Icons.edit, size: 18), title: Text('عنصر قائمة'))),

  // وسائط متعددة (4)
  _WI('مشغل_فيديو', 'وسائط_متعددة', '🎬', (_) => Container(width: 120, height: 70, color: Colors.black, child: const Center(child: Icon(Icons.play_circle, color: Colors.white, size: 36)))),
  _WI('مشغل_صوت', 'وسائط_متعددة', '🎵', (_) => Row(mainAxisSize: MainAxisSize.min, children: [const Icon(Icons.play_arrow), const SizedBox(width: 4), SizedBox(width: 80, child: LinearProgressIndicator(value: 0.4, color: Colors.green[400])), const SizedBox(width: 4), const Text('2:30', style: TextStyle(fontSize: 10))])),
  _WI('معرض_صور', 'وسائط_متعددة', '🖼️', (_) => Row(mainAxisSize: MainAxisSize.min, children: [Container(width: 40, height: 40, margin: const EdgeInsets.all(2), color: Colors.red[200]), Container(width: 40, height: 40, margin: const EdgeInsets.all(2), color: Colors.blue[200]), Container(width: 40, height: 40, margin: const EdgeInsets.all(2), color: Colors.green[200])])),
  _WI('عرض_دوار', 'وسائط_متعددة', '🎠', (_) => const Icon(Icons.view_carousel, size: 40, color: Colors.deepOrange)),

  // متقدم (11)
  _WI('نص_منسق', 'متقدم', '📝', (_) => RichText(text: const TextSpan(style: TextStyle(color: Colors.black), children: [TextSpan(text: 'نص ', style: TextStyle(fontWeight: FontWeight.bold)), TextSpan(text: 'منسق ', style: TextStyle(color: Colors.blue)), TextSpan(text: 'ملون', style: TextStyle(color: Colors.red))]))),
  _WI('ماركداون', 'متقدم', '📋', (_) => const Column(mainAxisSize: MainAxisSize.min, crossAxisAlignment: CrossAxisAlignment.start, children: [Text('# عنوان', style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold)), Text('**غامق** و *مائل*', style: TextStyle(fontSize: 12))])),
  _WI('لوحة', 'متقدم', '🎨', (_) => Container(width: 80, height: 60, decoration: BoxDecoration(border: Border.all(color: Colors.grey), borderRadius: BorderRadius.circular(4)), child: CustomPaint(painter: _SPainter()))),
  _WI('عرض_ويب', 'متقدم', '🌐', (_) => Container(width: 100, height: 60, decoration: BoxDecoration(border: Border.all(color: Colors.grey)), child: const Column(mainAxisAlignment: MainAxisAlignment.center, children: [Icon(Icons.language, size: 24, color: Colors.blue), Text('WebView', style: TextStyle(fontSize: 10))]))),
  _WI('خريطة', 'متقدم', '🗺️', (_) => Container(width: 100, height: 60, color: Colors.green[100], child: const Center(child: Icon(Icons.map, size: 32, color: Colors.green)))),
  _WI('جدول_بيانات', 'متقدم', '📊', (_) => const Icon(Icons.table_chart, size: 32, color: Colors.teal)),
  _WI('عرض_شجري', 'متقدم', '🌳', (_) => const Column(mainAxisSize: MainAxisSize.min, crossAxisAlignment: CrossAxisAlignment.start, children: [Text('📁 الجذر', style: TextStyle(fontSize: 11)), Padding(padding: EdgeInsets.only(right: 16), child: Text('📄 ملف1', style: TextStyle(fontSize: 11))), Padding(padding: EdgeInsets.only(right: 16), child: Text('📁 مجلد', style: TextStyle(fontSize: 11)))])),
  _WI('مسار_تنقل', 'متقدم', '🔗', (_) => const Row(mainAxisSize: MainAxisSize.min, children: [Text('الرئيسية', style: TextStyle(color: Colors.blue, fontSize: 11)), Text(' > '), Text('القسم', style: TextStyle(color: Colors.blue, fontSize: 11)), Text(' > '), Text('الصفحة', style: TextStyle(fontSize: 11))])),
  _WI('ترقيم_صفحات', 'متقدم', '📄', (_) => Row(mainAxisSize: MainAxisSize.min, children: [const Icon(Icons.chevron_right, size: 16), ...List.generate(4, (i) => Container(margin: const EdgeInsets.symmetric(horizontal: 2), padding: const EdgeInsets.all(4), decoration: BoxDecoration(color: i == 0 ? Colors.blue : Colors.grey[200], borderRadius: BorderRadius.circular(4)), child: Text('${i + 1}', style: TextStyle(fontSize: 10, color: i == 0 ? Colors.white : Colors.black)))), const Icon(Icons.chevron_left, size: 16)])),
  _WI('خط_زمني', 'متقدم', '📈', (_) => Column(mainAxisSize: MainAxisSize.min, children: [Row(mainAxisSize: MainAxisSize.min, children: [Container(width: 10, height: 10, decoration: const BoxDecoration(color: Colors.blue, shape: BoxShape.circle)), const SizedBox(width: 4), const Text('حدث 1', style: TextStyle(fontSize: 10))]), Container(width: 2, height: 10, color: Colors.grey), Row(mainAxisSize: MainAxisSize.min, children: [Container(width: 10, height: 10, decoration: const BoxDecoration(color: Colors.green, shape: BoxShape.circle)), const SizedBox(width: 4), const Text('حدث 2', style: TextStyle(fontSize: 10))])])),
  _WI('قسم', 'متقدم', '📑', (_) => Container(padding: const EdgeInsets.all(8), decoration: BoxDecoration(border: Border.all(color: Colors.grey[300]!), borderRadius: BorderRadius.circular(4)), child: const Column(mainAxisSize: MainAxisSize.min, crossAxisAlignment: CrossAxisAlignment.start, children: [Text('القسم', style: TextStyle(fontWeight: FontWeight.bold, fontSize: 12)), SizedBox(height: 4), Text('محتوى القسم', style: TextStyle(fontSize: 10))]))),

  // حركة (6)
  _WI('وميض', 'حركة', '✨', (_) => _BlinkD()),
  _WI('قائمة_متحركة', 'حركة', '🔄', (_) => const Icon(Icons.animation, size: 32, color: Colors.purple)),
  _WI('قابل_للطي', 'حركة', '📂', (_) => _CollapseD()),
  _WI('قابل_للسحب', 'حركة', '🖐️', (_) => Draggable<String>(data: 'ص', feedback: Material(child: Container(padding: const EdgeInsets.all(8), color: Colors.blue[100], child: const Text('اسحبني'))), child: Container(padding: const EdgeInsets.all(8), decoration: BoxDecoration(border: Border.all(color: Colors.blue), borderRadius: BorderRadius.circular(4)), child: const Text('اسحبني 🖐️')))),
  _WI('تنبيه', 'حركة', '⚠️', (_) => Container(padding: const EdgeInsets.all(8), decoration: BoxDecoration(color: Colors.amber[100], borderRadius: BorderRadius.circular(4), border: Border.all(color: Colors.amber)), child: const Row(mainAxisSize: MainAxisSize.min, children: [Icon(Icons.warning, color: Colors.amber, size: 18), SizedBox(width: 4), Text('تنبيه!', style: TextStyle(fontSize: 12))]))),
  _WI('مجموعة', 'حركة', '👥', (_) => const Row(mainAxisSize: MainAxisSize.min, children: [CircleAvatar(radius: 14, backgroundColor: Colors.blue, child: Text('1', style: TextStyle(color: Colors.white, fontSize: 10))), CircleAvatar(radius: 14, backgroundColor: Colors.red, child: Text('2', style: TextStyle(color: Colors.white, fontSize: 10))), CircleAvatar(radius: 14, backgroundColor: Colors.green, child: Text('3', style: TextStyle(color: Colors.white, fontSize: 10)))])),
];

// ═══════════════════════════════════════════════════════════════════════════════
// عناصر تفاعلية
// ═══════════════════════════════════════════════════════════════════════════════

class _ToggleBtn extends StatefulWidget { @override State<_ToggleBtn> createState() => _ToggleBtnS(); }
class _ToggleBtnS extends State<_ToggleBtn> {
  bool _on = false;
  @override Widget build(BuildContext context) => ToggleButtons(isSelected: [_on, !_on], onPressed: (i) => setState(() => _on = i == 0), children: const [Text(' تشغيل '), Text(' إيقاف ')]);
}

class _SwitchD extends StatefulWidget { @override State<_SwitchD> createState() => _SwitchDS(); }
class _SwitchDS extends State<_SwitchD> {
  bool _v = true;
  @override Widget build(BuildContext context) => Row(mainAxisSize: MainAxisSize.min, children: [const Text('داكن'), Switch(value: _v, onChanged: (v) => setState(() => _v = v))]);
}

class _SliderD extends StatefulWidget { @override State<_SliderD> createState() => _SliderDS(); }
class _SliderDS extends State<_SliderD> {
  double _v = 50;
  @override Widget build(BuildContext context) => SizedBox(width: 180, child: Slider(value: _v, max: 100, divisions: 10, label: '${_v.round()}', onChanged: (v) => setState(() => _v = v)));
}

class _CheckD extends StatefulWidget { @override State<_CheckD> createState() => _CheckDS(); }
class _CheckDS extends State<_CheckD> {
  bool _v = false;
  @override Widget build(BuildContext context) => Row(mainAxisSize: MainAxisSize.min, children: [Checkbox(value: _v, onChanged: (v) => setState(() => _v = v!)), const Text('أوافق')]);
}

class _DropD extends StatefulWidget { @override State<_DropD> createState() => _DropDS(); }
class _DropDS extends State<_DropD> {
  String _v = 'خيار1';
  @override Widget build(BuildContext context) => DropdownButton<String>(value: _v, items: ['خيار1', 'خيار2', 'خيار3'].map((e) => DropdownMenuItem(value: e, child: Text(e))).toList(), onChanged: (v) => setState(() => _v = v!));
}

class _RatingD extends StatefulWidget { @override State<_RatingD> createState() => _RatingDS(); }
class _RatingDS extends State<_RatingD> {
  int _v = 3;
  @override Widget build(BuildContext context) => Row(mainAxisSize: MainAxisSize.min, children: List.generate(5, (i) => GestureDetector(onTap: () => setState(() => _v = i + 1), child: Icon(i < _v ? Icons.star : Icons.star_border, color: Colors.amber, size: 28))));
}

class _RadioD extends StatefulWidget { @override State<_RadioD> createState() => _RadioDS(); }
class _RadioDS extends State<_RadioD> {
  int _v = 0;
  @override Widget build(BuildContext context) => Column(mainAxisSize: MainAxisSize.min, children: List.generate(3, (i) => Row(mainAxisSize: MainAxisSize.min, children: [
    // ignore: deprecated_member_use
    Radio<int>(value: i, groupValue: _v, onChanged: (v) => setState(() => _v = v!)), Text('خيار ${i + 1}', style: const TextStyle(fontSize: 12))])));
}

class _StepD extends StatelessWidget {
  @override Widget build(BuildContext context) => Row(mainAxisSize: MainAxisSize.min, children: [_s('١', true), _l(), _s('٢', true), _l(), _s('٣', false)]);
  Widget _s(String n, bool d) => Container(width: 24, height: 24, decoration: BoxDecoration(color: d ? Colors.blue : Colors.grey[300], shape: BoxShape.circle), child: Center(child: Text(n, style: TextStyle(color: d ? Colors.white : Colors.grey, fontSize: 11))));
  Widget _l() => Container(width: 20, height: 2, color: Colors.blue);
}

class _SegD extends StatefulWidget { @override State<_SegD> createState() => _SegDS(); }
class _SegDS extends State<_SegD> {
  int _v = 0;
  @override Widget build(BuildContext context) => SegmentedButton<int>(segments: const [ButtonSegment(value: 0, label: Text('يوم')), ButtonSegment(value: 1, label: Text('أسبوع')), ButtonSegment(value: 2, label: Text('شهر'))], selected: {_v}, onSelectionChanged: (s) => setState(() => _v = s.first));
}

class _BlinkD extends StatefulWidget { @override State<_BlinkD> createState() => _BlinkDS(); }
class _BlinkDS extends State<_BlinkD> with SingleTickerProviderStateMixin {
  late AnimationController _c;
  @override void initState() { super.initState(); _c = AnimationController(vsync: this, duration: const Duration(seconds: 1))..repeat(reverse: true); }
  @override void dispose() { _c.dispose(); super.dispose(); }
  @override Widget build(BuildContext context) => FadeTransition(opacity: _c, child: const Text('✨ وميض', style: TextStyle(fontSize: 18, color: Colors.amber)));
}

class _CollapseD extends StatefulWidget { @override State<_CollapseD> createState() => _CollapseDS(); }
class _CollapseDS extends State<_CollapseD> {
  bool _o = false;
  @override Widget build(BuildContext context) => Column(mainAxisSize: MainAxisSize.min, children: [GestureDetector(onTap: () => setState(() => _o = !_o), child: Row(mainAxisSize: MainAxisSize.min, children: [Icon(_o ? Icons.expand_less : Icons.expand_more), const Text('طي/فتح')])), AnimatedCrossFade(firstChild: const SizedBox.shrink(), secondChild: const Padding(padding: EdgeInsets.all(4), child: Text('محتوى قابل للطي')), crossFadeState: _o ? CrossFadeState.showSecond : CrossFadeState.showFirst, duration: const Duration(milliseconds: 300))]);
}

class _SPainter extends CustomPainter {
  @override void paint(Canvas canvas, Size size) {
    final p = Paint()..color = Colors.blue..strokeWidth = 2..style = PaintingStyle.stroke;
    canvas.drawCircle(Offset(size.width / 2, size.height / 2), 15, p);
    canvas.drawLine(Offset(10, size.height - 10), Offset(size.width - 10, 10), p..color = Colors.red);
  }
  @override bool shouldRepaint(covariant CustomPainter o) => false;
}
