#include <iostream>
#include <vector>
#include <queue>
#include <string>

using namespace std;

int N;
vector<vector<int>> grid;
vector<vector<int>> target;
vector<vector<bool>> locked;
int blank_r, blank_c;
string ans = "";

// Mapping based on the problem description:
// U: Tile below moves UP -> Blank moves DOWN (dr=1, dc=0)
// D: Tile above moves DOWN -> Blank moves UP (dr=-1, dc=0)
// L: Tile right moves LEFT -> Blank moves RIGHT (dr=0, dc=1)
// R: Tile left moves RIGHT -> Blank moves LEFT (dr=0, dc=-1)
int dr[] = {1, -1, 0, 0};
int dc[] = {0, 0, 1, -1};
char move_char[] = {'U', 'D', 'L', 'R'};

bool is_valid(int r, int c) {
    return r >= 0 && r < N && c >= 0 && c < N;
}

// Compute the shortest distance for the tile to travel through non-locked cells
void get_tile_dists(int start_r, int start_c, vector<vector<int>>& dist) {
    dist.assign(N, vector<int>(N, 1e9));
    queue<pair<int, int>> q;
    q.push({start_r, start_c});
    dist[start_r][start_c] = 0;

    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();

        for (int i = 0; i < 4; i++) {
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (is_valid(nr, nc) && !locked[nr][nc]) {
                if (dist[r][c] + 1 < dist[nr][nc]) {
                    dist[nr][nc] = dist[r][c] + 1;
                    q.push({nr, nc});
                }
            }
        }
    }
}

// BFS to find the shortest path for the blank space to reach the best adjacent cell
bool route_blank_to_closest(const vector<pair<int, int>>& targets_list, int avoid_r, int avoid_c, vector<int>& best_path, int& best_nr, int& best_nc) {
    for (auto p : targets_list) {
        if (blank_r == p.first && blank_c == p.second) {
            best_nr = p.first;
            best_nc = p.second;
            best_path.clear();
            return true;
        }
    }

    vector<vector<int>> dist(N, vector<int>(N, 1e9));
    vector<vector<int>> parent_move(N, vector<int>(N, -1));
    vector<vector<pair<int, int>>> parent_cell(N, vector<pair<int, int>>(N, {-1, -1}));

    queue<pair<int, int>> q;
    q.push({blank_r, blank_c});
    dist[blank_r][blank_c] = 0;

    int found_r = -1, found_c = -1;

    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();

        bool is_target = false;
        for (auto p : targets_list) {
            if (r == p.first && c == p.second) {
                is_target = true;
                found_r = r;
                found_c = c;
                break;
            }
        }
        if (is_target) break;

        for (int i = 0; i < 4; i++) {
            int nr = r + dr[i];
            int nc = c + dc[i];
            // Avoid locked cells and the tile we are currently trying to route
            if (is_valid(nr, nc) && !locked[nr][nc] && !(nr == avoid_r && nc == avoid_c)) {
                if (dist[r][c] + 1 < dist[nr][nc]) {
                    dist[nr][nc] = dist[r][c] + 1;
                    parent_move[nr][nc] = i;
                    parent_cell[nr][nc] = {r, c};
                    q.push({nr, nc});
                }
            }
        }
    }

    if (found_r == -1) return false;

    best_nr = found_r;
    best_nc = found_c;

    int cr = found_r, cc = found_c;
    vector<int> rev_path;
    while (cr != blank_r || cc != blank_c) {
        rev_path.push_back(parent_move[cr][cc]);
        auto p = parent_cell[cr][cc];
        cr = p.first;
        cc = p.second;
    }

    for (int i = rev_path.size() - 1; i >= 0; i--) {
        best_path.push_back(rev_path[i]);
    }
    return true;
}

// Applies the calculated optimal blank moves and outputs the chars
void apply_blank_moves(const vector<int>& path) {
    for (int m : path) {
        ans += move_char[m];
        int nr = blank_r + dr[m];
        int nc = blank_c + dc[m];
        swap(grid[blank_r][blank_c], grid[nr][nc]);
        blank_r = nr;
        blank_c = nc;
    }
}

// Swaps the tile with the blank space once adjacent
void move_tile_with_blank(int tile_r, int tile_c) {
    int move_dir = -1;
    for (int i = 0; i < 4; i++) {
        if (blank_r + dr[i] == tile_r && blank_c + dc[i] == tile_c) {
            move_dir = i;
            break;
        }
    }
    if (move_dir != -1) {
        ans += move_char[move_dir];
        swap(grid[blank_r][blank_c], grid[tile_r][tile_c]);
        blank_r = tile_r;
        blank_c = tile_c;
    }
}

// Main logic to route an individual tile into the correct central area
void place_tile(int tr, int tc) {
    int target_val = target[tr - 1][tc - 1];

    if (grid[tr][tc] == target_val) {
        locked[tr][tc] = true;
        return;
    }

    vector<vector<int>> tile_dist;
    get_tile_dists(tr, tc, tile_dist);

    int best_r = -1, best_c = -1;
    int min_d = 1e9;

    // Find the closest tile of the matching value
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            if (grid[r][c] == target_val && !locked[r][c]) {
                if (tile_dist[r][c] < min_d) {
                    min_d = tile_dist[r][c];
                    best_r = r;
                    best_c = c;
                }
            }
        }
    }

    int curr_r = best_r;
    int curr_c = best_c;

    // Walk the target tile to the destination
    while (curr_r != tr || curr_c != tc) {
        vector<pair<int, int>> primary_targets;
        vector<pair<int, int>> secondary_targets;

        // Categorize valid next steps for the tile
        for (int i = 0; i < 4; i++) {
            int nr = curr_r + dr[i];
            int nc = curr_c + dc[i];
            if (is_valid(nr, nc) && !locked[nr][nc]) {
                if (tile_dist[nr][nc] < tile_dist[curr_r][curr_c]) {
                    primary_targets.push_back({nr, nc});
                } else if (tile_dist[nr][nc] == tile_dist[curr_r][curr_c]) {
                    secondary_targets.push_back({nr, nc});
                }
            }
        }

        int best_nr = -1, best_nc = -1;
        vector<int> best_blank_path;

        if (!primary_targets.empty() && route_blank_to_closest(primary_targets, curr_r, curr_c, best_blank_path, best_nr, best_nc)) {
            // Attempt standard step closer to the target
        } else if (!secondary_targets.empty() && route_blank_to_closest(secondary_targets, curr_r, curr_c, best_blank_path, best_nr, best_nc)) {
            // Attempt sidestepping around an obstacle
        } else {
            break; 
        }

        apply_blank_moves(best_blank_path);
        move_tile_with_blank(curr_r, curr_c);
        curr_r = best_nr;
        curr_c = best_nc;
    }

    locked[tr][tc] = true;
}

int main() {
    // Fast I/O 
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    if (!(cin >> N)) return 0;

    grid.assign(N, vector<int>(N));
    locked.assign(N, vector<bool>(N, false));

    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            cin >> grid[r][c];
            if (grid[r][c] == -1) {
                blank_r = r;
                blank_c = c;
            }
        }
    }

    target.assign(N - 2, vector<int>(N - 2));
    for (int r = 0; r < N - 2; r++) {
        for (int c = 0; c < N - 2; c++) {
            cin >> target[r][c];
        }
    }

    for (int tr = 1; tr < N - 1; tr++) {
        for (int tc = 1; tc < N - 1; tc++) {
            place_tile(tr, tc);
        }
    }

    ans += 'S';
    cout << ans << "\n";
    cout << ans.size() << '\n';

    return 0;
}