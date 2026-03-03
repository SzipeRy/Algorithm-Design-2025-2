#include <iostream>
#include <vector>

int main (void)
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    std::vector<std::vector<int>> table;
    std::vector<std::vector<int>> sumTable;
    int n;
    int max = 0;
    std::cin >> n;
    table.resize(n, std::vector<int>(n));
    sumTable.resize(n, std::vector<int>(n));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            if (j <= i) std::cin >> table[i][j]; 
        }
    }

    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col <= row; col++)
        {
            if (col > row) break;
            if (row == 0 && row == 0)
            {
                sumTable[row][col] = table[row][col];
                max = std::max(max, sumTable[row][col]);
                break;
            }
            if (col - 1 < 0)
            {
                sumTable[row][col] = sumTable[row - 1][col] + table[row][col];
                max = std::max(max, sumTable[row][col]);
            }
            else if (col == row)
            {
                sumTable[row][col] = sumTable[row - 1][col - 1] + table[row][col];
                max = std::max(max, sumTable[row][col]);
            }
            else if (col < row)
            {                
                sumTable[row][col] = std::max(sumTable[row-1][col-1], sumTable[row-1][col]) + table[row][col];
                max = std::max(max, sumTable[row][col]);
            }
        }
    }
    std::cout << max << '\n';
}