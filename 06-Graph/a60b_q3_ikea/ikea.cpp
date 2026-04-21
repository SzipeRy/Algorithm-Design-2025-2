#include <iostream>
#include <vector>
#include <stack>
#include <queue>

bool topo_sort(std::vector<int> &plan, std::vector<std::vector<int>> &adj, std::vector<int> indegree);

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);
    
    int a; int b;
    int n; int e;
    std::vector<std::vector<int>> adj;
    std::vector<int> plan;
    std::vector<int> indegree;

    std::cin >> n >> e;
    adj.resize(n+1);
    plan.resize(n);
    indegree.resize(n+1, 0);
    for (int i = 0; i < e; i++)
    {
        std::cin >> a >> b;
        adj[a].push_back(b);
    }

    for (int i = 1; i <= n; i++)
    {
        for (auto x : adj[i])
        {
            indegree[x]++;
        }
    }


    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < n; j++)
        {
            std::cin >> plan[j];
        }
        if (topo_sort(plan, adj, indegree))
        {
            std::cout << "SUCCESS" << '\n';
        }
        else
        {
            std::cout << "FAIL" << '\n';
        }
    }
}

bool topo_sort(std::vector<int> &plan, std::vector<std::vector<int>> &adj, std::vector<int> indegree)
{
    for (int x : plan)
    {
        if (indegree[x] != 0) return false;
        for (int next : adj[x])
        {
            indegree[next]--;
        }
    }
    return true;
}