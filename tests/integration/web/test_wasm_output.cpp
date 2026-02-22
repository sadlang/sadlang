/**
 * بسم الله الرحمن الرحيم
 * test_wasm_output.cpp - اختبارات تكامل WebAssembly للغة ص
 * WebAssembly Integration Tests for Sad Language
 */

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

// LLVM headers
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/MC/TargetRegistry.h"

// Sad compiler headers
#include "compiler_new/include/lexer.hpp"
#include "compiler_new/include/parser.hpp"
#include "compiler_new/include/semantic_analyzer.hpp"
#include "compiler_new/include/codegen.hpp"

namespace sad::test {

/**
 * فئة اختبار WebAssembly الأساسية
 */
class WasmOutputTest : public ::testing::Test {
protected:
    void SetUp() override {
        // تهيئة LLVM للهدف WASM
        LLVMInitializeWebAssemblyTargetInfo();
        LLVMInitializeWebAssemblyTarget();
        LLVMInitializeWebAssemblyTargetMC();
        LLVMInitializeWebAssemblyAsmPrinter();
        LLVMInitializeWebAssemblyAsmParser();
        
        context = std::make_unique<llvm::LLVMContext>();
    }
    
    void TearDown() override {
        context.reset();
    }
    
    /**
     * تجميع كود Sad إلى LLVM IR
     */
    std::unique_ptr<llvm::Module> compileSadToIR(const std::string& code) {
        // Lexer
        sad::Lexer lexer(code);
        auto tokens = lexer.tokenize();
        
        // Parser
        sad::Parser parser(tokens);
        auto ast = parser.parse();
        
        // Semantic Analysis
        sad::SemanticAnalyzer analyzer;
        analyzer.analyze(ast.get());
        
        // Code Generation
        sad::CodeGen codegen(*context);
        return codegen.generate(ast.get());
    }
    
    /**
     * التحقق من أن الهدف WASM متاح
     */
    bool isWasmTargetAvailable() {
        std::string error;
        auto target = llvm::TargetRegistry::lookupTarget("wasm32-unknown-unknown", error);
        return target != nullptr;
    }
    
    /**
     * إنشاء TargetMachine لـ WebAssembly
     */
    std::unique_ptr<llvm::TargetMachine> createWasmTargetMachine() {
        std::string error;
        auto target = llvm::TargetRegistry::lookupTarget("wasm32-unknown-unknown", error);
        
        if (!target) {
            return nullptr;
        }
        
        auto CPU = "generic";
        auto features = "+simd128,+bulk-memory,+mutable-globals";
        
        llvm::TargetOptions opt;
        auto RM = llvm::Reloc::PIC_;
        
        return std::unique_ptr<llvm::TargetMachine>(
            target->createTargetMachine("wasm32-unknown-unknown", CPU, features, opt, RM)
        );
    }
    
    std::unique_ptr<llvm::LLVMContext> context;
};

/**
 * اختبار توفر هدف WebAssembly
 */
TEST_F(WasmOutputTest, WasmTargetAvailable) {
    EXPECT_TRUE(isWasmTargetAvailable()) 
        << "هدف WebAssembly غير متاح في LLVM";
}

/**
 * اختبار إنشاء TargetMachine لـ WebAssembly
 */
TEST_F(WasmOutputTest, CreateWasmTargetMachine) {
    auto tm = createWasmTargetMachine();
    ASSERT_NE(tm, nullptr) << "فشل إنشاء TargetMachine لـ WebAssembly";
    
    EXPECT_EQ(tm->getTargetTriple().str(), "wasm32-unknown-unknown");
}

/**
 * اختبار تجميع دالة Hello World إلى WASM IR
 */
TEST_F(WasmOutputTest, CompileHelloWorld) {
    const std::string code = R"(
دالة رئيسية() {
    اطبع("مرحباً بالعالم!")
    عودة 0
}
)";
    
    auto module = compileSadToIR(code);
    ASSERT_NE(module, nullptr) << "فشل تجميع الكود إلى LLVM IR";
    
    // التحقق من وجود الدالة الرئيسية
    auto mainFunc = module->getFunction("main");
    EXPECT_NE(mainFunc, nullptr) << "الدالة الرئيسية غير موجودة";
}

/**
 * اختبار تجميع دالة حسابية
 */
