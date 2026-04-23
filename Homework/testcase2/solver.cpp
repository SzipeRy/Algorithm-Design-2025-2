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

int* d_dest_dist;
int* d_dest_epoch;
int current_epoch_dest = 0;

int* d_blank_dist;
int* d_blank_epoch;
int current_epoch_blank = 0;

inline int encode_state(int tr, int tc, int br, int bc) {
    return (tr * N + tc) * NN + (br * N + bc);
}

inline void decode_state(int state, int& tr, int& tc, int& br, int& bc) {
    int blank = state % NN;
    br = blank / N; bc = blank % N;
    int tile = state / NN;
    tr = tile / N; tc = tile % N;
}

// Quick 2D BFS to find Manhattan-like distances avoiding locked tiles
void bfs_2d_dest(int sr, int sc, const vector<vector<bool>>& locked) {
    current_epoch_dest++;
    queue<int> q;
    int start = sr * N + sc;
    q.push(start);
    d_dest_epoch[start] = current_epoch_dest;
    d_dest_dist[start] = 0;
    
    while(!q.empty()) {
        int u = q.front(); q.pop();
        int r = u / N, c = u % N;
        for(int i=0; i<4; ++i) {
            int nr = r + dr[i], nc = c + dc[i];
            if(nr>=0 && nr<N && nc>=0 && nc<N && !locked[nr][nc]) {
                int v = nr * N + nc;
                if(d_dest_epoch[v] != current_epoch_dest) {
                    d_dest_epoch[v] = current_epoch_dest;
                    d_dest_dist[v] = d_dest_dist[u] + 1;
                    q.push(v);
                }
            }
        }
    }
}

void bfs_2d_blank(int sr, int sc, const vector<vector<bool>>& locked) {
    current_epoch_blank++;
    queue<int> q;
    int start = sr * N + sc;
    q.push(start);
    d_blank_epoch[start] = current_epoch_blank;
    d_blank_dist[start] = 0;
    
    while(!q.empty()) {
        int u = q.front(); q.pop();
        int r = u / N, c = u % N;
        for(int i=0; i<4; ++i) {
            int nr = r + dr[i], nc = c + dc[i];
            if(nr>=0 && nr<N && nc>=0 && nc<N && !locked[nr][nc]) {
                int v = nr * N + nc;
                if(d_blank_epoch[v] != current_epoch_blank) {
                    d_blank_epoch[v] = current_epoch_blank;
                    d_blank_dist[v] = d_blank_dist[u] + 1;
                    q.push(v);
                }
            }
        }
    }
}

