#include<algorithm>
#include<iostream>
#include<vector>

void combination(int n, std::vector<int> &sol, int len, int used, int max);

int main(void)
{
    int a, b;

    std::cin >> a >> b;
    std::vector<int> sol(b);
    combination(b, sol, 0, 0, a);

}

// used cannot be more than a
void combination(int n, std::vector<int> &sol, int len, int used, int max)
{
    if (len < n)
    {
        sol[len] = 0;
        combination(n, sol, len+1, used, max);
        if (used < max)
        {
            sol[len] = 1;
            ++used;
            combination(n, sol, len+1, used, max);
        }
    }
    else if (used == max)
    {
        for (auto &x : sol)
        {
            std::cout << x;
        }
        std::cout << '\n';
    }
}