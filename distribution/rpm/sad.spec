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
تتضمن مفسر فوري (sad) ومكتبة قياسية عربية غنية،
خادم LSP للتكامل مع المحررات، ومدير حزم.

المكونات:
  - sad: المفسر — يشغل ملفات .ص مباشرة
  - sadc: المترجم — يحوّل .ص إلى ملف تنفيذي (يتطلب LLVM 18)
  - sad-pkg: مدير الحزم
  - sad-lsp: خادم LSP للمحررات

%description -l en
Sad (ص) is a modern Arabic programming language built with C++17.
It includes an interpreter, standard library, LSP server,
package manager, and optional native compiler via LLVM.

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
%{_bindir}/sad-lsp-server
%{_bindir}/sad-repl
%{_bindir}/sad-fmt
%{_datadir}/sad-lang/

%post
echo ""
echo "╔═══════════════════════════════════════════════╗"
echo "║     لغة ص — تم التثبيت بنجاح!                ║"
echo "╚═══════════════════════════════════════════════╝"
echo ""
echo "  sad --help              عرض المساعدة"
echo "  sad script.ص           تشغيل ملف"
echo "  sadc script.ص          ترجمة إلى ملف تنفيذي"
echo "  sad-pkg init            إنشاء مشروع جديد"
echo ""

%changelog
* Mon Mar 01 2026 Sad Language Team <team@sad-lang.org> - 1.0.0-1
- الإصدار الأول
- المفسر (sad) والمترجم (sadc) ومدير الحزم (sad-pkg)
- المكتبة القياسية العربية الكاملة
- خادم LSP وأداة التنسيق