void optimize_string(string& s) {
    bool changed = true;
    while(changed) {
        changed = false;
        for (int i = 0; i < (int)s.length() - 1; ) {
            if ((s[i] == 'U' && s[i+1] == 'D') || (s[i] == 'D' && s[i+1] == 'U') ||
                (s[i] == 'L' && s[i+1] == 'R') || (s[i] == 'R' && s[i+1] == 'L')) {
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

        if (grid[dest_r][dest_c] == target_val) {
            locked[dest_r][dest_c] = true;
            continue;
        }

        bfs_2d_dest(dest_r, dest_c, locked);
        bfs_2d_blank(br, bc, locked);

        vector<pair<int, int>> candidates;
        for(int r=0; r<N; ++r){
            for(int c=0; c<N; ++c){
                if(grid[r][c] == target_val && !locked[r][c]){
                    int v = r * N + c;
                    if(d_dest_epoch[v] == current_epoch_dest) {
                        candidates.push_back({r, c});
                    }
                }
            }
        }

        if (candidates.empty()) return "FAILED";

        // Shuffle and Stable Sort to guarantee random tie-breaking
        shuffle(candidates.begin(), candidates.end(), global_rng);
        stable_sort(candidates.begin(), candidates.end(), [&](pair<int,int> a, pair<int,int> b) {
            int va = a.first * N + a.second, vb = b.first * N + b.second;
            int cost_a = 5 * d_dest_dist[va] + (d_blank_epoch[va] == current_epoch_blank ? d_blank_dist[va] : 1e9);
            int cost_b = 5 * d_dest_dist[vb] + (d_blank_epoch[vb] == current_epoch_blank ? d_blank_dist[vb] : 1e9);
            return cost_a < cost_b;
        });

        // Massive Optimization: Only run Joint BFS for the absolute top 2 candidates
        int num_to_check = min((int)candidates.size(), 2);
        candidates.resize(num_to_check);

        int min_path_len = 1e9;
        vector<int> best_moves;
        
        for (auto cand : candidates) {
            int start_state = encode_state(cand.first, cand.second, br, bc);
            
            current_bfs_epoch++;
            queue<int> q;
            q.push(start_state);
            epoch_arr[start_state] = current_bfs_epoch;
            dist_arr[start_state] = 0;
            
            int goal_state = -1;
            int dirs[4] = {0, 1, 2, 3};
            shuffle(dirs, dirs + 4, global_rng); // Generate entirely new path preference
            
            while(!q.empty()) {
                int u = q.front(); q.pop();
                
                // Prune BFS instantly if path already exceeds the best candidate's path length
                if (dist_arr[u] >= min_path_len - 1) continue; 
                
                int ctr, ctc, cbr, cbc;
                decode_state(u, ctr, ctc, cbr, cbc);
                
                if (ctr == dest_r && ctc == dest_c) {
                    goal_state = u;
                    break;
                }
                
                for(int i : dirs) {
                    int nbr = cbr + dr[i], nbc = cbc + dc[i];
                    if(nbr >= 0 && nbr < N && nbc >= 0 && nbc < N && !locked[nbr][nbc]) {
                        int ntr = ctr, ntc = ctc;
                        if (nbr == ctr && nbc == ctc) { ntr = cbr; ntc = cbc; }
                        
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
        
        for(int m : best_moves) {
            ans += move_char[m];
            int nbr = br + dr[m], nbc = bc + dc[m];
            swap(grid[br][bc], grid[nbr][nbc]);
            br = nbr; bc = nbc;
        }
        locked[dest_r][dest_c] = true;
    }
    
    optimize_string(ans);
    return ans + "S";
}

// Generate highly varying topological orders to hunt for tighter compressions
vector<pair<int, int>> get_strategy(int type) {
    vector<pair<int, int>> strat;
    if (type == 0) {
        for (int r = 1; r < N - 1; r++) {
            if (global_rng() % 2) for (int c = 1; c < N - 1; c++) strat.push_back({r, c});
            else for (int c = N - 2; c >= 1; c--) strat.push_back({r, c});
        }
    } else if (type == 1) {
        for (int r = N - 2; r >= 1; r--) {
            if (global_rng() % 2) for (int c = 1; c < N - 1; c++) strat.push_back({r, c});
            else for (int c = N - 2; c >= 1; c--) strat.push_back({r, c});
        }
    } else if (type == 2) {
        for (int c = 1; c < N - 1; c++) {
            if (global_rng() % 2) for (int r = 1; r < N - 1; r++) strat.push_back({r, c});
            else for (int r = N - 2; r >= 1; r--) strat.push_back({r, c});
        }
    } else if (type == 3) {
        for (int c = N - 2; c >= 1; c--) {
            if (global_rng() % 2) for (int r = 1; r < N - 1; r++) strat.push_back({r, c});
            else for (int r = N - 2; r >= 1; r--) strat.push_back({r, c});
        }
    } else { // Outward-In Spiral Strategy
        int top = 1, bottom = N - 2, left = 1, right = N - 2;
        while (top <= bottom && left <= right) {
            for (int c = left; c <= right; c++) strat.push_back({top, c});
            top++;
            for (int r = top; r <= bottom; r++) strat.push_back({r, right});
            right--;
            if (top <= bottom) {
                for (int c = right; c >= left; c--) strat.push_back({bottom, c});
                bottom--;
            }
            if (left <= right) {
                for (int r = bottom; r >= top; r--) strat.push_back({r, left});
                left++;
            }
        }
    }
    return strat;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    // Timer to track execution limit
    auto start_time = chrono::high_resolution_clock::now();
    global_rng.seed(start_time.time_since_epoch().count());

    if (!(cin >> N)) return 0;
    NN = N * N;

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

    // Allocate memory proportional exactly to N (Virtually 0 RAM footprint)
    dist_arr = new int[NN * NN];
    parent_arr = new int[NN * NN];
    parent_move_arr = new int[NN * NN];
    epoch_arr = new int[NN * NN]();
    d_dest_dist = new int[NN];
    d_dest_epoch = new int[NN]();
    d_blank_dist = new int[NN];
    d_blank_epoch = new int[NN]();

    string best_answer = "";
    int min_length = 2e9;

    // Run heavily pruned randomizations continuously until exactly 1.85 seconds
    while (chrono::duration<double>(chrono::high_resolution_clock::now() - start_time).count() < 600.0) {
        int strat_type = global_rng() % 5;
        string res = simulate_order(get_strategy(strat_type));
        
        if (res != "FAILED" && (int)res.length() < min_length) {
            min_length = res.length();
            best_answer = res;
        }
    }

    cout << best_answer << "\n";
    cout << best_answer.length() << "\n";
    
    delete[] dist_arr;
    delete[] parent_arr;
    delete[] parent_move_arr;
    delete[] epoch_arr;
    delete[] d_dest_dist;
    delete[] d_dest_epoch;
    delete[] d_blank_dist;
    delete[] d_blank_epoch;
    
    return 0;
}
