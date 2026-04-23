#pragma GCC optimize("O3,unroll-loops")
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

int N, NN;
vector<int> initial_grid;
vector<int> target_grid;
int initial_blank_r, initial_blank_c;

// Directions for Blank: UP, DOWN, LEFT, RIGHT
int dr[] = {-1, 1, 0, 0};
int dc[] = {0, 0, -1, 1};
char move_char[] = {'D', 'U', 'R', 'L'};

// Global RNG engine initialized with time
mt19937 global_rng;

// Dynamic memory pointers to handle N=67 safely
int* tile_epoch_arr;
int* tile_dist;
int* tile_parent;
int* blank_epoch_arr;
int* blank_parent;
int* blank_move;
int current_epoch = 0;

// O(L) Stack-Based String Optimizer
void optimize_string(string& s) {
    string res;
    res.reserve(s.size());
    for (char c : s) {
        if (!res.empty()) {
            char last = res.back();
            if ((last == 'U' && c == 'D') || (last == 'D' && c == 'U') ||
                (last == 'L' && c == 'R') || (last == 'R' && c == 'L')) {
                res.pop_back();
                continue;
            }
        }
        res.push_back(c);
    }
    s = res;
}

string simulate_order(const vector<int>& order) {
    vector<int> grid = initial_grid;
    vector<bool> locked(NN, false);
    int br = initial_blank_r, bc = initial_blank_c;
    string ans = "";
    ans.reserve(300000); // Massive pre-allocation for N=67 to prevent slowdowns

    for (int dest_idx : order) {
        int target_val = target_grid[dest_idx];
        if (grid[dest_idx] == target_val) {
            locked[dest_idx] = true;
            continue;
        }

        int dest_r = dest_idx / N, dest_c = dest_idx % N;

        // 1. Find nearest candidate tile
        current_epoch++;
        queue<int> q;
        q.push(dest_idx);
        tile_epoch_arr[dest_idx] = current_epoch;
        tile_dist[dest_idx] = 0;

        int min_dist = 1e9;
        vector<int> candidates;

        while(!q.empty()) {
            int u = q.front(); q.pop();
            if (grid[u] == target_val && !locked[u]) {
                if (tile_dist[u] < min_dist) {
                    min_dist = tile_dist[u];
                    candidates.clear();
                    candidates.push_back(u);
                } else if (tile_dist[u] == min_dist) {
                    candidates.push_back(u);
                }
            }
            if (tile_dist[u] > min_dist) continue; // Prune search

            int r = u / N, c = u % N;
            for(int i = 0; i < 4; ++i) {
                int nr = r + dr[i], nc = c + dc[i];
                if (nr >= 0 && nr < N && nc >= 0 && nc < N && !locked[nr * N + nc]) {
                    int v = nr * N + nc;
                    if (tile_epoch_arr[v] != current_epoch) {
                        tile_epoch_arr[v] = current_epoch;
                        tile_dist[v] = tile_dist[u] + 1;
                        q.push(v);
                    }
                }
            }
        }

        if (candidates.empty()) return "FAILED";
        
        // Pick a random tie-breaker candidate to enforce deep randomization
        int chosen_cand = candidates[global_rng() % candidates.size()];

        // 2. Generate a randomized shortest path for the tile to its destination
        current_epoch++;
        q.push(chosen_cand);
        tile_epoch_arr[chosen_cand] = current_epoch;
        tile_parent[chosen_cand] = -1;

        int goal = -1;
        while(!q.empty()) {
            int u = q.front(); q.pop();
            if (u == dest_idx) { goal = u; break; }

            int r = u / N, c = u % N;
            int dirs[4] = {0, 1, 2, 3};
            shuffle(dirs, dirs + 4, global_rng);

            for(int i : dirs) {
                int nr = r + dr[i], nc = c + dc[i];
                if (nr >= 0 && nr < N && nc >= 0 && nc < N && !locked[nr * N + nc]) {
                    int v = nr * N + nc;
                    if (tile_epoch_arr[v] != current_epoch) {
                        tile_epoch_arr[v] = current_epoch;
                        tile_parent[v] = u;
                        q.push(v);
                    }
                }
            }
        }

        if (goal == -1) return "FAILED";

        vector<int> t_path;
        int curr = dest_idx;
        while(curr != chosen_cand) {
            t_path.push_back(curr);
            curr = tile_parent[curr];
        }
        reverse(t_path.begin(), t_path.end());

        // 3. Move blank to guide the tile safely step-by-step
        int curr_t = chosen_cand;
        for (int next_t : t_path) {
            current_epoch++;
            queue<int> bq;
            int b_start = br * N + bc;
            bq.push(b_start);
            blank_epoch_arr[b_start] = current_epoch;
            blank_parent[b_start] = -1;
            blank_move[b_start] = -1;

            bool found = false;
            while(!bq.empty()) {
                int u = bq.front(); bq.pop();
                if (u == next_t) { found = true; break; }

                int r = u / N, c = u % N;
                int dirs[4] = {0, 1, 2, 3};
                shuffle(dirs, dirs + 4, global_rng);

                for(int i : dirs) {
                    int nr = r + dr[i], nc = c + dc[i];
                    if (nr >= 0 && nr < N && nc >= 0 && nc < N && !locked[nr * N + nc]) {
                        int v = nr * N + nc;
                        if (v == curr_t) continue; // Blank avoids disrupting the tile it's protecting

                        if (blank_epoch_arr[v] != current_epoch) {
                            blank_epoch_arr[v] = current_epoch;
                            blank_parent[v] = u;
                            blank_move[v] = i;
                            bq.push(v);
                        }
                    }
                }
            }

            if (!found) return "FAILED";

            vector<int> b_moves;
            int b_curr = next_t;
            while(b_curr != b_start) {
                b_moves.push_back(blank_move[b_curr]);
                b_curr = blank_parent[b_curr];
            }
            reverse(b_moves.begin(), b_moves.end());

            for (int m : b_moves) {
                ans += move_char[m];
                int nbr = br + dr[m], nbc = bc + dc[m];
                swap(grid[br * N + bc], grid[nbr * N + nbc]);
                br = nbr; bc = nbc;
            }

            int swap_dir = -1;
            int cr = curr_t / N, cc = curr_t % N;
            for (int i = 0; i < 4; i++) {
                if (br + dr[i] == cr && bc + dc[i] == cc) {
                    swap_dir = i; break;
                }
            }
            ans += move_char[swap_dir];
            swap(grid[br * N + bc], grid[cr * N + cc]);
            br = cr; bc = cc;
            curr_t = next_t;
        }
        locked[dest_idx] = true;
    }
    optimize_string(ans);
    return ans + "S";
}

