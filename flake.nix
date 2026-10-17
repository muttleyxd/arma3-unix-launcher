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
        buildInputs = with pkgs; [
          gcc
          cmake
          qt5.qt3d
          qt5.qtbase
        ];
      };
    };
}
