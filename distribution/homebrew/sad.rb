# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# Homebrew Formula — لغة ص / Sad Programming Language
# ═══════════════════════════════════════════════════════════════════════════════
#
# التثبيت / Installation:
#   brew tap sad-lang/tap
#   brew install sad
#
# أو مباشرة:
#   brew install sad-lang/tap/sad
#
# لنشر هذه الـ Formula:
#   1. أنشئ مستودع: github.com/sad-lang/homebrew-tap
#   2. ضع هذا الملف في: Formula/sad.rb
#   3. حدّث sha256 و url مع كل إصدار
#
# ═══════════════════════════════════════════════════════════════════════════════

class Sad < Formula
  desc "لغة ص — لغة برمجة عربية حديثة / Sad Arabic Programming Language"
  homepage "https://github.com/sad-lang/sad-language"
  license "MIT"
  version "1.0.0"

  # ── تنزيل حسب المنصة والبنية ──
  on_macos do
    on_arm do
      url "https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-full-v1.0.0-macos-aarch64.tar.gz"
      sha256 "0000000000000000000000000000000000000000000000000000000000000000"
    end
    on_intel do
      url "https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-full-v1.0.0-macos-x86_64.tar.gz"
      sha256 "0000000000000000000000000000000000000000000000000000000000000000"
    end
  end

  on_linux do
    on_arm do
      url "https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-full-v1.0.0-linux-aarch64.tar.gz"
      sha256 "0000000000000000000000000000000000000000000000000000000000000000"
    end
    on_intel do
      url "https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-full-v1.0.0-linux-x86_64.tar.gz"
      sha256 "0000000000000000000000000000000000000000000000000000000000000000"
    end
  end

  def install
    # تثبيت الملفات التنفيذية
    bin.install Dir["bin/*"]

    # تثبيت المكتبة القياسية
    (share/"sad/stdlib").install Dir["stdlib/*"] if Dir.exist?("stdlib")

    # تثبيت الوثائق
    doc.install "README.md" if File.exist?("README.md")
  end

  def caveats
    <<~EOS
      ╔═══════════════════════════════════════════════╗
      ║     لغة ص — Sad Programming Language         ║
      ╚═══════════════════════════════════════════════╝

      للبدء:
        sad --help              عرض المساعدة
        sad script.ص           تشغيل ملف
        sadc script.ص          ترجمة إلى ملف تنفيذي (يتطلب LLVM)
        sad-pkg init            إنشاء مشروع جديد

      المترجم sadc يتطلب LLVM 18:
        brew install llvm@18

      التوثيق: https://sad-lang.org/docs
    EOS
  end

  test do
    # اختبار بسيط للتأكد من عمل المفسر
    (testpath/"test.ص").write('اطبع_سطر("مرحباً من لغة ص!")')
    output = shell_output("#{bin}/sad #{testpath}/test.ص 2>&1", 0)
    assert_match "مرحباً من لغة ص!", output
  end
end
