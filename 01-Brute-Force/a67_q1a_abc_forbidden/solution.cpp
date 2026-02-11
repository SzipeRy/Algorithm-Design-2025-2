#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <string>

using namespace std;

void generatePermutations(string current, int a, int b, int c, int n, const set<string>& forbiddenPairs, vector<string>& results) {
    // Base case: if the current string has length N
    if (current.length() == n) {
        results.push_back(current);
        return;
    }

    // Try adding 'A', 'B', or 'C' if still possible
    if (a > 0) {
        if (current.empty() || forbiddenPairs.find(current.substr(current.length() - 1) + 'A') == forbiddenPairs.end()) {
            generatePermutations(current + 'A', a - 1, b, c, n, forbiddenPairs, results);
        }
    }

    if (b > 0) {
        if (current.empty() || forbiddenPairs.find(current.substr(current.length() - 1) + 'B') == forbiddenPairs.end()) {
            generatePermutations(current + 'B', a, b - 1, c, n, forbiddenPairs, results);
        }
    }

    if (c > 0) {
        if (current.empty() || forbiddenPairs.find(current.substr(current.length() - 1) + 'C') == forbiddenPairs.end()) {
            generatePermutations(current + 'C', a, b, c - 1, n, forbiddenPairs, results);
        }
    }
}

int main() {
    int n, i, j, k, m;
    cin >> n >> i >> j >> k;
    cin >> m;

    set<string> forbiddenPairs;
    for (int x = 0; x < m; x++) {
        string pair;
        cin >> pair;
        forbiddenPairs.insert(pair);
    }

    vector<string> results;
    generatePermutations("", i, j, k, n, forbiddenPairs, results);

    // Output the results
    cout << results.size() << endl;
    for (const auto& str : results) {
        cout << str << endl;
    }

    return 0;
}