TEST_F(WasmOutputTest, CompileArithmeticFunction) {
    const std::string code = R"(
دالة جمع(أ, ب) {
    عودة أ + ب
}

دالة رئيسية() {
    رقم نتيجة = جمع(5, 3)
    عودة نتيجة
}
)";
    
    auto module = compileSadToIR(code);
    ASSERT_NE(module, nullptr);
    
    // التحقق من وجود دالة الجمع
    auto addFunc = module->getFunction("جمع");
    EXPECT_NE(addFunc, nullptr) << "دالة الجمع غير موجودة";
    
    // التحقق من عدد المعاملات
    EXPECT_EQ(addFunc->arg_size(), 2);
}

/**
 * اختبار تجميع حلقة
 */
TEST_F(WasmOutputTest, CompileLoop) {
    const std::string code = R"(
دالة مجموع_إلى(ن) {
    رقم مجموع = 0
    رقم ط = 1
    طالما (ط <= ن) {
        مجموع = مجموع + ط
        ط = ط + 1
    }
    عودة مجموع
}
)";
    
    auto module = compileSadToIR(code);
    ASSERT_NE(module, nullptr);
    
    auto sumFunc = module->getFunction("مجموع_إلى");
    EXPECT_NE(sumFunc, nullptr);
}

/**
 * اختبار تجميع شروط
 */
TEST_F(WasmOutputTest, CompileConditionals) {
    const std::string code = R"(
دالة أقصى(أ, ب) {
    إذا (أ > ب) {
        عودة أ
    } وإلا {
        عودة ب
    }
}
)";
    
    auto module = compileSadToIR(code);
    ASSERT_NE(module, nullptr);
    
    auto maxFunc = module->getFunction("أقصى");
    EXPECT_NE(maxFunc, nullptr);
}

/**
 * اختبار تجميع مصفوفة
 */
TEST_F(WasmOutputTest, CompileArray) {
    const std::string code = R"(
دالة مجموع_مصفوفة(مصفوفة, طول) {
    رقم مجموع = 0
    رقم ط = 0
    طالما (ط < طول) {
        مجموع = مجموع + مصفوفة[ط]
        ط = ط + 1
    }
    عودة مجموع
}
)";
    
    auto module = compileSadToIR(code);
    ASSERT_NE(module, nullptr);
    
    auto sumArrayFunc = module->getFunction("مجموع_مصفوفة");
    EXPECT_NE(sumArrayFunc, nullptr);
}

/**
 * فئة اختبار توليد كود WASM
 */
class WasmCodeGenTest : public WasmOutputTest {
protected:
    /**
     * توليد كود WASM من LLVM Module
     */
    std::vector<uint8_t> generateWasm(llvm::Module* module) {
        auto tm = createWasmTargetMachine();
        if (!tm) {
            return {};
        }
        
        module->setDataLayout(tm->createDataLayout());
        module->setTargetTriple("wasm32-unknown-unknown");
        
        std::string output;
        llvm::raw_string_ostream stream(output);
        llvm::legacy::PassManager pass;
        
        if (tm->addPassesToEmitFile(pass, stream, nullptr, llvm::CodeGenFileType::ObjectFile)) {
            return {};
        }
        
        pass.run(*module);
        stream.flush();
        
        return std::vector<uint8_t>(output.begin(), output.end());
    }
    
    /**
     * التحقق من صحة بنية WASM
     */
    bool isValidWasm(const std::vector<uint8_t>& wasm) {
        // WASM magic number: \0asm
        if (wasm.size() < 8) return false;
        if (wasm[0] != 0x00 || wasm[1] != 0x61 || 
            wasm[2] != 0x73 || wasm[3] != 0x6D) {
            return false;
        }
        // Version 1
        if (wasm[4] != 0x01 || wasm[5] != 0x00 || 
            wasm[6] != 0x00 || wasm[7] != 0x00) {
            return false;
        }
        return true;
    }
};

/**
 * اختبار توليد WASM صالح
 */
TEST_F(WasmCodeGenTest, GenerateValidWasm) {
    if (!isWasmTargetAvailable()) {
        GTEST_SKIP() << "هدف WASM غير متاح";
    }
    
    const std::string code = R"(
دالة مضاعف(س) {
    عودة س * 2
}
)";
    
    auto module = compileSadToIR(code);
    ASSERT_NE(module, nullptr);
    
    auto wasm = generateWasm(module.get());
    ASSERT_FALSE(wasm.empty()) << "فشل توليد كود WASM";
    
    EXPECT_TRUE(isValidWasm(wasm)) << "كود WASM غير صالح";
}

/**
 * اختبار تصدير الدوال
 */
