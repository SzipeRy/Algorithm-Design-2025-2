/*
    This Problem Set is from https://atcoder.jp/contests/dp/tasks/dp_n
*/

#include <iostream>
#include <vector>

int main(void)
{
    int n;
    std::cin >> n;
    std::vector<int> a(n);
    for (int i = 0; i < n; i++)
    {
        std::cin >> a[i];
    }
    std::vector<std::vector<int>> table(n, std::vector<int>(n));
    for (int round = 0; round < n; round++)
    {
        int i = 0;
        for (int j = round; j < n; j++)
        {
            if (i == j) 
            {
                table[i][j] = 0;
            }
            int min = 1410065407;
            for (int m = i; m < j; m++)
            {
                int tmp = table[i][m] + table[m+1][j] + (a[i] * a[m+1] * a[j+1]);
                if (tmp < min)
                {
                    min = tmp;
                }
            }
            if (i != j) table[i][j] = min;
            i++;
        }
    }
    std::cout << table[0][n-1];
}