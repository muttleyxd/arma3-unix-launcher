{ lib
, stdenv
, cmake
, darwin
, ninja
, nukeReferences
, pkg-config
, qt6
, argparse
, curl
, curlpp
, fmt
, nlohmann_json
, pugixml
, spdlog
, steamworks-sdk
, writeShellScriptBin
}:

let
  # macdeployqt ad-hoc signs every binary it changes on arm64 using
  # `codesign --preserve-metadata=... --force -s - <file>` and finally runs `codesign --deep -v <app>`.
  # sigtool's codesign only signs Mach-O files and does not know --preserve-metadata, so drop that
  # flag, skip non-Mach-O paths and skip verification here; tools/ci/check-nix-package.sh verifies
  # the result with Apple's codesign.
  macdeployqt-codesign = writeShellScriptBin "codesign" ''
    args=()
    for arg in "$@"; do
      case "$arg" in
        -v|--verify) exit 0 ;;
        --preserve-metadata=*) ;;
        *) args+=("$arg") ;;
      esac
    done
    file="''${args[-1]}"
    if ! ${darwin.sigtool}/bin/sigtool --file "$file" show-arch > /dev/null 2>&1; then
      exit 0
    fi
    exec ${darwin.sigtool}/bin/codesign "''${args[@]}"
  '';
in
stdenv.mkDerivation {
  pname = "arma3-unix-launcher";
  version = "0-unstable";

  src = lib.fileset.toSource {
    root = ./..;
    fileset = lib.fileset.unions [
      ../CMakeLists.txt
      ../cmake
      ../external
      ../src
      ../tests
    ];
  };

  nativeBuildInputs = [
    cmake
    macdeployqt-codesign
    ninja
    nukeReferences
    pkg-config
  ];

  buildInputs = [
    qt6.qtbase
    qt6.qtsvg
    argparse
    curl
    curlpp
    fmt
    nlohmann_json
    pugixml
    spdlog
  ];

  cmakeFlags = [
    (lib.cmakeBool "A3UL_USE_SYSTEM_DEPS" true)
    (lib.cmakeFeature "STEAMWORKS_SDK_DIR" "${steamworks-sdk}")
    # cctools strip cannot handle current load commands; the fixup phase strips instead
    (lib.cmakeFeature "MACDEPLOYQT_EXTRA_ARGS" "-no-strip")
  ];

  # The .app is made self-contained by macdeployqt, wrapper scripts would break the bundle
  dontWrapQtApps = true;

  # Libraries copied in by macdeployqt keep compiled-in store paths (Qt prefix, ICU data,
  # OpenSSL config, ...). They are never loaded from there, but would pull the whole build
  # closure into the package, so they are replaced. nuke-refs re-signs Mach-O files.
  postInstall = ''
    mkdir -p $out/Applications
    mv "$out/Arma 3 Unix Launcher.app" $out/Applications/
    find "$out/Applications" -type f -exec nuke-refs {} +
  '';

  meta = {
    description = "Arma 3 launcher for macOS on Apple Silicon";
    homepage = "https://github.com/muttleyxd/arma3-unix-launcher";
    license = lib.licenses.mit;
    platforms = [ "aarch64-darwin" ];
    mainProgram = "Arma 3 Unix Launcher";
  };
}
