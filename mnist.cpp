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

template <typename T, typename Out=T> void dump_img(T *d) {
  for (int y = 0; y < 28; ++ y) {
    for (int x = 0; x < 28; ++x) {
      std::cerr << (Out) d[y * 28 + x] << " ";
    }
    std::cerr << std::endl;
  }
}

int main(int argc, const char **argv) {
  //  nnc::io::MnistReader<std::uint8_t, 3> reader("./datasets/mnist/train-images-idx3-ubyte");

  nnc::graph::Graph graph("MNIST CNN");

  auto &imageInput(graph.addTensor< nnc::op::Cast<std::uint8_t, float> >
                   (graph.addInputTensor<std::uint8_t, 28, 28>("image")));
  graph.addOutput(imageInput);

  //  auto &labelInput(graph.addTensor< nnc::op::Cast<std::uint8_t, std::uint32_t> >
  //                   (graph.addInputTensor<std::uint8_t>("label")));

  std::vector<int> newShape{28 * 28, 1};
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

  nnc::error::OstreamErrorReporter reporter(std::cerr);

  if ( !graph.check(reporter) ) {
    std::cerr << "There were errors creating the graph" << std::endl;
    return 1;
  }

  std::fstream o("mnist.dot", std::fstream::out);
  nnc::graphviz::DotOutput dotter(graph, o);
  graph.visit(dotter);

  nnc::executor::CpuExecutor exec(graph, 1);
  exec.build();

  std::cerr << "Loop structure" << std::endl;
  nnc::executor::CpuOpPrinter print(std::cerr);
  exec.visitAll(print);

  std::cerr << "Compiling to RTL" << std::endl;

  nnc::arch::x86_64::compile::DefaultManager mgr("mnist.");
  nnc::compile::RtlFunction &fn(mgr.translate(exec, "infer"));

  fn.dump(std::cerr);

  std::uint8_t testimg[28 * 28];
  float result[28 * 28];
  std::fill(std::begin(result), std::end(result), 0.0f);

  for (int i = 0; i < (28 * 28); ++i)
    testimg[i] = random();

  nnc::invoke::FunctionLibrary lib;
  mgr.compile(lib, fn);
  lib["mnist.infer"].invoke<void>(testimg, result);

  std::cerr << "Input: " << std::endl;
  dump_img<std::uint8_t, int>(testimg);

  std::cerr << "Result: " << std::endl;
  dump_img(result);

//
////  float input1[320], input2[320], result[320];
////  int testaeu[320];
////  std::fill(std::begin(input1), std::end(input1), 1);
////  std::fill(std::begin(input2), std::end(input2), 2);
////  std::fill(std::begin(result), std::end(result), 0);
//
//  float input1[] =
//    { 1.0, 2.0, 3.0,
//      4.0, 5.0, 6.0,
//      7.0, 8.0, 9.0 };
//
//  float result[9];
//  std::fill(std::begin(result), std::end(result), 0);
//
//  try {
//    lib["simple.run"].invoke<void>(input1, input1, result);
//  } catch (nnc::exception::RuntimeArityMismatch &e) {
//    std::cerr << "Arity mismatch... Expected " << e.expected() << ", given " << e.given() << std::endl;
//  }

//  std::cerr << "Result is: " << std::endl
//            << "  " << result[0] << " " << result[1] << " " << result[2] << std::endl
//            << "  " << result[3] << " " << result[4] << " " << result[5] << std::endl
//            << "  " << result[6] << " " << result[7] << " " << result[8] << std::endl;

  return 0;
}
