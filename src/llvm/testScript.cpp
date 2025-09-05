#include "llvm/Includes.h"
#include "llvm/testScript.h"

#ifdef USE_LLVM

// Adapter raw_ostream -> std::ostream
struct StdOutAdapter : public llvm::raw_ostream {
    std::ostream &os;

    StdOutAdapter(std::ostream &os_) : os(os_) {
        SetUnbuffered();
    }

    void write_impl(const char *ptr, size_t size) override {
        os.write(ptr, size);
    }

    uint64_t current_pos() const override {
        return os.tellp();
    }
};

#endif

void llvmHelloWorld(){

    #ifdef USE_LLVM

    // Initialisierung
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    // Minimaler Context und Module
    llvm::LLVMContext context;
    auto module = std::make_unique<llvm::Module>("HelloLLVMModule", context);
    llvm::IRBuilder<> builder(context);

    // Leere Funktion erstellen
    auto* funcType = llvm::FunctionType::get(builder.getVoidTy(), false);
    auto* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "testFunc", module.get());

    StdOutAdapter coutAdapter(std::cout);
    module->print(coutAdapter, nullptr);

    #else

    LOG << "LLVM wird nicht mitkompiliert" << ENDL;

    #endif
}

