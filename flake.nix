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
        # config = { allowUnfree = true; };
      };
    in rec {
      # packages exported by the flake
      packages = {};

      # nix run
      apps = {
        generate-dot-clangd = {
          type = "app";
          program = toString (pkgs.writeScript "generate-dot-clangd" ''
            UNIX_ODBC_DIR=${pkgs.unixODBC} \
              envsubst < ./templates/.clangd.template > .clangd
          '');
        };
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
                llvmPackages_16.clang
                openssl
                # unixODBCDrivers.msodbcsql17
                unixODBCDrivers.psql
                # unixODBCDrivers.mariadb
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
                # unixODBCDrivers.msodbcsql17
                unixODBCDrivers.psql
                # unixODBCDrivers.mariadb
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
          # pkgs.unixODBCDrivers.msodbcsql17
          pkgs.unixODBCDrivers.psql
          # pkgs.unixODBCDrivers.mariadb
          pkgs.postgresql_15
        ];
      };
    });
  in
    outputs;
}
