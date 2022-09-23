let sources = import ./nix/sources.nix;
in { pkgs ? import sources.nixpkgs {} }:

with pkgs;
stdenv.mkDerivation {
  name = "nnc";

  buildInputs = [ cmake opencl-headers catch2 clang pkg-config gdb flex bison valgrind ninja graphviz ccls ];
}
