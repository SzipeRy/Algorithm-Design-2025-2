#include <iostream>
#include <vector>
#include <algorithm>

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);
    
    int n;
    long long out = 0;
    std::vector<int> array;

    std::cin >> n;
    array.resize(n);
    for (int i = 0; i < n; i++)
    {
        std::cin >> array[i];
    }
    std::sort(array.begin(), array.end());

    for (int i = 0; i < n-1; i++)
    {
        out += array[i] * array[i+1];
    }
    std::cout << out << '\n';
}