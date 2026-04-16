/*
 * ============================================================================
 * LLVM Compiler Pipeline - ╪º┘ה╪¬┘ז┘ב┘ך╪░
 * LLVM Compiler Pipeline Implementation
 * ============================================================================
 *
 * ╪º┘ה┘ו╪ñ┘ה┘ב (Author): SadLanguage Compiler Team
 * ╪º┘ה╪¬╪º╪▒┘ך╪« (Date): January 4, 2026
 * ============================================================================
 */

#include "pipeline/llvm_compiler_pipeline.h"
#include "llvm_linker.h"
#include "../../../shared/utils/include/utf8_utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

// ╪º╪│╪¬╪«╪»╪º┘ו ┘ב╪╢╪º╪í ╪º┘ה╪ú╪│┘ו╪º╪í sad ┘ה┘ה╪▒╪º╪¿╪╖ / Using sad namespace for linker
using sad::LLVMLinker;

namespace Sad
{
    namespace Compiler
    {
        namespace LLVM
        {

            // ============================================================================
            // CompilationStats Implementation - ╪¬┘ז┘ב┘ך╪░ ╪º┘ה╪Ñ╪¡╪╡╪º╪ª┘ך╪º╪¬
            // ============================================================================

            void CompilationStats::print() const
            {
                std::cout << "\n========================================\n";
                std::cout << "╪Ñ╪¡╪╡╪º╪ª┘ך╪º╪¬ ╪º┘ה╪¬╪▒╪¼┘ו╪⌐ / Compilation Statistics\n";
                std::cout << "========================================\n\n";

                std::cout << "╪º┘ה╪ú┘ט┘ג╪º╪¬ / Timings:\n";
                std::cout << "  ╪º┘ה┘ו╪¡┘ה┘ה ╪º┘ה┘ו╪╣╪¼┘ו┘ך / Lexer:        " << std::fixed << std::setprecision(2) << lexerTime << " ms\n";
                std::cout << "  ╪º┘ה┘ו╪¡┘ה┘ה ╪º┘ה┘ז╪¡┘ט┘ך / Parser:         " << parserTime << " ms\n";
                std::cout << "  ┘ב╪¡╪╡ ╪º┘ה╪ú┘ז┘ט╪º╪╣ / Type Checking:    " << typeCheckTime << " ms\n";
                std::cout << "  ╪¿┘ז╪º╪í SIR / SIR Building:        " << sirBuildTime << " ms\n";
                std::cout << "  ╪¬┘ט┘ה┘ך╪» ╪º┘ה┘ד┘ט╪» / Code Generation:  " << codeGenTime << " ms\n";
                std::cout << "  ╪º┘ה╪¬╪¡╪│┘ך┘ז / Optimization:         " << optimizationTime << " ms\n";
                std::cout << "  ╪º┘ה╪Ñ╪╡╪»╪º╪▒ / Emission:             " << emitTime << " ms\n";
                std::cout << "  ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא\n";
                std::cout << "  ╪º┘ה┘ו╪¼┘ו┘ט╪╣ / Total:                " << totalTime << " ms\n\n";

                std::cout << "╪º┘ה╪Ñ╪¡╪╡╪º╪ª┘ך╪º╪¬ / Statistics:\n";
                std::cout << "  ╪º┘ה╪▒┘ו┘ט╪▓ / Tokens:                " << tokenCount << "\n";
                std::cout << "  ╪╣┘ג╪» AST / AST Nodes:            " << astNodeCount << "\n";
                std::cout << "  ╪¬╪╣┘ה┘ך┘ו╪º╪¬ SIR / SIR Instr.:      " << sirInstructionCount << "\n";
                std::cout << "  ╪¡╪¼┘ו LLVM IR / LLVM IR Size:     " << llvmIRSize << " bytes\n";
                std::cout << "  ╪¡╪¼┘ו Object / Object Size:       " << objectFileSize << " bytes\n";
                std::cout << "  ╪¡╪¼┘ו Executable / Exe Size:      " << executableSize << " bytes\n\n";

                std::cout << "╪º┘ה┘ו┘ד┘ט┘ז╪º╪¬ / Components:\n";
                std::cout << "  ╪º┘ה╪»┘ט╪º┘ה / Functions:             " << functionsCount << "\n";
                std::cout << "  ╪º┘ה╪ú╪╡┘ז╪º┘ב / Classes:              " << classesCount << "\n";
                std::cout << "  ┘ו╪¬╪║┘ך╪▒╪º╪¬ ╪╣╪º┘ו╪⌐ / Globals:        " << globalVarsCount << "\n\n";

                if (totalTime > 0)
                {
                    double ips = getInstructionsPerSecond();
                    std::cout << "╪º┘ה╪ú╪»╪º╪í / Performance:\n";
                    std::cout << "  ╪º┘ה╪│╪▒╪╣╪⌐ / Speed:                 " << std::fixed << std::setprecision(2)
                              << (ips / 1000.0) << " K instr/s\n";
                }

                std::cout << "========================================\n\n";
            }

            // ============================================================================
            // CompilationResult Implementation - ╪¬┘ז┘ב┘ך╪░ ╪º┘ה┘ז╪¬┘ך╪¼╪⌐
            // ============================================================================

            void CompilationResult::print() const
            {
                std::cout << "\n========================================\n";
                std::cout << "┘ז╪¬┘ך╪¼╪⌐ ╪º┘ה╪¬╪▒╪¼┘ו╪⌐ / Compilation Result\n";
                std::cout << "========================================\n\n";

                if (success)
                {
                    std::cout << "╪º┘ה╪¡╪º┘ה╪⌐ / Status: Γ£ו ┘ז╪¼╪¡╪¬ / Success\n\n";

                    if (!outputFile.empty())
                    {
                        std::cout << "┘ו┘ה┘ב ╪º┘ה╪Ñ╪«╪▒╪º╪¼ / Output File: " << outputFile << "\n";
                    }
                    if (!llvmIRFile.empty())
                    {
                        std::cout << "┘ו┘ה┘ב LLVM IR / LLVM IR File: " << llvmIRFile << "\n";
                    }
                    if (!assemblyFile.empty())
                    {
                        std::cout << "┘ו┘ה┘ב Assembly / Assembly File: " << assemblyFile << "\n";
                    }
                    if (!objectFile.empty())
                    {
                        std::cout << "┘ו┘ה┘ב Object / Object File: " << objectFile << "\n";
                    }

                    std::cout << "\n";
                    stats.print();

                    if (!warnings.empty())
                    {
                        std::cout << "╪º┘ה╪¬╪¡╪░┘ך╪▒╪º╪¬ / Warnings (" << warnings.size() << "):\n";
                        for (const auto &warning : warnings)
                        {
                            std::cout << "  Γתá∩╕ן  " << warning << "\n";
                        }
                        std::cout << "\n";
                    }
                }
                else
                {
                    std::cout << "╪º┘ה╪¡╪º┘ה╪⌐ / Status: Γ¥ל ┘ב╪┤┘ה╪¬ / Failed\n\n";
                    std::cout << "╪º┘ה╪«╪╖╪ú / Error: " << errorMessage << "\n\n";

                    if (!errors.empty())
                    {
                        std::cout << "╪º┘ה╪ú╪«╪╖╪º╪í / Errors (" << errors.size() << "):\n";
                        for (const auto &error : errors)
                        {
                            std::cout << "  Γ¥ל " << error << "\n";
                        }
                        std::cout << "\n";
                    }
                }

                std::cout << "========================================\n\n";
            }

            // ============================================================================
            // LLVMCompilerPipeline Implementation - ╪¬┘ז┘ב┘ך╪░ Pipeline ╪º┘ה╪▒╪ª┘ך╪│┘ך
            // ============================================================================

            /**
             * ╪º┘ה┘ו┘ז╪┤╪ª / Constructor
             */
            LLVMCompilerPipeline::LLVMCompilerPipeline(const Sad::Compiler::CompilerOptions &options)
                : options_(options), initialized_(false), hasErrors_(false)
            {
            }

            /**
             * ╪º┘ה┘ו┘ז╪┤╪ª ╪º┘ה╪º┘ב╪¬╪▒╪º╪╢┘ך / Default constructor
             */
            LLVMCompilerPipeline::LLVMCompilerPipeline()
                : initialized_(false), hasErrors_(false)
            {
                // ╪º╪│╪¬╪«╪»╪º┘ו ╪«┘ך╪º╪▒╪º╪¬ ╪º┘ב╪¬╪▒╪º╪╢┘ך╪⌐ / Use default options
            }

            /**
             * ╪º┘ה┘ו╪»┘ו╪▒ / Destructor
             */
            LLVMCompilerPipeline::~LLVMCompilerPipeline()
            {
                cleanup();
            }

