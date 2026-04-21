#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <utility>

std::vector<std::pair<int,int>> move = {{-1,0}, {0,1}, {1,0}, {0,-1}};

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);

    int row; int col;
    
    std::vector<std::vector<char>> array;
    std::vector<std::vector<int>> dist;
    std::queue<std::pair<int,int>> bfs;

    std::cin >> row >> col;

    std::pair<int, int> start = {0, 0};
    std::pair<int, int> end = {row-1, col-1};
    array.resize(row, std::vector<char>(col));
    dist.resize(row, std::vector<int>(col, -1));

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            std::cin >> array[i][j];
        }
    }

    if (array[start.first][start.second] == '.')
    {
        bfs.push(start);
        dist[start.first][start.second] = 0;
    }

    while (!bfs.empty())
    {
        auto [ux, uy] = bfs.front();
        bfs.pop();

        for (int i = 0; i < 4; i++)
        {
            int vx = ux + move[i].first; int vy = uy + move[i].second;
            if (vx < 0 || vy < 0 || vx >= row || vy >= col) continue;

            if (dist[vx][vy] == -1 && array[vx][vy] == '.')
            {
                dist[vx][vy] = dist[ux][uy] + 1;
                bfs.push(std::make_pair(vx, vy));
            }
        }
    }
    std::cout << dist[end.first][end.second] << '\n';

    // for (auto &x : dist)
    // {
    //     for (auto &y : x)
    //     {
    //         std::cout << y << ' ';
    //     }
    //     std::cout << '\n';
    // }
}