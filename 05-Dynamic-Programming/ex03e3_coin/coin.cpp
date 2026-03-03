#include <iostream>
#include <vector>

int const MAX_VALUE = 100000000;

int main(void)
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    std::vector<int> coin;
    std::vector<std::vector<int>> table;

    int n, target;
    
    std::cin >> n >> target;

    coin.resize(n);
    table.resize(coin.size(), std::vector<int>(target + 1, 0));
    for (int i = 0; i < n; i++)
    {
        std::cin >> coin[i];
    }
    for (int i = coin.size() - 1; i >= 0; i--)
    {
        for (int j = 1; j <= target; j++)
        {
            table[i][j] = MAX_VALUE;
            int take = MAX_VALUE;
            int noTake = MAX_VALUE;

            if (j - coin[i] >= 0)
            {
                take = table[i][j - coin[i]];
                if (take != MAX_VALUE) take++;
            }
            if (i + 1 < coin.size())
            {
                noTake = table[i+1][j];
            }
            table[i][j] = std::min(take, noTake);
        }
    }

    for (int i = 0; i < coin.size(); i++)
    {
        for (int j = 0; j <= target; j++)
        {
            std::cout << table[i][j] << ' ';
        }
        std::cout << '\n';
    }

    std::cout << table[0][target] << '\n';
}