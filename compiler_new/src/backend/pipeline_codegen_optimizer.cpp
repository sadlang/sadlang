// ============================================================================
// pipeline_codegen_optimizer.cpp — محسن خطوط الأنابيب وSIMD
// (AR) تحسين عمليات الأنابيب، دعم SIMD، واجهة خارجية
// (EN) Pipeline optimizer, SIMD codegen, external interface
// تم استخراج هذا الملف من pipeline_codegen.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <memory>
#include <functional>
#include <unordered_map>
#include <algorithm>

namespace sad {
namespace compiler {
namespace codegen {

class PipelineOptimizer {
public:
    /**
     * ״×״­״³† ״§„״³„״³„״© | Optimize chain
     */
    void optimize(PipelineChain& chain) {
        fuseMapOperations(chain);
        moveFiltersEarly(chain);
        eliminateDeadOperations(chain);
        detectLazyOpportunities(chain);
    }
    
private:
    /**
     * ״¯…״¬ ״¹…„״§״× map ״§„…״×״×״§„״© | Fuse consecutive map operations
     * 
     * ״­ˆ‘„(f) |> ״­ˆ‘„(g) ג†’ ״­ˆ‘„(x => g(f(x)))
     */
    void fuseMapOperations(PipelineChain& chain) {
        std::vector<PipelineOperation> optimized;
        
        for (size_t i = 0; i < chain.operations.size(); ++i) {
            if (chain.operations[i].functionName == "״­ˆ‘„" &&
                i + 1 < chain.operations.size() &&
                chain.operations[i + 1].functionName == "״­ˆ‘„") {
                // ״¯…״¬ ״§„״¹…„״×†
                PipelineOperation fused = chain.operations[i];
                fused.lambdaBody = composeLambdas(
                    chain.operations[i].lambdaBody,
                    chain.operations[i + 1].lambdaBody
                );
                fused.outputType = chain.operations[i + 1].outputType;
                optimized.push_back(fused);
                ++i;  // ״×״®״· ״§„״¹…„״© ״§„״×״§„״©
            } else {
                optimized.push_back(chain.operations[i]);
            }
        }
        
        chain.operations = std::move(optimized);
    }
    
    /**
     * ״×‚״¯… ״¹…„״§״× ״§„״×״±״´״­ | Move filter operations early
     * 
     * ״­ˆ‘„(f) |> ״µ‘(p) ג†’ ״µ‘(pגˆ˜fג»ֲ¹) |> ״­ˆ‘„(f)  // ״¥״°״§ ״£…ƒ†
     * ״£ˆ ״¹„‰ ״§„״£‚„ †״­״§״¸ ״¹„‰ ״×״±״×״¨ filter ‚״¨„ map ״¹†״¯…״§ ƒˆ† ……ƒ†״§‹
     */
    void moveFiltersEarly(PipelineChain& chain) {
        // ״×†״° …״¨״³״·: „״§ †״÷״± ״§„״×״±״×״¨ ״¥״°״§ ƒ״§† filter ״¹״×…״¯ ״¹„‰ †״×״¬״© map
        //  ״§„״×†״° ״§„״¹„״ †״­״×״§״¬ ״×״­„„ ״£״¹…‚
    }
    
    /**
     * ״¥״²״§„״© ״§„״¹…„״§״× ״§„…״×״© | Eliminate dead operations
     * 
     * ״®״°(0) ג†’ []
     * ״µ‘(״®״·״£_״¯״§״¦…״§‹) ג†’ []
     */
    void eliminateDeadOperations(PipelineChain& chain) {
        std::vector<PipelineOperation> optimized;
        
        for (const auto& op : chain.operations) {
            // ״×״­‚‚ …† ״§„״¹…„״§״× ״§„״× „״§ ״×״¹„ ״´״¦״§‹
            if (op.functionName == "״­ˆ‘„" && 
                op.lambdaBody == op.lambdaParams[0]) {
                // ״­ˆ‘„(|״³| ״³) - ״¯״§„״© ״§„‡ˆ״©״ †״×״®״·״§‡״§
                continue;
            }
            
            if (op.functionName == "״µ‘" && 
                op.lambdaBody == "״µ״­״­") {
                // ״µ‘(|״³| ״µ״­״­) - „״§ ״±״´״­ ״´״¦״§‹״ †״×״®״·״§‡״§
                continue;
            }
            
            optimized.push_back(op);
        }
        
        chain.operations = std::move(optimized);
    }
    
