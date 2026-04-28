#include <iostream>
#include <vector>
#include <queue>
#include <stack>

using pii = std::pair<int,int>; //{node, weight}

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);

    std::vector<std::vector<pii>> graph;
    std::vector<bool> visited;
    int n; int d;
    int out = 0;

    std::cin >> n;
    graph.resize(n);
    visited.resize(n, false);
    for (int i = 0; i < n-1 ;i++)
    {
        for (int j = i+1; j < n; j++)
        {
            std::cin >> d;
            graph[i].push_back({j,d});
            graph[j].push_back({i,d});
        }
    }
    
    
    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;

    int start = 0;
    pq.push({})
}