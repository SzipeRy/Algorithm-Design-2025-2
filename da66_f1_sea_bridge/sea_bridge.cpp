#include <bits/stdc++.h>

int main(void)
{
    std::vector<std::pair<int, int>> d = {{-1,0}, {0,1}, {1,0}, {0,-1}};
    
    int row; int col;
    std::vector<std::vector<int>> map;
    std::queue<int> q;

    std::cin >> row >> col;
    map.resize(row, std::vector<int>(col));
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            std::cin >> map[i][j];
        }
    }


}