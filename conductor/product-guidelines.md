# Product Guidelines

## UX Principles
- **Zero-config first run:** auto-detect Steam, Arma 3 and Workshop paths; only show the path chooser dialog when detection fails.
- **The mod list is the main screen.** Enabling, ordering, searching and launching should be reachable without opening menus.
- **Fast with big mod lists:** stay responsive with 500+ Workshop mods. Heavy work (scanning, thumbnail loading, Steam queries) runs off the UI thread and fills in as it finishes.
- **Never lose user state:** presets, mod order and parameters persist on change, and a malformed file never wipes settings.
- **Feature parity, not imitation:** match the official launcher's capabilities (presets, HTML import/export, parameters) while following macOS conventions.

## macOS Look and Feel
- Respect the system light/dark appearance; QTDark stays an option, not a forced theme.
- Use macOS conventions: ⌘ shortcuts, Preferences under the app menu, standard close/quit behavior, Retina-ready icons (SVG via open-iconic/blagoicons).
- Native file dialogs and native notification of update availability.

## Copy and Messaging
- Plain, short, player-focused language. Say "Mod not found in Workshop folder", not internal exception names.
- Error messages state what happened and what to do next (e.g. "Steam not found. Choose your Arma 3 folder…").
- Use Bohemia/Steam terminology players already know: Workshop, preset, mod, server, parameters.
- CLI help text stays terse and consistent with the existing argparse style.

## Reliability and Errors
- Parsers (VDF, HTML presets, mod.cpp/meta.cpp) must handle any input without crashing; keep them fuzz-tested.
- Avoid exceptions in hot paths (mod loading, Steam calls), because libsteam_api overrides __cxa_throw. Use return values or optional/expected-style results there.
- Log with spdlog: info by default, more detail with --verbose. Never log credentials or server passwords.

## Scope Discipline
- Apple Silicon macOS only. Don't add Linux, Intel, Proton or DayZ-specific behavior or UI.
- Prefer small, reviewable changes that keep the codebase easy to merge with upstream.
