{
  description = "Nix flake for the odbc_scanner duckdb extension";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    zig-overlay = {
      url = "github:mitchellh/zig-overlay";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = {
    self,
    flake-utils,
    zig-overlay,
    nixpkgs,
    ...
  }: let
    systems = ["x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin"];
    outputs = flake-utils.lib.eachSystem systems (system: let
      pkgs = import nixpkgs {
        inherit system;
        overlays = [
          zig-overlay.overlays.default
        ];
      };
    in rec {
      # packages exported by the flake
      packages = {
        zig-build-fast = pkgs.writeScriptBin "zig_build_fast" ''
          ${pkgs.zigpkgs.master}/bin/zig build \
            -Dopenssl-include-dir=${pkgs.openssl.dev}/include \
            -Doptimize=ReleaseFast \
            -Dbuild-shell
        '';
        zig-build-small = pkgs.writeScriptBin "zig_build_small" ''
          ${pkgs.zigpkgs.master}/bin/zig build \
            -Dopenssl-include-dir=${pkgs.openssl.dev}/include \
            -Doptimize=ReleaseSmall \
            -Dbuild-shell
        '';
        zig-build-debug = pkgs.writeScriptBin "zig_build_debug" ''
          ${pkgs.zigpkgs.master}/bin/zig build \
            -Dopenssl-include-dir=${pkgs.openssl.dev}/include \
            -Doptimize=Debug \
            -Dbuild-shell
        '';
      };

      # nix run
      apps = {
        build-fast = {
          type = "app";
          program = "${self.packages.${system}.zig-build-fast}/bin/zig_build_fast";
        };
        build-small = {
          type = "app";
          program = "${self.packages.${system}.zig-build-small}/bin/zig_build_small";
        };
        build-debug = {
          type = "app";
          program = "${self.packages.${system}.zig-build-debug}/bin/zig_build_debug";
        };
        default = apps.build-fast;
      };

      # nix fmt
      formatter = pkgs.alejandra;

      # nix develop -c $SHELL
      devShells.default = pkgs.mkShell {
        packages = [
          pkgs.openssl
          pkgs.zigpkgs.master
        ];
      };
    });
  in
    outputs;
}
