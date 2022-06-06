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

  auto &a(graph.addInputTensor<float, 32>("a"));
  auto &b(graph.addInputTensor<float, 32>("b"));
  auto &output(graph.addTensor<nnc::op::AddOp>(a, b));
  graph.addOutput(output);

  nnc::error::OstreamErrorReporter reporter(std::cerr);

  if ( !graph.check(reporter) ) {
    std::cerr << "There were errors creating the graph" << std::endl;
    return 1;
  }

  std::fstream o("graph.dot", std::fstream::out);
  nnc::graphviz::DotOutput dotter(graph, o);
  graph.visit(dotter);

  nnc::executor::CpuExecutor exec(graph, 10);
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

  float input1[320], input2[320], result[320];
  int testaeu[320];
  std::fill(std::begin(input1), std::end(input1), 1);
  std::fill(std::begin(input2), std::end(input2), 2);
  std::fill(std::begin(result), std::end(result), 0);
  lib["simple.run"].invoke<void>( input1, input2, result );

  std::cerr << "Result is " << result[1] << " " << result[0] << std::endl;

  // No errors, now let's try compiling the graph into something we can actually do

//
//  auto &layer1(graph.addTensor<nnc::op::Sigmoid>(graph.addTensor<nnc::op::AddOp>(graph.addTensor< nnc::op::MatVecMul >(weights, flattened),
//                                                                                 biases)));
//  auto &layer2(graph.addTensor<nnc::op::Sigmoid>(graph.addTensor<nnc::op::AddOp>(graph.addTensor< nnc::op::MatVecMul >(weights2, layer1),
//                                                                                 biases2)));
//
//  auto &error(graph.addTensor<nnc::op::SqrtOp>(graph.addTensor<nnc::op::SumOp>(graph.addTensor<nnc::op::PowOp>(graph.addTensor<nnc::op::SubOp>(layer2, expected),
//                                                                                                               graph.addTensor< nnc::op::ConstTensor<> >({ 2 })))));

//
//  // How this works... randomly choose ten images. Propagate backwards and forwards
//
//  auto &weightsGradient(graph.derivative(error, weights));
//  auto &weights2Gradient(graph.derivative(error, weights2));
//  auto &biasesGradient(graph.derivative(error, biases));
//  auto &biases2Gradient(graph.derivative(error, biases2));
//
//  // Executor of the graph at batch size 10
//  auto &executor(graph.makeExecutor().batchSize(10).calculate(weightsGradient).calculate(weights2Gradient).calculate(biasesGradient).calculate(biases2Gradient).build());
//
//  auto &invocation(executor.invocation()
//                   .input("image", randomImages)
//                   .input("label", expLabels));
//   
//  invocation.get(weightsGradient)
//
//    .invoke();

  /* Graph can be serialized to disk */

  /* This adds/removes operations in the graph, so that everything can
     be executed on the CPU */
  //  nnc::optimizer::cpu::auto_optimizer().optimize(graph);

//  auto invocation(graph.make_invocation());
//  invocation.setInput("image", mnist[i, nnc::ix::all, nnc::ix::all]);
//  invocation.invoke();



  /* Now cpu can be directly executed */

  return 0;
}
