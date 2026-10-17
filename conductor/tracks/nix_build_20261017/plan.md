# Plan: Nix-First Build for Apple Silicon

> TDD note: this track is build infrastructure. "Tests" are executable checks: CMake configure/build assertions, `nix flake check`, and existing doctest suites. Each task writes the failing check first (Red), then makes it pass (Green). The >80% coverage target applies to any new C++ code; build scripts are verified by checks instead.

## Phase 1: Offline-Capable CMake

- [x] Task: Write failing check: configuring with `-DA3UL_USE_SYSTEM_DEPS=ON -DFETCHCONTENT_FULLY_DISCONNECTED=ON` and a missing lib must error (not download). Add as a script under `tools/ci/` (e.g. `check_offline_configure.sh`). 74edaa1
- [x] Task: Add `A3UL_USE_SYSTEM_DEPS` option; make `setup_library()` call `message(FATAL_ERROR ...)` instead of FetchContent when set. 9543be7
- [x] Task: Make `setup_steamworkssdk()` accept `-DSTEAMWORKS_SDK_DIR=` (skip download when provided; required when `A3UL_USE_SYSTEM_DEPS=ON`). 61db51f
- [x] Task: Make header-only/library checks (`argparse`, `doctest`, `trompeloeil`, `nlohmann_json`, `scope_guard`, `fmt`, `spdlog`, `curlpp`, `pugixml`) resolve via `find_package` against system/Nix installs; fix any target-name mismatches. 2802770
- [ ] Task: Update `tech-stack.md` if any library resolution differs from the documented plan.
- [ ] Task: Conductor - User Manual Verification 'Phase 1: Offline-Capable CMake' (Protocol in workflow.md)

## Phase 2: Flake DevShell

- [ ] Task: Write failing check: `nix develop -c cmake -B build-nix -DA3UL_USE_SYSTEM_DEPS=ON -DRUN_TESTS=ON` fails with current flake (Qt5/gcc).
- [ ] Task: Restructure `flake.nix` for `aarch64-darwin`; define package overlays for pugixml fork (`fetchFromGitHub`, pinned hash) and Steamworks SDK 1.53 (`fetchzip`, pinned hash, `allowUnfree` scoped to it).
- [ ] Task: DevShell provides clang stdenv, cmake, ninja, qt6.qtbase, qt6.qtsvg, spdlog, fmt, argparse, nlohmann_json, curl, curlpp, doctest, trompeloeil, astyle; exports `STEAMWORKS_SDK_DIR` and `CMAKE_ARGS` defaults.
- [ ] Task: Verify `cmake --build build-nix && ctest --test-dir build-nix` passes inside the devShell (existing unit tests green); confirm `.envrc` (`use flake . --impure`) still works or drop `--impure` if no longer needed.
- [ ] Task: Conductor - User Manual Verification 'Phase 2: Flake DevShell' (Protocol in workflow.md)

## Phase 3: `nix build` Package and `nix flake check`

- [ ] Task: Write failing check: `nix build` and `nix flake check` fail (no outputs defined).
- [ ] Task: Add `packages.default` derivation: CMake configure with `A3UL_USE_SYSTEM_DEPS=ON`, Release, `CMAKE_OSX_DEPLOYMENT_TARGET`; install `.app` to `$out/Applications`.
- [ ] Task: Ensure `macdeployqt` from Nix Qt deploys frameworks and `libsteam_api.dylib` is copied; fix rpaths so the app runs outside the Nix shell (verify with `otool -L`).
- [ ] Task: Add `checks.default`: Debug build with `-DRUN_TESTS=ON -DDEVELOPER_MODE=ON -DCMAKE_CXX_FLAGS=-Werror`, run `ctest --output-on-failure`.
- [ ] Task: Add `apps.dmg` that runs `hdiutil`/CPack DragNDrop on `result/` outside the sandbox.
- [ ] Task: Conductor - User Manual Verification 'Phase 3: nix build Package and nix flake check' (Protocol in workflow.md)

## Phase 4: CI and Docs

- [ ] Task: Write failing check: CI workflow lint (e.g. `actionlint`) on updated workflow; ensure job references `nix flake check`.
- [ ] Task: Update `.github/workflows/mac_os_x_check.yml`: drop Intel matrix entry, install Nix (e.g. `DeterminateSystems/nix-installer-action`), run `nix flake check` and `nix build`. Leave Linux workflows unchanged.
- [ ] Task: Update `mac_os_x_release.yml` to build via `nix build` + `nix run .#dmg` for arm64 only.
- [ ] Task: Update README macOS section: Nix instructions, Qt6, CMake 3.21; note Apple Silicon only for this fork.
- [ ] Task: Conductor - User Manual Verification 'Phase 4: CI and Docs' (Protocol in workflow.md)
