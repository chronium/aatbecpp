project('lang', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++20'])

llvm_dep = dependency('llvm', version : '>= 13.0')

argparse = include_directories('extern/argparse/include')

includes = include_directories('include', 'include/lexer')

sources = [
  'src/codegen.cpp',
  'src/lexer/lexer.cpp',
  'src/lexer/token.cpp',
  'src/source/source_file.cpp',
  'src/parser/parser.cpp',
  'src/parser/type.cpp',
  'src/parser/expression.cpp',
  'src/codegen/expression.cpp',
  'src/jit.cpp',
]

tests = [
  'tests/src/lexer.cpp',
  'tests/src/parser/terminals.cpp',
  'tests/src/parser/types.cpp',
  'tests/src/parser/expressions.cpp',
  'tests/src/parser/functions.cpp'
]

libcomp = shared_library(
  'libcomp',
  sources,
  install: true,
  include_directories: [includes],
  dependencies: [llvm_dep],
  override_options : ['cpp_std=c++20'],
  cpp_args: ['-Wall', '-Wextra', '-fsanitize=address', '-Wno-unused-parameter'],
  link_args: ['-fsanitize=address']
)

comp_dep = declare_dependency(
  include_directories: [includes],
  link_with: libcomp
)

exe = executable('lang',
  'lang.cpp',
  install : true,
  dependencies: [llvm_dep, comp_dep],
  include_directories: [argparse, includes],
  override_options : ['cpp_std=c++20'],
  cpp_args: ['-Wall', '-Wextra', '-Wno-unused-parameter'])

project_dep = declare_dependency(
  include_directories: [includes],
  link_with : libcomp
)

test('execute', exe, args: ['../hello.aat'])

subdir('tests')

test('all_tests',
  executable(
    'run_tests',
    files(tests),
    dependencies: [project_dep, test_dep],
    install: false,
    override_options : ['cpp_std=c++20'],
    cpp_args: ['-Wno-unused-parameter']
  )
)