            /**
             * ╪¬┘ח┘ך╪ª╪⌐ Pipeline / Initialize pipeline
             */
            bool LLVMCompilerPipeline::initialize()
            {
                if (initialized_)
                {
                    return true; // Already initialized / ┘ו┘ן┘ח┘ך╪ú ┘ו╪│╪¿┘ג╪º┘כ
                }

                try
                {
                    // ╪¬┘ח┘ך╪ª╪⌐ Code Generator ┘ו╪╣ ╪«┘ך╪º╪▒╪º╪¬ / Initialize code generator with options
                    codeGen_ = std::make_unique<Sad::LLVM::LLVMCodeGen>();

                    // ╪¬┘ח┘ך╪ª╪⌐ LLVM ┘ו╪╣ ╪º┘ה┘ח╪»┘ב ╪º┘ה╪º┘ב╪¬╪▒╪º╪╢┘ך / Initialize LLVM with default target
                    std::string moduleName = "sad_module";
                    if (!codeGen_->initialize(moduleName, options_.target_triple))
                    {
                        logError("┘ב╪┤┘ה ╪¬┘ח┘ך╪ª╪⌐ LLVM Code Generator / Failed to initialize LLVM Code Generator");
                        return false;
                    }

                    // ╪¬╪╣┘ך┘ך┘ז ┘ו╪│╪¬┘ט┘י ╪º┘ה╪¬╪¡╪│┘ך┘ז / Set optimization level
                    codeGen_->setOptimizationLevel(options_.optimization_level);
                    codeGen_->setAutoOptimize(options_.auto_optimize);

                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪¬┘ו╪¬ ╪º┘ה╪¬┘ח┘ך╪ª╪⌐ ╪¿┘ז╪¼╪º╪¡ / Initialized successfully\n";
                        std::cout << "[Pipeline] ╪º┘ה┘ח╪»┘ב / Target: " << options_.target_triple << "\n";
                        std::cout << "[Pipeline] ┘ו╪│╪¬┘ט┘י ╪º┘ה╪¬╪¡╪│┘ך┘ז / Optimization: ";
                        switch (options_.optimization_level)
                        {
                        case sad::OptimizationLevel::O0:
                            std::cout << "O0 (No optimization)\n";
                            break;
                        case sad::OptimizationLevel::O1:
                            std::cout << "O1 (Basic)\n";
                            break;
                        case sad::OptimizationLevel::O2:
                            std::cout << "O2 (Standard)\n";
                            break;
                        case sad::OptimizationLevel::O3:
                            std::cout << "O3 (Aggressive)\n";
                            break;
                        case sad::OptimizationLevel::Os:
                            std::cout << "Os (Size)\n";
                            break;
                        case sad::OptimizationLevel::Oz:
                            std::cout << "Oz (Minimal size)\n";
                            break;
                        }
                    }

                    initialized_ = true;
                    return true;
                }
                catch (const std::exception &e)
                {
                    logError(std::string("╪«╪╖╪ú ┘ב┘ך ╪º┘ה╪¬┘ח┘ך╪ª╪⌐ / Initialization error: ") + e.what());
                    return false;
                }
            }

            /**
             * ╪¬╪▒╪¼┘ו╪⌐ ┘ו┘ה┘ב / Compile file
             */
            CompilationResult LLVMCompilerPipeline::compileFile(const std::string &sourceFile)
            {
                if (options_.verbose)
                {
                    std::cout << "[Pipeline] ┘ג╪▒╪º╪í╪⌐ ╪º┘ה┘ו┘ה┘ב / Reading file: " << sourceFile << "\n";
                }

                // ┘ג╪▒╪º╪í╪⌐ ╪º┘ה┘ו┘ה┘ב / Read file
                std::string source = readFile(sourceFile);
                if (source.empty())
                {
                    CompilationResult result;
                    result.success = false;
                    result.errorMessage = "┘ב╪┤┘ה ┘ג╪▒╪º╪í╪⌐ ╪º┘ה┘ו┘ה┘ב / Failed to read file: " + sourceFile;
                    return result;
                }

                // ╪¬╪▒╪¼┘ו╪⌐ ╪º┘ה┘ד┘ט╪» / Compile source
                return compileSource(source, sourceFile);
            }

            /**
             * ╪¬╪▒╪¼┘ו╪⌐ ┘ד┘ט╪» ┘ו╪¿╪º╪┤╪▒ / Compile source code
             */
            CompilationResult LLVMCompilerPipeline::compileSource(const std::string &sourceCode, const std::string &filename)
            {
                auto startTime = std::chrono::high_resolution_clock::now();

                CompilationResult result;
                result.success = false;
                hasErrors_ = false;

                // ╪º┘ה╪¬╪ú┘ד╪» ┘ו┘ז ╪º┘ה╪¬┘ח┘ך╪ª╪⌐ / Ensure initialization
                if (!initialized_ && !initialize())
                {
                    result.errorMessage = "┘ב╪┤┘ה ╪¬┘ח┘ך╪ª╪⌐ Pipeline / Failed to initialize pipeline";
                    return result;
                }

                if (options_.verbose)
                {
                    std::cout << "[Pipeline] ╪¿╪»╪í ╪º┘ה╪¬╪▒╪¼┘ו╪⌐ / Starting compilation...\n";
                }

                // ΓפאΓפאΓפא ┘ו╪▒╪¡┘ה╪⌐ ╪╡┘ב╪▒: ┘ב╪¡╪╡ ╪│┘ו╪º╪¬ Freestanding ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                // (AR) ┘ג╪¿┘ה ╪ú┘ך ┘ו╪▒╪¡┘ה╪⌐ ╪ú╪«╪▒┘י╪ל ┘ז┘ב╪¡╪╡ ╪º┘ה┘ד┘ט╪» ╪º┘ה┘ו╪╡╪»╪▒┘ך ┘ה┘ט╪¼┘ט╪» #![╪¿┘ה╪º_┘ו┘ד╪¬╪¿╪⌐_┘ג┘ך╪º╪│┘ך╪⌐]
                //      ┘ח╪░╪º ╪º┘ה┘ב╪¡╪╡ ╪│╪▒┘ך╪╣ (╪¿╪¡╪½ ┘ז╪╡┘ך ╪¿╪│┘ך╪╖) ┘ט┘ה╪º ┘ך┘ן╪┤╪║┘נ┘ס┘ה ╪º┘ה┘ו╪¡┘ה┘ה.
                //      ╪Ñ╪░╪º ┘ט┘ן╪¼╪»╪¬ ╪º┘ה╪│┘ו╪⌐╪ל ┘ך┘ן┘ב╪╣┘מ┘ס┘ה ┘ט╪╢╪╣ freestanding ╪¬┘ה┘ג╪º╪ª┘ך╪º┘כ.
                // (EN) Before any other stage, scan source for #![no_std] attributes.
                //      Fast text scan Γאפ runs the freestanding codegen setup if found.
                // ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                if (!scanForFreestandingAttributes(sourceCode))
                {
                    result.errorMessage = "┘ב╪┤┘ה ┘ב╪¡╪╡ ╪│┘ו╪º╪¬ Freestanding / Freestanding attribute scan failed";
                    result.errors = errors_;
                    return result;
                }

                // ΓפאΓפאΓפא ╪¬┘ח┘ך╪ª╪⌐ ┘ט╪╢╪╣ Freestanding ╪Ñ╪░╪º ┘ד╪º┘ז ┘ו┘ן┘ב╪╣┘מ┘ס┘ה╪º┘כ ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                if (freestandingModeActive_)
                {
                    if (!initializeFreestandingMode(filename))
                    {
                        result.errorMessage = "┘ב╪┤┘ה ╪¬┘ח┘ך╪ª╪⌐ ┘ט╪╢╪╣ Freestanding / Freestanding mode initialization failed";
                        result.errors = errors_;
                        return result;
                    }
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] Γתש ┘ט╪╢╪╣ ╪¿┘ה╪º_┘ו┘ד╪¬╪¿╪⌐_┘ג┘ך╪º╪│┘ך╪⌐ ┘ו┘ן┘ב╪╣┘מ┘ס┘ה / no_std mode is active\n";
                    }
                }

                // ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 1: ╪º┘ה╪¬╪¡┘ה┘ך┘ה ╪º┘ה┘ו╪╣╪¼┘ו┘ך / Stage 1: Lexical analysis
                auto stageStart = std::chrono::high_resolution_clock::now();
                if (!lexicalAnalysis(sourceCode, filename))
                {
                    result.errorMessage = "┘ב╪┤┘ה ╪º┘ה╪¬╪¡┘ה┘ך┘ה ╪º┘ה┘ו╪╣╪¼┘ו┘ך / Lexical analysis failed";
                    result.errors = errors_;
                    return result;
                }
                result.stats.lexerTime = measureTime(stageStart);
                result.stats.tokenCount = tokens_.size();

                // ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 2: ╪º┘ה╪¬╪¡┘ה┘ך┘ה ╪º┘ה┘ז╪¡┘ט┘ך / Stage 2: Parsing
                stageStart = std::chrono::high_resolution_clock::now();
                if (!parsing())
                {
                    result.errorMessage = "┘ב╪┤┘ה ╪º┘ה╪¬╪¡┘ה┘ך┘ה ╪º┘ה┘ז╪¡┘ט┘ך / Parsing failed";
                    result.errors = errors_;
                    return result;
                }
                result.stats.parserTime = measureTime(stageStart);
                result.stats.astNodeCount = ast_.size(); // ╪¬┘ג╪▒┘ך╪¿┘ך / Approximate

                // ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 3: ┘ב╪¡╪╡ ╪º┘ה╪ú┘ז┘ט╪º╪╣ (╪º╪«╪¬┘ך╪º╪▒┘ך) / Stage 3: Type checking (optional)
                if (options_.type_check_at_compile_time)
                {
                    stageStart = std::chrono::high_resolution_clock::now();
                    if (!typeChecking())
                    {
                        result.errorMessage = "┘ב╪┤┘ה ┘ב╪¡╪╡ ╪º┘ה╪ú┘ז┘ט╪º╪╣ / Type checking failed";
                        result.errors = errors_;
                        return result;
                    }
                    result.stats.typeCheckTime = measureTime(stageStart);
                }

                // ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 4: ╪¿┘ז╪º╪í SIR / Stage 4: SIR building
                stageStart = std::chrono::high_resolution_clock::now();
                if (!sirBuilding())
                {
                    result.errorMessage = "┘ב╪┤┘ה ╪¿┘ז╪º╪í SIR / SIR building failed";
                    result.errors = errors_;
                    return result;
                }
                result.stats.sirBuildTime = measureTime(stageStart);
                if (sirModule_)
                {
                    result.stats.sirInstructionCount = sirModule_->getTotalInstructions();
                    result.stats.functionsCount = sirModule_->getFunctionCount();
                    result.stats.globalVarsCount = sirModule_->getGlobalCount();
                }

                // ΓפאΓפאΓפא ┘ו╪▒╪¡┘ה╪⌐ 4.5: ╪º┘ה╪¬╪¡┘ג┘ג ╪º┘ה┘ז┘ח╪º╪ª┘ך ┘ו┘ז ┘ט╪¡╪»╪⌐ Freestanding ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                // (AR) ╪¿╪╣╪» ╪¿┘ז╪º╪í SIR ╪º┘ה┘ד╪º┘ו┘ה╪ל ┘ז╪¬╪¡┘ג┘ג ┘ו┘ז ╪º┘ד╪¬┘ו╪º┘ה ┘ט╪¡╪»╪⌐ freestanding:
                //   - ┘ט╪¼┘ט╪» ┘ז┘ג╪╖╪⌐_╪»╪«┘ט┘ה (┘ו╪╖┘ה┘ט╪¿ ╪Ñ╪░╪º no_main=true)
                //   - ┘ט╪¼┘ט╪» ┘ו╪╣╪º┘ה╪¼_╪░╪╣╪▒ (╪º╪«╪¬┘ך╪º╪▒┘ך Γאפ ┘ך┘ן╪│╪¬╪«╪»┘ו ╪º┘ה╪º┘ב╪¬╪▒╪º╪╢┘ך ╪Ñ╪░╪º ╪║╪º╪¿)
                //   - ╪Ñ╪¡╪╡╪º╪ª┘ך╪º╪¬ ╪¬┘ט┘ה┘ך╪» ╪º┘ה┘ד┘ט╪» (╪¼┘ו┘ה ╪º┘ה╪¬╪¡┘ד┘ו╪ל ╪º┘ה╪¡┘ה┘ג╪º╪¬...)
                // (EN) After complete SIR building, validate freestanding unit:
                //   - entry point present (required if no_main=true)
                //   - panic handler (optional Γאפ default used if absent)
                // ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                if (freestandingModeActive_)
                {
                    if (!finalizeFreestandingUnit())
                    {
                        result.errorMessage = "┘ב╪┤┘ה ╪º┘ה╪¬╪¡┘ג┘ג ┘ו┘ז ┘ט╪¡╪»╪⌐ Freestanding / Freestanding unit validation failed";
                        result.errors = errors_;
                        return result;
                    }
                }

                // ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 5: ╪¬┘ט┘ה┘ך╪» LLVM IR / Stage 5: Code generation
                stageStart = std::chrono::high_resolution_clock::now();
                if (!codeGeneration())
                {
                    result.errorMessage = "┘ב╪┤┘ה ╪¬┘ט┘ה┘ך╪» ╪º┘ה┘ד┘ט╪» / Code generation failed";
                    result.errors = errors_;
                    return result;
                }
                result.stats.codeGenTime = measureTime(stageStart);

                // ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 6: ╪º┘ה╪¬╪¡╪│┘ך┘ז (╪Ñ╪░╪º ┘ד╪º┘ז ┘ו┘ב╪╣┘ה╪º┘כ) / Stage 6: Optimization (if enabled)
                if (options_.isOptimizationEnabled())
                {
                    stageStart = std::chrono::high_resolution_clock::now();
                    if (!optimization())
                    {
                        // ╪º┘ה╪¬╪¡╪│┘ך┘ז ┘ב╪┤┘ה ┘ה┘ד┘ז ┘ז┘ט╪º╪╡┘ה / Optimization failed but continue
                        logWarning("┘ב╪┤┘ה ╪º┘ה╪¬╪¡╪│┘ך┘ז╪ל ╪º┘ה╪º╪│╪¬┘ו╪▒╪º╪▒ ╪¿╪»┘ט┘ז ╪¬╪¡╪│┘ך┘ז / Optimization failed, continuing without optimization");
                    }
                    result.stats.optimizationTime = measureTime(stageStart);
                }

                // ╪¡╪│╪º╪¿ ╪º┘ה┘ט┘ג╪¬ ╪º┘ה╪Ñ╪¼┘ו╪º┘ה┘ך / Calculate total time
                result.stats.totalTime = measureTime(startTime);

                // ╪¡┘ב╪╕ ╪º┘ה╪Ñ╪«╪▒╪º╪¼ ╪Ñ╪░╪º ╪╖┘ן┘ה╪¿ / Save output if requested
                if (options_.emit_llvm_ir)
                {
                    std::string irFile = options_.getLLVMIRFilename();
                    if (emitLLVMIR(irFile))
                    {
                        result.llvmIRFile = irFile;
                    }
                }

                if (options_.emit_assembly)
                {
                    std::string asmFile = options_.getAssemblyFilename();
                    if (emitAssembly(asmFile))
                    {
                        result.assemblyFile = asmFile;
                    }
                }

                if (options_.emit_object)
                {
                    std::string objFile = options_.getObjectFilename();
                    if (emitObjectFile(objFile))
                    {
                        result.objectFile = objFile;
                        result.outputFile = objFile;
                    }
                }

                // ╪º┘ה┘ז╪¼╪º╪¡! / Success!
                result.success = true;
                result.warnings = warnings_;
                lastResult_ = result;

                if (options_.verbose || options_.print_compilation_stats)
                {
                    result.print();
                }

