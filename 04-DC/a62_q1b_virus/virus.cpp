#pragma optimize("O3")

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <string>

bool solve(std::vector<int> virus, int start, int end);

int n, k, length;


int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(NULL);
    std::cin >> n >> k;
    length = pow(2, k);
    std::vector<int> virus;
    virus.resize(length);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < length; j++)
        {
            std::cin >> virus[j];
        }
        // std::cout << virus[length - 1] << "a" << '\n';
        bool result = solve(virus, 0, virus.size());
        if (result) {std::cout << "yes" << '\n';}
        else {std::cout << "no" << '\n';}
    }
}

bool solve(std::vector<int> virus, int start, int end)
{
    if (end - start == 2)
    {
        // std::cout << virus[start] << " " << virus[end - 1] << '\n';
        if (virus[start] == 0 && virus[end-1] == 1) return true;
        else return false;
    }
    int mid = (start + end) >> 1;
    bool result_r = solve(virus, mid, end);
    bool result_l_1 = solve(virus, start, mid);
    std::reverse(virus.begin()+start, virus.begin()+mid);
    bool result_l_2 = solve(virus, start, mid);
    if (result_r && (result_l_1 || result_l_2))
    {
        return true;
    }
    else
    {
        return false;
    }
}