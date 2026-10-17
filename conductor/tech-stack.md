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
- `nix build` → `result/Applications/Arma 3 Unix Launcher.app` (nix/package.nix); self-contained bundle, package closure is the app only
- `nix run` → starts the launcher from that bundle
- `nix flake check` → package with `withTests = true` (Debug, RUN_TESTS, DEVELOPER_MODE, -Werror), runs `ctest` in the sandbox
- `nix develop` / direnv (`use flake . --impure`) → devShell with the same inputs, for fast incremental `cmake --build` / `ctest` while hacking; it exports `A3UL_USE_SYSTEM_DEPS=ON`, `STEAMWORKS_SDK_DIR` and `MACDEPLOYQT_EXTRA_ARGS=-no-strip`, which CMake reads as defaults
- CMake ≥ 3.21 remains the underlying build system. Nix configures with `-DA3UL_USE_SYSTEM_DEPS=ON` (forces `FETCHCONTENT_FULLY_DISCONNECTED=ON`; any missing dependency is a configure error instead of a download) and `-DSTEAMWORKS_SDK_DIR=<store path>`. Libraries resolve via `find_package`, except curlpp (pkg-config, so `pkg-config` is a build input)
- Packaging: `macdeployqt -no-strip` with a sigtool-based `codesign` shim in the sandbox; `nuke-refs` strips embedded store paths. `nix run .#dmg` (nix/make-dmg.nix) re-signs the bundle with Apple's codesign and runs hdiutil outside the sandbox

## Libraries (all provided by the flake)
| Purpose | Library | Nix source |
|---|---|---|
| UI | Qt 6.11.2 (qtbase, qtsvg) | nixpkgs |
| Logging | spdlog 1.17.0 | nixpkgs |
| Formatting | fmt 12.2.0 | nixpkgs |
| CLI args | argparse 3.2 | nixpkgs |
| JSON | nlohmann_json 3.12.0 | nixpkgs |
| XML/HTML | pugixml 1.16 | nixpkgs |
| HTTP | curlpp 0.8.1 (via pkg-config) built against curlMinimal 8.22 (no IDN/PSL) | nixpkgs |
| Steam | Steamworks SDK 1.53 (`libsteam_api.dylib`, universal incl. arm64) | `fetchzip`, `sha256-SBNkSc2kkV20gfmUPtK5y6NYa7YL/DOyoPLRt+/nKhk=` (unfree) |
| Tests | doctest 2.5.3, trompeloeil 49 | nixpkgs |
| RAII guards | scope_guard | vendored (`external/`) |
| Crash traces | backward-cpp | vendored (`external/`) |

Versions are those in the pinned `flake.lock` nixpkgs as of 2026-10-17.

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
- CI still uses Homebrew and builds Intel too (deferred Phase 4 of nix_build_20261017)
- README still documents Qt5 / CMake 3.11 / Homebrew
- Update checker expects /etc/ssl/certs/ca-*.crt, which stock macOS lacks (/etc/ssl/cert.pem)

## Change Log
- **2026-10-17 (track nix_build_20261017, Phase 1):** pugixml comes from nixpkgs 1.16; the `muttleyxd/pugixml` `simple-build-for-a3ul` fork turned out to be unnecessary. curlpp resolves through pkg-config because it ships no CMake config. Added `A3UL_USE_SYSTEM_DEPS` and `STEAMWORKS_SDK_DIR` CMake options. Use an isolated devShell (`nix develop --ignore-environment`) when testing flake changes: the old Qt5 devShell leaked `NIX_CFLAGS_COMPILE` include paths into Qt6 builds.
- **2026-10-17 (track nix_build_20261017, Phases 2-3):** curlMinimal avoids a GNU/Apple `libiconv.2.dylib` clash in the flattened bundle. sigtool signatures can't seal bundle resources, so only the `.dmg` path (re-signed with Apple's codesign) produces a Gatekeeper-acceptable bundle.
