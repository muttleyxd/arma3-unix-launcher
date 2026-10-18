# Spec: Mod Search, Workshop Links and Thumbnail Preview Strip

## Overview
Players with large mod lists need to find mods quickly and recognise them visually. This track adds a live search over installed mods, a thumbnail preview strip (carousel) above the mod table that follows the selection, Workshop preview images fetched and cached in the background, and links that open a mod's Workshop page in the Steam client.

## Functional Requirements
**FR1 Search**
- A search box above the mod table filters installed mods (Workshop + local) as you type.
- Case-insensitive substring match on mod name and Workshop ID; multiple words must all match (e.g. `ace med` finds "ACE3 Medical").
- The preview strip shows the same filtered set. Clearing the box (or Esc) restores the full list; ⌘F focuses the box.
- Enabled/disabled state and the selected-mods counter are unaffected by filtering.
- Drag-and-drop reordering is disabled while a filter is active (prevents reordering against hidden rows).

**FR2 Preview strip (carousel)**
- A horizontal strip of mod cards above the table: thumbnail, mod name, enabled indicator.
- Selecting a table row scrolls the strip to centre that mod's card; clicking a card selects its row. Left/right arrows and scroll wheel/trackpad move through cards.
- Local mods and mods without a preview show a placeholder card.
- The strip can be collapsed; collapsed state persists in settings.

**FR3 Workshop thumbnails**
- Preview URLs come from the public Steam Web API `ISteamRemoteStorage/GetPublishedFileDetails` (no API key), batched (up to 100 IDs per request).
- Images download in the background, never on the UI thread; cards fill in as they arrive.
- Cached on disk under `~/Library/Caches/a3unixlauncher/thumbnails/<id>.<ext>`, scaled for the strip; cached images are used offline. Cache entries refresh after 7 days when online.
- HTTPS works on stock macOS: the CA bundle lookup (shared with the update checker) also checks `/etc/ssl/cert.pem`.
- Network failure or missing previews degrade silently to placeholders, logged at info/verbose level; the launcher never crashes or blocks.

**FR4 Workshop links**
- Workshop mods get an "Open in Steam" action (card button and table context menu) that opens `steam://url/CommunityFilePage/<id>`.
- If no handler for `steam://` exists, fall back to `https://steamcommunity.com/sharedfiles/filedetails/?id=<id>` in the default browser.
- Local mods don't show the action.

## Non-Functional Requirements
- Responsive with 500+ mods: filtering updates within one frame; strip creates card widgets lazily / reuses them.
- Search, URL building, Web API request/response handling and cache logic live in Qt-free library code (`src/common` / `arma3-unix-launcher-library`) with doctest unit tests (>80% coverage of new library code); no network in unit tests (HTTP behind an interface, mocked).
- No exceptions thrown on the mod-loading or thumbnail hot paths (libsteam_api `__cxa_throw` override); use optional/expected results.
- Follows astyle and the project C++ conventions; Apple Silicon macOS only.

## Acceptance Criteria
- [ ] Typing in the search box filters table and strip live; multi-word, case-insensitive; Esc clears; ⌘F focuses.
- [ ] Selecting a row centres its card; clicking a card selects the row; arrows scroll the strip.
- [ ] Workshop mods show their Workshop preview; local mods show a placeholder.
- [ ] Thumbnails load without freezing the UI and come from the disk cache on the next launch (also offline).
- [ ] "Open in Steam" opens the mod page in the Steam client; falls back to the browser without Steam.
- [ ] Unit tests cover filtering, link building, Web API parsing and cache logic; `nix flake check` passes.

## Out of Scope
- Searching the Steam Workshop catalog / subscribing to new mods.
- Decoding local `.paa` images from mod.cpp.
- Full-screen carousel or a separate browse tab.
- DayZ launcher, Linux, Intel.
