#include <iostream>
#include <vector>
#include <algorithm>

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<int> degree;

    std::cin >> n;
    adj.resize(n, std::vector<int>(n));
    degree.resize(n);
    
    int degree_max = 0;
    for (int i = 0; i < n; i++)
    {
        int count = 0;
        for (int j = 0; j < n; j++)
        {
            std::cin >> adj[i][j];
            if (adj[i][j] == 1)
            {
                ++count;
            }
        }
        if (count > degree_max)
        {
            degree_max = count;
        }
        degree[i] = count;
    }

    std::vector<int> answer(degree_max+1, 0);
    for (int i = 0; i < n; i++)
    {
        answer[degree[i]]++;
    }

    for (auto &x : answer)
    {
        std::cout << x << ' ';
    }
    std::cout << '\n';
}