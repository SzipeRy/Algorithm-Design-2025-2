#pragma GCC optimize("O3,unroll-loops")
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <unordered_map>

using namespace std;

int N;
int initial_grid[5][5];
int target_grid[3][3];
int initial_blank_r, initial_blank_c;

// Mapped correctly for the Tile's Apparent Movement (Blank moves DOWN -> Tile moves UP)
int dr[] = {1, -1, 0, 0};
int dc[] = {0, 0, 1, -1};
char move_char[] = {'U', 'D', 'L', 'R'};

// --- GLOBAL SEARCH MEMORY (~2.4 GB RAM for extreme pruning) ---
const uint64_t TT_SIZE = 268435456; // 2^28 entries
uint64_t* tt_keys;
int8_t* tt_depth;

inline void tt_set(uint64_t key, int8_t depth_rem) {
    uint64_t idx = key & (TT_SIZE - 1);
    for(int i = 0; i < 4; i++) {
        uint64_t p = (idx + i) & (TT_SIZE - 1);
        if (tt_keys[p] == 0 || tt_keys[p] == key) {
            tt_keys[p] = key;
            tt_depth[p] = depth_rem;
            return;
        }
    }
    tt_keys[idx] = key;
    tt_depth[idx] = depth_rem;
}

inline int8_t tt_get(uint64_t key) {
    uint64_t idx = key & (TT_SIZE - 1);
    for(int i = 0; i < 4; i++) {
        uint64_t p = (idx + i) & (TT_SIZE - 1);
        if (tt_keys[p] == key) return tt_depth[p];
        if (tt_keys[p] == 0) return -128;
    }
    return -128;
}

uint64_t zobrist[25][25];
int board[25];
int blank_id;

// --- EXACT BIPARTITE MATCHING HEURISTIC ---
int target_pos[25][9];
int board_pos[25][25];
int pos_index[25];
int target_count[25];
int board_count[25];
int bm_cost[25];

// Computes the exact minimum Manhattan distance for a specific color
inline int calc_BM(int c) {
    int k = target_count[c];
    if (k == 0) return 0;
    int m = board_count[c];
    
    // O(1) Fast path for 1 target
    if (k == 1) {
        int min_d = 1000;
        int tr = target_pos[c][0] / 5, tc = target_pos[c][0] % 5;
        for (int i = 0; i < m; i++) {
            int p = board_pos[c][i];
            int d = abs(p / 5 - tr) + abs(p % 5 - tc);
            if (d < min_d) min_d = d;
        }
        return min_d;
    }
    
    // O(1) Fast path for 2 targets
    if (k == 2) {
        int min_d = 1000;
        int tr0 = target_pos[c][0] / 5, tc0 = target_pos[c][0] % 5;
        int tr1 = target_pos[c][1] / 5, tc1 = target_pos[c][1] % 5;
        for (int i = 0; i < m; i++) {
            int p0 = board_pos[c][i];
            int d00 = abs(p0 / 5 - tr0) + abs(p0 % 5 - tc0);
            int d01 = abs(p0 / 5 - tr1) + abs(p0 % 5 - tc1);
            for (int j = i + 1; j < m; j++) {
                int p1 = board_pos[c][j];
                int d10 = abs(p1 / 5 - tr0) + abs(p1 % 5 - tc0);
                int d11 = abs(p1 / 5 - tr1) + abs(p1 % 5 - tc1);
                int cost = d00 + d11;
                if (d01 + d10 < cost) cost = d01 + d10;
                if (cost < min_d) min_d = cost;
            }
        }
        return min_d;
    }
    
    // Exact Bitmask DP for K >= 3
    int dp[1 << k];
    for (int i = 0; i < (1 << k); i++) dp[i] = 1000;
    dp[0] = 0;
    
    for (int i = 0; i < m; i++) {
        int p = board_pos[c][i];
        int br = p / 5, bc = p % 5;
        for (int mask = (1 << k) - 1; mask >= 0; mask--) {
            if (dp[mask] == 1000) continue;
            for (int j = 0; j < k; j++) {
                if (!(mask & (1 << j))) {
                    int t = target_pos[c][j];
                    int d = abs(br - t / 5) + abs(bc - t % 5);
                    int nmask = mask | (1 << j);
                    if (dp[mask] + d < dp[nmask]) dp[nmask] = dp[mask] + d;
                }
            }
        }
    }
    return dp[(1 << k) - 1];
}

// --- IDA* ENGINE ---
bool ida_found = false;
vector<int> ida_path;

struct MoveInfo {
    int dir, npos, new_H, old_cost, new_cost;
    uint64_t hash_new;
};

