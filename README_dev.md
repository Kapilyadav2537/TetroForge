# TetroForge: Developer Guide & Architecture Documentation

**A modular, layered Tetris-style puzzle game in C++17 with SFML**  
*Course: Object-Oriented Programming (CS203), Semester 3 | October 2026*  
*Author: Kapil Yadav (Roll No. 25/CS/191, Section A3), B.Tech CSE, Delhi Technological University*  

---

## Table of Contents
1. [Architectural Overview](#1-architectural-overview)
2. [Step-by-Step Development Journey](#2-step-by-step-development-journey)
   - [Phase 1: Math Primitives, Tetromino Hierarchy & Board Logic](#phase-1-math-primitives-tetromino-hierarchy--board-logic)
   - [Phase 2: Gameplay Orchestration, Scoring & Testing](#phase-2-gameplay-orchestration-scoring--testing)
   - [Phase 3: Presentation Layer, Screens & Input](#phase-3-presentation-layer-screens--input)
   - [Phase 4: Persistence, Procedural Audio & Retro Typography](#phase-4-persistence-procedural-audio--retro-typography)
3. [Deep Dive into Core Subsystems](#3-deep-dive-into-core-subsystems)
   - [Coordinate System and Math (Vec2i & Grid)](#coordinate-system-and-math-vec2i--grid)
   - [Tetromino Hierarchy & SRS Kick Tables](#tetromino-hierarchy--srs-kick-tables)
   - [Board Mechanics & Line Clearing](#board-mechanics--line-clearing)
   - [Game State Machine & PRNG](#game-state-machine--prng)
   - [Score Persistence & Error Handling](#score-persistence--error-handling)
4. [Deep Dive into Presentation Subsystems](#4-deep-dive-into-presentation-subsystems)
   - [Decoupled Rendering Interface (IRenderer)](#decoupled-rendering-interface-irenderer)
   - [Self-Contained 5x7 Arcade Typography](#self-contained-5x7-arcade-typography)
   - [Procedural 16-bit PCM Audio Synthesis](#procedural-16-bit-pcm-audio-synthesis)
   - [Polymorphic Screen Lifecycle](#polymorphic-screen-lifecycle)
5. [OOP Principles & Design Pattern Mapping](#5-oop-principles--design-pattern-mapping)
6. [Verification & Test Matrix](#6-verification--test-matrix)
7. [Build, Packaging & Distribution](#7-build-packaging--distribution)

---

## 1. Architectural Overview

TetroForge was engineered strictly following the **Single Responsibility Principle** and **Dependency Inversion**. The codebase is segregated into two completely decoupled layers:

```
+-------------------------------------------------------------------------------+
| PRESENTATION LAYER (SFML 2.5+)                                                |
|                                                                               |
|  +--------------------+   +-----------------------+   +--------------------+  |
|  |    Application     |-->|     IScreenHost       |<--|   MenuScreen       |  |
|  |  (Main Loop & Win) |   | (Transition Protocol) |   |   PlayScreen       |  |
|  +--------------------+   +-----------------------+   |   GameOverScreen   |  |
|             |                                         |   HighScoreScreen  |  |
|             v                                         +--------------------+  |
|  +--------------------+   +-----------------------+              |            |
|  |    SFMLRenderer    |-->|       IRenderer       |<-------------+            |
|  | (5x7 Pixel Engine) |   | (Rendering Interface) |                           |
|  +--------------------+   +-----------------------+                           |
|  +--------------------+   +-----------------------+                           |
|  |    SoundPlayer     |   |     InputHandler      |                           |
|  | (PCM Audio Synth)  |   | (Key -> Action Map)   |                           |
|  +--------------------+   +-----------------------+                           |
+-------------------------------------------------------------------------------+
                                       |
                                       | (Depends downward only)
                                       v
+-------------------------------------------------------------------------------+
| CORE LAYER (Pure Standard C++17, Zero GUI/Audio Dependencies)                 |
|                                                                               |
|  +--------------------+   +-----------------------+   +--------------------+  |
|  |       Game         |-->|         Board         |-->|     Grid<T,W,H>    |  |
|  |  (State & Scoring) |   | (Collision & Locking) |   | (Static Container) |  |
|  +--------------------+   +-----------------------+   +--------------------+  |
|        |         |                                                            |
|        |         +------------------------+                                   |
|        v                                  v                                   |
|  +--------------------+         +--------------------+                        |
|  |    ScoreManager    |         |    PieceFactory    |                        |
|  | (highscores.txt)   |         |  (Factory Method)  |                        |
|  +--------------------+         +--------------------+                        |
|                                           |                                   |
|                                           v                                   |
|                                 +--------------------+                        |
|                                 |     Tetromino      |                        |
|                                 |  (Abstract Base)   |                        |
|                                 +--------------------+                        |
|                                           |                                   |
|                 +-------+-------+-------+-+-+-------+-------+                 |
|                 |       |       |       |   |       |       |                 |
|                 v       v       v       v   v       v       v                 |
|               IPiece  OPiece  TPiece  SPiece ZPiece JPiece LPiece             |
+-------------------------------------------------------------------------------+
```

### Layering Boundary Enforced by Automated Testing
As mandated by **NFR-4**, the Core layer has zero knowledge of windows, graphics libraries, or audio hardware. This invariant is enforced by an automated CMake test (`CheckLayering.cmake`), which recursively scans every header and source file in `src/core/` and fails the test if any forbidden include (`SFML`, OS-specific headers, `imgui`) or non-deterministic clock/random device is detected.

---

## 2. Step-by-Step Development Journey

### Phase 1: Math Primitives, Tetromino Hierarchy & Board Logic
1. **Coordinate Geometry (`Vec2i.hpp`, `Vec2i.cpp`):**
   - Created a 2D integer vector struct with full operator overloading (`+`, `-`, unary `-`, `==`, `!=`, and stream `<<`).
   - Standardized on Cartesian coordinates where **`x` increases to the right** and **`y` increases upwards** (floor is `y = 0`). This directly aligns with the official Tetris Guideline SRS kick tables.
2. **Fixed-Size Grid Container (`Grid.hpp`):**
   - Implemented a templated `Grid<T, Width, Height>` container backed by `std::array<T, Width * Height>`.
   - Stack-allocated to eliminate runtime heap allocations in the board simulation.
   - Enforces bounds-checked element access with descriptive exceptions (`std::out_of_range`).
3. **Piece Hierarchy (`Tetromino.hpp`, `Pieces.hpp`, `Pieces.cpp`):**
   - Created the abstract base `Tetromino` defining the polymorphic interface: `cells(Rotation)`, `kicks(Rotation, Rotation)`, and `clone()`.
   - Implemented 7 concrete piece classes (`IPiece`, `OPiece`, `TPiece`, `SPiece`, `ZPiece`, `JPiece`, `LPiece`).
   - Derived rotations algorithmically from the spawn orientation at compile-time using the formula $(x, y) \to (y, \text{boxSize} - 1 - x)$.
   - Embedded full Super Rotation System (SRS) kick offset tables for all piece types.
4. **Factory & Prototype Patterns (`PieceFactory.hpp`, `PieceFactory.cpp`):**
   - Implemented `PieceFactory::create(PieceType)` returning `std::unique_ptr<Tetromino>`.
   - Equipped `Tetromino` with `clone()` to support deep polymorphic copies required by the ghost piece and testing fixtures.
5. **Board Physics & Line Clears (`Board.hpp`, `Board.cpp`):**
   - Configured the board to 10 visible columns by 40 total rows (20 visible + 20 hidden spawn rows).
   - Implemented collision detection (`canPlace`), locking (`lock`), and wall kick resolution (`tryRotate`).
   - Developed an in-place two-pointer row compaction algorithm for line clears (`clearLines`).

### Phase 2: Gameplay Orchestration, Scoring & Testing
1. **Game Engine (`Game.hpp`, `Game.cpp`):**
   - Engineered the deterministic game orchestrator using a 32-bit Linear Congruential Generator (LCG) PRNG (`seed = 0xC0FFEEU`).
   - Implemented the official Tetris Guideline **7-Bag Randomizer** using a Fisher-Yates shuffle over `kAllPieceTypes`, guaranteeing uniform piece distributions with zero piece starvation.
   - Added **Hold Piece** support (`Action::Hold`, mapped to `C` / `Shift`), allowing the player to swap the active piece into a hold queue once per piece drop.
   - Implemented **Counter-Clockwise Rotation** (`Action::RotateCCW`, mapped to `Z` / `LCtrl`) using full SRS kick tables.
   - Added gravity timing calculations: $\text{fallIntervalMs} = \max(100, 1000 - 90 \times (\text{level} - 1))$.
   - Implemented soft drop ($1 \text{ pt/cell}$) and hard drop ($2 \text{ pts/cell}$) scoring.
   - Implemented Guideline line clear scoring: $100 / 300 / 500 / 800 \times \text{level}$.
   - Added level progression: advances every 10 cleared lines ($\text{lines} / 10 + 1$).
   - Implemented real-time ghost piece projection.
2. **Comprehensive Catch2 Test Suite:**
   - Authored unit test suites (`test_vec2i.cpp`, `test_grid.cpp`, `test_pieces.cpp`, `test_kicks.cpp`, `test_board.cpp`, `test_game.cpp`).
   - Verified SRS test cases **TC-01** (wall kicks), **TC-02** (line clear compaction), **TC-03** (level 2 quadruple line clear scoring), **TC-04** (hard drop to ghost), **TC-05** (spawn collision game-over), **TC-06** (10-line level up), and **TC-08** (pause state immutability).
   - Validated 7-bag permutation integrity across multiple cycles, counter-clockwise rotation states, and hold piece swap/lock mechanics.

### Phase 3: Presentation Layer, Screens & Input
1. **Hardware-Accelerated Drawing (`IRenderer.hpp`, `SFMLRenderer.hpp`, `SFMLRenderer.cpp`):**
   - Defined `IRenderer` abstract interface for drawing board cells, pieces, ghost previews, HUD, and text.
   - Implemented `SFMLRenderer` targeting an `sf::RenderWindow` locked at 60 FPS.
2. **Polymorphic Screen Hierarchy (`Screen.hpp`, `MenuScreen`, `PlayScreen`, `GameOverScreen`, `HighScoreScreen`):**
   - Refactored presentation logic from a monolithic switch statement into discrete classes derived from `Screen`.
   - Implemented `IScreenHost` transition protocol enabling safe, deferred screen switches.
3. **Input Mapping (`InputHandler.hpp`, `InputHandler.cpp`):**
   - Decoupled SFML window events from game logic by translating raw keystrokes into `core::Action` enums.
   - Added dual input support: keyboard navigation (arrows/Enter/Space) and mouse hover/clicks across all menus.

### Phase 4: Persistence, Procedural Audio & Retro Typography
1. **High Score Management (`ScoreManager.hpp`, `ScoreManager.cpp`):**
   - Implemented file persistence targeting `highscores.txt` located next to the executable.
   - Created custom `ScoreFileError` derived from `std::runtime_error`.
   - Handled corrupt files, negative values, and non-integer tokens gracefully by falling back to an empty leaderboard without crashing (**TC-07**).
2. **Self-Contained 5x7 Arcade Typography:**
   - Designed a built-in mathematical 5x7 bitmap font matrix for all uppercase letters, digits, and punctuation.
   - Removed external TTF disk dependencies, guaranteeing zero startup crashes and zero missing-font warnings on any Linux/Windows system.
3. **Procedural 16-bit PCM Audio Synthesis (`SoundPlayer.hpp`, `SoundPlayer.cpp`):**
   - Synthesized real-time retro sound effects mathematically using trigonometric sine functions and exponential decay envelopes:
     - **Rotate:** 45 ms frequency chirp ($550 \text{ Hz} \to 850 \text{ Hz}$).
     - **Lock:** 70 ms solid bass thud ($180 \text{ Hz} \to 100 \text{ Hz}$).
     - **Line Clear:** 240 ms major chord arpeggio ($C_5, E_5, G_5, C_6$).
     - **Game Over:** 480 ms melancholic descending tones ($G_4, F_4, E\flat_4, C_4$).

---

## 3. Deep Dive into Core Subsystems

### Coordinate System and Math (`Vec2i` & `Grid`)
In Tetris Guideline SRS tables, kick vectors are documented with positive $y$ pointing upwards. Traditional graphics coordinate systems place $(0, 0)$ at the top-left with $y$ pointing downwards. To ensure exact conformity with the mathematical specification:
- Core uses Cartesian coordinates: $(0, 0)$ is the bottom-left of the board.
- The presentation renderer translates coordinates during drawing:
  $$\text{screenY} = \text{originY} + (\text{kVisibleHeight} - 1 - y) \times \text{cellSize}$$

```cpp
// Vec2i operator overloading example
[[nodiscard]] constexpr Vec2i operator+(Vec2i a, Vec2i b) noexcept {
    return Vec2i{a.x + b.x, a.y + b.y};
}
```

### Tetromino Hierarchy & SRS Kick Tables
Every piece defines a bounding box: $4 \times 4$ for `IPiece` and `OPiece`, and $3 \times 3$ for `TPiece`, `SPiece`, `ZPiece`, `JPiece`, `LPiece`.
Rotations are generated at compile time:
```cpp
constexpr CellArray rotateCwOnce(const CellArray& cells, int boxSize) noexcept {
    CellArray result{};
    for (std::size_t i = 0; i < cells.size(); ++i) {
        result[i] = Vec2i{cells[i].y, boxSize - 1 - cells[i].x};
    }
    return result;
}
```
When rotating, `Board::tryRotate` tests 5 kick offsets in succession. The first offset where all 4 piece cells remain free and within bounds is accepted. If all 5 collide, the rotation is safely rejected and the piece state remains unmodified.

### Board Mechanics & Line Clearing
The board maintains an internal grid of $10 \times 40$ cells. Line clearing uses a two-pointer compaction algorithm operating in $O(\text{height} \times \text{width})$ time and $O(1)$ extra memory:
```cpp
int Board::clearLines() {
    int write = 0;
    for (int read = 0; read < kTotalHeight; ++read) {
        if (isRowFull(read)) continue;
        if (write != read) copyRow(read, write);
        ++write;
    }
    const int cleared = kTotalHeight - write;
    for (int y = write; y < kTotalHeight; ++y) clearRow(y);
    return cleared;
}
```

### Game State Machine & PRNG
The game uses a Linear Congruential Generator:
$$X_{n+1} = (a X_n + c) \pmod{2^{32}}$$
where $a = 1664525$ and $c = 1013904223$.
Because `Game` accepts an initial seed, piece generation is completely deterministic. This guarantees reproducibility for regression tests and future replay verification.

### Score Persistence & Error Handling
Scores are stored in plain text, one integer per line, sorted descending.
If `highscores.txt` contains invalid tokens or corrupt entries:
1. `ScoreManager::load()` catches the parse failure and throws `ScoreFileError`.
2. The caller in `Application::run()` catches `ScoreFileError`, logs the event, and initializes an empty leaderboard.
3. The game never crashes under any file corruption scenario (**NFR-2, TC-07**).

---

## 4. Deep Dive into Presentation Subsystems

### Decoupled Rendering Interface (`IRenderer`)
`IRenderer` exposes pure virtual methods for rendering all visual elements. This enables mock renderers in unit tests or alternate backends (e.g. OpenGL, Vulkan, or Console ANSI) without altering a single line of game logic.

### Self-Contained 5x7 Arcade Typography
To eliminate external font file dependencies, `SFMLRenderer` embeds an ASCII glyph matrix where each character is defined as 7 bytes (one byte per row, 5 bits per row):
```
Character 'A':
Row 0: .###. (0x0E)
Row 1: #...# (0x11)
Row 2: #...# (0x11)
Row 3: ##### (0x1F)
Row 4: #...# (0x11)
Row 5: #...# (0x11)
Row 6: #...# (0x11)
```
Each active bit is rendered as a scaled `sf::RectangleShape`. This gives TetroForge an authentic, arcade-cabinet aesthetic that compiles and runs identically across Linux, Windows, and macOS.

### Procedural 16-bit PCM Audio Synthesis
`SoundPlayer` synthesizes mono audio buffers at $44,100 \text{ Hz}$ on startup using mathematical waveforms:
$$s(t) = A \cdot E(t) \cdot \sin(2\pi \cdot f(t) \cdot t)$$
where $E(t)$ is an exponential attack/decay envelope preventing speaker clicks, and $f(t)$ is a linear frequency ramp.
If `sfml-audio` is not installed on the host system, preprocessor guards (`TF_HAS_SFML_AUDIO`) gracefully stub out audio calls so the game compiles cleanly.

### Polymorphic Screen Lifecycle
Screen transitions use a deferred pointer swap to eliminate dangling-pointer crashes:
```cpp
// Inside Application main loop
while (window.pollEvent(event)) {
    currentScreen->handleEvent(event); // May call host.changeScreen(next)
}
if (nextScreen_) {
    currentScreen_ = std::move(nextScreen_); // Safely swapped between frames
}
currentScreen_->update(dt);
currentScreen_->draw(*renderer_);
```

---

## 5. OOP Principles & Design Pattern Mapping

| OOP Principle / Pattern | Concrete Implementation in TetroForge |
|---|---|
| **Encapsulation** | `Board::grid_` is private and accessible only via `canPlace` and `cellAt`; `Game` hides counters and exposes const getters; `ScoreManager` protects its vector. |
| **Abstraction** | Abstract bases `Tetromino`, `Screen`, and interface `IRenderer` define contracts without leaking implementation details. |
| **Inheritance** | 7 piece subclasses derived from `Tetromino`; 4 screen subclasses derived from `Screen`; `SFMLRenderer` derived from `IRenderer`. |
| **Runtime Polymorphism** | Virtual method dispatch on `Tetromino::cells()`, `Tetromino::kicks()`, `Screen::handleEvent()`, `Screen::update()`, and `Screen::draw()`. |
| **Composition** | `Game` owns `Board` and `Tetromino`; `Application` owns `sf::RenderWindow`, `IRenderer`, and `Screen`. |
| **Factory Method** | `PieceFactory::create(PieceType)` encapsulates piece instantiation logic. |
| **Prototype Pattern** | `Tetromino::clone()` produces polymorphic deep copies for ghost projection and move simulations. |
| **Operator Overloading** | `Vec2i` implements `+`, `-`, unary `-`, `==`, `!=`, and stream `<<`. |
| **Exception Handling** | Custom `ScoreFileError` derived from `std::runtime_error` protects against filesystem corruptions. |
| **Smart Pointers** | `std::unique_ptr` used exclusively for dynamic allocations. **Zero raw owning pointers** throughout the codebase. |

---

## 6. Verification & Test Matrix

All test cases specified in **SRS §7.2** and extended features (**SRS §10**) are validated via Catch2 v3:

| ID | Requirement | Scenario | Test File | Result |
|---|---|---|---|---|
| **TC-01** | FR-1.5 | Rotate I piece against right wall kicks inward without leaving board | `test_board.cpp` | **PASSED** |
| **TC-02** | FR-1.9 | Fill 9 columns of 4 rows, lock vertical I, clears all 4 rows and shifts stack down | `test_board.cpp` | **PASSED** |
| **TC-03** | FR-2.1 | Clear 4 rows at level 2 increases score by exactly 1600 ($800 \times 2$) | `test_game.cpp` | **PASSED** |
| **TC-04** | FR-1.6, FR-2.2 | Hard drop lands at ghost position and awards 2 points per dropped cell | `test_game.cpp` | **PASSED** |
| **TC-05** | FR-1.10 | Spawn piece onto filled spawn area immediately triggers `GameOver` | `test_game.cpp` | **PASSED** |
| **TC-06** | FR-2.3, FR-2.4 | Clear 10 lines reaches level 2 with fall interval 910 ms ($1000 - 90 \times 1$) | `test_game.cpp` | **PASSED** |
| **TC-07** | FR-4.2 | Load corrupt `highscores.txt` yields empty list without crashing | `test_scores.cpp` | **PASSED** |
| **TC-08** | FR-1.11 | Pausing game halts gravity tick and freezes piece position | `test_game.cpp` | **PASSED** |
| **7-Bag** | SRS §10 | 7-bag randomizer yields all 7 unique pieces in every 7-piece cycle | `test_game.cpp` | **PASSED** |
| **Rotate-CCW** | SRS §10 | Counter-clockwise rotation cycles through SRS orientations | `test_game.cpp` | **PASSED** |
| **Hold-Piece** | SRS §10 | Hold piece stores active, swaps correctly, blocks reuse, and resets on lock | `test_game.cpp` | **PASSED** |
| **Layering** | NFR-4 | Enforces Core has no forbidden headers (`SFML`, audio libraries, etc.) | `CheckLayering.cmake` | **PASSED** |

---

## 7. Build, Packaging & Distribution

### Building the Project

#### Linux (Ninja + GCC/Clang)
```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
./build/debug/src/tetroforge
```

#### Windows (Visual Studio / Developer PowerShell)
```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
.\build\src\Release\tetroforge.exe
```

### Creating the Clean Distribution Archive
To package ONLY the files strictly necessary to build and run the game along with `README.md` (no tests, no runtime files/folders):

#### Cross-Platform (Python)
```bash
python3 package_project.py
```
*(Packages `tetroforge/` containing `src/`, `cmake/ProjectOptions.cmake`, `CMakeLists.txt`, `CMakePresets.json`, and `README.md`. Also creates `tetroforge_bin.zip` if a compiled executable is present).*

#### Linux / macOS (Bash)
```bash
./package.sh
# Or directly:
zip -r tetroforge.zip CMakeLists.txt CMakePresets.json README.md cmake/ProjectOptions.cmake src/
```

