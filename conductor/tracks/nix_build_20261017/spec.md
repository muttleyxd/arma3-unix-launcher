# Spec: Nix-First Build for Apple Silicon

## Background
`tech-stack.md` makes the Nix flake the single source of truth for the toolchain, all libraries, and builds. Today:
- `flake.nix` has only a devShell with `gcc`, `cmake`, `qt5.qt3d`, `qt5.qtbase`, but the project requires Qt6 Widgets + Svg and builds with Clang on macOS.
- `cmake/external_dependencies.cmake` `setup_library()` silently falls back to `FetchContent` (network, unpinned `master` for curlpp/doctest) when a system lib isn't found; `setup_steamworkssdk()` always downloads the SDK zip.
- macOS CI installs deps via Homebrew and builds arm64 + Intel.

## Goals
1. `nix build` (aarch64-darwin) produces a working `Arma 3 Unix Launcher.app` with Qt frameworks deployed and `libsteam_api.dylib` bundled.
2. `nix flake check` builds with `-DRUN_TESTS=ON -DDEVELOPER_MODE=ON -Werror` and runs `ctest` successfully.
3. `nix develop` (and direnv `use flake`) gives the same toolchain and libs for incremental `cmake --build` / `ctest`.
4. No network access during CMake configure: every dependency resolves from Nix.
5. CI's macOS arm64 job uses Nix instead of Homebrew; the Intel job is removed.
6. A `nix run .#dmg` app (or documented step) produces a `.dmg` outside the build sandbox.

## Functional Requirements
- **FR1 Flake outputs** for `aarch64-darwin`: `packages.default`, `checks.default`, `devShells.default`, `apps.dmg`. Other systems are not required.
- **FR2 Dependencies from Nix:** qt6.qtbase, qt6.qtsvg, cmake, ninja (optional), spdlog, fmt, argparse, nlohmann_json, curl + curlpp, doctest, trompeloeil, astyle (devShell), pugixml fork `simple-build-for-a3ul` (via `fetchFromGitHub` + pinned hash), Steamworks SDK 1.53 (`fetchzip`, pinned hash, marked `unfree`).
- **FR3 Offline CMake:** a new option (e.g. `-DA3UL_USE_SYSTEM_DEPS=ON`, set by the flake) makes `setup_library()` and `setup_steamworkssdk()` fail loudly instead of downloading; `FETCHCONTENT_FULLY_DISCONNECTED=ON`. Steamworks path is passable via `-DSTEAMWORKS_SDK_DIR=`. Default behavior without the option stays unchanged for upstream compatibility.
- **FR4 App bundle:** `macdeployqt` (from Nix Qt) runs in the derivation; result installs to `$out/Applications/Arma 3 Unix Launcher.app` and launches outside the Nix shell.
- **FR5 CI:** `.github/workflows/mac_os_x_check.yml` arm64 job installs Nix and runs `nix flake check` + `nix build`; Intel matrix entry removed. Linux workflows untouched.
- **FR6 Docs:** README macOS build section replaced with Nix instructions; Qt6 / CMake 3.21 requirements corrected.

## Non-Functional Requirements
- Pinned inputs via `flake.lock`; builds reproducible from a clean checkout.
- Incremental devShell builds stay fast (no rebuild of deps per change).
- Do not break the upstream Linux build path (no edits to Linux-only CMake branches beyond the opt-in option).

## Acceptance Criteria
- [ ] `nix flake check` passes on aarch64-darwin from a clean checkout.
- [ ] `nix build` → `result/Applications/Arma 3 Unix Launcher.app` opens and detects Arma 3 / Workshop mods.
- [ ] Configure with `A3UL_USE_SYSTEM_DEPS=ON` performs zero downloads (verified with network-disabled sandbox build).
- [ ] `nix develop -c cmake -B build -DRUN_TESTS=ON && cmake --build build && ctest --test-dir build` passes.
- [ ] `nix run` starts the launcher from the built bundle.
- [ ] ~~`nix run .#dmg` produces a mountable `.dmg`.~~ Deferred (2026-10-17): the app exists and passes tools/ci/check-dmg.sh, but manual .dmg verification is postponed; iterate by running the binary instead.
- [ ] ~~macOS CI job is green on Nix.~~ Deferred (2026-10-17) with README updates (FR5, FR6) to a future track.

## Out of Scope
- Linux, Intel/universal binaries, DayZ launcher, Proton.
- Code signing / notarization.
- Search & thumbnails feature (next track).