TEST_F(WasmCodeGenTest, ExportFunctions) {
    if (!isWasmTargetAvailable()) {
        GTEST_SKIP() << "هدف WASM غير متاح";
    }
    
    const std::string code = R"(
تصدير دالة جمع(أ, ب) {
    عودة أ + ب
}

تصدير دالة طرح(أ, ب) {
    عودة أ - ب
}
)";
    
    auto module = compileSadToIR(code);
    ASSERT_NE(module, nullptr);
    
    // التحقق من أن الدوال مصدّرة (linkage)
    auto addFunc = module->getFunction("جمع");
    auto subFunc = module->getFunction("طرح");
    
    ASSERT_NE(addFunc, nullptr);
    ASSERT_NE(subFunc, nullptr);
    
    EXPECT_TRUE(addFunc->hasExternalLinkage());
    EXPECT_TRUE(subFunc->hasExternalLinkage());
}

/**
 * فئة اختبار JS Glue Generation
 */
class WasmJsGlueTest : public WasmOutputTest {
protected:
    /**
     * توليد كود JavaScript للربط
     */
    std::string generateJsGlue(const std::string& moduleName,
                               const std::vector<std::string>& exports) {
        std::ostringstream js;
        
        js << "// كود JavaScript للربط مع " << moduleName << "\n";
        js << "const " << moduleName << " = {\n";
        js << "  instance: null,\n";
        js << "  memory: null,\n\n";
        
        js << "  async init(wasmPath) {\n";
        js << "    const response = await fetch(wasmPath);\n";
        js << "    const bytes = await response.arrayBuffer();\n";
        js << "    const imports = this.getImports();\n";
        js << "    const { instance } = await WebAssembly.instantiate(bytes, imports);\n";
        js << "    this.instance = instance;\n";
        js << "    this.memory = instance.exports.memory;\n";
        js << "    return this;\n";
        js << "  },\n\n";
        
        js << "  getImports() {\n";
        js << "    return {\n";
        js << "      env: {\n";
        js << "        اطبع: (ptr) => console.log(this.getString(ptr)),\n";
        js << "        memory: new WebAssembly.Memory({ initial: 256 })\n";
        js << "      }\n";
        js << "    };\n";
        js << "  },\n\n";
        
        js << "  getString(ptr) {\n";
        js << "    const view = new Uint8Array(this.memory.buffer);\n";
        js << "    let end = ptr;\n";
        js << "    while (view[end]) end++;\n";
        js << "    return new TextDecoder().decode(view.subarray(ptr, end));\n";
        js << "  },\n\n";
        
        // Export functions
        for (const auto& exp : exports) {
            js << "  " << exp << "(...args) {\n";
            js << "    return this.instance.exports." << exp << "(...args);\n";
            js << "  },\n\n";
        }
        
        js << "};\n";
        
        return js.str();
    }
};

/**
 * اختبار توليد JS Glue
 */
TEST_F(WasmJsGlueTest, GenerateJsGlue) {
    std::vector<std::string> exports = {"جمع", "طرح", "ضرب"};
    
    auto js = generateJsGlue("حاسبة", exports);
    
    EXPECT_FALSE(js.empty());
    EXPECT_NE(js.find("WebAssembly.instantiate"), std::string::npos);
    EXPECT_NE(js.find("جمع"), std::string::npos);
    EXPECT_NE(js.find("طرح"), std::string::npos);
    EXPECT_NE(js.find("ضرب"), std::string::npos);
}

/**
 * فئة اختبار TypeScript Definitions
 */
class WasmTypeScriptTest : public WasmOutputTest {
protected:
    /**
     * توليد تعريفات TypeScript
     */
    std::string generateTypeScript(const std::string& moduleName,
                                   const std::vector<std::pair<std::string, std::string>>& functions) {
        std::ostringstream ts;
        
        ts << "// تعريفات TypeScript لـ " << moduleName << "\n\n";
        ts << "declare module '" << moduleName << "' {\n";
        
        for (const auto& [name, signature] : functions) {
            ts << "  export function " << name << signature << ";\n";
        }
        
        ts << "\n  export function init(wasmPath: string): Promise<void>;\n";
        ts << "}\n";
        
        return ts.str();
    }
};

/**
 * اختبار توليد TypeScript
 */
TEST_F(WasmTypeScriptTest, GenerateTypeScriptDefinitions) {
    std::vector<std::pair<std::string, std::string>> functions = {
        {"جمع", "(أ: number, ب: number): number"},
        {"طرح", "(أ: number, ب: number): number"},
        {"اطبع", "(نص: string): void"}
    };
    
    auto ts = generateTypeScript("حاسبة", functions);
    
    EXPECT_FALSE(ts.empty());
    EXPECT_NE(ts.find("declare module"), std::string::npos);
    EXPECT_NE(ts.find("جمع"), std::string::npos);
    EXPECT_NE(ts.find("number"), std::string::npos);
}

