{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
  }:
    flake-utils.lib.eachDefaultSystem (system: let
      pkgs = nixpkgs.legacyPackages.${system};

      zfs_properties = pkgs.stdenv.mkDerivation (finalAttrs: {
        pname = "zfs_properties";
        version = "0.1.0";

        src = ./src;

        buildInputs = [
          pkgs.pkg-config
          pkgs.zfs.dev
          pkgs.json_c.dev
        ];

        buildPhase = ''
          set -e
          gcc -o zfs_properties zfs_properties.c \
            $(pkg-config --cflags --libs libzfs) \
            $(pkg-config --cflags --libs json-c)
        '';
        installPhase = ''
          mkdir -p $out/bin
          cp zfs_properties $out/bin
        '';
      });
    in {
      packages.default = zfs_properties;
    });
}
