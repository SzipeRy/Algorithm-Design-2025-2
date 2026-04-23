# Algorithm Design 2025/2

## Compilation Command

```bash
g++ -std=c++17 -Wall -O2 player.cpp board.cpp -o player
```

## Visualizer (Matplotlib)

The repository now includes `visualizer.py` to replay moves step-by-step with
the same behavior as `player.cpp` and `board.cpp`.

### Run with moves from command line

```bash
python3 visualizer.py --input sample/sample.in --moves "ULLDRRS"
```

### Run with moves from a file

```bash
python3 visualizer.py --input testdata/1.in --moves-file moves.txt
```

### Run from stdin (same format as player input)

```bash
cat input_with_moves.txt | python3 visualizer.py
```

Controls in the window:

- Left/Right or P/N: previous/next step
- Space: play/pause autoplay
- Home/End: jump to first/last step

Notes:

- `S` is treated as a no-op, matching `player.cpp`.
- `U`, `D`, `L`, `R` follow the exact move semantics in `board.cpp`.
