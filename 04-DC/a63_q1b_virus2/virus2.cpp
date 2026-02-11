#include <iostream>
#include <vector>
#include <cmath>

bool solve(int start, int end);

int n, k;
int length;
std::vector<int> virus;

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(NULL);
    std::cin >> n >> k;
    length = pow(2,k);
    virus.clear();
    virus.resize(length);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < length; j++)
        {
            std::cin >> virus[j];
        }
        if (solve(0, length))
        {
            std::cout << "yes" << '\n';
        }
        else
        {
            std::cout << "no" << '\n';
        }
    }
}

bool solve(int start, int end)
{
    int count_one_l = 0;
    int count_one_r = 0;
    if (end - start == 2)
    {
        return true;
    }
    int mid = (start + end) >> 1;

    bool l = solve(start, mid);
    bool r = solve(mid, end);
    if (l && r)
    {
        for (int i = start; i < mid; i++)
        {
            if (virus[i] == 1) count_one_l++;
        }
        for (int i = mid; i < end; i++)
        {
            if (virus[i] == 1) count_one_r++;
        }
        if (std::abs(count_one_l - count_one_r) > 1) return false;
        else return true;
    }
    return false;
}