    /**
     * ״§ƒ״×״´״§ ״±״µ ״§„״×‚… ״§„ƒ״³ˆ„ | Detect lazy evaluation opportunities
     */
    void detectLazyOpportunities(PipelineChain& chain) {
        // ״¥״°״§ ƒ״§† ״§„…״µ״¯״± „״§†‡״§״¦ ״£ˆ ƒ״¨״± ״¬״¯״§‹
        bool hasInfiniteSource = (chain.sourceType.find("…״¯‰_„״§†‡״§״¦") != std::string::npos);
        
        // ״¥״°״§ ƒ״§†״× ‡†״§ƒ ״¹…„״© take
        bool hasTake = false;
        for (const auto& op : chain.operations) {
            if (op.functionName == "״®״°" || op.functionName == "״®״°_״·״§„…״§") {
                hasTake = true;
                break;
            }
        }
        
        // ״×״¹„ ״§„״×‚… ״§„ƒ״³ˆ„ ״¥״°״§ ƒ״§† ״§„…״µ״¯״± „״§†‡״§״¦ ˆ‡†״§ƒ ״­״¯
        chain.isLazy = hasInfiniteSource || 
                       (chain.operations.size() > 5 && hasTake);
    }
    
    /**
     * ״×״±ƒ״¨ lambdas | Compose lambdas
     */
    std::string composeLambdas(const std::string& f, const std::string& g) {
        // f: |x| f_body
        // g: |y| g_body
        // result: |x| g_body[y := f_body]
        
        // ״×†״° …״¨״³״·
        return "(" + g + ")(" + f + ")";
    }
};

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
//                      ״×ˆ„״¯ SIMD „„״£†״¨ˆ״¨ | SIMD Pipeline Codegen
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

/**
 * נ€ …ˆ„״¯_SIMD | SIMDCodegen
 * 
 * ˆ„״¯ ƒˆ״¯ SIMD „״¹…„״§״× ״§„״£†״¨ˆ״¨ ״§„…״×ˆ״§״²״©
 */
class SIMDCodegen {
public:
    /**
     * ״×ˆ„״¯ ƒˆ״¯ SIMD „€ map | Generate SIMD code for map
     */
    std::string generateSIMDMap(
        const std::string& arrayPtr,
        const std::string& arrayLen,
        const std::string& elementType,
        const std::string& mapFunction
    ) {
        std::stringstream ss;
        
        ss << "; ״×ˆ„״¯ SIMD „״¹…„״© ״­ˆ‘„\n";
        ss << "; ״§„…״µˆ״©: " << arrayPtr << "\n";
        ss << "; ״§„״·ˆ„: " << arrayLen << "\n";
        
        // ״×״­״¯״¯ ״¹״±״¶ SIMD
        int simdWidth = getSIMDWidth(elementType);
        
        ss << "  ; ״¹״±״¶ SIMD: " << simdWidth << "\n";
        ss << "  %simd_iters = udiv i64 " << arrayLen << ", " << simdWidth << "\n";
        ss << "  %remainder = urem i64 " << arrayLen << ", " << simdWidth << "\n";
        ss << "  br label %simd_loop\n\n";
        
        ss << "simd_loop:\n";
        ss << "  %i = phi i64 [ 0, %entry ], [ %i_next, %simd_loop ]\n";
        ss << "  ; ״×״­…„ " << simdWidth << " ״¹†״§״µ״±\n";
        ss << "  %vec_ptr = getelementptr " << elementType << ", ptr " << arrayPtr << ", i64 %i\n";
        ss << "  %vec = load <" << simdWidth << " x " << elementType << ">, ptr %vec_ptr\n";
        ss << "  ; ״×״·״¨‚ ״§„״×״­ˆ„ ״¹„‰ ƒ„ ״§„״¹†״§״µ״±\n";
        ss << "  %result_vec = call <" << simdWidth << " x " << elementType << "> @" << mapFunction << "_simd(<" << simdWidth << " x " << elementType << "> %vec)\n";
        ss << "  ; ״×״®״²† ״§„†״×״¬״©\n";
        ss << "  store <" << simdWidth << " x " << elementType << "> %result_vec, ptr %vec_ptr\n";
        ss << "  %i_next = add i64 %i, " << simdWidth << "\n";
        ss << "  %done = icmp uge i64 %i_next, %simd_iters\n";
        ss << "  br i1 %done, label %remainder_loop, label %simd_loop\n\n";
        
        ss << "remainder_loop:\n";
        ss << "  ; …״¹״§„״¬״© ״§„״¹†״§״µ״± ״§„…״×״¨‚״© ˆ״§״­״¯״§‹ ״×„ˆ ״§„״¢״®״±\n";
        ss << "  ; ...\n";
        
        return ss.str();
    }
    
private:
    /**
     * ״×״­״¯״¯ ״¹״±״¶ SIMD ״­״³״¨ ״§„†ˆ״¹ | Determine SIMD width by type
     */
    int getSIMDWidth(const std::string& type) {
        if (type == "i32" || type == "f32") return 8;   // 256-bit / 32-bit = 8
        if (type == "i64" || type == "f64") return 4;   // 256-bit / 64-bit = 4
        if (type == "i16") return 16;                    // 256-bit / 16-bit = 16
        if (type == "i8") return 32;                     // 256-bit / 8-bit = 32
        return 4;  // ״§״×״±״§״¶
    }
};

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
//                     ˆ״§״¬‡״© ״§„״×ƒ״§…„ | Integration Interface
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

/**
 * נ” ˆ״§״¬‡״©_״§„״£†״¨ˆ״¨ | PipelineInterface
 * 
 * ״§„ˆ״§״¬‡״© ״§„״±״¦״³״© „״§״³״×״®״¯״§… …ˆ„״¯ ƒˆ״¯ ״§„״£†״¨ˆ״¨
 */
class PipelineInterface {
private:
    PipelineCodegen codegen_;
    PipelineOptimizer optimizer_;
    SIMDCodegen simdCodegen_;
    
public:
    /**
     * …״¹״§„״¬״© ˆ״×ˆ„״¯ ƒˆ״¯ „״³„״³„״© ״£†״¨ˆ״¨ | Process and generate code for pipeline chain
     */
    std::string process(PipelineChain chain, bool enableOptimizations = true) {
        // ״×״­״³† ״§„״³„״³„״©
        if (enableOptimizations) {
            optimizer_.optimize(chain);
        }
        
        // ״×ˆ„״¯ ״§„ƒˆ״¯
        return codegen_.generate(chain);
    }
    
