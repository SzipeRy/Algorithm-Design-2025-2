#include <iostream>
#include <vector>
#include <utility>

int main(void)
{
    int n;
    std::vector<int> start;
    std::vector<int> finish;
    
    int count = 0;
    int prev;

    std::cin >> n;
    start.resize(n);
    finish.resize(n);

    for (int i = 0; i < n; i++)
    {
        std::cin >> start[i];
    }
    for (int i = 0; i < n; i++)
    {
        std::cin >> finish[i];
    }

    for (int i = 0; i < n ;i++)
    {
        if (i == 0)
        {
            ++count;
            prev = finish[i];
        }
        else
        {
            if (start[i] >= prev)
            {
                prev = finish[i];
                ++count;
            }
        }
    }

    std::cout << count << '\n';
}