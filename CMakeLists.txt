project(nnc)
cmake_minimum_required(VERSION 3.16)

find_package(Catch2 REQUIRED)
find_package(FLEX)
find_package(BISON)

FLEX_TARGET(ArchDescLexer ./insnsel/lexer.l ./insnsel/lexer.cpp DEFINES_FILE ./insnsel/FlexLexer.h)
BISON_TARGET(ArchDescParser ./insnsel/parser.y ./insnsel/parser.cpp)
ADD_FLEX_BISON_DEPENDENCY(ArchDescLexer ArchDescParser)

include_directories(PRIVATE src)

add_executable(archdescc insnsel/main.cpp ${FLEX_ArchDescLexer_OUTPUTS} ${BISON_ArchDescParser_OUTPUTS}
  insnsel/ast.cpp insnsel/archdesc.cpp insnsel/regclass.cpp insnsel/error.cpp
  insnsel/register.cpp insnsel/insnrule.cpp insnsel/templatestring.cpp insnsel/insn.cpp)
target_include_directories(archdescc PUBLIC ${FLEX_INCLUDE_DIRS})
target_link_libraries(archdescc ${BISON_LIBRARIES})
set_property(TARGET archdescc PROPERTY CXX_STANDARD 20)

macro(nnc_arch)
  set(options)
  set(oneValueArgs NAME ARCHDESC OUTPUTDIR)
  set(multiValueArgs)
  cmake_parse_arguments(NNC_ARCH "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  if(NOT DEFINED NNC_ARCH_NAME)
    message(FATAL_ERROR "nnc_arch requires NAME parameter")
  endif()

  if(NOT DEFINED NNC_ARCH_ARCHDESC)
    message(FATAL_ERROR "nnc_arch requires ARCHDESC parameter")
  endif()

  if(NOT DEFINED NNC_ARCH_OUTPUTDIR)
    set(NNC_ARCH_OUTPUTDIR "gen")
  endif()

  set(NNC_ARCH_DIR "${NNC_ARCH_OUTPUTDIR}/arch/${NNC_ARCH_NAME}")
  set(NNC_ARCH_SRCS "${NNC_ARCH_DIR}/rules.cpp" "${NNC_ARCH_DIR}/insns.cpp")
  set(NNC_ARCH_OUTPUTS
    "${NNC_ARCH_DIR}/schedule.hpp"
    "${NNC_ARCH_DIR}/insns.hpp"
    ${NNC_ARCH_SRCS})
  add_custom_command(OUTPUT "${NNC_ARCH_OUTPUTDIR}/arch.dep/${NNC_ARCH_NAME}.d"
    COMMAND ${PROJECT_BINARY_DIR}/archdescc --deps-prefix ${NNC_ARCH_DIR} -M ${NNC_ARCH_ARCHDESC} '${NNC_ARCH_OUTPUTDIR}/arch.dep/${NNC_ARCH_NAME}.d'
    DEPENDS archdescc)

  add_custom_command(OUTPUT ${NNC_ARCH_OUTPUTS}
    COMMAND archdescc ${NNC_ARCH_ARCHDESC} ${NNC_ARCH_OUTPUTDIR}
    MAIN_DEPENDENCY "${NNC_ARCH_ARCHDESC}"
    DEPENDS archdescc "${NNC_ARCH_OUTPUTDIR}/arch.dep/${NNC_ARCH_NAME}.d"
    DEPFILE "${NNC_ARCH_OUTPUTDIR}/arch.dep/${NNC_ARCH_NAME}.d")

  set(NNC_ARCH_SOURCE ${NNC_ARCH_SOURCE} ${NNC_ARCH_SRCS})
  set(NNC_ARCH_INCLUDE "${NNC_ARCH_OUTPUTDIR}")
  set_source_files_properties(${NNC_ARCH_SRCS} PROPERTIES GENERATED TRUE)
endmacro(nnc_arch)

NNC_ARCH(NAME "x86_64"
  ARCHDESC "src/compile/x86_64/arch.archdesc")

add_library(nnc SHARED src/tensor.cpp src/graph.cpp src/exception.cpp
  src/primop.cpp src/error_reporter.cpp src/categorical.cpp src/errors.cpp
  src/castop.cpp src/graphviz.cpp src/executor.cpp src/executor/basic.cpp

  src/cpu/cpu.cpp src/cpu/cpu_op.cpp src/cpu/rtl.cpp

  src/op/reshape.cpp src/op/matmul.cpp

  src/compile/rtl_types.cpp src/compile/rtl.cpp src/compile/rtl_ops.cpp
  src/compile/constant_propagation.cpp src/compile/unused.cpp

  src/compile/x86_64.cpp src/compile/x86_64/types.cpp

  src/compile/controlflow.cpp src/compile/dataflow.cpp
  src/compile/ssa.cpp src/compile/genericschedule.cpp src/compile/insnsel.cpp ${NNC_ARCH_SOURCE})
set_property(TARGET nnc PROPERTY CXX_STANDARD 20)
target_include_directories(nnc PRIVATE ${NNC_ARCH_INCLUDE})

#add_executable(nnc-test test/test.cpp test/tensor.cpp)
## target_include_directories(nnc-test PUBLIC ${CATCH2_INCLUDE_DIRS})
#target_link_libraries(nnc-test nnc Catch2::Catch2)

add_executable(main main.cpp)
target_link_libraries(main nnc)
set_property(TARGET main PROPERTY CXX_STANDARD 20)
target_include_directories(main PRIVATE ${NNC_ARCH_INCLUDE})

include(CTest)
#include(Catch)
#catch_discover_tests(nnc-test)