                return result;
            }

            /**
             * ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 1: ╪º┘ה╪¬╪¡┘ה┘ך┘ה ╪º┘ה┘ו╪╣╪¼┘ו┘ך / Stage 1: Lexical analysis
             */
            bool LLVMCompilerPipeline::lexicalAnalysis(const std::string &source, const std::string &filename)
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 1: ╪º┘ה╪¬╪¡┘ה┘ך┘ה ╪º┘ה┘ו╪╣╪¼┘ו┘ך / Stage 1: Lexical analysis\n";
                    }

                    // ╪Ñ┘ז╪┤╪º╪í Lexer / Create lexer
                    // Source: include/lexer/lexer_core.h:44 - LexerCore(const std::string& source)
                    lexer_ = std::make_unique<Lexer::LexerCore>(source);

                    // ┘ו┘ה╪º╪¡╪╕╪⌐: ┘ה╪º ┘ז╪│╪¬╪»╪╣┘ך tokenize() ┘ח┘ז╪º ┘ה╪ú┘ז Parser ┘ך╪¡╪¬╪º╪¼ Lexer
                    // ╪¼╪»┘ך╪»╪º┘כ ┘ה┘ך╪│╪¬╪«╪▒╪¼ ╪º┘ה╪▒┘ו┘ט╪▓ ╪¬╪»╪▒┘ך╪¼┘ך╪º┘כ
                    // Note: We don't call tokenize() here because Parser needs
                    // fresh Lexer to extract tokens incrementally

                    // ┘ה╪ú╪║╪▒╪º╪╢ ╪º┘ה╪Ñ╪¡╪╡╪º╪í ┘ב┘ג╪╖╪ל ┘ז╪¡╪│╪¿ ╪º┘ה╪▒┘ו┘ט╪▓
                    // For statistics only, count tokens
                    Lexer::LexerCore tempLexer(source);
                    tokens_ = tempLexer.tokenize();

                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪¬┘ו ╪º╪│╪¬╪«╪▒╪º╪¼ " << tokens_.size() << " ╪▒┘ו╪▓ / Extracted " << tokens_.size() << " tokens\n";
                    }

                    return true;
                }
                catch (const std::exception &e)
                {
                    logError(std::string("╪«╪╖╪ú ┘ב┘ך ╪º┘ה╪¬╪¡┘ה┘ך┘ה ╪º┘ה┘ו╪╣╪¼┘ו┘ך / Lexical analysis error: ") + e.what());
                    return false;
                }
            }

            /**
             * ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 2: ╪º┘ה╪¬╪¡┘ה┘ך┘ה ╪º┘ה┘ז╪¡┘ט┘ך / Stage 2: Parsing
             */
            bool LLVMCompilerPipeline::parsing()
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 2: ╪º┘ה╪¬╪¡┘ה┘ך┘ה ╪º┘ה┘ז╪¡┘ט┘ך / Stage 2: Parsing\n";
                    }

                    // ╪Ñ┘ז╪┤╪º╪í Parser / Create parser
                    parser_ = std::make_unique<Parser::ParserCore>(*lexer_);

                    // ╪¬╪¡┘ה┘ך┘ה ╪º┘ה╪¿╪▒┘ז╪º┘ו╪¼ / Parse program
                    ast_ = parser_->parseProgram();

                    // ╪º┘ה╪¬╪¡┘ג┘ג ┘ו┘ז ╪º┘ה╪ú╪«╪╖╪º╪í / Check for errors
                    if (parser_->hasErrors())
                    {
                        auto parserErrors = parser_->getErrors();
                        for (const auto &error : parserErrors)
                        {
                            logError(error);
                        }
                        return false;
                    }

                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪¬┘ו ╪¿┘ז╪º╪í AST ╪¿┘ז╪¼╪º╪¡ / AST built successfully\n";
                    }

                    // ╪╖╪¿╪º╪╣╪⌐ AST ╪Ñ╪░╪º ╪╖┘ן┘ה╪¿ / Print AST if requested
                    if (options_.print_ast)
                    {
                        std::cout << "\n=== AST ===\n";
                        // (AR) ╪╖╪¿╪º╪╣╪⌐ ╪╣┘ג╪» AST Γאפ ╪¬┘ו╪½┘ך┘ה ┘ז╪╡┘ך ┘ה┘ד┘ה ╪╣╪¿╪º╪▒╪⌐ ╪▒╪ª┘ך╪│┘ך╪⌐
                        // (EN) Print AST nodes Γאפ string representation for each top-level statement
                        for (size_t i = 0; i < ast_.size(); ++i)
                        {
                            if (ast_[i])
                            {
                                std::cout << "  [" << i << "] " << ast_[i]->toString() << "\n";
                            }
                        }
                        std::cout << "╪╣╪»╪» ╪º┘ה╪╣╪¿╪º╪▒╪º╪¬ / Statement count: " << ast_.size() << "\n\n";
                    }

                    return true;
                }
                catch (const std::exception &e)
                {
                    logError(std::string("╪«╪╖╪ú ┘ב┘ך ╪º┘ה╪¬╪¡┘ה┘ך┘ה ╪º┘ה┘ז╪¡┘ט┘ך / Parsing error: ") + e.what());
                    return false;
                }
            }

            /**
             * ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 3: ┘ב╪¡╪╡ ╪º┘ה╪ú┘ז┘ט╪º╪╣ / Stage 3: Type checking (optional)
             */
            bool LLVMCompilerPipeline::typeChecking()
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 3: ┘ב╪¡╪╡ ╪º┘ה╪ú┘ז┘ט╪º╪╣ / Stage 3: Type checking\n";
                    }

                    // (AR) ┘ב╪¡╪╡ ╪º┘ה╪ú┘ז┘ט╪º╪╣ Γאפ ┘ו╪▒╪¡┘ה╪⌐ ╪º╪«╪¬┘ך╪º╪▒┘ך╪⌐
                    // (EN) Type checking Γאפ optional stage
                    // (AR) TypeChecker ┘ו┘ט╪¼┘ט╪» ┘ב┘ך compiler_new/src/semantic/type_checker.cpp
                    //      ┘ה┘ד┘ז┘ח ┘ך╪╣┘ו┘ה ╪╣┘ה┘י ┘ו╪│╪¬┘ט┘י ╪º┘ה┘א AST ┘ט┘ך╪¡╪¬╪º╪¼ ╪▒╪¿╪╖ ┘ו╪╣ ╪º┘ה┘א pipeline
                    //      ╪│┘ך╪¬┘ו ╪¬┘ב╪╣┘ך┘ה┘ח ╪╣┘ז╪» ╪º┘ד╪¬┘ו╪º┘ה ╪º┘ה╪¬┘ד╪º┘ו┘ה ┘ו╪╣ TypeEnvironment ┘טInferenceContext
                    // (EN) TypeChecker exists in compiler_new/src/semantic/type_checker.cpp
                    //      but operates at AST level and needs pipeline integration
                    //      Will be enabled when TypeEnvironment/InferenceContext integration is complete

                    // (AR) ╪¡╪º┘ה┘ך╪º┘כ: ┘ז╪¬╪«╪╖┘י ┘ב╪¡╪╡ ╪º┘ה╪ú┘ז┘ט╪º╪╣ Γאפ ╪º┘ה╪ú╪«╪╖╪º╪í ╪¬┘ן┘ד╪¬╪┤┘ב ┘ב┘ך ┘ו╪▒╪¡┘ה╪⌐ SIR/LLVM
                    // (EN) Currently: skip type checking Γאפ errors caught in SIR/LLVM stages

                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪¬┘ו ╪¬╪«╪╖┘ך ┘ב╪¡╪╡ ╪º┘ה╪ú┘ז┘ט╪º╪╣ (┘ג┘ך╪» ╪º┘ה╪¬╪╖┘ט┘ך╪▒) / Type checking skipped (under development)\n";
                    }

                    return true;
                }
                catch (const std::exception &e)
                {
                    logError(std::string("╪«╪╖╪ú ┘ב┘ך ┘ב╪¡╪╡ ╪º┘ה╪ú┘ז┘ט╪º╪╣ / Type checking error: ") + e.what());
                    return false;
                }
            }

            /**
             * ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 4: ╪¿┘ז╪º╪í SIR / Stage 4: SIR building
             */
            bool LLVMCompilerPipeline::sirBuilding()
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 4: ╪¿┘ז╪º╪í SIR / Stage 4: SIR building\n";
                    }

                    // ╪Ñ┘ז╪┤╪º╪í SIR Builder / Create SIR builder
                    sirBuilder_ = std::make_unique<SIR::SIRBuilder>();

                    // ╪¿┘ז╪º╪í SIR Module ┘ו┘ז AST / Build SIR module from AST
                    // Note: buildModule expects ProgramNode (StmtList)
                    sirModule_ = sirBuilder_->buildModule(&ast_);

                    if (!sirModule_)
                    {
                        logError("┘ב╪┤┘ה ╪¿┘ז╪º╪í SIR Module / Failed to build SIR module");
                        return false;
                    }

                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪¬┘ו ╪¿┘ז╪º╪í SIR ╪¿┘ז╪¼╪º╪¡ / SIR built successfully\n";
                        std::cout << "[Pipeline]   ╪º┘ה╪»┘ט╪º┘ה / Functions: " << sirModule_->getFunctionCount() << "\n";
                        std::cout << "[Pipeline]   ╪º┘ה╪¬╪╣┘ה┘ך┘ו╪º╪¬ / Instructions: " << sirModule_->getTotalInstructions() << "\n";
                    }

                    // ╪╖╪¿╪º╪╣╪⌐ SIR ╪Ñ╪░╪º ╪╖┘ן┘ה╪¿ / Print SIR if requested
                    if (options_.print_sir)
                    {
                        std::cout << "\n=== SIR ===\n";
                        sirModule_->print();
                        std::cout << "\n";
                    }

                    return true;
                }
                catch (const std::exception &e)
                {
                    logError(std::string("╪«╪╖╪ú ┘ב┘ך ╪¿┘ז╪º╪í SIR / SIR building error: ") + e.what());
                    return false;
                }
            }

            /**
             * ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 5: ╪¬┘ט┘ה┘ך╪» LLVM IR / Stage 5: Code generation
             */
            bool LLVMCompilerPipeline::codeGeneration()
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 5: ╪¬┘ט┘ה┘ך╪» LLVM IR / Stage 5: Code generation\n";
                    }

                    // ╪╖╪¿╪º╪╣╪⌐ IR ┘ג╪¿┘ה ╪º┘ה╪¬╪¡╪│┘ך┘ז ╪Ñ╪░╪º ╪╖┘ן┘ה╪¿ / Print IR before optimization if requested
                    bool printBefore = options_.print_ir_before_opt;

                    // ╪¬┘ט┘ה┘ך╪» LLVM Module ┘ו┘ז SIR / Generate LLVM module from SIR
                    // ┘ו┘ה╪º╪¡╪╕╪⌐: generate() ╪¬╪¬╪¡┘ג┘ג ┘ו┘ז ╪º┘ה┘א module ╪»╪º╪«┘ה┘ך╪º┘כ ┘ג╪¿┘ה ╪Ñ╪▒╪¼╪º╪╣┘ח
                    // Note: generate() verifies the module internally before returning

                    llvmModule_ = codeGen_->generate(sirModule_);

                    if (!llvmModule_)
                    {
                        logError("┘ב╪┤┘ה ╪¬┘ט┘ה┘ך╪» LLVM Module / Failed to generate LLVM module");
                        return false;
                    }

                    // ┘ה╪º ┘ז╪│╪¬╪»╪╣┘ך verify() ┘ח┘ז╪º ┘ה╪ú┘ז:
                    // 1. generate() ╪¬╪¬╪¡┘ג┘ג ╪»╪º╪«┘ה┘ך╪º┘כ
                    // 2. ╪¿╪╣╪» generate()╪ל module_ ┘ב┘ך codeGen_ ┘ך╪╡╪¿╪¡ null ╪¿╪│╪¿╪¿ std::move
                    // We don't call verify() here because:
                    // 1. generate() verifies internally
                    // 2. After generate(), module_ in codeGen_ becomes null due to std::move

                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪¬┘ו ╪¬┘ט┘ה┘ך╪» LLVM IR ╪¿┘ז╪¼╪º╪¡ / LLVM IR generated successfully\n";
                    }

                    // ╪╖╪¿╪º╪╣╪⌐ IR ┘ג╪¿┘ה ╪º┘ה╪¬╪¡╪│┘ך┘ז / Print IR before optimization
                    if (printBefore)
                    {
                        std::cout << "\n=== LLVM IR (┘ג╪¿┘ה ╪º┘ה╪¬╪¡╪│┘ך┘ז / Before Optimization) ===\n";
                        llvmModule_->print(llvm::outs(), nullptr);
                        std::cout << "\n";
                    }

                    return true;
                }
                catch (const std::exception &e)
                {
                    logError(std::string("╪«╪╖╪ú ┘ב┘ך ╪¬┘ט┘ה┘ך╪» ╪º┘ה┘ד┘ט╪» / Code generation error: ") + e.what());
                    return false;
                }
            }

            /**
             * ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 6: ╪º┘ה╪¬╪¡╪│┘ך┘ז / Stage 6: Optimization
             */
            bool LLVMCompilerPipeline::optimization()
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 6: ╪º┘ה╪¬╪¡╪│┘ך┘ז / Stage 6: Optimization\n";
                    }

                    // ╪º┘ה╪¬╪¡╪│┘ך┘ז ┘ך╪¬┘ו ╪¬┘ה┘ג╪º╪ª┘ך╪º┘כ ┘ב┘ך codeGen->generate() ╪Ñ╪░╪º ┘ד╪º┘ז auto_optimize ┘ו┘ב╪╣┘ה╪º┘כ
                    // Optimization happens automatically in codeGen->generate() if auto_optimize is enabled

                    // ╪╖╪¿╪º╪╣╪⌐ IR ╪¿╪╣╪» ╪º┘ה╪¬╪¡╪│┘ך┘ז ╪Ñ╪░╪º ╪╖┘ן┘ה╪¿ / Print IR after optimization if requested
                    if (options_.print_ir_after_opt)
                    {
                        std::cout << "\n=== LLVM IR (╪¿╪╣╪» ╪º┘ה╪¬╪¡╪│┘ך┘ז / After Optimization) ===\n";
                        llvmModule_->print(llvm::outs(), nullptr);
                        std::cout << "\n";
                    }

                    // ╪╖╪¿╪º╪╣╪⌐ ╪Ñ╪¡╪╡╪º╪ª┘ך╪º╪¬ ╪º┘ה╪¬╪¡╪│┘ך┘ז ╪Ñ╪░╪º ╪╖┘ן┘ה╪¿ / Print optimization stats if requested
                    if (options_.print_optimization_stats)
                    {
                        // (AR) ╪Ñ╪¡╪╡╪º╪ª┘ך╪º╪¬ ╪º┘ה╪¬╪¡╪│┘ך┘ז Γאפ ╪¬┘ן╪│╪¬╪«╪▒╪¼ ┘ו┘ז ╪╣╪»╪» ╪º┘ה╪¬╪╣┘ה┘ך┘ו╪º╪¬ ┘ג╪¿┘ה/╪¿╪╣╪»
                        // (EN) Optimization stats Γאפ extracted from instruction count before/after
                        if (options_.verbose && llvmModule_)
                        {
                            size_t funcCount = 0;
                            size_t instrCount = 0;
                            for (auto &F : *llvmModule_)
                            {
                                if (!F.isDeclaration())
                                {
                                    funcCount++;
                                    for (auto &BB : F)
                                    {
                                        instrCount += BB.size();
                                    }
                                }
                            }
                            std::cout << "[Pipeline] ╪Ñ╪¡╪╡╪º╪ª┘ך╪º╪¬ ╪º┘ה╪¬╪¡╪│┘ך┘ז / Optimization stats:\n";
                            std::cout << "  ╪º┘ה╪»┘ט╪º┘ה / Functions: " << funcCount << "\n";
                            std::cout << "  ╪º┘ה╪¬╪╣┘ה┘ך┘ו╪º╪¬ / Instructions: " << instrCount << "\n";
                        }
                    }

                    return true;
                }
                catch (const std::exception &e)
                {
                    logError(std::string("╪«╪╖╪ú ┘ב┘ך ╪º┘ה╪¬╪¡╪│┘ך┘ז / Optimization error: ") + e.what());
                    return false;
                }
            }

            /**
             * ╪Ñ╪╡╪»╪º╪▒ LLVM IR ╪Ñ┘ה┘י ┘ו┘ה┘ב / Emit LLVM IR to file
             */
            bool LLVMCompilerPipeline::emitLLVMIR(const std::string &filename)
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪Ñ╪╡╪»╪º╪▒ LLVM IR ╪Ñ┘ה┘י / Emitting LLVM IR to: " << filename << "\n";
                    }

                    if (!llvmModule_)
                    {
                        logError("┘ה╪º ╪¬┘ט╪¼╪» ┘ט╪¡╪»╪⌐ LLVM / No LLVM module available");
                        return false;
                    }

                    return codeGen_->emitToFile(filename);
                }
                catch (const std::exception &e)
                {
                    logError(std::string("╪«╪╖╪ú ┘ב┘ך ╪Ñ╪╡╪»╪º╪▒ LLVM IR / LLVM IR emission error: ") + e.what());
                    return false;
                }
            }

            /**
             * ╪Ñ╪╡╪»╪º╪▒ Assembly / Emit assembly
             */
            bool LLVMCompilerPipeline::emitAssembly(const std::string &filename)
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪Ñ╪╡╪»╪º╪▒ Assembly ╪Ñ┘ה┘י / Emitting assembly to: " << filename << "\n";
                    }

                    if (!llvmModule_)
                    {
                        logError("┘ה╪º ╪¬┘ט╪¼╪» ┘ט╪¡╪»╪⌐ LLVM / No LLVM module available");
                        return false;
                    }

                    // ╪º╪│╪¬╪«╪»╪º┘ו ╪º┘ה┘ט╪¡╪»╪⌐ ╪º┘ה┘ו╪¡┘ה┘ך╪⌐ llvmModule_ ╪¿╪»┘ה╪º┘כ ┘ו┘ז codeGen_->module_
                    // Use local llvmModule_ instead of codeGen_->module_
                    return codeGen_->emitAssembly(filename, llvmModule_.get());
                }
                catch (const std::exception &e)
                {
                    logError(std::string("╪«╪╖╪ú ┘ב┘ך ╪Ñ╪╡╪»╪º╪▒ Assembly / Assembly emission error: ") + e.what());
                    return false;
                }
            }

            /**
             * ╪Ñ╪╡╪»╪º╪▒ Object file / Emit object file
             */
            bool LLVMCompilerPipeline::emitObjectFile(const std::string &filename)
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪Ñ╪╡╪»╪º╪▒ Object file ╪Ñ┘ה┘י / Emitting object file to: " << filename << "\n";
                    }

                    if (!llvmModule_)
                    {
                        logError("┘ה╪º ╪¬┘ט╪¼╪» ┘ט╪¡╪»╪⌐ LLVM / No LLVM module available");
                        return false;
                    }

                    // ╪º╪│╪¬╪«╪»╪º┘ו ╪º┘ה┘ט╪¡╪»╪⌐ ╪º┘ה┘ו╪¡┘ה┘ך╪⌐ llvmModule_ ╪¿╪»┘ה╪º┘כ ┘ו┘ז codeGen_->module_
                    // ┘ה╪ú┘ז generate() ┘ז┘ג┘ה╪¬ ╪º┘ה┘ו┘ה┘ד┘ך╪⌐ ╪¿┘א std::move
                    // Use local llvmModule_ instead of codeGen_->module_
                    // because generate() moved ownership with std::move
                    return codeGen_->emitObjectFile(filename, llvmModule_.get());
                }
                catch (const std::exception &e)
                {
                    logError(std::string("╪«╪╖╪ú ┘ב┘ך ╪Ñ╪╡╪»╪º╪▒ Object file / Object file emission error: ") + e.what());
                    return false;
                }
            }

            /**
             * ╪Ñ╪╡╪»╪º╪▒ Executable / Emit executable
             * Enhanced with Toolchain Detection / ┘ו╪¡╪│┘ס┘ז ╪¿┘ז╪╕╪º┘ו ┘ד╪┤┘ב ╪│┘ה╪│┘ה╪⌐ ╪º┘ה╪ú╪»┘ט╪º╪¬
             */
            bool LLVMCompilerPipeline::emitExecutable(const std::string &filename)
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ╪Ñ┘ז╪┤╪º╪í Executable: " << filename << "\n";
                    }

                    // ╪ú┘ט┘ה╪º┘כ: ╪Ñ┘ז╪┤╪º╪í object file / First: Create object file
                    std::string objFile = filename + ".o";
                    if (!emitObjectFile(objFile))
                    {
                        return false;
                    }

                    // ╪½╪º┘ז┘ך╪º┘כ: ╪º╪│╪¬╪«╪»╪º┘ו LLVMLinker ╪º┘ה┘ו╪¡╪│┘ס┘ז / Second: Use enhanced LLVMLinker
                    LLVMLinker linker;
                    if (!linker.initialize(options_.target_triple))
                    {
                        logWarning("┘ב╪┤┘ה ╪¬┘ח┘ך╪ª╪⌐ ╪º┘ה╪▒╪º╪¿╪╖╪ל ┘ו╪¡╪º┘ט┘ה╪⌐ ╪º┘ה╪▒╪¿╪╖ ╪º┘ה┘ך╪»┘ט┘ך / Linker initialization failed, trying manual link");
                    }

                    linker.addObjectFile(objFile);
                    linker.setOutputFile(filename);
                    linker.setEntryPoint("main");
                    linker.setVerbose(options_.verbose);

