#include <iostream>
#include <vector>
#include <cmath>

std::vector<int> array;

bool solve(int start, int end, int val);

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(0);
    int n;
    int k, x;
    int length;
    std::cin >> n;
    for (int i = 0; i < n; i++)
    {
        std::cin >> k >> x;
        length = pow(2,k);
        array.clear();
        array.resize(length);
        for (int j = 0; j < length; j++)
        {
            std::cin >> array[j];
        }
        if (solve(0, length, x)) 
        {
            std::cout << "SH" << '\n';
        }
        else 
        {
            std::cout << "NG" << '\n';
        }
    }
}

bool solve(int start, int end, int val)
{
    bool right = false;
    bool left = false;
    if (end - start == 1)
    {
        return (array[start] == val);
    }

    int mid = (start + end) >> 1;
    bool l_check = true;
    bool r_check = true;
    
    for (int i = start; i < mid; i++)
    {
        if (array[i] != val) 
        {
            l_check = false; 
            break;
        }
    }
    if (l_check)
    {
        right = solve(mid, end, val + 1);
    }
    if (right)
    {
        return true;
    }
    for (int i = mid; i < end; i++)
    {
        if (array[i] != val) 
        {
            r_check = false;
            break;
        }
    }
    if (r_check)
    {
        left = solve(start, mid, val + 1);
    }
    if (left)
    {
        return true;
    }
    return false;
}