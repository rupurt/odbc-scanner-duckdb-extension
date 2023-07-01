{
  description = "Nix flake for the odbc_scanner duckdb extension";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    flake-utils,
    nixpkgs,
    ...
  }: let
    systems = ["x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin"];
    outputs = flake-utils.lib.eachSystem systems (system: let
      pkgs = import nixpkgs {
        inherit system;
      };
    in rec {
      # packages exported by the flake
      packages = {};

      # nix run
      apps = {
        generate-dot-clangd = {
          type = "app";
          program = toString (pkgs.writeScript "generate-dot-clangd" ''
            echo "CompileFlags:" > .clangd
            echo "  Add:" >> .clangd
            echo "    - -I$PWD/src/include" >> .clangd
            echo "    - -I${pkgs.unixODBC}/include" >> .clangd
            echo "    - -I$PWD/duckdb/src/include" >> .clangd
            echo "    - -I$PWD/duckdb/third_party/re2" >> .clangd
            echo "    - -I$PWD/duckdb/third_party/parquet" >> .clangd
            echo "    - -I$PWD/duckdb/third_party/thrift" >> .clangd
            echo "    - -I$PWD/duckdb/third_party/utf8proc" >> .clangd
            echo "    - -I$PWD/duckdb/third_party/tools/sqlite3_api_wrapper/include" >> .clangd
            echo "    - -I$PWD/duckdb/third_party/tools/sqlite3_api_wrapper/sqlite3_udf_api/include" >> .clangd
            echo "    - -I$PWD/duckdb/third_party/imdb/include" >> .clangd
          '');
        };
        test = {
          type = "app";
          program = toString (pkgs.writeScript "test" ''
            export PATH="${pkgs.lib.makeBinPath (
              with pkgs; [
                git
                gnumake
                cmake
                ninja
                llvmPackages_16.clang
                openssl
                unixODBCDrivers.psql
              ]
            )}:$PATH"
            export CC=${pkgs.llvmPackages_16.clang}/bin/clang
            export CXX=${pkgs.llvmPackages_16.clang}/bin/clang++

            make \
              GEN=ninja \
              test CLIENT_FLAGS="-DODBC_CONFIG=${pkgs.unixODBC}/bin/odbc_config"
          '');
        };
        build = {
          type = "app";
          program = toString (pkgs.writeScript "build" ''
            export PATH="${pkgs.lib.makeBinPath (
              with pkgs; [
                git
                gnumake
                cmake
                ninja
                llvmPackages_16.clang
                openssl
                unixODBCDrivers.psql
              ]
            )}:$PATH"
            export CC=${pkgs.llvmPackages_16.clang}/bin/clang
            export CXX=${pkgs.llvmPackages_16.clang}/bin/clang++

            make \
              GEN=ninja \
              CLIENT_FLAGS="-DODBC_CONFIG=${pkgs.unixODBC}/bin/odbc_config"
          '');
        };
        default = apps.build;
      };

      # nix fmt
      formatter = pkgs.alejandra;

      # nix develop -c $SHELL
      devShells.default = pkgs.mkShell {
        packages = [
          pkgs.git
          pkgs.gnumake
          pkgs.cmake
          pkgs.ninja
          pkgs.llvmPackages_16.clang
          pkgs.openssl
          pkgs.unixODBC
          pkgs.unixODBCDrivers.psql
          pkgs.postgresql_15
        ];
      };
    });
  in
    outputs;
}
