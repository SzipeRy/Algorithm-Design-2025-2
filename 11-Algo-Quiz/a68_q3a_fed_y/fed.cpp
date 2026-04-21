#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <utility>

using pii = std::pair<int,int>;

void dijkstra(int start);

std::vector<int> dist;
std::vector<bool> visited;
std::vector<std::vector<pii>> graph;

int n; int m;

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);

    int a; int b; int d;
    int max_d = 0;

    std::cin >> n >> m;
    graph.resize(n+1);
    dist.resize(n+1, INT_MAX);
    visited.resize(n+1, false);

    for (int i = 0; i < m; i++)
    {
        std::cin >> a >> b >> d;
        graph[a].push_back({b,d});
        graph[b].push_back({a,d});
    }

    dijkstra(1);
    max_d = *std::max_element(dist.begin()+1, dist.end());


    for (int i = 2; i <= n; i++)
    {
        std::cout << max_d - dist[i] << '\n';
    }

}

void dijkstra(int start)
{

    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> q;
    dist[start] = 0;
    q.push({dist[start], start});
    while(!q.empty())
    {
        int d = q.top().first; 
        int u = q.top().second;
        q.pop();
        if (visited[u])
        {
            continue;
        } 
        visited[u] = true;
        for (auto vw : graph[u])
        {
            int v = vw.first;
            int w = vw.second;
            if (!visited[v] && dist[u] + w < dist[v])
            {
                dist[v] = dist[u] + w;
                q.push({dist[v], v});
            }
        }
    }
}