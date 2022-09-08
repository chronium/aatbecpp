#include <iostream>

#include <argparse/argparse.hpp>

#include <codegen.hpp>
#include <jit.hpp>

#include <lexer/lexer.hpp>

#include <llvm/IR/Verifier.h>

using namespace aatbe::lexer;
using namespace aatbe::source;
using namespace aatbe::jit;

#define PROJECT_NAME "lang"

int main(int argc, char **argv) {
  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmParser();
  LLVMInitializeNativeAsmPrinter();
  argparse::ArgumentParser args(PROJECT_NAME, "A simple language interpreter");

  args.add_argument("INPUT").help("Input file to be compiled").required();

  try {
    args.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << args;
    std::exit(1);
  }

  auto file = args.get("INPUT");

  printf("=================Start=================\n");
  aatbe::codegen::compile_file(file);

  printf("================Codegen================\n");
  aatbe::codegen::Module->print(llvm::outs(), nullptr);

  if (!llvm::verifyModule(*aatbe::codegen::Module, &llvm::errs())) {
    auto jit = AatbeJit::Create();
    if (!jit)
      std::cout << toString(jit.takeError()) << std::endl;

    auto _ = jit->get()->addModule(ThreadSafeModule(std::move(aatbe::codegen::Module),
                                    std::move(aatbe::codegen::LLVMContext)));

    printf("==================RUN==================\n");
    auto *main = (int (*)(int argc, char **argv))jit->get()->lookup("main")->getAddress();

    auto jitargv = new char*[1];
    jitargv[0] = strdup("<main>");
    main(1, jitargv);
  }

  return 0;
}