// ╪Ñ╪╢╪º┘ב╪⌐ ╪º┘ה┘ו┘ד╪¬╪¿╪º╪¬ ╪º┘ה╪º┘ב╪¬╪▒╪º╪╢┘ך╪⌐ ╪¡╪│╪¿ ╪º┘ה┘ז╪╕╪º┘ו / Add default libraries based on system
#ifdef _WIN32
                    linker.addLinkerFlag("/SUBSYSTEM:CONSOLE");
                    linker.addLinkerFlag("/MACHINE:X64");
#else
                    linker.addLinkerFlag("-pie");
#endif

                    bool linkSuccess = linker.link();

                    if (!linkSuccess)
                    {
                        logWarning("┘ב╪┤┘ה ╪º┘ה╪▒╪¿╪╖ ╪¿┘א LLVMLinker╪ל Object file ┘ו╪¬╪º╪¡ / Linking with LLVMLinker failed, object file available: " + objFile);

                        // ╪╖╪¿╪º╪╣╪⌐ ┘ו╪╣┘ה┘ט┘ו╪º╪¬ ╪º┘ה┘ו╪│╪º╪╣╪»╪⌐ / Print help information
                        const auto &info = linker.getLinkingInfo();
                        for (const auto &err : info.errors)
                        {
                            logWarning("Linker error: " + err);
                        }

                        // ┘ה╪º ┘ז┘ן╪▒╪¼╪╣ false ┘ה╪ú┘ז ╪º┘ה┘א object file ┘ו╪¬╪º╪¡ ┘ה┘ה╪º╪│╪¬╪«╪»╪º┘ו ╪º┘ה┘ך╪»┘ט┘ך
                        // Don't return false because object file is available for manual linking
                    }
                    else
                    {
                        if (options_.verbose)
                        {
                            linker.printLinkingInfo();
                        }
                    }

                    return true;
                }
                catch (const std::exception &e)
                {
                    logError(std::string("╪«╪╖╪ú ┘ב┘ך ╪Ñ┘ז╪┤╪º╪í Executable / Executable creation error: ") + e.what());
                    return false;
                }
            }

            /**
             * ╪º┘ה╪¡╪╡┘ט┘ה ╪╣┘ה┘י ╪º┘ה┘ט╪¡╪»╪⌐ ╪º┘ה╪¡╪º┘ה┘ך╪⌐ / Get current module
             */
            llvm::Module *LLVMCompilerPipeline::getCurrentModule()
            {
                return llvmModule_.get();
            }

            // ============================================================================
            // Helper Functions - ╪º┘ה╪»┘ט╪º┘ה ╪º┘ה┘ו╪│╪º╪╣╪»╪⌐
            // ============================================================================

            /**
             * ┘ג╪▒╪º╪í╪⌐ ┘ו┘ה┘ב / Read file
             */
            std::string LLVMCompilerPipeline::readFile(const std::string &filename)
            {
                auto file = sad::utf8::open_ifstream(filename);
                if (!file.is_open())
                {
                    logError("┘ב╪┤┘ה ┘ב╪¬╪¡ ╪º┘ה┘ו┘ה┘ב / Failed to open file: " + filename);
                    return "";
                }

                std::stringstream buffer;
                buffer << file.rdbuf();
                return buffer.str();
            }

            /**
             * ╪¬┘ז╪╕┘ך┘ב ╪º┘ה┘ו┘ט╪º╪▒╪» / Cleanup resources
             */
            void LLVMCompilerPipeline::cleanup()
            {
                tokens_.clear();
                ast_.clear();
                sirModule_.reset();
                llvmModule_.reset();
                errors_.clear();
                warnings_.clear();
            }

            /**
             * ┘ג┘ך╪º╪│ ╪º┘ה┘ט┘ג╪¬ / Measure time
             */
            double LLVMCompilerPipeline::measureTime(const std::chrono::high_resolution_clock::time_point &start) const
            {
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                return duration.count() / 1000.0; // ╪¬╪¡┘ט┘ך┘ה ╪Ñ┘ה┘י ┘ו┘ך┘ה┘ך ╪½╪º┘ז┘ך╪⌐ / Convert to milliseconds
            }

            /**
             * ╪¬╪│╪¼┘ך┘ה ╪«╪╖╪ú / Log error
             */
            void LLVMCompilerPipeline::logError(const std::string &message)
            {
                errors_.push_back(message);
                hasErrors_ = true;

                if (options_.verbose)
                {
                    std::cerr << "[Pipeline Error] " << message << "\n";
                }
            }

            /**
             * ╪¬╪│╪¼┘ך┘ה ╪¬╪¡╪░┘ך╪▒ / Log warning
             */
            void LLVMCompilerPipeline::logWarning(const std::string &message)
            {
                warnings_.push_back(message);

                if (options_.verbose)
                {
                    std::cout << "[Pipeline Warning] " << message << "\n";
                }
            }

            // ============================================================================
            // Freestanding Mode Support Γאפ ╪»╪╣┘ו ┘ט╪╢╪╣ Freestanding (╪¿┘ה╪º ┘ו┘ד╪¬╪¿╪⌐ ┘ג┘ך╪º╪│┘ך╪⌐)
            // ============================================================================
            //
            // ┘ח╪░┘ח ╪º┘ה╪»┘ט╪º┘ה ╪¬┘ן╪┤┘ד┘נ┘ס┘ה ╪º┘ה╪╖╪¿┘ג╪⌐ ╪º┘ה╪¬┘ך ╪¬╪»┘ו╪¼ ┘ט╪╢╪╣ no_std ┘ב┘ך ╪«╪╖ ╪º┘ה╪¬╪▒╪¼┘ו╪⌐.
            // ╪¬┘ן╪┤╪║┘מ┘ס┘ה ┘ג╪¿┘ה ┘ט╪¿╪╣╪» ┘ו╪▒╪º╪¡┘ה ╪º┘ה╪¬╪¡┘ה┘ך┘ה ┘ה╪º┘ד╪¬╪┤╪º┘ב ┘ט╪¬╪╖╪¿┘ך┘ג ┘ג┘ך┘ט╪» freestanding:
            //
            //  1. scanForFreestandingAttributes()  Γאפ ┘ג╪¿┘ה ╪º┘ה╪¬╪¡┘ה┘ך┘ה ╪º┘ה┘ז╪¡┘ט┘ך
            //  2. initializeFreestandingMode()     Γאפ ╪¿╪╣╪» ╪º┘ה╪º┘ד╪¬╪┤╪º┘ב
            //  3. checkFreestandingSymbol()        Γאפ ╪«┘ה╪º┘ה ╪¿┘ז╪º╪í SIR (┘ה┘ד┘ה ╪▒┘ו╪▓)
            //  4. finalizeFreestandingUnit()       Γאפ ╪¿╪╣╪» ╪¿┘ז╪º╪í SIR
            //
            // This layer integrates no_std mode into the compilation pipeline.
            // Runs before/after parsing to detect and enforce freestanding constraints.
            // ============================================================================

            /**
             * (AR) ┘ב╪¡╪╡ ╪º┘ה┘ד┘ט╪» ╪º┘ה┘ו╪╡╪»╪▒┘ך ┘ה╪│┘ו╪º╪¬ #![╪¿┘ה╪º_┘ו┘ד╪¬╪¿╪⌐_┘ג┘ך╪º╪│┘ך╪⌐]
             *
             * ┘ך┘ן╪┤╪║┘מ┘ס┘ה ┘ו╪▒╪⌐ ┘ט╪º╪¡╪»╪⌐ ┘ג╪¿┘ה ╪º┘ה╪¬╪¡┘ה┘ך┘ה ╪º┘ה┘ז╪¡┘ט┘ך.
             * ┘ך╪¡╪»┘ס╪½:
             *   - options_.no_std                  Γזנ ┘ח┘ה ┘ט┘ן╪¼╪» #![╪¿┘ה╪º_┘ו┘ד╪¬╪¿╪⌐_┘ג┘ך╪º╪│┘ך╪⌐]╪ƒ
             *   - options_.no_main                 Γזנ ┘ח┘ה ┘ט┘ן╪¼╪» #![╪¿┘ה╪º_╪▒╪ª┘ך╪│┘ך╪⌐]╪ƒ
             *   - options_.abort_on_panic          Γזנ ┘ח┘ה ┘ט┘ן╪¼╪» #![╪Ñ┘ך┘ג╪º┘ב_╪╣┘ז╪»_╪░╪╣╪▒]╪ƒ
             *   - options_.freestanding_entry_point Γזנ ╪º╪│┘ו ╪»╪º┘ה╪⌐ #[┘ז┘ג╪╖╪⌐_╪»╪«┘ט┘ה]
             *   - options_.freestanding_panic_handler Γזנ ╪º╪│┘ו ╪»╪º┘ה╪⌐ #[┘ו╪╣╪º┘ה╪¼_╪░╪╣╪▒]
             *   - options_.freestanding_auto_detected Γזנ true ╪Ñ╪░╪º ╪º┘ד╪¬┘ן╪┤┘ב ╪¬┘ה┘ג╪º╪ª┘ך╪º┘כ
             *   - freestandingModeActive_          Γזנ true ╪Ñ╪░╪º ┘ט╪¼╪¿ ╪¬┘ב╪╣┘ך┘ה ╪º┘ה┘ט╪╢╪╣
             *
             * (EN) Scan source code for #![no_std] attributes.
             * Runs once before parsing. Updates options and freestandingModeActive_.
             */
            bool LLVMCompilerPipeline::scanForFreestandingAttributes(const std::string &sourceCode)
            {
                // ΓפאΓפאΓפא ┘ב╪¡╪╡ ╪º┘ה┘ו╪╡╪»╪▒ ┘ה╪¼┘ו┘ך╪╣ ╪│┘ו╪º╪¬ no_std ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                noStdScanResult_ = sad::compiler::pipeline::NoStdIntegration::scanSourceForNoStd(sourceCode);

                // ΓפאΓפאΓפא ╪Ñ╪░╪º ┘ט┘ן╪¼╪»╪¬ ╪│┘ו╪⌐ ╪¿┘ה╪º_┘ו┘ד╪¬╪¿╪⌐_┘ג┘ך╪º╪│┘ך╪⌐ Γזע ┘ב╪╣┘ס┘ה ╪º┘ה┘ט╪╢╪╣ ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                if (noStdScanResult_.hasNoStd || options_.no_std)
                {
                    // ╪¬╪¡╪»┘ך╪½ ╪º┘ה╪«┘ך╪º╪▒╪º╪¬ ╪º┘ה╪¬┘ה┘ג╪º╪ª┘ך
                    if (noStdScanResult_.hasNoStd && !options_.no_std)
                    {
                        options_.no_std = true;
                        options_.freestanding_auto_detected = true;
                    }
                    if (noStdScanResult_.hasNoMain)
                    {
                        options_.no_main = true;
                    }
                    if (noStdScanResult_.hasAbortOnPanic)
                    {
                        options_.abort_on_panic = true;
                    }

                    // استخراج أسماء الدوال المعلّمة من نتيجة الفحص.
                    for (const auto &[attributeName, functionName] : noStdScanResult_.functionAttributes)
                    {
                        if ((attributeName == "entry_point" || attributeName == sad::compiler::pipeline::سمات::نقطة_دخول) && options_.freestanding_entry_point.empty())
                        {
                            options_.freestanding_entry_point = functionName;
                        }
                        else if ((attributeName == "panic_handler" || attributeName == sad::compiler::pipeline::سمات::معالج_ذعر) && options_.freestanding_panic_handler.empty())
                        {
                            options_.freestanding_panic_handler = functionName;
                        }
                    }

                    freestandingModeActive_ = true;

                    if (options_.verbose)
                    {
                        std::cout << "[Freestanding] Γ£ו ╪º┘ד╪¬┘ן╪┤┘ב ┘ט╪╢╪╣ ╪¿┘ה╪º_┘ו┘ד╪¬╪¿╪⌐_┘ג┘ך╪º╪│┘ך╪⌐ / no_std mode detected\n";
                        if (!options_.freestanding_entry_point.empty())
                        {
                            std::cout << "[Freestanding] ┘ז┘ג╪╖╪⌐ ╪º┘ה╪»╪«┘ט┘ה / Entry point: "
                                      << options_.freestanding_entry_point << "\n";
                        }
                        if (!options_.freestanding_panic_handler.empty())
                        {
                            std::cout << "[Freestanding] ┘ו╪╣╪º┘ה╪¼ ╪º┘ה╪░╪╣╪▒ / Panic handler: "
                                      << options_.freestanding_panic_handler << "\n";
                        }
                    }
                }

                // ╪º┘ה┘ב╪¡╪╡ ╪»╪º╪ª┘ו╪º┘כ ┘ז╪º╪¼╪¡ Γאפ ╪¡╪¬┘י ┘ה┘ט ┘ה┘ו ┘ך┘ן┘ד╪¬╪┤┘ב no_std
                // Scan always succeeds Γאפ even if no_std was not detected
                return true;
            }

            /**
             * (AR) ╪¬┘ח┘ך╪ª╪⌐ ┘ט╪╢╪╣ Freestanding ╪¿╪╣╪» ╪º┘ד╪¬╪┤╪º┘ב┘ח
             *
             * ┘ך┘ן┘ז╪┤╪ª NoStdConfig ┘ו┘ז ╪º┘ה╪«┘ך╪º╪▒╪º╪¬ ╪º┘ה┘ו╪¼┘ו┘מ┘ס╪╣╪⌐╪ל
             * ╪½┘ו ┘ך┘ן┘ז╪┤╪ª FreestandingCodeGen ┘ט┘ך┘ן╪╣╪»┘נ┘ס┘ח.
             * ┘ך┘ב╪¬╪¡ ┘ט╪¡╪»╪⌐ ╪º┘ה╪¬╪▒╪¼┘ו╪⌐ ┘ה╪¿╪»╪í ╪º┘ה╪¬╪¬╪¿╪╣.
             *
             * ┘ך╪¼╪¿ ╪º╪│╪¬╪»╪╣╪º╪ñ┘ח ╪¿╪╣╪» scanForFreestandingAttributes()
             * ┘ט┘ג╪¿┘ה ╪ú┘ך ┘ו╪▒╪¡┘ה╪⌐ ╪¿┘ז╪º╪í SIR.
             *
             * (EN) Initialize freestanding mode after detection.
             * Creates NoStdConfig from gathered options, then FreestandingCodeGen.
             */
            bool LLVMCompilerPipeline::initializeFreestandingMode(const std::string &filename)
            {
                if (!freestandingModeActive_)
                {
                    // ┘ט╪╢╪╣ freestanding ╪║┘ך╪▒ ┘ו┘ן┘ב╪╣┘מ┘ס┘ה Γאפ ┘ה╪º ╪┤┘ך╪í ┘ה┘ה╪¬┘ח┘ך╪ª╪⌐
                    return true;
                }

                // ΓפאΓפאΓפא ╪¿┘ז╪º╪í ╪Ñ╪╣╪»╪º╪» NoStdConfig ┘ו┘ז ╪«┘ך╪º╪▒╪º╪¬ ╪º┘ה┘ו╪¬╪▒╪¼┘ו ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                noStdConfig_.noStdEnabled = true;
                noStdConfig_.noMainEnabled = options_.no_main;
                noStdConfig_.abortOnPanic = options_.abort_on_panic;
                noStdConfig_.entryPoint = options_.freestanding_entry_point;
                noStdConfig_.panicHandler = options_.freestanding_panic_handler;
                noStdConfig_.allowAlloc = options_.freestanding_allow_alloc;
                noStdConfig_.allowFloat = options_.freestanding_allow_float;
                noStdConfig_.allowAtomics = options_.freestanding_allow_atomics;
                noStdConfig_.targetTriple = options_.target_triple;
                noStdConfig_.linkerScript = options_.freestanding_linker_script;

                // ΓפאΓפאΓפא ╪Ñ┘ז╪┤╪º╪í ┘ו┘ט┘ה┘ס╪» ╪º┘ה┘ד┘ט╪» ┘ה┘ט╪╢╪╣ freestanding ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                freestandingCodeGen_ = std::make_unique<
                    sad::compiler::freestanding::FreestandingCodeGen>(noStdConfig_);

                // ΓפאΓפאΓפא ┘ב╪¬╪¡ ┘ט╪¡╪»╪⌐ ╪º┘ה╪¬╪▒╪¼┘ו╪⌐ ┘ה╪¿╪»╪í ╪º┘ה╪¬╪¬╪¿╪╣ ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                auto result = freestandingCodeGen_->ابدأ_وحدة(filename);
                if (!result.نجاح)
                {
                    logError("[Freestanding] فشل تهيئة الوحدة: " + result.رسالة_الخطأ_عربي);
                    return false;
                }

                if (options_.verbose)
                {
                    std::cout << "[Freestanding] Γ£ו ╪¬┘ו╪¬ ╪º┘ה╪¬┘ח┘ך╪ª╪⌐ Γאפ ╪º┘ה┘ט╪¡╪»╪⌐: " << filename << "\n";
                    std::cout << "[Freestanding]   no_main:      " << (noStdConfig_.noMainEnabled ? "┘ז╪╣┘ו" : "┘ה╪º") << "\n";
                    std::cout << "[Freestanding]   abort_panic:  " << (noStdConfig_.abortOnPanic ? "┘ז╪╣┘ו" : "┘ה╪º") << "\n";
                    std::cout << "[Freestanding]   allow_alloc:  " << (noStdConfig_.allowAlloc ? "┘ז╪╣┘ו" : "┘ה╪º") << "\n";
                    std::cout << "[Freestanding]   ┘ח╪»┘ב / target: " << noStdConfig_.targetTriple << "\n";
                }

                return true;
            }

            /**
             * (AR) ╪º┘ה╪¬╪¡┘ג┘ג ╪º┘ה┘ז┘ח╪º╪ª┘ך ┘ו┘ז ╪º┘ד╪¬┘ו╪º┘ה ┘ט╪¡╪»╪⌐ freestanding
             *
             * ┘ך┘ן╪┤╪║┘מ┘ס┘ה ┘ב┘ך ┘ז┘ח╪º┘ך╪⌐ ┘ו╪▒╪¡┘ה╪⌐ ╪¿┘ז╪º╪í SIR ╪¿╪╣╪» ┘ו╪╣╪º┘ה╪¼╪⌐ ╪¼┘ו┘ך╪╣ ╪º┘ה╪¬╪╣╪▒┘ך┘ב╪º╪¬.
             * ┘ך╪¬╪¡┘ג┘ג ┘ו┘ז:
             *   Γ£ף ┘ט╪¼┘ט╪» ┘ז┘ג╪╖╪⌐_╪»╪«┘ט┘ה   (┘ו╪╖┘ה┘ט╪¿ ╪Ñ╪░╪º no_main = true)
             *   Γ£ף ┘ט╪¼┘ט╪» ┘ו╪╣╪º┘ה╪¼_╪░╪╣╪▒   (┘ו╪╖┘ה┘ט╪¿ ╪»╪º╪ª┘ו╪º┘כ ┘ב┘ך freestanding)
             *   Γ£ף ╪╖╪¿╪º╪╣╪⌐ ╪º┘ה╪Ñ╪¡╪╡╪º╪ª┘ך╪º╪¬ ╪Ñ╪░╪º verbose
             *   Γ£ף ╪╖╪¿╪º╪╣╪⌐ ╪º┘ה╪¬╪¡╪░┘ך╪▒╪º╪¬
             *
             * ┘ך┘ן╪¬╪│╪º┘ו╪¡ ┘ו╪╣ ╪║┘ך╪º╪¿ ┘ו╪╣╪º┘ה╪¼_╪░╪╣╪▒ (┘ך┘ן╪│╪¬╪«╪»┘ו ╪º┘ה╪º┘ב╪¬╪▒╪º╪╢┘ך).
             * ┘ך┘ן┘ב╪┤┘נ┘ה ╪º┘ה╪¬╪▒╪¼┘ו╪⌐ ╪Ñ╪░╪º ┘ד╪º┘ז╪¬ no_main=true ┘ט┘ה╪º ╪¬┘ט╪¼╪» ┘ז┘ג╪╖╪⌐_╪»╪«┘ט┘ה.
             *
             * (EN) Final validation of freestanding unit completeness.
             * Runs at end of SIR building after all definitions are processed.
             */
            bool LLVMCompilerPipeline::finalizeFreestandingUnit()
            {
                if (!freestandingModeActive_ || !freestandingCodeGen_)
                {
                    return true; // ┘ט╪╢╪╣ ╪╣╪º╪»┘ך Γאפ ┘ה╪º ╪¡╪º╪¼╪⌐ ┘ה┘ה╪¬╪¡┘ג┘ג
                }

                // ΓפאΓפאΓפא ╪Ñ╪║┘ה╪º┘ג ╪º┘ה┘ט╪¡╪»╪⌐ + ╪º┘ה╪¬╪¡┘ג┘ג ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                auto result = freestandingCodeGen_->أنهِ_وحدة();

                // ΓפאΓפאΓפא ╪╖╪¿╪º╪╣╪⌐ ╪º┘ה╪¬╪¡╪░┘ך╪▒╪º╪¬ ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                for (const auto &warning : freestandingCodeGen_->احصل_على_التحذيرات())
                {
                    logWarning("[Freestanding] " + warning);
                }

                // ΓפאΓפאΓפא ╪╖╪¿╪º╪╣╪⌐ ╪º┘ה╪ú╪«╪╖╪º╪í ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                for (const auto &error : freestandingCodeGen_->احصل_على_الأخطاء())
                {
                    logError("[Freestanding] " + error.رسالة_الخطأ_عربي);
                }

                // ΓפאΓפאΓפא ╪╖╪¿╪º╪╣╪⌐ ╪º┘ה╪Ñ╪¡╪╡╪º╪ª┘ך╪º╪¬ ┘ב┘ך ╪º┘ה┘ט╪╢╪╣ ╪º┘ה┘ו┘ב╪╡┘ס┘ה ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                if (options_.verbose)
                {
                    std::cout << freestandingCodeGen_->احصل_على_الإحصائيات();
                }

                // ΓפאΓפאΓפא ╪¬┘ג╪▒┘ך╪▒ ╪º┘ה┘ז╪¬┘ך╪¼╪⌐ ╪º┘ה┘ז┘ח╪º╪ª┘ך╪⌐ ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                if (!result.نجاح)
                {
                    // ┘ז┘ג╪╖╪⌐ ╪º┘ה╪»╪«┘ט┘ה ┘ו┘ב┘ג┘ט╪»╪⌐ ┘ב┘ך ┘ט╪╢╪╣ no_main Γאפ ╪«╪╖╪ú ┘ב╪º╪»╪¡
                    if (result.رمز_الخطأ ==
                        sad::compiler::freestanding::FreestandingError::نقطة_دخول_مفقودة)
                    {
                        logError(
                            "[Freestanding] Γ¥ל ┘ז┘ג╪╖╪⌐ ╪º┘ה╪»╪«┘ט┘ה ┘ו┘ב┘ג┘ט╪»╪⌐!\n"
                            "  ╪ú╪╢┘ב ╪º┘ה╪│┘ו╪⌐ #[┘ז┘ג╪╖╪⌐_╪»╪«┘ט┘ה] ┘ג╪¿┘ה ╪»╪º┘ה╪⌐ ╪º┘ה╪¿╪»╪í:\n"
                            "  #[┘ז┘ג╪╖╪⌐_╪»╪«┘ט┘ה]\n"
                            "  ┘ה┘ז_╪¬╪▒╪¼╪╣ ╪»╪º┘ה╪⌐ _start() { ... }\n"
                            "\n"
                            "  ╪ú┘ט ╪Ñ╪░╪º ┘ד╪º┘ז╪¬ ╪»╪º┘ה╪⌐ main ┘ו┘ט╪¼┘ט╪»╪⌐╪ל ╪ú╪╢┘ב #![╪¿┘ה╪º_╪▒╪ª┘ך╪│┘ך╪⌐] ┘ב┘ך ╪ú╪╣┘ה┘י ╪º┘ה┘ו┘ה┘ב.");
                        return false;
                    }

                    // ┘ו╪╣╪º┘ה╪¼ ╪º┘ה╪░╪╣╪▒ ┘ו┘ב┘ג┘ט╪» Γאפ ╪¬╪¡╪░┘ך╪▒ (╪│┘ך┘ן╪│╪¬╪«╪»┘ו ╪º┘ה╪º┘ב╪¬╪▒╪º╪╢┘ך)
                    if (result.رمز_الخطأ ==
                        sad::compiler::freestanding::FreestandingError::معالج_ذعر_مفقود)
                    {
                        logWarning(
                            "[Freestanding] Γתá ┘ו╪╣╪º┘ה╪¼ ╪░╪╣╪▒ ┘ו╪«╪╡╪╡ ╪║┘ך╪▒ ┘ו┘ט╪¼┘ט╪»╪ל ╪│┘ך┘ן╪│╪¬╪«╪»┘ו ╪º┘ה╪º┘ב╪¬╪▒╪º╪╢┘ך.\n"
                            "  ╪º┘ה╪º┘ב╪¬╪▒╪º╪╢┘ך: ╪¡┘ה┘ג╪⌐ ┘ה╪º┘ז┘ח╪º╪ª┘ך╪⌐ + hlt (┘ו┘ז╪º╪│╪¿ ┘ה┘ה┘ז┘ט╪º╪⌐).\n"
                            "  ┘ה╪¬┘ט┘ב┘ך╪▒ ┘ו╪╣╪º┘ה╪¼ ┘ו╪«╪╡╪╡:\n"
                            "  #[┘ו╪╣╪º┘ה╪¼_╪░╪╣╪▒]\n"
                            "  ┘ה┘ז_╪¬╪▒╪¼╪╣ ╪»╪º┘ה╪⌐ ╪╣┘ז╪»_╪º┘ה╪░╪╣╪▒(┘ו╪╣┘ה┘ט┘ו╪º╪¬: &┘ו╪╣┘ה┘ט┘ו╪º╪¬_╪░╪╣╪▒) { ... }");
                        // ┘ה┘ך╪│ ╪«╪╖╪ú Γאפ ┘ז╪│╪¬┘ו╪▒
                    }
                }

                if (options_.verbose)
                {
                    std::cout << "[Freestanding] Γ£ו ╪º┘ד╪¬┘ו┘ה ╪º┘ה╪¬╪¡┘ג┘ג ╪º┘ה┘ז┘ח╪º╪ª┘ך ┘ו┘ז ╪º┘ה┘ט╪¡╪»╪⌐\n";
                }

                return true;
            }

            /**
             * (AR) ┘ב╪¡╪╡ ╪▒┘ו╪▓ ┘ב┘ך ┘ט╪╢╪╣ freestanding
             *
             * ┘ך┘ן╪│╪¬╪»╪╣┘י ╪«┘ה╪º┘ה ╪¿┘ז╪º╪í SIR ╪╣┘ז╪» ┘ו╪╡╪º╪»┘ב╪⌐ ╪»╪º┘ה╪⌐ ╪ú┘ט ┘ז┘ט╪╣.
             * ┘ך╪¬╪¡┘ג┘ג ┘ו┘ז ╪ú┘ז ╪º┘ה╪▒┘ו╪▓ ┘ה┘ך╪│ ┘ו┘ז ╪º┘ה┘ו┘ד╪¬╪¿╪⌐ ╪º┘ה┘ג┘ך╪º╪│┘ך╪⌐.
             *
             * ╪ú┘ו╪½┘ה╪⌐ ╪╣┘ה┘י ╪▒┘ו┘ט╪▓ ┘ו╪▒┘ב┘ט╪╢╪⌐:
             *   "printf"     Γזע ╪º╪│╪¬╪«╪»┘ו ╪¿╪º┘ב╪▒_┘ז╪╡ + ╪╖╪¿╪º╪╣╪⌐_╪¬╪│┘ה╪│┘ה┘ך()
             *   "std::string" Γזע ╪º╪│╪¬╪«╪»┘ו ╪╣╪▒╪╢_┘ז╪╡
             *   "malloc"     Γזע ╪º╪│╪¬╪«╪»┘ו sad_alloc()
             *   "std::vector" Γזע ╪º╪│╪¬╪«╪»┘ו ┘ו╪╡┘ב┘ט┘ב╪⌐ ╪½╪º╪¿╪¬╪⌐
             *
             * @param symbolName ╪º╪│┘ו ╪º┘ה╪▒┘ו╪▓
             * @return true ╪Ñ╪░╪º ┘ד╪º┘ז ╪º┘ה╪▒┘ו╪▓ ┘ו┘ג╪¿┘ט┘ה╪º┘כ ┘ב┘ך ┘ט╪╢╪╣ freestanding
             *
             * (EN) Check symbol in freestanding mode.
             * Called during SIR building when encountering functions/types.
             */
            bool LLVMCompilerPipeline::checkFreestandingSymbol(const std::string &symbolName)
            {
                if (!freestandingModeActive_ || !freestandingCodeGen_)
                {
                    return true; // ┘ט╪╢╪╣ ╪╣╪º╪»┘ך Γאפ ┘ד┘ה ╪º┘ה╪▒┘ו┘ט╪▓ ┘ו┘ג╪¿┘ט┘ה╪⌐
                }

                // ΓפאΓפאΓפא ┘ב╪¡╪╡ ╪º╪│┘ו ╪º┘ה╪▒┘ו╪▓ ΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפאΓפא
                auto result = freestandingCodeGen_->تحقق_من_الرمز(symbolName);
                if (!result.نجاح)
                {
                    logError("[Freestanding] " + result.رسالة_الخطأ_عربي);
                    return false;
                }

                return true;
            }

        } // namespace LLVM
    } // namespace Compiler
} // namespace Sad
