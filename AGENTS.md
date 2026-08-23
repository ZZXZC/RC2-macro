# RC2 Fishing Macro

External C++ auto-fishing macro for Roblox Refinery Caves 2. Pure Win32, no external dependencies.

## Build

```bash
# MinGW/GCC
g++ -O2 -Wall -std=c++17 -DUNICODE -D_UNICODE -o rc2fish.exe *.cpp -lgdi32 -luser32 -lgdiplus -lcomctl32 -mwindows
```

Or use the Makefile: `make`

## Fishing Cycle

```
1. M1 click to cast on water
2. Scan for perfect-cast pixel turning green (#0af809) -> click to release at max power
3. Wait for bar minigame to appear
4. MINIGAME: white bar moves across horizontal bar -> click when centered on blue hitbar
5. Minigame complete -> click to dismiss popup -> wait Dismiss Delay -> click to recast
6. Repeat from step 2
```

## Detection

- **Cast**: Monitor pixel for `#0af809` (green), tolerance 10
- **Bar**: Thin horizontal strip, scan for `#628aef` (blue bar) and `#ffffff` (white line)
- **Hit zone**: White line within 35% of blue bar center triggers click
- **End detection**: Bar gone for 1.25s = minigame complete

## Hotkeys

| Key | Action |
|-----|--------|
| F1 | Start/Stop |
| F2 | Select region (drag strip around bar) |
| F3 | Exit |
| F4 | Pause/Resume |
| F5 | Calibrate colors |

## File Structure

| File | Purpose |
|------|---------|
| `config.h` | Settings struct, hex/rgb helpers |
| `resource.h` | Win32 control IDs |
| `screen.h/.cpp` | Window capture, pixel reading |
| `detector.h/.cpp` | Color detection for cast + bar |
| `input.h/.cpp` | SendInput mouse/keyboard |
| `selector.h/.cpp` | Fullscreen overlay for region selection |
| `calibrator.h/.cpp` | Auto-detect bar/line colors |
| `fisher.h/.cpp` | State machine |
| `window.h/.cpp` | Win32 GUI with tabs |
| `main.cpp` | Entry point, hotkeys |
| `Makefile` | Build script |

## Key Settings (GUI configurable)

| Setting | Default | Description |
|---------|---------|-------------|
| Bar Color | `#628aef` | Blue hitbar color |
| Target Line | `#ffffff` | White moving line |
| Tolerance | 30 | RGB tolerance |
| Scan Every | 5ms | Minigame scan interval |
| Click Cooldown | 50ms | Min time between clicks |
| Center Window | 35% | Hit zone as % of bar width |
| End Wait | 1.25s | How long bar must disappear |
| Cast Color | `#0af809` | Perfect-cast pixel color |
| Recast Delay | 0.75s | Delay before next cast |
| Dismiss Delay | 0.25s | Delay between dismiss and recast |

## Notes

- External macro only (no DLL injection, no memory reading)
- Region selection adapts to any resolution
- Calibrate (F5) auto-detects colors from selected region
- Requires Roblox window to be visible and not minimized
