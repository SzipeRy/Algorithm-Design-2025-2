from __future__ import annotations

import argparse
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import List, Tuple

import matplotlib.pyplot as plt
from matplotlib import patches
from matplotlib.widgets import Button, Slider

DIRS = {
    "U": (1, 0),
    "D": (-1, 0),
    "L": (0, 1),
    "R": (0, -1),
}


@dataclass
class ReplayFrame:
    board: List[List[int]]
    move: str
    move_count: int


def _copy_board(board: List[List[int]]) -> List[List[int]]:
    return [row[:] for row in board]


def _read_text(path: str) -> str:
    if path == "-":
        return sys.stdin.read()
    return Path(path).read_text(encoding="utf-8")


def _read_moves_file(path: str) -> str:
    text = Path(path).read_text(encoding="utf-8")
    tokens = text.split()
    return "".join(tokens)


def parse_problem(text: str) -> Tuple[int, List[List[int]], List[List[int]], str]:
    tokens = text.split()
    if not tokens:
        raise ValueError("Input is empty.")

    idx = 0

    def take_int() -> int:
        nonlocal idx
        if idx >= len(tokens):
            raise ValueError("Unexpected end of input while parsing integers.")
        try:
            value = int(tokens[idx])
        except ValueError as exc:
            raise ValueError(f"Expected integer, got '{tokens[idx]}'.") from exc
        idx += 1
        return value

    n = take_int()
    if n <= 0:
        raise ValueError("n must be positive.")

    board = [[take_int() for _ in range(n)] for _ in range(n)]

    m = n - 2
    if m < 0:
        raise ValueError("n must be at least 2.")
    expected = [[take_int() for _ in range(m)] for _ in range(m)]

    moves = ""
    if idx < len(tokens):
        moves = "".join(tokens[idx:])

    return n, board, expected, moves


def resolve_moves(cli_moves: str | None, moves_file: str | None, embedded_moves: str) -> str:
    if cli_moves is not None:
        return "".join(cli_moves.split()).upper()
    if moves_file is not None:
        return _read_moves_file(moves_file).upper()
    return embedded_moves.upper()


def find_blank(board: List[List[int]]) -> Tuple[int, int]:
    positions = [(r, c) for r, row in enumerate(board) for c, val in enumerate(row) if val == -1]
    if len(positions) != 1:
        raise ValueError(f"Expected exactly one blank (-1), found {len(positions)}.")
    return positions[0]


def simulate(board: List[List[int]], moves: str) -> List[ReplayFrame]:
    n = len(board)
    cur = _copy_board(board)
    blank_r, blank_c = find_blank(cur)

    frames: List[ReplayFrame] = [ReplayFrame(_copy_board(cur), "START", 0)]
    move_count = 0

    for i, move in enumerate(moves, start=1):
        if move == "S":
            frames.append(ReplayFrame(_copy_board(cur), move, move_count))
            continue
        if move not in DIRS:
            raise ValueError(f"Invalid move '{move}' at step {i}. Allowed: U D L R S.")

        dr, dc = DIRS[move]
        nr, nc = blank_r + dr, blank_c + dc
        if nr < 0 or nr >= n or nc < 0 or nc >= n:
            raise ValueError(
                f"Illegal move '{move}' at step {i}: blank at ({blank_r}, {blank_c}) "
                f"cannot swap with ({nr}, {nc})."
            )

        cur[blank_r][blank_c], cur[nr][nc] = cur[nr][nc], cur[blank_r][blank_c]
        blank_r, blank_c = nr, nc
        move_count += 1
        frames.append(ReplayFrame(_copy_board(cur), move, move_count))

    return frames


def submit_ok(board: List[List[int]], expected: List[List[int]]) -> bool:
    s = len(expected)
    n = len(board)
    start = (n - s) // 2
    for r in range(s):
        for c in range(s):
            if board[start + r][start + c] != expected[r][c]:
                return False
    return True


def target_cell_ok(board: List[List[int]], expected: List[List[int]], r: int, c: int) -> bool:
    s = len(expected)
    n = len(board)
    start = (n - s) // 2
    if r < start or r >= start + s or c < start or c >= start + s:
        return True
    return board[r][c] == expected[r - start][c - start]


