#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>

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

// Tiny memory footprint using 1D Arrays and an Epoch tracker
int* b_dist;
int* b_parent;
int* b_epoch;
int* d_dest_dist;
int* d_dest_epoch;
int* d_blank_dist;
int* d_blank_epoch;
int current_epoch = 0;

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

        // If a correct tile is already here, lock it
        if (grid[dest_r][dest_c] == target_val) {
            locked[dest_r][dest_c] = true;
            continue;
        }

        current_epoch++;
        int dest_idx = dest_r * N + dest_c;
        d_dest_epoch[dest_idx] = current_epoch;
        d_dest_dist[dest_idx] = 0;
        queue<int> q;
        q.push(dest_idx);

        while(!q.empty()){
            int u = q.front(); q.pop();
            int r = u / N, c = u % N;
            for(int i=0; i<4; ++i){
                int nr = r + dr[i], nc = c + dc[i];
                if(nr>=0 && nr<N && nc>=0 && nc<N && !locked[nr][nc]){
                    int v = nr * N + nc;
                    if(d_dest_epoch[v] != current_epoch){
                        d_dest_epoch[v] = current_epoch;
                        d_dest_dist[v] = d_dest_dist[u] + 1;
                        q.push(v);
                    }
                }
            }
        }

        current_epoch++;
        int b_idx = br * N + bc;
        d_blank_epoch[b_idx] = current_epoch;
        d_blank_dist[b_idx] = 0;
        queue<int> bq;
        bq.push(b_idx);
        
        while(!bq.empty()){
            int u = bq.front(); bq.pop();
            int r = u / N, c = u % N;
            for(int i=0; i<4; ++i){
                int nr = r + dr[i], nc = c + dc[i];
                if(nr>=0 && nr<N && nc>=0 && nc<N && !locked[nr][nc]){
                    int v = nr * N + nc;
                    if(d_blank_epoch[v] != current_epoch){
                        d_blank_epoch[v] = current_epoch;
                        d_blank_dist[v] = d_blank_dist[u] + 1;
                        bq.push(v);
                    }
                }
            }
        }

        int best_tr = -1, best_tc = -1;
        int min_dist = 1e9;
        int min_blank_dist = 1e9;

        // Choose the absolute closest matching tile
        for(int r=0; r<N; ++r){
            for(int c=0; c<N; ++c){
                if(grid[r][c] == target_val && !locked[r][c]){
                    int v = r * N + c;
                    if(d_dest_epoch[v] == current_epoch - 1) { 
                        int d_dest = d_dest_dist[v];
                        int d_blank = (d_blank_epoch[v] == current_epoch) ? d_blank_dist[v] : 1e9;
                        
                        if(d_dest < min_dist){
                            min_dist = d_dest;
                            min_blank_dist = d_blank;
                            best_tr = r; best_tc = c;
                        } else if (d_dest == min_dist && d_blank < min_blank_dist) {
                            min_blank_dist = d_blank;
                            best_tr = r; best_tc = c;
                        }
                    }
                }
            }
        }

        if (best_tr == -1) return "FAILED";

        vector<pair<int, int>> tile_path;
        int curr_r = best_tr, curr_c = best_tc;
        while(curr_r != dest_r || curr_c != dest_c) {
            int next_r = -1, next_c = -1;
            int min_d = 1e9;
            for(int i=0; i<4; ++i){
                int nr = curr_r + dr[i], nc = curr_c + dc[i];
                if(nr>=0 && nr<N && nc>=0 && nc<N && !locked[nr][nc]){
                    int v = nr * N + nc;
                    if(d_dest_epoch[v] == current_epoch - 1 && d_dest_dist[v] < min_d){
                        min_d = d_dest_dist[v];
                        next_r = nr; next_c = nc;
                    }
                }
            }
            tile_path.push_back({next_r, next_c});
            curr_r = next_r; curr_c = next_c;
        }

        // Move the tile along the path safely using the blank
        int tr = best_tr, tc = best_tc;
        for (auto [next_tr, next_tc] : tile_path) {
            current_epoch++;
            queue<int> blank_q;
            int start_idx = br * N + bc;
            int target_idx = next_tr * N + next_tc;
            
            blank_q.push(start_idx);
            b_epoch[start_idx] = current_epoch;
            b_dist[start_idx] = 0;

            bool found = false;
            while(!blank_q.empty()){
                int u = blank_q.front(); blank_q.pop();
                if (u == target_idx) {
                    found = true;
                    break;
                }
                int r = u / N, c = u % N;
                for(int i=0; i<4; ++i){
                    int nr = r + dr[i], nc = c + dc[i];
                    if(nr>=0 && nr<N && nc>=0 && nc<N && !locked[nr][nc]){
                        if (nr == tr && nc == tc) continue; // Blanket avoids touching the tile we are protecting
                        int v = nr * N + nc;
                        if (b_epoch[v] != current_epoch) {
                            b_epoch[v] = current_epoch;
                            b_dist[v] = b_dist[u] + 1;
                            b_parent[v] = i;
                            blank_q.push(v);
                        }
                    }
                }
            }

            if (!found) return "FAILED";

            vector<int> b_moves;
            int cr = next_tr, cc = next_tc;
            while(cr != br || cc != bc){
                int dir = b_parent[cr * N + cc];
                b_moves.push_back(dir);
                cr -= dr[dir];
                cc -= dc[dir];
            }
            reverse(b_moves.begin(), b_moves.end());

            for(int dir : b_moves){
                ans += move_char[dir];
                int nr = br + dr[dir];
                int nc = bc + dc[dir];
                swap(grid[br][bc], grid[nr][nc]);
                br = nr; bc = nc;
            }

            // Execute the swap
            int swap_dir = -1;
            for(int i=0; i<4; ++i){
                if (br + dr[i] == tr && bc + dc[i] == tc) {
                    swap_dir = i;
                    break;
                }
            }
            ans += move_char[swap_dir];
            swap(grid[br][bc], grid[tr][tc]);
            br = tr; bc = tc;

            tr = next_tr; tc = next_tc;
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

    // Allocate memory proportional only to the grid (Virtually 0 RAM footprint)
    b_dist = new int[N*N];
    b_parent = new int[N*N];
    b_epoch = new int[N*N]();
    d_dest_dist = new int[N*N];
    d_dest_epoch = new int[N*N]();
    d_blank_dist = new int[N*N];
    d_blank_epoch = new int[N*N]();

    // 8 Different Board Locking Strategies to guarantee the mathematically lowest string output
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

    for (const auto& order : strategies) {
        string res = simulate_order(order);
        if (res != "FAILED" && (int)res.length() < min_length) {
            min_length = res.length();
            best_answer = res;
        }
    }

    cout << best_answer << "\n";
    cout << best_answer.length() << "\n";
    
    // Memory cleanup
    delete[] b_dist;
    delete[] b_parent;
    delete[] b_epoch;
    delete[] d_dest_dist;
    delete[] d_dest_epoch;
    delete[] d_blank_dist;
    delete[] d_blank_epoch;
    
    return 0;
}