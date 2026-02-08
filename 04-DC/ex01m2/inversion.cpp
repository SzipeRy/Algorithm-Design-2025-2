#pragma gcc optimize("Ofast, unroll-loops")

#include <iostream>
#include <algorithm>
#include <vector>

int n;
int a;
int out = 0;
std::vector<int> array;

int main(void)
{
    std::cin >> n;

    for (int i = 0; i < n; i++)
    {
        std::cin >> a;
        std::sort(array.begin(), array.end());
        if (array.empty())
        {
            array.push_back(a);
            continue;
        }
        auto it = std::upper_bound(array.begin(), array.end(), a);
        int pos = array.end() - it;
        array.push_back(a);

        out += pos;
    }

    std::cout << out << '\n';

}