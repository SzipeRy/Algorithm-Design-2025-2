#pragma GCC optimize("O3,unroll-loops")
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

int N;
vector<vector<int>> initial_grid;
vector<vector<int>> target;
int initial_blank_r, initial_blank_c;

// Directions for Blank: UP, DOWN, LEFT, RIGHT
int dr[] = {-1, 1, 0, 0};
int dc[] = {0, 0, -1, 1};
// Corresponding Tile apparent movements: DOWN, UP, RIGHT, LEFT
char move_char[] = {'D', 'U', 'R', 'L'};

// Global RNG engine initialized with time
mt19937 global_rng;

// Dynamic memory pointers to handle any N safely without crashing
int* dist_arr;
int* parent_arr;
int* parent_move_arr;
int* epoch_arr;
int current_bfs_epoch = 0;

// Helper to safely encode/decode the joint state of the Tile and Blank space
inline int encode_state(int tr, int tc, int br, int bc) {
    return (tr * N + tc) * (N * N) + (br * N + bc);
}

inline void decode_state(int state, int& tr, int& tc, int& br, int& bc) {
    int NN = N * N;
    int blank = state % NN;
    br = blank / N; bc = blank % N;
    int tile = state / NN;
    tr = tile / N; tc = tile % N;
}

// Removes any redundant back-and-forth wiggles to heavily optimize move count
void optimize_string(string& s) {
    bool changed = true;
    while(changed) {
        changed = false;
        for (int i = 0; i < (int)s.length() - 1; ) {
            if ((s[i] == 'U' && s[i+1] == 'D') ||
                (s[i] == 'D' && s[i+1] == 'U') ||
                (s[i] == 'L' && s[i+1] == 'R') ||
                (s[i] == 'R' && s[i+1] == 'L')) {
                s.erase(i, 2);
                changed = true;
                if (i > 0) i--; 
            } else {
                i++;
            }
        }
    }
}

string simulate_order(const vector<pair<int, int>>& order) {
    vector<vector<int>> grid = initial_grid;
    vector<vector<bool>> locked(N, vector<bool>(N, false));
    int br = initial_blank_r, bc = initial_blank_c;
    string ans = "";

    for (auto [dest_r, dest_c] : order) {
        int target_val = target[dest_r - 1][dest_c - 1];

        // If a correct tile is already here, lock it instantly
        if (grid[dest_r][dest_c] == target_val) {
            locked[dest_r][dest_c] = true;
            continue;
        }

        vector<pair<int, int>> candidates;
        for(int r=0; r<N; ++r){
            for(int c=0; c<N; ++c){
                if(grid[r][c] == target_val && !locked[r][c]){
                    candidates.push_back({r, c});
                }
            }
        }

        if (candidates.empty()) return "FAILED";

        int min_path_len = 1e9;
        vector<int> best_moves;
        
        // Shuffle candidates to add randomness on ties
        shuffle(candidates.begin(), candidates.end(), global_rng);

        // Joint-State BFS: Evaluates the shortest sequence to push the chosen tile
        for (auto cand : candidates) {
            int start_state = encode_state(cand.first, cand.second, br, bc);
            
            current_bfs_epoch++;
            queue<int> q;
            q.push(start_state);
            epoch_arr[start_state] = current_bfs_epoch;
            dist_arr[start_state] = 0;
            
            int goal_state = -1;
            
            while(!q.empty()) {
                int u = q.front(); q.pop();
                int ctr, ctc, cbr, cbc;
                decode_state(u, ctr, ctc, cbr, cbc);
                
                if (ctr == dest_r && ctc == dest_c) {
                    goal_state = u;
                    break;
                }
                
                // Randomize BFS branches to explore completely new valid paths
                int dirs[4] = {0, 1, 2, 3};
                shuffle(dirs, dirs + 4, global_rng);
                
                for(int i : dirs) {
                    int nbr = cbr + dr[i];
                    int nbc = cbc + dc[i];
                    
                    if(nbr >= 0 && nbr < N && nbc >= 0 && nbc < N && !locked[nbr][nbc]) {
                        int ntr = ctr, ntc = ctc;
                        
                        // If blank moves into the tile, they swap places
                        if (nbr == ctr && nbc == ctc) {
                            ntr = cbr; ntc = cbc;
                        }
                        
                        int v = encode_state(ntr, ntc, nbr, nbc);
                        if (epoch_arr[v] != current_bfs_epoch) {
                            epoch_arr[v] = current_bfs_epoch;
                            dist_arr[v] = dist_arr[u] + 1;
                            parent_arr[v] = u;
                            parent_move_arr[v] = i;
                            q.push(v);
                        }
                    }
                }
            }
            
            if (goal_state != -1 && dist_arr[goal_state] < min_path_len) {
                min_path_len = dist_arr[goal_state];
                
                vector<int> moves;
                int curr = goal_state;
                while(curr != start_state) {
                    moves.push_back(parent_move_arr[curr]);
                    curr = parent_arr[curr];
                }
                reverse(moves.begin(), moves.end());
                best_moves = moves;
            }
        }
        
        if (min_path_len == 1e9) return "FAILED";
        
        // Apply the mathematically shortest valid sequence of moves found
        for(int m : best_moves) {
            ans += move_char[m];
            int nbr = br + dr[m];
            int nbc = bc + dc[m];
            swap(grid[br][bc], grid[nbr][nbc]);
            br = nbr; bc = nbc;
        }
        
        locked[dest_r][dest_c] = true;
    }
    
    optimize_string(ans);
    return ans + "S";
}