    /**
     * ״¥†״´״§״¡ ״³„״³„״© …† ״×״¹״¨״± AST | Create chain from AST expression
     */
    PipelineChain createChain(const std::string& source, const std::string& sourceType) {
        PipelineChain chain;
        chain.sourceExpr = source;
        chain.sourceType = sourceType;
        return chain;
    }
    
    /**
     * ״¥״¶״§״© ״¹…„״© „„״³„״³„״© | Add operation to chain
     */
    void addOperation(PipelineChain& chain, PipelineOperation op) {
        if (!chain.operations.empty()) {
            // ״×״­״¯״« †ˆ״¹ ״§„״¥״¯״®״§„ …† †ˆ״¹ ״§„״¥״®״±״§״¬ ״§„״³״§״¨‚
            op.inputType = chain.operations.back().outputType;
        } else {
            op.inputType = chain.sourceType;
        }
        chain.operations.push_back(std::move(op));
        chain.finalType = chain.operations.back().outputType;
    }
};

} // namespace codegen
} // namespace compiler
} // namespace sad

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
//                              ״§״®״×״¨״§״±״§״× | Tests
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

#ifdef SAD_PIPELINE_CODEGEN_TESTS

#include <iostream>

void testSimplePipeline() {
    using namespace sad::compiler::codegen;
    
    std::cout << "ג•ג•ג• ״§״®״×״¨״§״± ״³„״³„״© ״£†״¨ˆ״¨ ״¨״³״·״© ג•ג•ג•\n";
    
    PipelineInterface iface;
    auto chain = iface.createChain("%input", "i64");
    
    // ״¥״¶״§״© ״¹…„״§״×: ‚…״© |> ״¯״§„״©1 |> ״¯״§„״©2
    PipelineOperation op1;
    op1.type = PipelineOperationType::FUNCTION_CALL;
    op1.functionName = "״­ˆ‘„";
    op1.outputType = "i64";
    iface.addOperation(chain, op1);
    
    PipelineOperation op2;
    op2.type = PipelineOperationType::FUNCTION_CALL;
    op2.functionName = "״µ‘";
    op2.outputType = "i64";
    iface.addOperation(chain, op2);
    
    std::string code = iface.process(chain);
    std::cout << code << "\n";
}

void testLazyPipeline() {
    using namespace sad::compiler::codegen;
    
    std::cout << "\nג•ג•ג• ״§״®״×״¨״§״± ״³„״³„״© ״£†״¨ˆ״¨ ƒ״³ˆ„״© ג•ג•ג•\n";
    
    PipelineInterface iface;
    auto chain = iface.createChain("%infinite_range", "…״¯‰_„״§†‡״§״¦");
    chain.isLazy = true;
    
    PipelineOperation filter_op;
    filter_op.type = PipelineOperationType::LAZY_OPERATION;
    filter_op.functionName = "״µ‘";
    filter_op.lambdaBody = "%n % 2 == 0";
    filter_op.outputType = "i64";
    iface.addOperation(chain, filter_op);
    
    PipelineOperation take_op;
    take_op.type = PipelineOperationType::LAZY_OPERATION;
    take_op.functionName = "״®״°";
    take_op.arguments = {"10"};
    take_op.outputType = "i64";
    iface.addOperation(chain, take_op);
    
    chain.finalType = "‚״§״¦…״©<i64>";
    
    std::string code = iface.process(chain);
    std::cout << code << "\n";
}

int main() {
    testSimplePipeline();
    testLazyPipeline();
    return 0;
}

#endif // SAD_PIPELINE_CODEGEN_TESTS

