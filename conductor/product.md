# Initial Concept

Existing project (brownfield): a native Arma 3 launcher for macOS, forked from upstream arma3-unix-launcher, scoped to Apple Silicon.

# Product Guide

## Overview
Arma 3 Unix Launcher is a native desktop launcher for Bohemia Interactive's Arma 3. The official launcher is Windows-only, so this project gives players a first-class way to manage mods and start the game. Upstream also targets GNU/Linux, Intel Macs and DayZ; **this project's scope is macOS on Apple Silicon only.**

## Target Platform
- macOS on Apple Silicon (aarch64-darwin), native Steam client running the native macOS Arma 3 build
- All dependencies and builds go through the Nix flake: `nix build` produces the app, `nix run` starts it, `nix flake check` runs the tests, and the devShell (`nix develop` or direnv) is for day-to-day incremental CMake work
- Distributed as a `.dmg` app bundle (`nix run .#dmg`; release packaging not yet wired into CI)
- Out of scope: Linux, Intel/universal macOS binaries, Proton handling, and the DayZ launcher variant (left in the tree from upstream but not maintained or tested here)

## Target Users
- Arma 3 players on Apple Silicon Macs
- Modded-server players who juggle many Steam Workshop subscriptions and presets
- Power users who launch presets from the CLI

## Core Features
- Auto-detect Steam install, Arma 3 directory, and Workshop content (VDF parsing)
- Mod list management: enable/disable, ordering, Workshop + local mods
- Presets: save/load, import/export Arma 3 Launcher HTML preset format
- Bohemia launch parameters exposed in a Parameters UI (server connect, skip intro, etc.)
- Steamworks integration (optional; stub fallback with --disable-steam-integration)
- CLI mode: list presets, run a preset headless, connect to server
- Update notifications

## Goals
- A polished, native-feeling macOS experience
- Fully reproducible builds: one `nix build` gives every contributor the same app from the same toolchain and libraries
- Fast incremental builds in the devShell while developing
- Reliability: never crash on malformed VDF/HTML/mod metadata (fuzz-tested parsers)

## Current Focus
- Mod list search/filtering and mod thumbnails (branch: feature/search-and-thumbnails)
