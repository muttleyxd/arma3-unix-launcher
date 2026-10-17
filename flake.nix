{
  description = "Arma 3 Unix Launcher for macOS on Apple Silicon";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "aarch64-darwin";
      pkgs = import nixpkgs {
        inherit system;
        # The Steamworks SDK is the only unfree dependency
        config.allowUnfreePredicate = pkg: nixpkgs.lib.getName pkg == "steamworks-sdk";
      };
      steamworks-sdk = pkgs.callPackage ./nix/steamworks-sdk.nix { };
    in
    {
      packages.${system} = {
        inherit steamworks-sdk;
      };

      devShells.${system}.default = pkgs.mkShell {
        # mkShell uses the darwin stdenv, so the compiler is clang
        packages = with pkgs; [
          cmake
          ninja
          pkg-config
          astyle

          qt6.qtbase
          qt6.qtsvg
          argparse
          curl
          curlpp
          fmt
          nlohmann_json
          pugixml
          spdlog

          doctest
          trompeloeil
        ];

        # Read by CMakeLists.txt and cmake/external_dependencies.cmake as option defaults
        A3UL_USE_SYSTEM_DEPS = "ON";
        STEAMWORKS_SDK_DIR = "${steamworks-sdk}";
      };
    };
}
