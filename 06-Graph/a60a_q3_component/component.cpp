#include <iostream>
#include <vector>
#include <stack>

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(NULL);

    std::vector<std::vector<int>> adj;
    std::stack<int> dfs;

    int v; int e;
    int a; int b;
    int out = 0;
    std::cin >> v >> e;

    adj.resize(v+1);
    for (int i = 0; i < e; i++)
    {
        std::cin >> a >> b;
        adj[a].push_back(b);
        adj[b].push_back(a);
    }

    std::vector<bool> visited(v+1, false);
    for (int i = 1; i <= v; i++)
    {   
        if (!visited[i])
        {
            ++out;
            dfs.push(i);
            visited[i] = true;
            while (dfs.size() > 0)
            {
                int u = dfs.top();
                dfs.pop();
                for (auto &x:adj[u])
                {
                    if (!visited[x])
                    {
                        visited[x] = true;
                        dfs.push(x);
                    }
                }
            }
        }
    }

    std::cout << out << '\n';
}