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
  homepage "https://github.com/sadlang/sadlang"
  license "MIT"
  version "1.0.0"

  # ── تنزيل حسب المنصة والبنية ──
  # (AR) 🔑 أُزيلت زجاجاتُ ماك: مجرى الإصدارِ لا يبني لماك (أُزيلت مدخلاتُها من
  #      المصفوفتَين)، فكانت الصيغةُ تَعِدُ بأرشيفَينِ لا يُنتَجانِ أبدًا.
  #      وصُحِّحَ اسمُ أرشيفِ لينكس: المجرى يُنتِجُ `sad-v{VER}-{OS}-x86_64`
  #      لا `sad-{VER}-{OS}-x64` — فكان التنزيلُ يقعُ على 404 ولو وُجدَ الإصدار.
  #      ⚠️ والبصمةُ صفرٌ عمدًا: لا إصدارَ منشورًا بعدُ من `sadlang/sadlang`،
  #      وأرشيفُ الوسمِ القادمِ يختلفُ بايتًا (LLVM صارت ساكنة) فالبصمةُ
  #      القديمةُ باطلةٌ قطعًا. تُولَّدُ مع أوّلِ وسم.
  # (EN) macOS bottles removed: the release workflow builds no macOS artifact, so
  #      the formula promised two archives that are never produced. The Linux
  #      archive name is corrected to what the workflow actually emits.
  #      ⚠️ The sha256 is deliberately zero: nothing is published yet from
  #      sadlang/sadlang, and the next tag's archive differs byte-wise anyway
  #      (LLVM is now statically linked), so the old digest is certainly void.
  on_linux do
    on_intel do
      url "https://github.com/sadlang/sadlang/releases/download/v1.0.0/sad-v1.0.0-linux-x86_64.tar.gz"
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
