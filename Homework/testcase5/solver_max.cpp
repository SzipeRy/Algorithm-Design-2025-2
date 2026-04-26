#pragma GCC optimize("O3,unroll-loops")
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>

using namespace std;

// ==========================================
const double MAX_RUNTIME_SECONDS = 300.0; 
const int NUM_THREADS = 10; // Optimized for Ryzen 5 5600X
// ==========================================

int N, NN;
vector<int> initial_grid;
vector<int> target_grid;
int initial_blank_r, initial_blank_c;

int dr[] = {-1, 1, 0, 0};
int dc[] = {0, 0, -1, 1};
char move_char[] = {'D', 'U', 'R', 'L'};

// Global variables to store the absolute best answer across all threads
string global_best_answer = "";
int global_min_length = 2e9;
mutex ans_mutex; // Protects the global answer from data races

// Wrap the solver in a Class so every thread gets its own private memory and RNG
class SolverWorker {
public:
    mt19937 local_rng;
    int* tile_epoch_arr;
    int* tile_dist;
    int* tile_parent;
    int* blank_epoch_arr;
    int* blank_parent;
    int* blank_move;
    int* blank_heuristic_dist;
    int current_epoch = 0;
    int current_blank_epoch = 0;

    SolverWorker(int seed) {
        local_rng.seed(seed);
        tile_epoch_arr = new int[NN]();
        tile_dist = new int[NN]();
        tile_parent = new int[NN]();
        blank_epoch_arr = new int[NN]();
        blank_parent = new int[NN]();
        blank_move = new int[NN]();
        blank_heuristic_dist = new int[NN]();
    }

    ~SolverWorker() {
        delete[] tile_epoch_arr;
        delete[] tile_dist;
        delete[] tile_parent;
        delete[] blank_epoch_arr;
        delete[] blank_parent;
        delete[] blank_move;
        delete[] blank_heuristic_dist;
    }

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

    void calc_blank_distances(int br, int bc, const vector<bool>& locked) {
        current_blank_epoch++;
        queue<int> q;
        int start = br * N + bc;
        q.push(start);
        blank_epoch_arr[start] = current_blank_epoch;
        blank_heuristic_dist[start] = 0;

        while(!q.empty()) {
            int u = q.front(); q.pop();
            int r = u / N, c = u % N;
            for(int i = 0; i < 4; ++i) {
                int nr = r + dr[i], nc = c + dc[i];
                if (nr >= 0 && nr < N && nc >= 0 && nc < N && !locked[nr * N + nc]) {
                    int v = nr * N + nc;
                    if (blank_epoch_arr[v] != current_blank_epoch) {
                        blank_epoch_arr[v] = current_blank_epoch;
                        blank_heuristic_dist[v] = blank_heuristic_dist[u] + 1;
                        q.push(v);
                    }
                }
            }
        }
    }

    string simulate_order(const vector<int>& order) {
        vector<int> grid = initial_grid;
        vector<bool> locked(NN, false);
        int br = initial_blank_r, bc = initial_blank_c;
        string ans = "";
        ans.reserve(400000); 

        for (int dest_idx : order) {
            int target_val = target_grid[dest_idx];
            if (grid[dest_idx] == target_val) {
                locked[dest_idx] = true;
                continue;
            }

            calc_blank_distances(br, bc, locked);

            current_epoch++;
            queue<int> q;
            q.push(dest_idx);
            tile_epoch_arr[dest_idx] = current_epoch;
            tile_dist[dest_idx] = 0;

            int min_cost = 2e9;
            vector<int> candidates;

            while(!q.empty()) {
                int u = q.front(); q.pop();
                
                if (grid[u] == target_val && !locked[u]) {
                    int b_dist = (blank_epoch_arr[u] == current_blank_epoch) ? blank_heuristic_dist[u] : 1e9;
                    int cost = (tile_dist[u] * 3) + b_dist; 
                    
                    if (cost < min_cost) {
                        min_cost = cost;
                        candidates.clear();
                        candidates.push_back(u);
                    } else if (cost == min_cost) {
                        candidates.push_back(u);
                    }
                }

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
            int chosen_cand = candidates[local_rng() % candidates.size()];

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
                shuffle(dirs, dirs + 4, local_rng);

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
                    shuffle(dirs, dirs + 4, local_rng);

                    for(int i : dirs) {
                        int nr = r + dr[i], nc = c + dc[i];
                        if (nr >= 0 && nr < N && nc >= 0 && nc < N && !locked[nr * N + nc]) {
                            int v = nr * N + nc;
                            if (v == curr_t) continue; 

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

    vector<int> get_strategy(int type) {
        vector<int> strat;
        if (type == 0) {
            for (int r = 1; r < N - 1; r++) {
                if (local_rng() % 2) for (int c = 1; c < N - 1; c++) strat.push_back(r * N + c);
                else for (int c = N - 2; c >= 1; c--) strat.push_back(r * N + c);
            }
        } else if (type == 1) {
            for (int c = 1; c < N - 1; c++) {
                if (local_rng() % 2) for (int r = 1; r < N - 1; r++) strat.push_back(r * N + c);
                else for (int r = N - 2; r >= 1; r--) strat.push_back(r * N + c);
            }
        } else if (type == 2) { 
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
        } else {
            for (int r = 1; r < N - 1; r++) {
                vector<int> row;
                for (int c = 1; c < N - 1; c++) row.push_back(r * N + c);
                shuffle(row.begin(), row.end(), local_rng);
                strat.insert(strat.end(), row.begin(), row.end());
            }
        }
        return strat;
    }
};

// This function acts as the "Brain" for each individual thread
void thread_task(int thread_id, std::chrono::time_point<std::chrono::high_resolution_clock> start_time) {    // Give each thread a highly unique seed so they don't test the same paths
    SolverWorker worker(start_time.time_since_epoch().count() + thread_id * 19937);

    while (chrono::duration<double>(chrono::high_resolution_clock::now() - start_time).count() < MAX_RUNTIME_SECONDS) {
        int strat_type = worker.local_rng() % 4; 
        string res = worker.simulate_order(worker.get_strategy(strat_type));
        
        if (res != "FAILED") {
            // CRITICAL: We must lock the mutex before reading or writing to the global variables
            lock_guard<mutex> lock(ans_mutex); 
            if ((int)res.length() < global_min_length) {
                global_min_length = res.length();
                global_best_answer = res;
            }
        }
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

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

    auto start_time = chrono::high_resolution_clock::now();

    // 1. Launch all 12 threads
    vector<thread> threads;
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(thread_task, i, start_time);
    }

    // 2. Wait for all 12 threads to finish their work
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i].join();
    }

    // 3. Print the absolute best answer found by the 12-core collective
    cout << global_best_answer << "\n";
    cout << global_best_answer.length() << "\n";
    
    return 0;
}