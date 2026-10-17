# Tech Stack

## Platform
- macOS on Apple Silicon (arm64 / aarch64-darwin) only

## Language & Compiler
- C++20 (`CMAKE_CXX_STANDARD 20`, required)
- Clang from the Nix darwin stdenv
- Warnings: `-Wall -Wextra -Wpedantic`; Debug/check builds add `-Werror` and `-DDEVELOPER_MODE=ON`

## UI
- Qt6 (nixpkgs `qt6.qtbase`, `qt6.qtsvg`): Widgets + Svg
- Qt Designer `.ui` forms, AUTOMOC / AUTOUIC / AUTORCC
- QTDark stylesheet; open-iconic + blagoicons SVG icon sets

## Build System: Nix First
- `flake.nix` is the single source of truth for the toolchain and **all** libraries; nothing comes from Homebrew or the network at configure time
- `nix build` → builds `Arma 3 Unix Launcher.app` (CMake run inside the derivation)
- `nix flake check` → configures with `-DRUN_TESTS=ON`, builds, runs `ctest`
- `nix develop` / direnv (`use flake . --impure`) → devShell with the same inputs, for fast incremental `cmake --build` / `ctest` while hacking
- CMake ≥ 3.21 remains the underlying build system; `FetchContent` downloads are disabled (`FETCHCONTENT_FULLY_DISCONNECTED=ON`) so every `setup_*()` resolves via `find_package` against Nix-provided packages
- Packaging: `macdeployqt` in the derivation for the `.app`; `.dmg` creation (`hdiutil`) may run as a separate `nix run` app outside the build sandbox

## Libraries (all provided by the flake)
| Purpose | Library | Nix source |
|---|---|---|
| Logging | spdlog 1.17 | nixpkgs |
| Formatting | fmt 12.1 | nixpkgs |
| CLI args | argparse 3.2 | nixpkgs |
| JSON | nlohmann_json 3.12 | nixpkgs |
| XML/HTML | pugixml (fork `simple-build-for-a3ul`) | `fetchFromGitHub` override |
| HTTP | curlpp + curl | nixpkgs |
| Steam | Steamworks SDK 1.53 (`libsteam_api.dylib`) | `fetchzip` with pinned hash (unfree) |
| Tests | doctest, trompeloeil | nixpkgs |
| RAII guards | scope_guard | vendored (`external/`) |
| Crash traces | backward-cpp | vendored (`external/`) |

## Testing
- Unit: doctest + trompeloeil mocks (`tests/unit`), run by `nix flake check` and `ctest` in the devShell
- Fuzzing: libFuzzer-style harnesses for VDF, mod and cppfilter parsers (`tests/fuzzing`)

## Code Style
- astyle (`astylerc`, `astyle-format.sh`), provided by the devShell: Allman braces, 4-space indent, 120-col max, `align-pointer=name`

## Architecture
- `src/common`: shared filesystem/string/std utils, update checker
- `src/arma3-unix-launcher-library`: core logic (Arma3Client, VDF parser, Mod, Steam utils, HTML preset export)
- `src/steam-integration`: Steamworks wrapper with real impl + stub
- `src/arma3-unix-launcher`: Qt6 app (MainWindow, ModTableWidget, settings, HTML preset parser)
- `src/dayz-*`: upstream DayZ variant, out of scope

## CI
- GitHub Actions macOS arm64 job runs `nix flake check` and `nix build` (replacing Homebrew installs)

## Known Gaps (to address)
- Current `flake.nix` only has a devShell with gcc + Qt5: needs clang, Qt6, all libs above, a package output and checks
- `cmake/external_dependencies.cmake` falls back to FetchContent; must work fully offline against Nix packages
- README still documents Qt5 / CMake 3.11 / Homebrew
