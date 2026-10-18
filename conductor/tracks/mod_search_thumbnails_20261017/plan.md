# Plan: Mod Search, Workshop Links and Thumbnail Preview Strip

> TDD per workflow.md: each task writes failing doctest tests first, then the implementation. New library code lives in `src/common` (Qt-free) with tests in `tests/unit/common`. UI tasks keep logic in the library and are verified manually.

## Phase 1: Search, Links and HTTPS Foundations [checkpoint: 44e2883]
- [x] Task: Coverage tooling 1fa36a9
    - [x] Add `A3UL_COVERAGE` CMake option (clang source-based coverage) and llvm tools to the devShell
    - [x] Add `tools/ci/coverage.sh` printing an `llvm-cov report` for `src/common` and the library
- [x] Task: Mod search matching (`common/mod_search.{hpp,cpp}`) b0043d1
    - [x] Tests: case-insensitive substring, multi-word AND (`ace med`), Workshop ID match, empty/whitespace query matches all, unicode-safe lowercase of ASCII
    - [x] Implement `ModSearch::Matches(name, workshop_id, query)`
- [x] Task: Workshop links (`common/workshop_links.{hpp,cpp}`) b068ffc
    - [x] Tests: `steam://url/CommunityFilePage/<id>` and `https://steamcommunity.com/sharedfiles/filedetails/?id=<id>`; reject non-numeric IDs
    - [x] Implement `WorkshopLinks::SteamClientUrl` / `BrowserUrl`
- [x] Task: CA bundle lookup for macOS (`common/ca_bundle.{hpp,cpp}`) 995ddd4
    - [x] Tests (injected file-exists predicate): finds `/etc/ssl/cert.pem`, keeps existing Linux paths, returns `std::nullopt` when none exist (no throw)
    - [x] Implement and switch `update_checker.cpp` to it
- [x] Task: Conductor - User Manual Verification 'Phase 1: Search, Links and HTTPS Foundations' (Protocol in workflow.md)

## Phase 2: Workshop Preview Metadata and Thumbnail Cache [checkpoint: 5310a77]
- [x] Task: HTTP client interface (`common/http_client.{hpp,cpp}`) 4263279
    - [x] `IHttpClient` (POST form / GET bytes → `std::optional`), libcurl C API implementation (curlpp throws) using the CA bundle; trompeloeil mock in `tests/unit/mock`
- [x] Task: Web API request building (`common/workshop_details.{hpp,cpp}`) e851908
    - [x] Tests: form body `itemcount` + `publishedfileids[i]`, batches of at most 100 IDs
    - [x] Implement request builder
- [x] Task: Web API response parsing f20f220
    - [x] Tests with JSON fixtures: id → preview_url and title; skips `result != 1`, missing `preview_url`; malformed JSON returns empty (parsed with exceptions disabled)
    - [x] Implement parser
- [x] Task: Thumbnail disk cache (`common/thumbnail_cache.{hpp,cpp}`) 139b1e8
    - [x] Tests (temp dir, injected clock): path `<cache>/thumbnails/<id>.<ext>`, fresh vs stale after 7 days, store/load, corrupt/unwritable dir handled without throwing
    - [x] Implement cache rooted at `~/Library/Caches/a3unixlauncher`
- [x] Task: Thumbnail service (`common/thumbnail_service.{hpp,cpp}`) cff682f
    - [x] Tests (mock HTTP, synchronous executor): cached+fresh served without network; missing/stale fetched in batches; offline falls back to stale cache; failures reported as "no thumbnail"; callback per ID
    - [x] Implement with a background worker thread and cancellation on shutdown
- [x] Task: Conductor - User Manual Verification 'Phase 2: Workshop Preview Metadata and Thumbnail Cache' (Protocol in workflow.md)

## Phase 3: UI Integration
- [x] Task: Search box 8524f2a
    - [x] Add search field above the mod table; filter rows with `ModSearch` (`setRowHidden`), Esc clears, ⌘F focuses
    - [x] Disable drag reordering while filtered; verify selected-mods counter unaffected
- [x] Task: Preview strip widget (`previewstrip.{h,cpp}`) c156e5d
    - [x] Horizontal `QListView` (icon mode, uniform item sizes) over a model of the visible mods; placeholder card art
    - [x] Two-way selection sync with the table, centre on selection, arrow keys / wheel scrolling
    - [x] Collapse toggle persisted in settings
- [~] Task: Thumbnails in the UI
    - [ ] Run `ThumbnailService` off the UI thread, deliver images via queued signals, scale once and cache `QPixmap`s
- [ ] Task: Open in Steam
    - [ ] Card button and table context menu for Workshop mods; `QDesktopServices::openUrl` with browser fallback
- [ ] Task: Large-list check
    - [ ] Script that creates 500 fake Workshop mods in a temp Arma layout; confirm filtering and scrolling stay responsive
- [ ] Task: Update `tech-stack.md` (new common modules, Web API usage, cache location)
- [ ] Task: Conductor - User Manual Verification 'Phase 3: UI Integration' (Protocol in workflow.md)
