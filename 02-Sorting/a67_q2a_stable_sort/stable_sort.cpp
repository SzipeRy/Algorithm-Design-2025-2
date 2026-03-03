#include <algorithm>
#include <iostream>
#include <vector>
#include <utility>

std::pair<bool,bool> isSort(std::vector<int> a, std::vector<int> x);

int main(void)
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int n, m;
    std::vector<int> a; // The Main array
    std::vector<int> x; // The input testing array

    std::cin >> n >> m;

    a.resize(n);
    x.resize(n);
    for (int i = 0; i < n; i++)
    {
        std::cin >> a[i];
    }
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            std::cin >> x[j];
        }
        auto out = isSort(a, x);
        std::cout << out.first << ' ' << out.second << '\n';
    }
}

std::pair<bool,bool> isSort(std::vector<int> a, std::vector<int> x)
{
    bool stable = true;
    int prev = a[x[0] - 1];
    for (int i = 1; i < x.size(); i++)
    {
        if (a[x[i] - 1] < prev) return {false, false};
        if (a[x[i] - 1] == prev)
        {
            if (x[i] < x[i-1] && stable) stable = false;
        }
        prev = a[x[i] - 1];
    }
    return {true,stable};
}