int dfs(int blank_pos, int g, int limit, int last_move, int H, uint64_t hash) {
    int f = g + H;
    if (f > limit) return f;
    
    // Global optimum reached
    if (H == 0) {
        ida_found = true;
        return f;
    }

    int8_t depth_rem = limit - f;
    int8_t cached = tt_get(hash);
    if (cached >= depth_rem) return 1e9; 
    tt_set(hash, depth_rem);

    MoveInfo moves[4];
    int num_moves = 0;
    int r = blank_pos / 5, c = blank_pos % 5;

    for (int i = 0; i < 4; i++) {
        if (last_move != -1 && (last_move ^ 1) == i) continue; // Prevent direct Undo
        int nr = r + dr[i], nc = c + dc[i];

        if (nr >= 0 && nr < 5 && nc >= 0 && nc < 5) {
            int npos = nr * 5 + nc;
            int tile_id = board[npos];

            uint64_t new_hash = hash ^ zobrist[blank_pos][blank_id] ^ zobrist[npos][tile_id]
                                     ^ zobrist[blank_pos][tile_id] ^ zobrist[npos][blank_id];

            int new_H = H, old_cost = 0, new_cost = 0;

            if (tile_id != 0) {
                int idx = pos_index[npos];
                board_pos[tile_id][idx] = blank_pos; 
                
                old_cost = bm_cost[tile_id];
                new_cost = calc_BM(tile_id);
                new_H = H - old_cost + new_cost;
                
                board_pos[tile_id][idx] = npos; // Backtrack simulation
            }

            moves[num_moves++] = {i, npos, new_H, old_cost, new_cost, new_hash};
        }
    }

    // Sort to explore best branches first
    for(int i = 0; i < num_moves - 1; i++) {
        for(int j = i + 1; j < num_moves; j++) {
            if(moves[j].new_H < moves[i].new_H) swap(moves[i], moves[j]);
        }
    }

    int min_f = 1e9;
    for(int i = 0; i < num_moves; i++) {
        int dir = moves[i].dir, npos = moves[i].npos, tile_id = board[npos];

        // Apply Move
        board[blank_pos] = tile_id;
        board[npos] = blank_id;

        if (tile_id != 0) {
            int idx = pos_index[npos];
            board_pos[tile_id][idx] = blank_pos;
            pos_index[blank_pos] = idx;
            bm_cost[tile_id] = moves[i].new_cost;
        }

        ida_path.push_back(dir);

        // Search deeper
        int res = dfs(npos, g + 1, limit, dir, moves[i].new_H, moves[i].hash_new);

        if (ida_found) return res;
        min_f = min(min_f, res);

        // Undo Move
        ida_path.pop_back();

        if (tile_id != 0) {
            int idx = pos_index[blank_pos];
            board_pos[tile_id][idx] = npos;
            pos_index[npos] = idx;
            bm_cost[tile_id] = moves[i].old_cost;
        }

        board[npos] = tile_id;
        board[blank_pos] = blank_id;
    }
    return min_f;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    if (!(cin >> N)) return 0;

    for(int i = 0; i < 25; i++) {
        target_count[i] = 0;
        board_count[i] = 0;
    }

    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 5; c++) {
            cin >> initial_grid[r][c];
            if (initial_grid[r][c] == -1) {
                initial_blank_r = r; 
                initial_blank_c = c;
            }
        }
    }

    unordered_map<int, int> color_to_id;
    int id_counter = 1;

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            cin >> target_grid[r][c];
            if (color_to_id.find(target_grid[r][c]) == color_to_id.end()) {
                color_to_id[target_grid[r][c]] = id_counter++;
            }
            int id = color_to_id[target_grid[r][c]];
            int tr = r + 1, tc = c + 1;
            target_pos[id][target_count[id]++] = tr * 5 + tc;
        }
    }
    blank_id = id_counter;

    srand(1337);
    for(int i = 0; i < 25; i++) {
        for(int j = 0; j <= blank_id; j++) zobrist[i][j] = ((uint64_t)rand() << 32) ^ rand();
    }

    uint64_t start_hash = 0;
    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 5; c++) {
            int val = initial_grid[r][c];
            int idx = r * 5 + c;
            int id = 0;
            
            if (val == -1) id = blank_id;
            else if (color_to_id.find(val) != color_to_id.end()) id = color_to_id[val];
            
            board[idx] = id;
            if (id != blank_id && id != 0) {
                pos_index[idx] = board_count[id];
                board_pos[id][board_count[id]++] = idx;
            }
            start_hash ^= zobrist[idx][id];
        }
    }

    int initial_H = 0;
    for (int i = 1; i < id_counter; i++) {
        bm_cost[i] = calc_BM(i);
        initial_H += bm_cost[i];
    }

    if (initial_H == 0) {
        cout << "S\n";
        return 0;
    }

    // Allocate TT ~ 2.4 GB 
    tt_keys = new uint64_t[TT_SIZE]();
    tt_depth = new int8_t[TT_SIZE]();

    // Pure optimal run - No fallback - Will strictly find the absolute shortest string
    int limit = initial_H;
    while (!ida_found) {
        fill(tt_keys, tt_keys + TT_SIZE, 0); 
        int next_limit = dfs(initial_blank_r * 5 + initial_blank_c, 0, limit, -1, initial_H, start_hash);
        if (next_limit == 1e9) break; 
        limit = next_limit; 
    }

    string final_ans = "";
    if (ida_found) {
        for (int dir : ida_path) final_ans += move_char[dir];
        cout << final_ans << "S\n";
        cout << final_ans.length() << "\n";
    }

    delete[] tt_keys;
    delete[] tt_depth;
    return 0;
}