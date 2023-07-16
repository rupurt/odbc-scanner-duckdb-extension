{
  description = "Nix flake for the odbc_scanner duckdb extension";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    odbc-drivers.url = "github:rupurt/odbc-drivers-nix";
  };

  outputs = {
    flake-utils,
    odbc-drivers,
    ...
  }: let
    systems = ["x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin"];
    outputs = flake-utils.lib.eachSystem systems (system: let
      # use old version of nix packages that builds against glibc 2.35
      pkgs =
        import (builtins.fetchGit {
          name = "nixpkgs-with-glibc-2.35-224";
          url = "https://github.com/nixos/nixpkgs";
          ref = "refs/heads/nixpkgs-unstable";
          rev = "8ad5e8132c5dcf977e308e7bf5517cc6cc0bf7d8";
        }) {
          inherit system;
          overlays = [
            odbc-drivers.overlay
          ];
        };
      stdenv = pkgs.llvmPackages_15.stdenv;
    in rec {
      # packages exported by the flake
      packages = {
        db2-odbc-driver = pkgs.db2-odbc-driver {};
      };

      # nix run
      apps = {
        generate-dot-clangd = {
          type = "app";
          program = toString (pkgs.writeScript "generate-dot-clangd" ''
            UNIX_ODBC_DIR=${pkgs.unixODBC} \
              envsubst < ./templates/.clangd.template > .clangd
          '');
        };
        generate-odbc-ini = {
          type = "app";
          program = toString (pkgs.writeScript "generate-odbc-ini" ''
            cp ./templates/.odbc.ini.template .odbc.ini
          '');
        };
        generate-odbcinst-ini = {
          type = "app";
          program = toString (pkgs.writeScript "generate-odbcinst-ini" ''
            DB2_DRIVER_PATH=${packages.db2-odbc-driver}/lib/${if stdenv.isDarwin then "libdb2.dylib" else "libdb2.so"} \
              envsubst < ./templates/.odbcinst.ini.template > .odbcinst.ini
          '');
        };
        ls-odbc-driver-paths = {
          type = "app";
          program = toString (pkgs.writeScript "ls-odbc-driver-paths" ''
            echo "db2 ${packages.db2-odbc-driver}/lib/${if stdenv.isDarwin then "libdb2.dylib" else "libdb2.so"}"
          '');
        };
        load-db2-schema = {
          type = "app";
          program = toString (pkgs.writeScript "load-db2-schema" ''
            echo "TODO: load db2 schema"
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
                openssl
                packages.db2-odbc-driver
              ]
            )}:$PATH"
            export CC=${stdenv.cc}/bin/clang
            export CXX=${stdenv.cc}/bin/clang++

            make \
              GEN=ninja \
              ODBCSYSINI=$PWD \
              ODBCINSTINI=.odbcinst.ini \
              ODBCINI=$PWD/.odbc.ini \
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
                openssl
                packages.db2-odbc-driver
              ]
            )}:$PATH"
            export CC=${stdenv.cc}/bin/clang
            export CXX=${stdenv.cc}/bin/clang++

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
          # faster cmake builds
          pkgs.ninja
          # clangd lsp
          pkgs.llvmPackages_15.bintools
          pkgs.llvmPackages_15.clang
          pkgs.envsubst
          pkgs.openssl
          pkgs.unixODBC
          # psql cli
          pkgs.postgresql_15
          packages.db2-odbc-driver
        ];
      };
    });
  in
    outputs;
}
