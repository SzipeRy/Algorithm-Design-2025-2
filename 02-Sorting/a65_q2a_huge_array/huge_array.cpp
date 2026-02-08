#include <algorithm>
#include <iostream>
#include <vector>
#include <utility>

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);

    int n, q;
    int a, b; // a is value, b is amount of a
    int pos = 0;
    std::cin >> n >> q;

    std::vector<std::pair<int, int>> array(n);
    for (int i = 0; i < n; i++)
    {
        std::cin >> array[i].first >> array[i].second;
    }

    std::sort(array.begin(), array.end());

    std::vector<std::pair<int, int>> out(n+1);
    for (int i = 0; i < n; i++)
    {
        pos += array[i].second;
        out[i].second = array[i].first;
        out[i].first = pos;
    }
    pos += array[n-1].second;
    out[n].second = array[n-1].first;
    out[n].first = pos;
    int m;
    for (int i = 0; i < q; i++)
    {
        std::cin >> m;
        auto it = std::lower_bound(out.begin(), out.end(), std::make_pair(m,0));
        if (it != out.end())
        {
            std::cout << it->second << '\n';
        }
    }
}