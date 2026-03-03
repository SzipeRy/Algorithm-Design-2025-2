/*
    This following code is from Algorithm Design 4-8: Modular Exponential by Nattee115
*/

#include <iostream>

long mod_expo(long a, long n, long k);

int main(void)
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    long a, n, k;
    std::cin >> a >> n >> k;
    std::cout << mod_expo(a, n, k) << '\n';
    return 0;
}

long mod_expo(long a, long n, long k)
{
    if (n <= 1)
    {
        return a % k;
    }
    if (n % 2 == 0)
    {
        long tmp = mod_expo(a, n/2, k);
        return (tmp * tmp) % k;
    }
    else
    {
        long tmp = mod_expo(a, n/2, k);
        tmp = (tmp * tmp) % k;
        return (tmp * (a % k)) % k;
    }
}