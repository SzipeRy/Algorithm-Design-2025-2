#include <iostream>
#include <vector>

struct Edge
{
    int a, b, w;
};

int INF = 2147480000;
int n, m, k;
int v;

std::vector<Edge> graph;
std::vector<int> start;
std::vector<int> dist;

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);
    
    int a,b,x;

    std::cin >> n >> m >> k;
    std::cin >> v;
    graph.resize(n);
    start.resize(k);
    dist.resize(n, INF);
    dist[v] = 0;
    for (int i = 0; i < k; i++)
    {
        std::cin >> start[i];
    }
    for (int i = 0; i < m; i++)
    {
        std::cin >> a >> b >> x;
        graph.push_back({a,b,x});
    }

    for (int i = 1; i < n; i++)
    {
        for (auto [a,b,x] : graph)
        {
            if (dist[a] > dist[b] + x)
            {
                dist[a] = dist[b] + x;
            }
        }
    }
    int out = INF;
    for (auto x : start)
    {
        out = std::min(out, dist[x]);
    }
    std::cout << out << '\n';
}   
