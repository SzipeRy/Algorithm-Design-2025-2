import sys
import time
import random
import heapq
import collections

# Global time tracker to ensure we never get killed by the grading system
global_start_time = time.time()

def read_input():
    data = sys.stdin.read().split()
    if not data: return None
    N = int(data[0])
    board = []
    blank_pos = -1
    idx = 1
    for i in range(N * N):
        val = int(data[idx])
        idx += 1
        board.append(val)
        if val == -1:
            blank_pos = i
            
    target = {}
    target_list = []
    for r in range(1, N - 1):
        for c in range(1, N - 1):
            val = int(data[idx])
            idx += 1
            pos = r * N + c
            target[pos] = val
            target_list.append((pos, val))
            
    return N, board, blank_pos, target, target_list

def optimize_string(s):
    """Removes redundant back-and-forth wiggles (e.g., Up then Down) to compress moves."""
    s_list = list(s)
    changed = True
    while changed:
        changed = False
        i = 0
        while i < len(s_list) - 1:
            if s_list[i:i+2] in (['U','D'], ['D','U'], ['L','R'], ['R','L']):
                del s_list[i:i+2]
                changed = True
                if i > 0: i -= 1
            else:
                i += 1
    return "".join(s_list)

def main():
    res = read_input()
    if not res: return
    N, board, blank_pos, target, target_list = res
    
    # Precompute valid moves for the blank space
    adj = [[] for _ in range(N * N)]
    for i in range(N * N):
        r, c = divmod(i, N)
        # Proper mapping: Blank moves Down -> Tile moves Up ('U')
        if r < N - 1: adj[i].append((i + N, 'U')) 
        if r > 0:     adj[i].append((i - N, 'D')) 
        if c < N - 1: adj[i].append((i + 1, 'L')) 
        if c > 0:     adj[i].append((i - 1, 'R')) 
        # Shuffle adjacency list to break ties differently across random runs
        random.shuffle(adj[i])
        
    def solve_astar(weight, time_limit):
        """Global A* Search: Finds the mathematically shortest path using Mismatches Heuristic."""
        start_board = tuple(board)
        start_h = sum(1 for p, c in target_list if start_board[p] != c)
        if start_h == 0: return ""
        
        pq = []
        heapq.heappush(pq, (start_h * weight, random.random(), 0, start_h, start_board, blank_pos, ""))
        visited = {start_board: 0}
        
        st = time.time()
        nodes = 0
        while pq:
            nodes += 1
            # Check time periodically to prevent timeout crashes
            if nodes % 4096 == 0 and time.time() - st > time_limit:
                return None
                
            f, _, g, h, b, b_pos, path = heapq.heappop(pq)
            
            if h == 0:
                return path
                
            if visited.get(b, float('inf')) < g:
                continue
                
            for n_pos, move_char in adj[b_pos]:
                new_b = list(b)
                tile = new_b[n_pos]
                new_b[b_pos], new_b[n_pos] = tile, new_b[b_pos]
                
                # Fast O(1) Heuristic Update
                new_h = h
                if b_pos in target and target[b_pos] == tile: new_h -= 1
                if n_pos in target and target[n_pos] == tile: new_h += 1
                    
                new_b = tuple(new_b)
                new_g = g + 1
                
                if new_g < visited.get(new_b, float('inf')):
                    visited[new_b] = new_g
                    new_f = new_g + new_h * weight
                    heapq.heappush(pq, (new_f, random.random(), new_g, new_h, new_b, n_pos, path + move_char))
        return None

    def route_tile(t_pos, b_pos, dest_pos, locked_positions):
        """Joint-State BFS: Safely pushes a single tile without trapping the blank space."""
        q = collections.deque([(t_pos, b_pos, "")])
        visited = {(t_pos, b_pos)}
        
        while q:
            curr_t, curr_b, path = q.popleft()
            if curr_t == dest_pos:
                return path, curr_b
                
            dirs = list(adj[curr_b])
            random.shuffle(dirs) # Randomize BFS branches to generate entirely different paths
            for n_b, move_char in dirs:
                if n_b in locked_positions and n_b != curr_t:
                    continue # Do not disrupt solved tiles
                n_t = curr_t
                if n_b == curr_t: n_t = curr_b # They swapped
                state = (n_t, n_b)
                if state not in visited:
                    visited.add(state)
                    q.append((n_t, n_b, path + move_char))
        return None, b_pos

    def solve_macro(order):
        """Builds the target board using a specific geometric strategy."""
        path = ""
        current_board = list(board)
        current_b = blank_pos
        current_locked = set()
        
        for dest in order:
            color = target[dest]
            if current_board[dest] == color:
                current_locked.add(dest)
                continue
                
            candidates = []
            for i in range(N * N):
                if current_board[i] == color and i not in current_locked:
                    r1, c1 = divmod(i, N)
                    r2, c2 = divmod(dest, N)
                    candidates.append((abs(r1-r2)+abs(c1-c2), i))
                    
            if not candidates: return None
            
            candidates.sort()
            min_dist = candidates[0][0]
            best_cands = [c[1] for c in candidates if c[0] == min_dist]
            chosen_t = random.choice(best_cands) # Randomly pick among identically close tiles
            
            res_path, new_b = route_tile(chosen_t, current_b, dest, current_locked)
            if res_path is None: return None
            
            for m in res_path:
                path += m
                for n_b, move_char in adj[current_b]:
                    if move_char == m:
                        current_board[current_b], current_board[n_b] = current_board[n_b], current_board[current_b]
                        current_b = n_b
                        break
            current_locked.add(dest)
        return path

    best_path = None
    min_len = float('inf')

    # PHASE 1: Pure Optimal Global Search (Will find exact 28 moves for testcase 1)
    ans = solve_astar(weight=1.0, time_limit=0.4)
    if ans is not None:
        best_path = ans
        min_len = len(ans)
    else:
        # PHASE 2: Weighted Global Search
        ans = solve_astar(weight=1.5, time_limit=0.4)
        if ans is not None:
            best_path = ans
            min_len = len(ans)

    # PHASE 3: Randomized Macro Solver (Fallback & Deep Optimizer)
    base_strategies = [
        [r * N + c for r in range(1, N-1) for c in range(1, N-1)],
        [r * N + c for r in range(1, N-1) for c in range(N-2, 0, -1)],
        [r * N + c for r in range(N-2, 0, -1) for c in range(1, N-1)],
        [r * N + c for r in range(N-2, 0, -1) for c in range(N-2, 0, -1)],
        [r * N + c for c in range(1, N-1) for r in range(1, N-1)],
        [r * N + c for c in range(1, N-1) for r in range(N-2, 0, -1)],
        [r * N + c for c in range(N-2, 0, -1) for r in range(1, N-1)],
        [r * N + c for c in range(N-2, 0, -1) for r in range(N-2, 0, -1)]
    ]

    # Guarantee at least one valid macro execution if A* failed (Crucial for N=7 or N=9)
    if best_path is None:
        for order in base_strategies:
            res = solve_macro(order)
            if res is not None:
                res = optimize_string(res)
                if len(res) < min_len:
                    min_len = len(res)
                    best_path = res

    # Run randomization continuously using up exactly 1.85 seconds of your CPU time limit
    # This acts like a Monte Carlo engine, hunting for the most compressed paths.
    while time.time() - global_start_time < 1.85:
        order = random.choice(base_strategies)
        res = solve_macro(order)
        if res is not None:
            res = optimize_string(res)
            if len(res) < min_len:
                min_len = len(res)
                best_path = res

    # Output your final result string correctly suffixed with 'S'
    if best_path is not None:
        print(best_path + "S")
        print(len(best_path))
    else:
        print("S")

if __name__ == '__main__':
    main()