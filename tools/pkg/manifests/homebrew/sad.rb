# Homebrew formula for Sad Programming Language
# لغة ص - لغة برمجة عربية

class Sad &lt; Formula
  desc "لغة ص - لغة برمجة عربية احترافية | Sad Programming Language"
  homepage "https://sad-lang.org"
  url "https://github.com/sad-lang/sad/archive/refs/tags/v1.0.0.tar.gz"
  sha256 "0000000000000000000000000000000000000000000000000000000000000000"
  license "MIT"
  head "https://github.com/sad-lang/sad.git", branch: "main"

  bottle do
    sha256 cellar: :any,                 arm64_sonoma:   "0000000000000000000000000000000000000000000000000000000000000000"
    sha256 cellar: :any,                 arm64_ventura:  "0000000000000000000000000000000000000000000000000000000000000000"
    sha256 cellar: :any,                 arm64_monterey: "0000000000000000000000000000000000000000000000000000000000000000"
    sha256 cellar: :any,                 sonoma:         "0000000000000000000000000000000000000000000000000000000000000000"
    sha256 cellar: :any,                 ventura:        "0000000000000000000000000000000000000000000000000000000000000000"
    sha256 cellar: :any,                 monterey:       "0000000000000000000000000000000000000000000000000000000000000000"
    sha256 cellar: :any_skip_relocation, x86_64_linux:   "0000000000000000000000000000000000000000000000000000000000000000"
  end

  depends_on "cmake" =&gt; :build
  depends_on "llvm@18"
  depends_on "sdl2" =&gt; :recommended

  def install
    # Configure with CMake
    system "cmake", "-S", ".", "-B", "build",
                    "-DCMAKE_BUILD_TYPE=Release",
                    "-DENABLE_LLVM_BACKEND=ON",
                    *std_cmake_args
    
    # Build
    system "cmake", "--build", "build"
    
    # Install binaries
    bin.install "build/bin/sad"
    bin.install "build/bin/sadc"
    bin.install "build/bin/sad-pkg"
    bin.install "build/bin/sad-fmt"
    
    # Install standard library
    (lib/"sad/stdlib").install Dir["stdlib/*"]
    
    # Install documentation
    doc.install "docs/SAD_LANGUAGE_COMPLETE_REFERENCE.md"
    doc.install "README.md"
    
    # Install man pages
    man1.install "docs/man/sad.1"
    
    # Shell completions
    bash_completion.install "completions/sad.bash" =&gt; "sad"
    zsh_completion.install "completions/sad.zsh" =&gt; "_sad"
    fish_completion.install "completions/sad.fish"
  end

  def caveats
    &lt;&lt;~EOS
      لغة ص مثبتة بنجاح! 🎉
      
      للبدء:
        sad                    # المفسر التفاعلي
        sadc hello.ص -o hello  # الترجمة إلى ملف تنفيذي
        sad hello.ص            # تشغيل مباشر
      
      الوثائق: https://sad-lang.org/docs
      Discord: https://discord.gg/sad-lang
      
      Sad Programming Language installed successfully!
      Documentation: https://sad-lang.org/en/docs
    EOS
  end

  test do
    # Test interpreter
    (testpath/"hello.ص").write(&lt;&lt;~EOS)
      اطبع_سطر("مرحباً!")
    EOS
    assert_equal "مرحباً!\n", shell_output("#{bin}/sad #{testpath}/hello.ص")
    
    # Test compiler
    system "#{bin}/sadc", "--version"
    
    # Test package manager
    system "#{bin}/sad-pkg", "--version"
  end
end
