{
  description = "NNC neural network compiler";

  inputs = {
    nixpugs.url = "github:nixos/nixpkgs/nixos-22.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let pkgs = import nixpkgs { inherit system; };
      in rec {
        packages = rec {
          nnc = pkgs.stdenv.mkDerivation {
            name = "nnc";
            nativeBuildInputs = with pkgs; [ cmake ];
            buildInputs = with pkgs; [ opencl-headers catch2 clang pkg-config flex bison valgrind ];
          };

          default = nnc;
        };

        devShell = packages.default.overrideAttrs (oldAttrs: {
          nativeBuildInputs = oldAttrs.nativeBuildInputs ++ (with pkgs; [gdb graphviz ccls protobuf ]);
        });
      }
    );
}
