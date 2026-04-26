#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <set>

std::vector<int> tail;
std::vector<int> point;
std::vector<std::set<int>> array;
std::vector<bool> check;
std::set<int> tmp;
std::set<int> record;

int used = 0;
int n, x;
int out = 0;
int tmp_out;

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);

    std::cin >> n;
    tail.resize(n);
    point.resize(n);
    array.resize(n);
    check.resize(n, false);

    for (int i = 0; i < n; i++) std::cin >> point[i];
    for (int i = n-2; i >= 0; i--) tail[i] = tail[i+1] + point[i];

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            std::cin >> x;
            if (x) array[i].insert(j);
            else continue;
        }
    }
    int prev_count = -1;
    int i = 0;
    while (used < n)
    {
        tmp_out = 0;
        int count = 0;
        if (!check[i])
        {
            bool ok = true;
            check[i] = true;
            used++;
            count++;
            for (auto x : array[i])
            {
                if (array[x] != array[i])
                {
                    ok = false;
                    break;
                }
            }
            if (ok)
            {
                
                for (auto x : array[i])
                {
                    tmp_out += point[x];
                    check[x] = true;
                    used++;
                }
            }
        }
        if (tmp_out > out)
        {
            out = tmp_out;
        }
        i++;
    }
    std::cout << out << '\n';
}