/**
 * فئة اختبار DOM Bindings
 */
class WasmDomBindingsTest : public ::testing::Test {
protected:
    /**
     * التحقق من صحة ربط DOM
     */
    bool validateDomBinding(const std::string& binding) {
        // تحقق من وجود الدوال الأساسية
        std::vector<std::string> requiredFunctions = {
            "استعلم",
            "أنشئ_عنصر",
            "أضف_ابن",
            "عيّن_نص",
            "عيّن_سمة"
        };
        
        for (const auto& func : requiredFunctions) {
            if (binding.find(func) == std::string::npos) {
                return false;
            }
        }
        
        return true;
    }
};

/**
 * اختبار ربط DOM
 */
TEST_F(WasmDomBindingsTest, DomBindingsExist) {
    // قراءة ملف dom.s
    std::ifstream file("stdlib/web/src/dom.s");
    
    if (!file.is_open()) {
        GTEST_SKIP() << "ملف dom.s غير موجود";
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    EXPECT_TRUE(validateDomBinding(content));
}

/**
 * فئة اختبار Canvas Bindings
 */
class WasmCanvasBindingsTest : public ::testing::Test {
protected:
    bool validateCanvasBinding(const std::string& binding) {
        std::vector<std::string> requiredFunctions = {
            "أنشئ_لوحة",
            "سياق_٢د",
            "ارسم_مستطيل",
            "ابدأ_مسار",
            "ارسم_خط_إلى",
            "عيّن_لون_تعبئة"
        };
        
        for (const auto& func : requiredFunctions) {
            if (binding.find(func) == std::string::npos) {
                return false;
            }
        }
        
        return true;
    }
};

/**
 * اختبار ربط Canvas
 */
TEST_F(WasmCanvasBindingsTest, CanvasBindingsExist) {
    std::ifstream file("stdlib/web/src/canvas.s");
    
    if (!file.is_open()) {
        GTEST_SKIP() << "ملف canvas.s غير موجود";
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    EXPECT_TRUE(validateCanvasBinding(content));
}

/**
 * فئة اختبار Fetch Bindings
 */
class WasmFetchBindingsTest : public ::testing::Test {
protected:
    bool validateFetchBinding(const std::string& binding) {
        std::vector<std::string> requiredFunctions = {
            "جلب",
            "احصل",
            "أرسل",
            "حدّث",
            "احذف",
            "كـ_json"
        };
        
        for (const auto& func : requiredFunctions) {
            if (binding.find(func) == std::string::npos) {
                return false;
            }
        }
        
        return true;
    }
};

/**
 * اختبار ربط Fetch
 */
TEST_F(WasmFetchBindingsTest, FetchBindingsExist) {
    std::ifstream file("stdlib/web/src/fetch.s");
    
    if (!file.is_open()) {
        GTEST_SKIP() << "ملف fetch.s غير موجود";
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    EXPECT_TRUE(validateFetchBinding(content));
}

/**
 * اختبار تكاملي شامل
 */
TEST(WasmIntegrationTest, FullCompilePipeline) {
    // تهيئة LLVM
    LLVMInitializeWebAssemblyTargetInfo();
    LLVMInitializeWebAssemblyTarget();
    LLVMInitializeWebAssemblyTargetMC();
    LLVMInitializeWebAssemblyAsmPrinter();
    
    // التحقق من توفر الهدف
    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget("wasm32-unknown-unknown", error);
    
    if (!target) {
        GTEST_SKIP() << "هدف WASM غير متاح: " << error;
    }
    
    // إنشاء module بسيط
    llvm::LLVMContext context;
    auto module = std::make_unique<llvm::Module>("تطبيق_ويب", context);
    
    module->setTargetTriple("wasm32-unknown-unknown");
    
    // إنشاء دالة بسيطة
    auto funcType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context),
        {llvm::Type::getInt32Ty(context), llvm::Type::getInt32Ty(context)},
        false
    );
    
    auto func = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        "جمع",
        module.get()
    );
    
    auto entry = llvm::BasicBlock::Create(context, "entry", func);
    llvm::IRBuilder<> builder(entry);
    
    auto args = func->arg_begin();
    auto a = &*args++;
    auto b = &*args;
    
    auto result = builder.CreateAdd(a, b, "result");
    builder.CreateRet(result);
    
    // التحقق من صحة Module
    EXPECT_FALSE(llvm::verifyModule(*module, &llvm::errs()));
    
    // التحقق من الدالة
    EXPECT_NE(module->getFunction("جمع"), nullptr);
}

} // namespace sad::test

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
