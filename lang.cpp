#include <iostream>

#include <argparse/argparse.hpp>

#include <codegen.hpp>

#include <lexer/lexer.hpp>

using namespace aatbe::lexer;
using namespace aatbe::source;

#define PROJECT_NAME "lang"

int main(int argc, char **argv) {
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

  auto module = aatbe::codegen::compile_file(file);

  module->print(llvm::outs(), nullptr);

  return 0;
}