int main() {
    // Fast I/O
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    // Seed the RNG using Time for perfectly unique randomized runs
    global_rng.seed(chrono::steady_clock::now().time_since_epoch().count());
    
    // Timer to track execution limit
    auto start_time = chrono::high_resolution_clock::now();

    if (!(cin >> N)) return 0;

    initial_grid.assign(N, vector<int>(N));
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            cin >> initial_grid[r][c];
            if (initial_grid[r][c] == -1) {
                initial_blank_r = r; initial_blank_c = c;
            }
        }
    }

    target.assign(N - 2, vector<int>(N - 2));
    for (int r = 0; r < N - 2; r++) {
        for (int c = 0; c < N - 2; c++) {
            cin >> target[r][c];
        }
    }

    // Allocate memory exactly proportional to N (Virtually 0 RAM footprint)
    int NN = N * N;
    dist_arr = new int[NN * NN];
    parent_arr = new int[NN * NN];
    parent_move_arr = new int[NN * NN];
    epoch_arr = new int[NN * NN]();

    // 8 Different Base Board Locking Strategies
    vector<vector<pair<int, int>>> strategies;
    vector<pair<int, int>> strat;

    for(int r=1; r<N-1; r++) for(int c=1; c<N-1; c++) strat.push_back({r, c});
    strategies.push_back(strat); strat.clear();
    for(int r=1; r<N-1; r++) for(int c=N-2; c>=1; c--) strat.push_back({r, c});
    strategies.push_back(strat); strat.clear();
    for(int r=N-2; r>=1; r--) for(int c=1; c<N-1; c++) strat.push_back({r, c});
    strategies.push_back(strat); strat.clear();
    for(int r=N-2; r>=1; r--) for(int c=N-2; c>=1; c--) strat.push_back({r, c});
    strategies.push_back(strat); strat.clear();
    for(int c=1; c<N-1; c++) for(int r=1; r<N-1; r++) strat.push_back({r, c});
    strategies.push_back(strat); strat.clear();
    for(int c=1; c<N-1; c++) for(int r=N-2; r>=1; r--) strat.push_back({r, c});
    strategies.push_back(strat); strat.clear();
    for(int c=N-2; c>=1; c--) for(int r=1; r<N-1; r++) strat.push_back({r, c});
    strategies.push_back(strat); strat.clear();
    for(int c=N-2; c>=1; c--) for(int r=N-2; r>=1; r--) strat.push_back({r, c});
    strategies.push_back(strat); strat.clear();

    string best_answer = "";
    int min_length = 2e9;

    // Guaranteed execution: First pass through the 8 basic strategies
    for (const auto& order : strategies) {
        string res = simulate_order(order);
        if (res != "FAILED" && (int)res.length() < min_length) {
            min_length = res.length();
            best_answer = res;
        }
    }

    // Randomization Loop: Re-evaluate strategies with shuffled tie-breaking and paths
    // Runs constantly until 1.8 seconds has passed to hunt for highly compressed moves
    while (chrono::duration<double>(chrono::high_resolution_clock::now() - start_time).count() < 1.8) {
        uniform_int_distribution<int> strat_dist(0, strategies.size() - 1);
        string res = simulate_order(strategies[strat_dist(global_rng)]);
        
        if (res != "FAILED" && (int)res.length() < min_length) {
            min_length = res.length();
            best_answer = res;
        }
    }

    // Simply Output the best string
    cout << best_answer << "\n";
    cout << best_answer.length() << "\n";
    
    // Clean up Memory safely
    delete[] dist_arr;
    delete[] parent_arr;
    delete[] parent_move_arr;
    delete[] epoch_arr;
    
    return 0;
}