#include <iostream>
#include <vector>
#include <queue>
#include <utility>

using pii = std::pair<int,int>;

std::vector<std::pair<int, int>> direction = {{-1,0}, {0,1}, {1,0}, {0,-1}};

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);
    int row; int col;
    std::vector<std::vector<int>> map;
    std::vector<std::vector<int>> dist;
    std::queue<pii> q;

    std::cin >> row >> col;
    map.resize(row, std::vector<int>(col));
    dist.resize(row, std::vector<int>(col, 1e8));
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            std::cin >> map[i][j];
            if (map[i][j] == 1) 
            {
                q.push(std::make_pair(i,j));
                dist[i][j] = 1;
            }
        }
    }

    

    while (!q.empty())
    {
        int i = q.front().first;
        int j = q.front().second;
        q.pop();

        for (auto d : direction)
        {
            int ni = i + d.first;
            int nj = j + d.second;
            if (ni < 0 || ni >= row || nj < 0 || nj >= col || map[ni][nj] == 3) continue;
            if (dist[ni][nj] > dist[i][j] + 1)
            {
                dist[ni][nj] = dist[i][j] + 1;
                q.push(std::make_pair(ni,nj));
                if (map[ni][nj] == 2)
                {
                    std::cout << dist[ni][nj] << '\n';
                    return(0);
                }
            }
        }
    }
}