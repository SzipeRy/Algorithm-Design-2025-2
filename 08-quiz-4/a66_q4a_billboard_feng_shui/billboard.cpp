#include <iostream>
#include <vector>
#include <algorithm>

void solve(int idx, int sum);
bool backtrack(int idx);

int n; int k;
std::vector<int> c;
std::vector<int> p;
std::vector<int> tail;
std::vector<int> pick;

int out = -1;

int main(void)
{

    std::cin >> n >> k;
    c.resize(n);
    p.resize(k);
    tail.resize(n);
    pick.resize(n);
    for (int i = 0; i < n; i++)
    {
        std::cin >> c[i];
    }
    for (int i = 0; i < k; i++)
    {
        std::cin >> p[i];
    }
    tail = c;
    for (int i = n-2; i >= 0; i--)
    {
        tail[i] = tail[i+1] + c[i];
    }
    solve(0,0);
    std::cout << out;
}

void solve(int idx, int sum)
{
    if (backtrack(idx)) return;
    if (sum+tail[idx] <= out) return;
    if (idx == n)
    {
        out = std::max(out, sum);
    }
    pick[idx] = 1;
    solve(idx+1,sum + c[idx]);
    pick[idx] = 0;
    solve(idx+1, sum);
}

bool backtrack(int idx)
{
    if (idx > 1)
    {
        if (pick[idx-1]&&pick[idx-2]) return true;
    }
    if (idx < k) return false;
    for (int i = 0; i != k; i++) if(pick[idx-i-1] != p[k-i-1]) return false;
    return true;
}