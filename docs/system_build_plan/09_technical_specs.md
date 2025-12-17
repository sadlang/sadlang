# المواصفات التقنية التفصيلية
# Detailed Technical Specifications

**الإصدار / Version:** 1.0  
**آخر تحديث / Last Updated:** ديسمبر 2025 / December 2025

---

## 📋 جدول المحتويات / Table of Contents

1. [معمارية المترجم / Compiler Architecture](#compiler-architecture)
2. [مواصفات الذاكرة / Memory Specifications](#memory-specs)
3. [System Call Interface](#syscall-interface)
4. [ABI Conventions](#abi-conventions)
5. [Binary Format](#binary-format)
6. [Kernel Specifications](#kernel-specs)
7. [Driver Interface](#driver-interface)
8. [Network Protocol Stack](#network-stack)

---

<a name="compiler-architecture"></a>
## 🏗️ معمارية المترجم / Compiler Architecture

### العربية

#### Pipeline المترجم

```
Source Code (.s)
    ↓
┌─────────────────┐
│  Lexer          │ → Tokens
│  المحلل اللغوي   │
└─────────────────┘
    ↓
┌─────────────────┐
│  Parser         │ → AST
│  المحلل النحوي   │
└─────────────────┘
    ↓
┌─────────────────┐
│  Semantic       │ → Validated AST
│  Analysis       │
│  التحليل الدلالي │
└─────────────────┘
    ↓
┌─────────────────┐
│  IR Generation  │ → LLVM IR
│  توليد IR       │
└─────────────────┘
    ↓
┌─────────────────┐
│  Optimization   │ → Optimized IR
│  التحسين        │
└─────────────────┘
    ↓
┌─────────────────┐
│  Code Gen       │ → Machine Code
│  توليد الكود     │
└─────────────────┘
    ↓
┌─────────────────┐
│  Linking        │ → Executable
│  الربط          │
└─────────────────┘
```

### Compiler Phases Detailed

#### Phase 1: Lexical Analysis

**Input:** Raw source code  
**Output:** Token stream

**Token Types:**
```cpp
enum class TokenType {
    // Keywords (Arabic)
    KEYWORD_NUMBER,      // رقم
    KEYWORD_TEXT,        // نص
    KEYWORD_BOOLEAN,     // منطقي
    KEYWORD_IF,          // إذا
    KEYWORD_ELSE,        // وإلا
    KEYWORD_WHILE,       // بينما
    KEYWORD_FOR,         // لكل
    KEYWORD_FUNCTION,    // دالة
    KEYWORD_CLASS,       // صنف
    KEYWORD_RETURN,      // أرجع
    KEYWORD_NEW,         // جديد
    KEYWORD_THIS,        // هذا
    KEYWORD_POINTER,     // مؤشر
    
    // Keywords (English)
    KEYWORD_NUMBER_EN,   // number
    KEYWORD_TEXT_EN,     // text
    // ... (English equivalents)
    
    // Literals
    LITERAL_NUMBER,      // 123, 3.14
    LITERAL_STRING,      // "hello"
    LITERAL_BOOLEAN,     // true, false, صحيح, خطأ
    
    // Operators
    OP_PLUS,            // +
    OP_MINUS,           // -
    OP_MULTIPLY,        // *
    OP_DIVIDE,          // /
    OP_MODULO,          // %
    OP_ASSIGN,          // =
    OP_EQUAL,           // ==
    OP_NOT_EQUAL,       // !=
    OP_LESS,            // <
    OP_GREATER,         // >
    OP_LESS_EQUAL,      // <=
    OP_GREATER_EQUAL,   // >=
    OP_AND,             // &&
    OP_OR,              // ||
    OP_NOT,             // !
    OP_BITWISE_AND,     // &
    OP_BITWISE_OR,      // |
    OP_BITWISE_XOR,     // ^
    OP_BITWISE_NOT,     // ~
    OP_LEFT_SHIFT,      // <<
    OP_RIGHT_SHIFT,     // >>
    OP_ARROW,           // ->
    OP_DOT,             // .
    
    // Delimiters
    DELIMITER_SEMICOLON,  // ;
    DELIMITER_COMMA,      // ,
    DELIMITER_LPAREN,     // (
    DELIMITER_RPAREN,     // )
    DELIMITER_LBRACE,     // {
    DELIMITER_RBRACE,     // }
    DELIMITER_LBRACKET,   // [
    DELIMITER_RBRACKET,   // ]
    
    // Identifiers
    IDENTIFIER,           // variable/function names
    
    // Special
    END_OF_FILE,
    ERROR
};
```

**Lexer Algorithm:**
```cpp
class Lexer {
    std::string source;
    size_t position;
    
public:
    Token nextToken() {
        skipWhitespace();
        skipComments();
        
        if (isAtEnd()) return Token(END_OF_FILE);
        
        char current = peek();
        
        // Numbers
        if (isDigit(current)) return scanNumber();
        
        // Strings
        if (current == '"' || current == '\'') return scanString();
        
        // Keywords and Identifiers
        if (isAlpha(current) || isArabic(current)) return scanIdentifier();
        
        // Operators and Delimiters
        return scanOperator();
    }
};
```

---

#### Phase 2: Syntax Analysis (Parser)

**Input:** Token stream  
**Output:** Abstract Syntax Tree (AST)

**AST Node Types:**
```cpp
// Base class
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(Visitor* v) = 0;
};

// Expressions
class ExprNode : public ASTNode { };

class LiteralExpr : public ExprNode {
    Value value;
};

class BinaryExpr : public ExprNode {
    std::unique_ptr<ExprNode> left;
    TokenType op;
    std::unique_ptr<ExprNode> right;
};

class CallExpr : public ExprNode {
    std::string callee;
    std::vector<std::unique_ptr<ExprNode>> args;
};

class MemberAccessExpr : public ExprNode {
    std::unique_ptr<ExprNode> object;
    std::string member;
};

class PointerExpr : public ExprNode {
    enum Type { DEREFERENCE, ADDRESS_OF };
    Type type;
    std::unique_ptr<ExprNode> operand;
};

// Statements
class StmtNode : public ASTNode { };

class VarDeclStmt : public StmtNode {
    std::string name;
    std::string type;
    std::unique_ptr<ExprNode> initializer;
};

class FunctionDeclStmt : public StmtNode {
    std::string name;
    std::vector<Parameter> params;
    std::string returnType;
    std::unique_ptr<BlockStmt> body;
};

class ClassDeclStmt : public StmtNode {
    std::string name;
    std::vector<std::unique_ptr<VarDeclStmt>> fields;
    std::vector<std::unique_ptr<FunctionDeclStmt>> methods;
    AccessLevel defaultAccess;
};

class IfStmt : public StmtNode {
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<StmtNode> thenBranch;
    std::unique_ptr<StmtNode> elseBranch;
};

class WhileStmt : public StmtNode {
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<StmtNode> body;
};

class ReturnStmt : public StmtNode {
    std::unique_ptr<ExprNode> value;
};

class InlineAsmStmt : public StmtNode {
    std::string asmCode;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::vector<std::string> clobbers;
};
```

---

#### Phase 3: Semantic Analysis

**Type System:**
```cpp
class Type {
public:
    enum Kind {
        VOID,
        NUMBER,
        TEXT,
        BOOLEAN,
        POINTER,
        ARRAY,
        CLASS,
        FUNCTION
    };
    
    Kind kind;
    
    // For pointer/array
    Type* elementType;
    
    // For class
    std::string className;
    
    // For function
    std::vector<Type*> paramTypes;
    Type* returnType;
};

class TypeChecker {
public:
    void check(ASTNode* root);
    
private:
    Type* inferType(ExprNode* expr);
    bool isAssignableFrom(Type* target, Type* source);
    void checkFunctionCall(CallExpr* call);
    void checkBinaryOp(BinaryExpr* binary);
    void checkPointerOp(PointerExpr* ptr);
};
```

**Symbol Table:**
```cpp
class SymbolTable {
    struct Symbol {
        std::string name;
        Type* type;
        bool isMutable;
        int scope;
    };
    
    std::unordered_map<std::string, Symbol> symbols;
    SymbolTable* parent;
    
public:
    void define(const std::string& name, Type* type);
    Symbol* resolve(const std::string& name);
    void enterScope();
    void exitScope();
};
```

---

#### Phase 4: IR Generation (LLVM)

**LLVM IR Generation:**
```cpp
class IRGenerator : public ASTVisitor {
    llvm::LLVMContext& context;
    llvm::Module* module;
    llvm::IRBuilder<>* builder;
    std::unordered_map<std::string, llvm::Value*> namedValues;
    
public:
    void visit(LiteralExpr* expr) override {
        if (expr->value.isNumber()) {
            return llvm::ConstantFP::get(context, 
                llvm::APFloat(expr->value.asNumber()));
        }
        // ... handle other types
    }
    
    void visit(BinaryExpr* expr) override {
        llvm::Value* L = generate(expr->left);
        llvm::Value* R = generate(expr->right);
        
        switch (expr->op) {
            case TokenType::OP_PLUS:
                return builder->CreateFAdd(L, R, "addtmp");
            case TokenType::OP_MULTIPLY:
                return builder->CreateFMul(L, R, "multmp");
            // ... handle other ops
        }
    }
    
    void visit(CallExpr* expr) override {
        llvm::Function* calleeF = module->getFunction(expr->callee);
        
        std::vector<llvm::Value*> argsV;
        for (auto& arg : expr->args) {
            argsV.push_back(generate(arg.get()));
        }
        
        return builder->CreateCall(calleeF, argsV, "calltmp");
    }
    
    void visit(FunctionDeclStmt* stmt) override {
        // Create function type
        std::vector<llvm::Type*> argTypes;
        for (auto& param : stmt->params) {
            argTypes.push_back(getLLVMType(param.type));
        }
        
        llvm::FunctionType* FT = llvm::FunctionType::get(
            getLLVMType(stmt->returnType),
            argTypes,
            false
        );
        
        // Create function
        llvm::Function* F = llvm::Function::Create(
            FT,
            llvm::Function::ExternalLinkage,
            stmt->name,
            module
        );
        
        // Create entry block
        llvm::BasicBlock* BB = llvm::BasicBlock::Create(
            context, "entry", F
        );
        builder->SetInsertPoint(BB);
        
        // Generate body
        generate(stmt->body.get());
    }
    
    void visit(PointerExpr* expr) override {
        llvm::Value* operand = generate(expr->operand);
        
        if (expr->type == PointerExpr::DEREFERENCE) {
            return builder->CreateLoad(
                operand->getType()->getPointerElementType(),
                operand,
                "deref"
            );
        } else {  // ADDRESS_OF
            return operand;  // Already an address
        }
    }
    
    void visit(InlineAsmStmt* stmt) override {
        // Create inline assembly
        llvm::FunctionType* asmFuncType = /* ... */;
        llvm::InlineAsm* inlineAsm = llvm::InlineAsm::get(
            asmFuncType,
            stmt->asmCode,
            /* constraints */ "",
            /* has side effects */ true
        );
        
        builder->CreateCall(inlineAsm, /* args */);
    }
};
```

---

#### Phase 5: Optimization

**Optimization Levels:**

```cpp
enum OptLevel {
    O0,  // No optimization
    O1,  // Basic optimization
    O2,  // Moderate optimization
    O3,  // Aggressive optimization
    Os,  // Optimize for size
};

void optimize(llvm::Module* M, OptLevel level) {
    llvm::PassManagerBuilder PMB;
    
    switch (level) {
        case O0:
            // No optimization
            break;
        case O1:
            PMB.OptLevel = 1;
            PMB.Inliner = llvm::createFunctionInliningPass(225);
            break;
        case O2:
            PMB.OptLevel = 2;
            PMB.Inliner = llvm::createFunctionInliningPass(275);
            break;
        case O3:
            PMB.OptLevel = 3;
            PMB.Inliner = llvm::createFunctionInliningPass();
            break;
        case Os:
            PMB.OptLevel = 2;
            PMB.SizeLevel = 1;
            break;
    }
    
    llvm::legacy::PassManager PM;
    PMB.populateModulePassManager(PM);
    PM.run(*M);
}
```

**Optimization Passes:**
- Dead Code Elimination (DCE)
- Constant Folding
- Inline Expansion
- Loop Unrolling
- Common Subexpression Elimination (CSE)
- Register Allocation
- Instruction Scheduling

---

#### Phase 6: Code Generation

**Target Architectures:**

```cpp
enum class Architecture {
    X86_64,
    ARM64,
    RISCV64
};

class CodeGenerator {
public:
    void generateMachineCode(
        llvm::Module* M,
        Architecture arch,
        const std::string& outputFile
    ) {
        // Initialize target
        llvm::InitializeAllTargetInfos();
        llvm::InitializeAllTargets();
        llvm::InitializeAllTargetMCs();
        llvm::InitializeAllAsmParsers();
        llvm::InitializeAllAsmPrinters();
        
        // Get target triple
        std::string targetTriple = getTargetTriple(arch);
        M->setTargetTriple(targetTriple);
        
        // Get target machine
        std::string error;
        auto target = llvm::TargetRegistry::lookupTarget(
            targetTriple, error
        );
        
        llvm::TargetOptions opt;
        auto targetMachine = target->createTargetMachine(
            targetTriple,
            "generic",
            "",
            opt,
            llvm::Optional<llvm::Reloc::Model>()
        );
        
        // Set data layout
        M->setDataLayout(targetMachine->createDataLayout());
        
        // Emit object file
        std::error_code EC;
        llvm::raw_fd_ostream dest(outputFile, EC);
        
        llvm::legacy::PassManager pass;
        auto fileType = llvm::CGFT_ObjectFile;
        
        targetMachine->addPassesToEmitFile(
            pass, dest, nullptr, fileType
        );
        
        pass.run(*M);
        dest.flush();
    }
    
private:
    std::string getTargetTriple(Architecture arch) {
        switch (arch) {
            case Architecture::X86_64:
                return "x86_64-unknown-none-elf";
            case Architecture::ARM64:
                return "aarch64-unknown-none-elf";
            case Architecture::RISCV64:
                return "riscv64-unknown-none-elf";
        }
    }
};
```

---

<a name="memory-specs"></a>
## 🧠 مواصفات الذاكرة / Memory Specifications

### Virtual Memory Layout (x86-64)

```
0xFFFFFFFF_FFFFFFFF  ┌─────────────────────┐
                     │   Kernel Space      │
                     │   (Higher Half)     │
0xFFFF8000_00000000  ├─────────────────────┤
                     │                     │
                     │   (Unmapped)        │
                     │                     │
0x00007FFF_FFFFFFFF  ├─────────────────────┤
                     │   Stack             │
                     │   (grows down)      │
                     │        ↓            │
                     ├─────────────────────┤
                     │                     │
                     │   (Free)            │
                     │                     │
                     ├─────────────────────┤
                     │        ↑            │
                     │   Heap              │
                     │   (grows up)        │
                     ├─────────────────────┤
                     │   .bss              │
                     ├─────────────────────┤
                     │   .data             │
                     ├─────────────────────┤
                     │   .rodata           │
                     ├─────────────────────┤
                     │   .text             │
0x00000000_00400000  ├─────────────────────┤
                     │   (Reserved)        │
0x00000000_00000000  └─────────────────────┘
```

### Memory Management APIs

```sad
# memory.s - Memory Management API

# تخصيص ذاكرة / Allocate memory
دالة خصص<T>(رقم count) -> مؤشر<T> خارجي ;

# تحرير ذاكرة / Free memory
دالة حرر<T>(مؤشر<T> ptr) -> فارغ خارجي ;

# نسخ ذاكرة / Copy memory
دالة انسخ_ذاكرة(مؤشر<فارغ> dest, مؤشر<فارغ> src, رقم size) -> فارغ خارجي ;

# ملء ذاكرة / Fill memory
دالة املأ_ذاكرة(مؤشر<فارغ> ptr, بايت value, رقم size) -> فارغ خارجي ;

# تخصيص صفحات / Allocate pages
دالة خصص_صفحات(رقم count) -> مؤشر<فارغ> خارجي ;

# تحرير صفحات / Free pages
دالة حرر_صفحات(مؤشر<فارغ> ptr, رقم count) -> فارغ خارجي ;

# خريطة ذاكرة / Memory mapping
دالة خريطة_ذاكرة(
    مؤشر<فارغ> addr,
    رقم length,
    رقم prot,
    رقم flags,
    رقم fd,
    رقم offset
) -> مؤشر<فارغ> خارجي ;
```

### Page Table Structure

```cpp
// Page Directory Entry (4KB pages)
struct PDEntry {
    uint64_t present     : 1;   // Page present
    uint64_t writable    : 1;   // Read/Write
    uint64_t user        : 1;   // User/Supervisor
    uint64_t writeThrough: 1;   // Write-through caching
    uint64_t cacheDisable: 1;   // Cache disabled
    uint64_t accessed    : 1;   // Accessed
    uint64_t ignored1    : 1;
    uint64_t pageSize    : 1;   // 0=4KB, 1=4MB
    uint64_t ignored2    : 4;
    uint64_t address     : 40;  // Physical address >> 12
    uint64_t reserved    : 11;
    uint64_t noExecute   : 1;   // No execute
};

// Page Table Entry
struct PTEntry {
    uint64_t present     : 1;
    uint64_t writable    : 1;
    uint64_t user        : 1;
    uint64_t writeThrough: 1;
    uint64_t cacheDisable: 1;
    uint64_t accessed    : 1;
    uint64_t dirty       : 1;   // Page has been written to
    uint64_t pat         : 1;   // Page attribute table
    uint64_t global      : 1;   // Global page
    uint64_t ignored     : 3;
    uint64_t address     : 40;
    uint64_t reserved    : 11;
    uint64_t noExecute   : 1;
};
```

---

<a name="syscall-interface"></a>
## 🔌 System Call Interface

### Syscall Numbers

```cpp
// syscall_numbers.h
#define SYS_read             0
#define SYS_write            1
#define SYS_open             2
#define SYS_close            3
#define SYS_stat             4
#define SYS_fstat            5
#define SYS_lstat            6
#define SYS_poll             7
#define SYS_lseek            8
#define SYS_mmap             9
#define SYS_mprotect        10
#define SYS_munmap          11
#define SYS_brk             12
#define SYS_rt_sigaction    13
#define SYS_rt_sigprocmask  14
#define SYS_rt_sigreturn    15
#define SYS_ioctl           16
#define SYS_pread64         17
#define SYS_pwrite64        18
#define SYS_readv           19
#define SYS_writev          20
#define SYS_access          21
#define SYS_pipe            22
#define SYS_select          23
#define SYS_sched_yield     24
#define SYS_mremap          25
#define SYS_msync           26
#define SYS_mincore         27
#define SYS_madvise         28
#define SYS_shmget          29
#define SYS_shmat           30
#define SYS_shmctl          31
#define SYS_dup             32
#define SYS_dup2            33
#define SYS_pause           34
#define SYS_nanosleep       35
#define SYS_getitimer       36
#define SYS_alarm           37
#define SYS_setitimer       38
#define SYS_getpid          39
#define SYS_sendfile        40
#define SYS_socket          41
#define SYS_connect         42
#define SYS_accept          43
#define SYS_sendto          44
#define SYS_recvfrom        45
#define SYS_sendmsg         46
#define SYS_recvmsg         47
#define SYS_shutdown        48
#define SYS_bind            49
#define SYS_listen          50
#define SYS_fork            57
#define SYS_vfork           58
#define SYS_execve          59
#define SYS_exit            60
#define SYS_wait4           61
#define SYS_kill            62
```

### Syscall Calling Convention (x86-64)

**Registers:**
- **rax**: Syscall number
- **rdi**: Argument 1
- **rsi**: Argument 2
- **rdx**: Argument 3
- **r10**: Argument 4
- **r8**: Argument 5
- **r9**: Argument 6
- **Return**: rax (return value or -errno)

**Example:**
```sad
# syscall_example.s

دالة sys_write(رقم fd, مؤشر<نص> buf, رقم count) -> رقم {
    رقم result ;
    
    تجميع {
        mov rax, 1          # SYS_write
        mov rdi, fd         # fd
        mov rsi, buf        # buffer
        mov rdx, count      # count
        syscall
        mov result, rax
    }
    
    أرجع result ;
}
```

---

<a name="abi-conventions"></a>
## 📐 ABI Conventions

### x86-64 System V ABI

**Function Calling Convention:**

**Integer/Pointer Arguments (in order):**
1. rdi
2. rsi
3. rdx
4. rcx
5. r8
6. r9
7. Stack (right-to-left)

**Floating-Point Arguments:**
1-8: xmm0-xmm7

**Return Values:**
- Integer/Pointer: rax (rdx for 128-bit)
- Floating-Point: xmm0

**Callee-Saved Registers:**
- rbx, rbp, r12-r15

**Caller-Saved Registers:**
- rax, rcx, rdx, rsi, rdi, r8-r11

**Stack Alignment:**
- 16-byte aligned before call

**Example:**
```sad
# abi_example.s

# دالة بـ 8 parameters
دالة test_abi(
    رقم a,   # rdi
    رقم b,   # rsi
    رقم c,   # rdx
    رقم d,   # rcx
    رقم e,   # r8
    رقم f,   # r9
    رقم g,   # [rsp+8]
    رقم h    # [rsp+16]
) -> رقم {
    أرجع a + b + c + d + e + f + g + h ;
}
```

---

<a name="binary-format"></a>
## 📦 Binary Format Specification

### ELF Header (64-bit)

```cpp
struct Elf64_Ehdr {
    unsigned char e_ident[16];  // Magic number and other info
    uint16_t      e_type;        // Object file type
    uint16_t      e_machine;     // Architecture (x86-64 = 62)
    uint32_t      e_version;     // Object file version
    uint64_t      e_entry;       // Entry point virtual address
    uint64_t      e_phoff;       // Program header table file offset
    uint64_t      e_shoff;       // Section header table file offset
    uint32_t      e_flags;       // Processor-specific flags
    uint16_t      e_ehsize;      // ELF header size in bytes
    uint16_t      e_phentsize;   // Program header table entry size
    uint16_t      e_phnum;       // Program header table entry count
    uint16_t      e_shentsize;   // Section header table entry size
    uint16_t      e_shnum;       // Section header table entry count
    uint16_t      e_shstrndx;    // Section header string table index
};
```

### Program Header

```cpp
struct Elf64_Phdr {
    uint32_t   p_type;    // Segment type
    uint32_t   p_flags;   // Segment flags
    uint64_t   p_offset;  // Segment file offset
    uint64_t   p_vaddr;   // Segment virtual address
    uint64_t   p_paddr;   // Segment physical address
    uint64_t   p_filesz;  // Segment size in file
    uint64_t   p_memsz;   // Segment size in memory
    uint64_t   p_align;   // Segment alignment
};
```

### Section Types

- **.text**: Executable code
- **.rodata**: Read-only data
- **.data**: Initialized data
- **.bss**: Uninitialized data
- **.symtab**: Symbol table
- **.strtab**: String table
- **.debug**: Debug information

---

<a name="kernel-specs"></a>
## 🔨 Kernel Specifications

### Kernel Entry Point

```sad
# kernel/boot.s - Kernel entry point

استيراد "multiboot.s" ;

ثابت STACK_SIZE = 16384 ;  # 16KB

قسم .bss
    مصفوفة<بايت> boot_stack[STACK_SIZE] محاذاة(16) ;
نهاية

قسم .text
دالة _start() خارجي محاذاة(4) {
    # Setup stack
    تجميع {
        mov esp, boot_stack + STACK_SIZE
        mov ebp, esp
        
        # Save multiboot info
        push ebx  # multiboot info pointer
        push eax  # multiboot magic
    }
    
    # Call kernel main
    kernel_main() ;
    
    # Halt
    بينما (صحيح) {
        تجميع { 
            cli
            hlt 
        }
    }
}
```

### Interrupt Descriptor Table

```cpp
struct IDTEntry {
    uint16_t offset_low;    // Offset bits 0-15
    uint16_t selector;      // Code segment selector
    uint8_t  ist;           // Interrupt Stack Table offset
    uint8_t  type_attr;     // Type and attributes
    uint16_t offset_mid;    // Offset bits 16-31
    uint32_t offset_high;   // Offset bits 32-63
    uint32_t zero;          // Reserved
} __attribute__((packed));

struct IDTR {
    uint16_t limit;         // Size of IDT - 1
    uint64_t base;          // Address of IDT
} __attribute__((packed));
```

### Process Control Block

```cpp
struct PCB {
    uint64_t pid;                 // Process ID
    uint64_t parent_pid;          // Parent process ID
    ProcessState state;           // RUNNING, READY, BLOCKED, etc.
    
    // CPU context
    struct Registers {
        uint64_t rax, rbx, rcx, rdx;
        uint64_t rsi, rdi, rbp, rsp;
        uint64_t r8, r9, r10, r11;
        uint64_t r12, r13, r14, r15;
        uint64_t rip, rflags;
        uint64_t cr3;  // Page directory base
    } registers;
    
    // Memory info
    uint64_t page_directory;
    uint64_t heap_start;
    uint64_t heap_end;
    uint64_t stack_start;
    
    // File descriptors
    FileDescriptor* fds[MAX_FDS];
    
    // Priority and scheduling
    int priority;
    uint64_t time_slice;
    uint64_t cpu_time;
    
    // List pointers
    PCB* next;
    PCB* prev;
};
```

---

<a name="driver-interface"></a>
## 🔌 Driver Interface

### Driver Structure

```sad
# driver_interface.s

صنف Driver {
    نص name ;
    رقم major_number ;
    رقم minor_number ;
    
    # Required methods
    دالة init() -> منطقي افتراضي ;
    دالة probe() -> منطقي افتراضي ;
    دالة remove() -> فارغ افتراضي ;
    
    # File operations
    دالة open(رقم flags) -> رقم افتراضي ;
    دالة close() -> رقم افتراضي ;
    دالة read(مؤشر<بايت> buf, رقم count) -> رقم افتراضي ;
    دالة write(مؤشر<بايت> buf, رقم count) -> رقم افتراضي ;
    دالة ioctl(رقم cmd, مؤشر<فارغ> arg) -> رقم افتراضي ;
    
    # Interrupt handler
    دالة irq_handler() -> فارغ افتراضي ;
}

# تسجيل driver
دالة register_driver(مؤشر<Driver> drv) -> منطقي خارجي ;
دالة unregister_driver(مؤشر<Driver> drv) -> فارغ خارجي ;
```

### Example: Serial Driver

```sad
# drivers/serial.s

صنف SerialDriver : Driver {
    ثابت COM1 = 0x3F8 ;
    
    دالة init() -> منطقي {
        name = "serial" ;
        major_number = 4 ;
        
        # Initialize COM1
        io_write(COM1 + 1, 0x00) ;    # Disable interrupts
        io_write(COM1 + 3, 0x80) ;    # Enable DLAB
        io_write(COM1 + 0, 0x03) ;    # Divisor low byte (38400 baud)
        io_write(COM1 + 1, 0x00) ;    # Divisor high byte
        io_write(COM1 + 3, 0x03) ;    # 8 bits, no parity, 1 stop bit
        io_write(COM1 + 2, 0xC7) ;    # Enable FIFO, clear, 14-byte threshold
        io_write(COM1 + 4, 0x0B) ;    # IRQs enabled, RTS/DSR set
        
        أرجع صحيح ;
    }
    
    دالة write(مؤشر<بايت> buf, رقم count) -> رقم {
        لكل (رقم i = 0 ; i < count ; i++) {
            # Wait for transmit buffer empty
            بينما ((io_read(COM1 + 5) & 0x20) == 0) { }
            
            io_write(COM1, buf[i]) ;
        }
        أرجع count ;
    }
    
    دالة read(مؤشر<بايت> buf, رقم count) -> رقم {
        رقم received = 0 ;
        
        بينما (received < count) {
            # Check if data available
            إذا ((io_read(COM1 + 5) & 0x01) != 0) {
                buf[received] = io_read(COM1) ;
                received = received + 1 ;
            }
        }
        
        أرجع received ;
    }
}
```

---

<a name="network-stack"></a>
## 🌐 Network Protocol Stack

### Network Architecture

```
┌──────────────────────────────┐
│     Application Layer        │
│  (HTTP, FTP, DNS, etc.)      │
└──────────────┬───────────────┘
               │
┌──────────────┴───────────────┐
│     Transport Layer          │
│      (TCP, UDP)              │
└──────────────┬───────────────┘
               │
┌──────────────┴───────────────┐
│     Network Layer            │
│      (IP, ICMP)              │
└──────────────┬───────────────┘
               │
┌──────────────┴───────────────┐
│     Data Link Layer          │
│    (Ethernet, ARP)           │
└──────────────┬───────────────┘
               │
┌──────────────┴───────────────┐
│     Physical Layer           │
│    (Network Driver)          │
└──────────────────────────────┘
```

### Socket API

```sad
# network/socket.s

# Socket types
ثابت SOCK_STREAM = 1 ;   # TCP
ثابت SOCK_DGRAM = 2 ;    # UDP

# Address families
ثابت AF_INET = 2 ;       # IPv4

# Socket address structure
صنف sockaddr_in {
    غير_صحيح16 sin_family ;
    غير_صحيح16 sin_port ;
    غير_صحيح32 sin_addr ;
    مصفوفة<بايت> sin_zero[8] ;
}

# Create socket
دالة socket(رقم domain, رقم type, رقم protocol) -> رقم خارجي ;

# Bind socket
دالة bind(رقم sockfd, مؤشر<sockaddr_in> addr, رقم addrlen) -> رقم خارجي ;

# Listen
دالة listen(رقم sockfd, رقم backlog) -> رقم خارجي ;

# Accept connection
دالة accept(رقم sockfd, مؤشر<sockaddr_in> addr, مؤشر<رقم> addrlen) -> رقم خارجي ;

# Connect
دالة connect(رقم sockfd, مؤشر<sockaddr_in> addr, رقم addrlen) -> رقم خارجي ;

# Send data
دالة send(رقم sockfd, مؤشر<فارغ> buf, رقم len, رقم flags) -> رقم خارجي ;

# Receive data
دالة recv(رقم sockfd, مؤشر<فارغ> buf, رقم len, رقم flags) -> رقم خارجي ;

# Close socket
دالة close_socket(رقم sockfd) -> رقم خارجي ;
```

### Example: TCP Server

```sad
# tcp_server_example.s

استيراد "network/socket" ;

دالة tcp_server() {
    # Create socket
    رقم sockfd = socket(AF_INET, SOCK_STREAM, 0) ;
    إذا (sockfd < 0) {
        اطبع("خطأ في إنشاء socket") ;
        أرجع ;
    }
    
    # Setup address
    sockaddr_in addr ;
    addr.sin_family = AF_INET ;
    addr.sin_port = htons(8080) ;
    addr.sin_addr = INADDR_ANY ;
    
    # Bind
    إذا (bind(sockfd, عنوان(addr), sizeof(addr)) < 0) {
        اطبع("خطأ في bind") ;
        أرجع ;
    }
    
    # Listen
    إذا (listen(sockfd, 10) < 0) {
        اطبع("خطأ في listen") ;
        أرجع ;
    }
    
    اطبع("Server listening on port 8080...") ;
    
    # Accept loop
    بينما (صحيح) {
        sockaddr_in client_addr ;
        رقم addr_len = sizeof(client_addr) ;
        
        رقم client_sock = accept(sockfd, عنوان(client_addr), عنوان(addr_len)) ;
        
        إذا (client_sock >= 0) {
            # Handle client
            handle_client(client_sock) ;
            close_socket(client_sock) ;
        }
    }
}

دالة handle_client(رقم sock) {
    مصفوفة<بايت> buffer[1024] ;
    
    # Receive data
    رقم n = recv(sock, عنوان(buffer), 1024, 0) ;
    
    إذا (n > 0) {
        اطبع("Received: ", buffer) ;
        
        # Send response
        نص response = "HTTP/1.1 200 OK\r\n\r\nHello from SadOS!" ;
        send(sock, عنوان(response), طول(response), 0) ;
    }
}
```

---

## 📚 المراجع / References

### العربية

1. **Intel 64 and IA-32 Architectures Software Developer's Manual**
2. **System V Application Binary Interface**
3. **LLVM Language Reference Manual**
4. **ELF-64 Object File Format**
5. **Multiboot Specification**
6. **UEFI Specification**
7. **TCP/IP Illustrated** by W. Richard Stevens

### English

1. Intel Architecture Manuals
2. System V ABI Documentation
3. LLVM Documentation
4. ELF Format Specification
5. OS Development Resources (OSDev Wiki)
6. Linux Kernel Source Code (reference)
7. Network Protocol RFCs

---

**السابق / Previous:** [الجدول الزمني](08_timeline.md)  
**التالي / Next:** [README](README.md)
