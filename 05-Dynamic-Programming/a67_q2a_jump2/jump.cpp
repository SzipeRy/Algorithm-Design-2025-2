#include <iostream>
#include <vector>

int main(void)
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int n, k;
    std::vector<int> a;
    std::vector<int> b;
    std::vector<int> score;


    std::cin >> n >> k;
    a.resize(n);
    b.resize(k);
    score.resize(n);
    for (int i = 0 ; i < n; i++)
    {
        std::cin >> a[i];
    }
    for (int i = 0 ; i < k; i++)
    {
        std::cin >> b[i];
    }
    
    score[0] = a[0];
    for (int i = 1; i < n; i++)
    {
        int tmp = -1000000;
        for (int p = 1; p <= k; p++)
        {
            if (i - p < 0) break;
            tmp = std::max(tmp, std::max(score[i-p] - b[p-1] + a[i],tmp));
        }
        score[i] = tmp;
    }
    std::cout << score[n-1] << '\n';
}