// 7 Uniquely randomized topological mapping strategies to continuously compress constraints
vector<int> get_strategy(int type) {
    vector<int> strat;
    if (type == 0) { // Top-to-Bottom
        for (int r = 1; r < N - 1; r++) {
            if (global_rng() % 2) for (int c = 1; c < N - 1; c++) strat.push_back(r * N + c);
            else for (int c = N - 2; c >= 1; c--) strat.push_back(r * N + c);
        }
    } else if (type == 1) { // Bottom-to-Top
        for (int r = N - 2; r >= 1; r--) {
            if (global_rng() % 2) for (int c = 1; c < N - 1; c++) strat.push_back(r * N + c);
            else for (int c = N - 2; c >= 1; c--) strat.push_back(r * N + c);
        }
    } else if (type == 2) { // Left-to-Right (Columns)
        for (int c = 1; c < N - 1; c++) {
            if (global_rng() % 2) for (int r = 1; r < N - 1; r++) strat.push_back(r * N + c);
            else for (int r = N - 2; r >= 1; r--) strat.push_back(r * N + c);
        }
    } else if (type == 3) { // Right-to-Left (Columns)
        for (int c = N - 2; c >= 1; c--) {
            if (global_rng() % 2) for (int r = 1; r < N - 1; r++) strat.push_back(r * N + c);
            else for (int r = N - 2; r >= 1; r--) strat.push_back(r * N + c);
        }
    } else if (type == 4) { // Inward Spiral (Edges to Center)
        int top = 1, bottom = N - 2, left = 1, right = N - 2;
        while (top <= bottom && left <= right) {
            for (int c = left; c <= right; c++) strat.push_back(top * N + c);
            top++;
            for (int r = top; r <= bottom; r++) strat.push_back(r * N + right);
            right--;
            if (top <= bottom) {
                for (int c = right; c >= left; c--) strat.push_back(bottom * N + c);
                bottom--;
            }
            if (left <= right) {
                for (int r = bottom; r >= top; r--) strat.push_back(r * N + left);
                left++;
            }
        }
    } else if (type == 5) { // Reverse Inward Spiral (Center to Edges - highly effective on some seeds)
        int top = 1, bottom = N - 2, left = 1, right = N - 2;
        vector<int> temp;
        while (top <= bottom && left <= right) {
            for (int c = left; c <= right; c++) temp.push_back(top * N + c);
            top++;
            for (int r = top; r <= bottom; r++) temp.push_back(r * N + right);
            right--;
            if (top <= bottom) {
                for (int c = right; c >= left; c--) temp.push_back(bottom * N + c);
                bottom--;
            }
            if (left <= right) {
                for (int r = bottom; r >= top; r--) temp.push_back(r * N + left);
                left++;
            }
        }
        reverse(temp.begin(), temp.end());
        strat = temp;
    } else { // Pure Random Zig-Zag blocks
        for (int r = 1; r < N - 1; r++) {
            vector<int> row;
            for (int c = 1; c < N - 1; c++) row.push_back(r * N + c);
            shuffle(row.begin(), row.end(), global_rng);
            strat.insert(strat.end(), row.begin(), row.end());
        }
    }
    return strat;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    auto start_time = chrono::high_resolution_clock::now();
    global_rng.seed(start_time.time_since_epoch().count());

    if (!(cin >> N)) return 0;
    NN = N * N;

    initial_grid.assign(NN, 0);
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            cin >> initial_grid[r * N + c];
            if (initial_grid[r * N + c] == -1) {
                initial_blank_r = r; initial_blank_c = c;
            }
        }
    }

    target_grid.assign(NN, -1);
    for (int r = 0; r < N - 2; r++) {
        for (int c = 0; c < N - 2; c++) {
            cin >> target_grid[(r + 1) * N + (c + 1)];
        }
    }

    // Allocate 1D Arrays globally to prevent heap overflow and speed up execution Memory cache
    tile_epoch_arr = new int[NN]();
    tile_dist = new int[NN]();
    tile_parent = new int[NN]();
    blank_epoch_arr = new int[NN]();
    blank_parent = new int[NN]();
    blank_move = new int[NN]();

    string best_answer = "";
    int min_length = 2e9;

    // Use full 1.85 seconds to run the ultra-fast decoupled permutations
    while (chrono::duration<double>(chrono::high_resolution_clock::now() - start_time).count() < 1800) {
        int strat_type = global_rng() % 7; // Select from 7 strategies
        string res = simulate_order(get_strategy(strat_type));
        
        if (res != "FAILED" && (int)res.length() < min_length) {
            min_length = res.length();
            best_answer = res;
        }
    }

    cout << best_answer << "\n";
    cout << best_answer.length() << "\n";
    
    delete[] tile_epoch_arr;
    delete[] tile_dist;
    delete[] tile_parent;
    delete[] blank_epoch_arr;
    delete[] blank_parent;
    delete[] blank_move;
    
    return 0;
}