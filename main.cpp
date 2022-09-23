#include "tensor.hpp"
#include "memory_tensor.hpp"
#include "mnist.hpp"
#include "pretty.hpp"
#include "graph.hpp"
#include "primop.hpp"
#include "error_reporter.hpp"
#include "categorical.hpp"
#include "castop.hpp"
#include "graphviz.hpp"
#include "cpu/cpu_op.hpp"
#include "cpu/rtl.hpp"
#include "cpu.hpp"
#include "op/reshape.hpp"
#include "op/matmul.hpp"
#include "compile/constant_propagation.hpp"
#include "compile/regalloc.hpp"
#include "compile/linear_alloc.hpp"
#include "compile/x86_64/manager.hpp"
#include "invoke.hpp"

#include <iostream>
#include <sstream>
#include <cstring>
#include <cerrno>
extern "C" {
  #include <sys/mman.h>
  #include <unistd.h>
}

int test1() {
  nnc::graph::Graph graph("Basic matmul");

  auto &weights(graph.addTensor<nnc::ConstTensor<float, std::allocator<float>,
                10, 784> >());
  auto &input(graph.addInputTensor<float, 784, 1>("input"));
  auto &output(graph.addTensor<nnc::op::MatMul> (weights, input));

  std::fstream o("test1.dot", std::fstream::out);
  nnc::graphviz::DotOutput dotter(graph, o);
  graph.visit(dotter);

  nnc::error::OstreamErrorReporter reporter(std::cerr);

  if ( !graph.check(reporter) ) {
    std::cerr << "There were errors creating the graph" << std::endl;
    return 1;
  }

  nnc::executor::CpuExecutor exec(graph, 10);
  exec.build();

  std::cerr << "Loop structure" << std::endl;
  nnc::executor::CpuOpPrinter print(std::cerr);
  exec.visitAll(print);

  return 1;
}

int main(int argc, const char **argv) {
  //  return test1();

  nnc::ConstantLayout<1, 2, 3> layout;

  std::cout << layout.dims() << std::endl;
  std::cout << layout.sizeInDimension(0) << std::endl;

  nnc::ConstTensor<double, std::allocator<double>, 1, 2> test{1.0, 3.0};
  std::cout << "Tensor type: " << test.dataType().name() << std::endl;

  //  nnc::io::MnistReader<std::uint8_t, 3> reader("./datasets/mnist/train-images-idx3-ubyte");

  nnc::graph::Graph graph("MNIST CNN");
//
//  auto &imageInput(graph.addTensor< nnc::op::Cast<std::uint8_t, float> >
//                   (graph.addInputTensor<std::uint8_t, 28, 28>("image")));
//  auto &labelInput(graph.addTensor< nnc::op::Cast<std::uint8_t, std::uint32_t> >
//                   (graph.addInputTensor<std::uint8_t>("label")));
//
//  std::vector<int> newShape{28 * 28, 1};
//  auto &flattened(graph.addTensor< nnc::op::Reshape >(newShape, imageInput));
//
//  auto &weights(graph.addTensor< nnc::ConstTensor<float, std::allocator<float>, 10, 784> >());
//  auto &bias(graph.addTensor< nnc::ConstTensor<float, std::allocator<float>, 10, 1> >());
//  //  nnc::utils::random(weights);
//
//  auto &output(graph.addTensor<nnc::op::AddOp>( graph.addTensor<nnc::op::MatMul> (weights, flattened),
//                                                bias ));
//
//  auto &expected(graph.addTensor< nnc::op::ToCategorical<float> >(labelInput, 10));
//  graph.addOutput(expected);
//
//  graph.addOutput(labelInput);

//  auto &weights(graph.addTensor< nnc::ConstTensor<float, std::allocator<float>, 10, 784> >());
//  auto &flattened(graph.addTensor< nnc::ConstTensor<float, std::allocator<float>, 784, 1> >());
//  auto &output(graph.addTensor<nnc::op::MatMul> (weights, flattened));
//  graph.addOutput(output);

//  auto &a(graph.addInputTensor<float, 8>("a"));
//  auto &b(graph.addInputTensor<float, 8>("b"));
//  auto &output(graph.addTensor<nnc::op::AddOp>(a, b));
//  graph.addOutput(output);

  auto &a(graph.addInputTensor<float, 3, 3>("a"));
  auto &b(graph.addInputTensor<float, 3, 3>("b"));
  auto &output(graph.addTensor<nnc::op::MatMul>(a, b));
  graph.addOutput(output);

  nnc::error::OstreamErrorReporter reporter(std::cerr);

  if ( !graph.check(reporter) ) {
    std::cerr << "There were errors creating the graph" << std::endl;
    return 1;
  }

  std::fstream o("graph.dot", std::fstream::out);
  nnc::graphviz::DotOutput dotter(graph, o);
  graph.visit(dotter);

  nnc::executor::CpuExecutor exec(graph, 1);
  exec.build();

  std::cerr << "Loop structure" << std::endl;
  nnc::executor::CpuOpPrinter print(std::cerr);
  exec.visitAll(print);

  std::cerr << "Compiling to RTL" << std::endl;

  nnc::arch::x86_64::compile::DefaultManager mgr("simple.");
  nnc::compile::RtlFunction &fn(mgr.translate(exec, "run"));

  fn.dump(std::cerr);

  nnc::invoke::FunctionLibrary lib;
  mgr.compile(lib, fn);

//  float input1[320], input2[320], result[320];
//  int testaeu[320];
//  std::fill(std::begin(input1), std::end(input1), 1);
//  std::fill(std::begin(input2), std::end(input2), 2);
//  std::fill(std::begin(result), std::end(result), 0);

  float input1[] =
    { 1.0, 2.0, 3.0,
      4.0, 5.0, 6.0,
      7.0, 8.0, 9.0 };

  float result[9];
  std::fill(std::begin(result), std::end(result), 0);

  try {
    lib["simple.run"].invoke<void>(input1, input1, result);
  } catch (nnc::exception::RuntimeArityMismatch &e) {
    std::cerr << "Arity mismatch... Expected " << e.expected() << ", given " << e.given() << std::endl;
  }

  std::cerr << "Result is: " << std::endl
            << "  " << result[0] << " " << result[1] << " " << result[2] << std::endl
            << "  " << result[3] << " " << result[4] << " " << result[5] << std::endl
            << "  " << result[6] << " " << result[7] << " " << result[8] << std::endl;

  /* Now cpu can be directly executed */

  return 0;
}
