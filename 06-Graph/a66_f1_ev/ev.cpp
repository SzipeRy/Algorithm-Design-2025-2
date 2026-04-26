#include <iostream>
#include <vector>
#include <queue>

using pii = std::pair<int,int>;

int n, m;
int a, b, e;

std::vector<std::vector<int>> graph;
std::vector<std::vector<int>> dist;

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);
    
    std::cin >> n >> m;
    graph.resize(n, std::vector<int>(n, 1e9));
    dist.resize(n, std::vector<int>(n, 1e9));

    for (int i = 0; i < m; i++)
    {
        std::cin >> a >> b >> e;
        graph[a][b] = e;
        graph[b][a] = e;
    }

    
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n ;j++)
        {
            if (i == j) dist[i][j] = 0;
            else dist[i][j] = graph[i][j];
            
        }
    }
    
    for (int k = 0; k < n; k++)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (dist[i][k] < 1e9 && dist[k][j] < 1e9)
                {
                    dist[i][j] = std::min(dist[i][j], dist[i][k]+dist[k][j]);
                }
            }
        }
    }
    
    int max = -1;
    for (auto x : dist)
    {
        for (auto y : x)
        {
            max = std::max(max, y);
        }
    }
    std::cout << max << '\n';
}