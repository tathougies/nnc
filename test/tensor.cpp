#include <catch2/catch.hpp>
#include <array>

#include "memory_tensor.hpp"

TEST_CASE("Tensor shape dimension is correct for const tensor") {
  nnc::ConstTensor<double, std::allocator<double>, 10, 11, 12, 13> c;
  REQUIRE(c.shape().dims() == 4);
  REQUIRE(c.shape().sizeInDimension(0) == 10);
  REQUIRE(c.shape().sizeInDimension(1) == 11);
  REQUIRE(c.shape().sizeInDimension(2) == 12);
  REQUIRE(c.shape().sizeInDimension(3) == 13);
}

TEST_CASE("Const tensor layout fails when asked for invalid dimension") {
  nnc::ConstTensor<double, std::allocator<double>, 10, 10, 10> x;
  REQUIRE_THROWS( x.shape().sizeInDimension(3) );
}

TEST_CASE("Const tensor indexing is correct") {
  nnc::ConstTensor<double, std::allocator<double>, 7, 5, 3> x;
  REQUIRE( x.transformIndexA({0, 0, 0}) == 0 );
}
