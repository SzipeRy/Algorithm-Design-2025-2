#include <iostream>
#include <vector>
#include <utility>

long max_search;
long min_search;
std::vector<std::vector<char>> array;
int queen_count = INT_MAX;


int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);

    int n; int m;
    std::vector<int> queen;
    
    
    std::cin >> n >> m;
    if (n < m)
    {
        max_search = m;
        min_search = n;
    }
    else
    {
        max_search = n;
        min_search = m;
    }
    array.resize(n, std::vector<char>(m));
    std::vector<std::vector<bool>> check;
    check.resize(n, std::vector<bool>(m, false));
    queen.resize(min_search, -1);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            std::cin >> array[i][j];
            if (array[i][j] == '#') check[i][j] = true;
        }
    }
}

// min_search is row
void solve1(int step, std::vector<std::vector<bool>> check, std::vector<int>queen)
{
    if (step < min_search || step < queen_count)
    {
        for (int i = 0; i < max_search; i++)
        {
            if (!check[step][i])
            {
                check[step][i] = true;
                queen[step] = i;
                solve1(step+1,check,queen);
                check[step][i] = false;
            }
        }
    }
    else
}