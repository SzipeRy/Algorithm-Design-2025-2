#pragma GCC optimize("O3")

#include <stdio.h>
#include <iostream>
#include <vector>

bool solve(int start, int end, int val);

int t, k, x, length;
std::vector<int> array;

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);
    std::cin >> t;
    for (int i = 0; i < t; i++)
    {
        std::cin >> k >> x;
        length = 1 << k;
        array.clear();
        array.resize(length);
        for (int j = 0; j < length; j++)
        {
            std::cin >> array[j];
        }
        if (solve(0, length, x)) printf("YES\n");
        else printf("NO\n");
    }
}

bool solve(int start, int end, int val)
{
    bool left = true;
    bool right = true;
    if (end - start == 1)
    {
        return (array[start] == val);
    }

    int mid = (start + end) >> 1;

    bool check_l = true;
    for (int i = start; i < mid; i++)
    {
        if (array[i] != val)
        {
            check_l = false;
            break;
        }
    }
    
   
    bool check_r = true;
    for (int i = mid; i < end; i++)
    {
        if (array[i] != val)
        {
            check_r = false;
            break;
        }
    }
    if (!check_l && !check_r)
    {
        return false;
    }
    if (check_l) right = solve(mid, end, val + 1);
    if (check_r) left = solve(start, mid, val + 1);
    

    if (left && right)
    {
        return true;
    }
    else return false;

}