#include <iostream>
#include <vector>
using namespace std;

int const MOD = 1e6 + 3;    

int main(void)
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, k;
    vector<int> s;
    vector<int> dp;

    cin >> n >> k;
    s.resize(k);
    dp.resize(n + 1);
    for (int i = 0; i < k; i++) cin >> s[i];
    
    dp[0] = 0;
    for (int i = 1; i <= n; i++)
    {

    }
}

// 5
// 1 2 3

// 1 1 1 1 1
// 1 1 1 2
// 1 1 2 1
// 1 2 1 1
// 2 1 1 1
// 


// dp[1] = 1 : 1
// dp[2] = 1 : 1 1
// dp[3] = 3 : 1 2, 2 1, 1 1 1
// dp[4] = 6 : 1 1 1 1, 1 1 2, 1 2 1, 2 1 1, 1 3, 3 1