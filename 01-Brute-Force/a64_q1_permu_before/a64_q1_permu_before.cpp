#pragma GCC optimized("O2")

#include<algorithm>
#include<iostream>
#include<vector>

// THE INDEX OF THE PROBLEM START AT 1
void permutation(int n, int len, std::vector<bool> &used, std::vector<int> &constraint, std::vector<int> &out);

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(NULL);
    
    int a, b;
    int n, m;
    std::cin >> n >> m;
    std::vector<int> constraint(n, -1);
    std::vector<bool> used(n, false);
    std::vector<int> out(n);
    
    for (int i = 0; i < m; i++)
    {
        std::cin >> a >> b;
        constraint[b] = a;
    }
    permutation(n, 0, used, constraint, out);
    
    return 0;
}

void permutation(int n, int len, std::vector<bool> &used, std::vector<int> &constraint, std::vector<int> &out)
{
    if (len < n)
    {
        for (int i = 0; i < n; i++)
        {
            if (used[i] == false && (constraint[i] == -1 || used[constraint[i]]))
            {
                used[i] = true;
                out[len] = i;
                permutation(n, len + 1, used, constraint, out);
                used[i] = false;
            }
        }
    }
    else
    {
        for (auto &x : out) std::cout << x << ' ';
        std::cout << '\n';
    }
}