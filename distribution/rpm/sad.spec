# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# RPM Spec — لغة ص / Sad Programming Language
# ═══════════════════════════════════════════════════════════════════════════════
#
# بناء الحزمة / Build:
#   rpmbuild -bb sad.spec
#
# أو باستخدام سكريبت البناء:
#   ./build-rpm.sh 1.0.0
#
# التثبيت / Install:
#   sudo rpm -i sad-lang-1.0.0-1.x86_64.rpm
#   أو: sudo dnf install ./sad-lang-1.0.0-1.x86_64.rpm
#
# ═══════════════════════════════════════════════════════════════════════════════

Name:           sad-lang
Version:        1.0.0~beta.1
Release:        1%{?dist}
Summary:        لغة ص — لغة برمجة عربية حديثة
Summary(en):    Sad — Modern Arabic Programming Language

License:        MIT
URL:            https://github.com/sad-lang/sad-language
Source0:        https://github.com/sad-lang/sad-language/releases/download/v1.0.0-beta.1/sad-full-v1.0.0-beta.1-linux-x86_64.tar.gz

# لا نبني من المصدر — نستخدم الثنائيات الجاهزة
AutoReqProv:    no

%description
لغة ص (Sad) هي لغة برمجة عربية حديثة مبنية بـ C++17.
تتضمن مترجمًا أصليًّا ومكتبة قياسية عربية غنية،
خادم LSP للتكامل مع المحررات، ومدير حزم.

المكونات:
  - sad: مركز الأدوات — يُنادي البقيّة (sad build، sad check…)
  - sadc: المترجم — يحوّل .ص إلى ملف تنفيذي (LLVM مربوطة ساكنًا)
  - sad-pkg: مدير الحزم
  - sad-lsp: خادم LSP للمحررات

%description -l en
Sad (ص) is a modern Arabic programming language built with C++17.
It includes a native LLVM compiler, standard library, LSP server,
and package manager.

%prep
%setup -q -n sad-full-v%{version}-linux-x86_64

%install
# الملفات التنفيذية
mkdir -p %{buildroot}%{_bindir}
for f in bin/*; do
    install -m 755 "$f" %{buildroot}%{_bindir}/ 2>/dev/null || true
done

# المكتبة القياسية
mkdir -p %{buildroot}%{_datadir}/sad-lang/stdlib
cp -r stdlib/* %{buildroot}%{_datadir}/sad-lang/stdlib/ 2>/dev/null || true

# التوثيق
mkdir -p %{buildroot}%{_docdir}/sad-lang
cp README.md %{buildroot}%{_docdir}/sad-lang/ 2>/dev/null || true

%files
%license LICENSE
%doc README.md
%{_bindir}/sad
%{_bindir}/sadc
%{_bindir}/sad-pkg
%{_bindir}/sad-lsp
# (AR) 🔑 **`%files` تُقاسُ في الاتّجاهَين، لا في واحد.**
#      · حُذف `sad-repl`: `%install` ينسخُ `bin/*` فلا يقعُ ملفٌّ زال مع المفسّر،
#        ثمّ يطلبُه `%files` صراحةً ⇒ `File not found` ⇒ البناءُ يُخفِق.
#      · ⚠️ وأُضيفَ `sad-build` و`sad-check`: أرشيفُ الإصدارِ يحملُهما
#        (`SAD_TOOL_TABLE` في scripts/ci/release_tools.sh)، و`%install` ينسخُ
#        `bin/*` فيقعان، ثمّ لا تذكرُهما `%files` ⇒ «Installed but unpackaged»
#        ⇒ **البناءُ يُخفِقُ كذلك**. ولا `%_unpackaged_files_terminate_build 0`
#        في الشجرةِ كلِّها. فالاتّجاهُ الثاني كان يكسرُ البناءَ اليومَ فعلًا.
# (EN) %files must be measured in BOTH directions. sad-repl was removed because
#      %install copies bin/* so a file that went with the interpreter never lands
#      while %files demands it — File not found. And sad-build and sad-check were
#      ADDED: the release archive carries both (SAD_TOOL_TABLE in
#      scripts/ci/release_tools.sh), %install copies bin/* so both land, and
#      %files did not list them — "Installed (but unpackaged) file(s) found",
#      which fails the build too, and no %_unpackaged_files_terminate_build 0
#      exists anywhere in the tree.
%{_bindir}/sad-build
%{_bindir}/sad-check
%{_bindir}/sad-fmt
%{_datadir}/sad-lang/

%post
echo ""
echo "╔═══════════════════════════════════════════════╗"
echo "║     لغة ص — تم التثبيت بنجاح!                ║"
echo "╚═══════════════════════════════════════════════╝"
echo ""
echo "  sad --help              عرض المساعدة"
echo "  sad build script.ص     ترجمة إلى ملف تنفيذي"
echo "  sad check script.ص     فحص ملكية وأنواع"
echo "  sad-pkg init            إنشاء مشروع جديد"
echo ""

%changelog
* Mon Mar 01 2026 Sad Language Team <team@sad-lang.org> - 1.0.0-1
- الإصدار الأول
- مركز الأدوات (sad) والمترجم (sad-build/sadc) ومدير الحزم (sad-pkg)
- المكتبة القياسية العربية الكاملة
- خادم LSP وأداة التنسيق
