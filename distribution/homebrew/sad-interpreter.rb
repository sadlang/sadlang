# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# Homebrew Formula — المفسر فقط (بدون مترجم)
# ═══════════════════════════════════════════════════════════════════════════════
#
# التثبيت:
#   brew install sad-lang/tap/sad-interpreter
#
# ═══════════════════════════════════════════════════════════════════════════════

class SadInterpreter < Formula
  desc "لغة ص — المفسر فقط / Sad Language Interpreter"
  homepage "https://github.com/sad-lang/sad-language"
  license "MIT"
  version "1.0.0"

  conflicts_with "sad", because: "sad includes the interpreter"

  on_macos do
    on_arm do
      url "https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-v1.0.0-macos-aarch64.tar.gz"
      sha256 "0000000000000000000000000000000000000000000000000000000000000000"
    end
    on_intel do
      url "https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-v1.0.0-macos-x86_64.tar.gz"
      sha256 "0000000000000000000000000000000000000000000000000000000000000000"
    end
  end

  on_linux do
    on_arm do
      url "https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-v1.0.0-linux-aarch64.tar.gz"
      sha256 "0000000000000000000000000000000000000000000000000000000000000000"
    end
    on_intel do
      url "https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-v1.0.0-linux-x86_64.tar.gz"
      sha256 "0000000000000000000000000000000000000000000000000000000000000000"
    end
  end

  def install
    bin.install Dir["bin/*"]
    (share/"sad/stdlib").install Dir["stdlib/*"] if Dir.exist?("stdlib")
  end

  test do
    (testpath/"test.ص").write('اطبع_سطر("مرحباً!")')
    output = shell_output("#{bin}/sad #{testpath}/test.ص 2>&1", 0)
    assert_match "مرحباً!", output
  end
end
