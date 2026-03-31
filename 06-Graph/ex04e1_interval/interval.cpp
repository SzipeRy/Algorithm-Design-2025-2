#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

int main(void)
{
    int n;
    std::vector<std::pair<int,int>> start;
    std::vector<std::pair<int,int>> finish;
    int count = 0;
    int prev;
    int in;

    std::cin >> n;
    start.resize(n);
    finish.resize(n);

    for (int i = 0; i < n; i++)
    {
        std::cin >> in;
        start[i] = std::make_pair(in, i);
    }
    for (int i = 0; i < n; i++)
    {
        std::cin >> in;
        finish[i] = std::make_pair(in, i);
    }
    std::sort(start.begin(), start.end());
    std::sort(finish.begin(), finish.end());

    for (int i = 0; i < n ;i++)
    {
        if (i == 0)
        {
            ++count;
            prev = finish[i].second;
        }
        else
        {
            if (start[i].second >= prev)
            {
                prev = finish[i].second;
                ++count;
            }
        }
    }

    std::cout << count << '\n';
}