class Visualizer:
    def __init__(self, frames: List[ReplayFrame], expected: List[List[int]], interval_ms: int) -> None:
        self.frames = frames
        self.expected = expected
        self.max_step = len(frames) - 1
        self.step = 0
        self.playing = False

        self.n = len(frames[0].board)
        self.s = len(expected)
        self.target_start = (self.n - self.s) // 2 if self.s > 0 else 0

        values = sorted({v for row in frames[0].board for v in row if v != -1})
        cmap = plt.cm.get_cmap("tab20", max(1, len(values)))
        self.value_color = {v: cmap(i) for i, v in enumerate(values)}

        self.fig, self.ax = plt.subplots(figsize=(7.5, 8.5))
        plt.subplots_adjust(left=0.08, right=0.92, top=0.9, bottom=0.22)

        slider_ax = self.fig.add_axes([0.12, 0.12, 0.76, 0.03])
        self.slider = Slider(
            ax=slider_ax,
            label="Step",
            valmin=0,
            valmax=self.max_step,
            valinit=0,
            valstep=1,
            valfmt="%0.0f",
        )
        self.slider.on_changed(self._on_slider)

        prev_ax = self.fig.add_axes([0.12, 0.05, 0.14, 0.05])
        play_ax = self.fig.add_axes([0.30, 0.05, 0.14, 0.05])
        next_ax = self.fig.add_axes([0.48, 0.05, 0.14, 0.05])

        self.btn_prev = Button(prev_ax, "Prev")
        self.btn_play = Button(play_ax, "Play")
        self.btn_next = Button(next_ax, "Next")

        self.btn_prev.on_clicked(self._on_prev)
        self.btn_play.on_clicked(self._on_play_pause)
        self.btn_next.on_clicked(self._on_next)

        self.info_text = self.fig.text(
            0.5,
            0.01,
            "Controls: Left/Right step, Space play/pause, Home start, End final",
            ha="center",
            va="bottom",
            fontsize=10,
        )

        self.fig.canvas.mpl_connect("key_press_event", self._on_key)

        self.timer = self.fig.canvas.new_timer(interval=max(40, interval_ms))
        self.timer.add_callback(self._on_timer)

        self._draw()

    def _draw(self) -> None:
        frame = self.frames[self.step]
        board = frame.board

        self.ax.clear()
        self.ax.set_xlim(0, self.n)
        self.ax.set_ylim(0, self.n)
        self.ax.invert_yaxis()
        self.ax.set_aspect("equal")
        self.ax.set_xticks(range(self.n + 1))
        self.ax.set_yticks(range(self.n + 1))
        self.ax.grid(which="both", color="#555555", linewidth=1)
        self.ax.set_xticklabels([])
        self.ax.set_yticklabels([])

        for r in range(self.n):
            for c in range(self.n):
                value = board[r][c]
                if value == -1:
                    face = "#f2f2f2"
                    txt = ""
                else:
                    face = self.value_color.get(value, "#dddddd")
                    txt = str(value)

                cell = patches.Rectangle((c, r), 1, 1, facecolor=face, edgecolor="#222222", linewidth=1.0)
                self.ax.add_patch(cell)
                self.ax.text(
                    c + 0.5,
                    r + 0.5,
                    txt,
                    ha="center",
                    va="center",
                    fontsize=12,
                    color="#111111",
                    fontweight="bold",
                )

                if self.s > 0 and self.target_start <= r < self.target_start + self.s and self.target_start <= c < self.target_start + self.s:
                    ok = target_cell_ok(board, self.expected, r, c)
                    border = "#2e7d32" if ok else "#c62828"
                    inner = patches.Rectangle(
                        (c + 0.08, r + 0.08),
                        0.84,
                        0.84,
                        facecolor="none",
                        edgecolor=border,
                        linewidth=2,
                    )
                    self.ax.add_patch(inner)

        if self.s > 0:
            outer = patches.Rectangle(
                (self.target_start, self.target_start),
                self.s,
                self.s,
                facecolor="none",
                edgecolor="#111111",
                linewidth=2.8,
            )
            self.ax.add_patch(outer)

        status = "CORRECT" if submit_ok(board, self.expected) else "WRONG"
        self.ax.set_title(
            f"Step {self.step}/{self.max_step} | Move: {frame.move} | Player move uses: {frame.move_count} | {status}",
            fontsize=12,
        )

        self.fig.canvas.draw_idle()

    def _set_step(self, step: int) -> None:
        step = max(0, min(self.max_step, step))
        self.slider.set_val(step)

    def _set_playing(self, value: bool) -> None:
        self.playing = value
        self.btn_play.label.set_text("Pause" if value else "Play")
        if value:
            self.timer.start()
        else:
            self.timer.stop()

    def _on_slider(self, value: float) -> None:
        new_step = int(value)
        if new_step == self.step:
            return
        self.step = new_step
        self._draw()

    def _on_prev(self, _event) -> None:
        self._set_playing(False)
        self._set_step(self.step - 1)

    def _on_next(self, _event) -> None:
        self._set_playing(False)
        self._set_step(self.step + 1)

    def _on_play_pause(self, _event) -> None:
        if self.step >= self.max_step:
            self._set_step(0)
        self._set_playing(not self.playing)

    def _on_timer(self) -> None:
        if not self.playing:
            return
        if self.step >= self.max_step:
            self._set_playing(False)
            return
        self._set_step(self.step + 1)

    def _on_key(self, event) -> None:
        if event.key in {"right", "n"}:
            self._set_playing(False)
            self._set_step(self.step + 1)
        elif event.key in {"left", "p"}:
            self._set_playing(False)
            self._set_step(self.step - 1)
        elif event.key == " ":
            if self.step >= self.max_step:
                self._set_step(0)
            self._set_playing(not self.playing)
        elif event.key == "home":
            self._set_playing(False)
            self._set_step(0)
        elif event.key == "end":
            self._set_playing(False)
            self._set_step(self.max_step)


def main() -> int:
    parser = argparse.ArgumentParser(description="Visualize board replay step-by-step using matplotlib.")
    parser.add_argument(
        "--input",
        "-i",
        default="-",
        help="Input file path with n, board, expected (and optional moves). Use '-' for stdin.",
    )
    parser.add_argument(
        "--moves",
        "-m",
        default=None,
        help="Move string (UDLRS). Overrides any move string embedded in input.",
    )
    parser.add_argument(
        "--moves-file",
        default=None,
        help="Read move string from a text file. Overrides embedded moves if --moves is not used.",
    )
    parser.add_argument(
        "--interval",
        type=int,
        default=500,
        help="Autoplay interval in milliseconds (default: 500).",
    )
    args = parser.parse_args()

    try:
        _, board, expected, embedded_moves = parse_problem(_read_text(args.input))
        moves = resolve_moves(args.moves, args.moves_file, embedded_moves)
        if not moves:
            raise ValueError("No moves found. Pass --moves, --moves-file, or append moves to input.")

        frames = simulate(board, moves)
    except Exception as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1

    final_status = "CORRECT" if submit_ok(frames[-1].board, expected) else "WRONG"
    print(f"Loaded {len(frames) - 1} steps. Final submit status: {final_status}.")

    Visualizer(frames, expected, interval_ms=args.interval)
    plt.show()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
