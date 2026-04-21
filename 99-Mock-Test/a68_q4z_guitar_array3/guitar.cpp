#include <iostream>
#include <vector>

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);

    int n; int m;
    char b;
    std::vector<std::vector<char>> array;

    std::cin >> n >> m;
    array.resize(n, std::vector<char>(m));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            std::cin >> array[i][j];
        }
    }
    
}