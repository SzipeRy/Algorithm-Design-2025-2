#include <iostream>
#include <vector>

int main(void)
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    int n;
    std::vector<int> d;
    std::vector<std::vector<int>> m;
    int min = 100000000;
    std::cin >> n;
    d.resize(n+1);
    m.resize(n+1, std::vector<int>(n+1, 0));
    for (int i = 0; i < d.size(); i++)
    {
        std::cin >> d[i];
    }

    for (int len = 2; len < d.size(); len++)
    {
        for (int i = 0; i < d.size() - len; i++)
        {
            int j = i + len;
            m[i][j] = INT_MAX;

            for (int k = i + 1; k < j; k++)
            {
                m[i][j] = std::min(m[i][j], m[i][k] + m[k][j] + (d[i] * d[k] * d[j]));
            }
        }
    }
    std::cout << m[0][n] << '\n';
}