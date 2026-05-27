# R.A.B.I.D.S — Agent Instructions

## Build

Configure and build with CMake (no Makefile, no npm/pip):

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The binary is `build/R.A.B.I.D.S`. Requires `config.cfg` in CWD at runtime and a `./data/` directory (created automatically).

## Dependencies

| Dep | Source | Notes |
|-----|--------|-------|
| Boost (date_time, log, log_setup) | System (`libboost-all-dev` on Ubuntu) | Static linked (`Boost_USE_STATIC_LIBS ON`) |
| OpenSSL | System | Used for PBKDF2 password hashing |
| `nlohmann_json` | FetchContent (v3.10.5) | Pulled via ArthurSonzogni/cmake wrapper |
| `sleepy-discord` | FetchContent (SteamDragon/sleepy-discord, origin/develop) | Custom fork, not upstream |
| `fmt` | FetchContent (tag 8.1.1) | |
| `dl` | System (`target_link_libraries(... dl)`) | |
| SQLite3 | Bundled (`src/sqlite3.c` + `include/sqlite3.h`) | Compiled as C within C++17 target |

## Testing / lint / format / typecheck

**None exist.** No test framework, no test targets, no CI test step. No `.clang-format`, `.clang-tidy`, or pre-commit config. SonarLint IDE configs live in `.sonarlint/` but are not enforced in CI.

To add tests, GoogleTest or Catch2 would need to be wired via CMake `FetchContent`.

## CI

| Workflow | Trigger | What it does |
|----------|---------|-------------|
| `ci.yaml` | Push / PR / tag `v**.**.**` | Delegates to `x64.yml` (build only); on tags also calls `release.yaml` |
| `x64.yml` | Reusable | Ubuntu, `gcc-10`/`g++-10`, CMake Release build, uploads `R.A.B.I.D.S_x64` artifact |
| `release.yaml` | Reusable | Zips artifact, creates **draft** GitHub release |
| `codeql.yml` | Push/PR to `develop`, weekly | CodeQL C++ analysis |
| `scorecards.yml` | Branch protection, weekly | OpenSSF Scorecards |

No test or lint step runs in CI.

## Architecture notes

- **Entrypoint**: `src/main.cpp` — init Boost.Log, load `config.cfg` as JSON, instantiate `RABIDS` (extends `SleepyDiscord::DiscordClient`), call `startClient()`.
- **Config self-mutation**: `SetRestartInterval()` writes back to `config.cfg` on disk at runtime.
- **SyncEngine** (`include/syncEngine.h`, `src/syncEngine.cpp`) — replaces direct file access to Foundry VTT data. Reads/writes bridge JSON files in `BridgeDir` and enqueues commands for the Foundry module.
- **Foundry module** (`modules/rabids-bridge/`) — JS module for Foundry VTT v12+, syncs LevelDB → JSON files on world load and via hooks. Processes registration commands from the bridge directory.
- **No direct LevelDB/SQLite access to Foundry data**. All C++ ↔ Foundry communication goes through JSON files in `BridgeDir` (default: `./data/bridge/`).
- **Russian locale**: All message strings in the example config are Russian. A `TranslitRusEng` function handles Cyrillic → Latin output in downtime tables.
- **SonarCloud**: Configured in `sonar-project.properties` (project key `SteamDragon_rabids`). Sources: `src,include`. Exclusions: `sqlite3.{c,h}`, `stdiohandler.h`, `VariadicTable.h`.
- **OS support**: Linux primary (CI). Windows via VS with Ninja + MSVC (`CMakeSettings.json` points to `boost_1_78_0`).
- **Общение с пользователем — на русском**. Комментарии в коде и документация — на английском.

## 🚫 Git & reporting

- **NEVER** commit, push, or create PRs on your own.
- Summarize changes before asking for confirmation. Explain scope and side effects.
- If stuck after 2 attempts, stop and propose alternatives.
