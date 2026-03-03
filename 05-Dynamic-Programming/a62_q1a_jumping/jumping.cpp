#include <iostream>
#include <vector>

int main(void)
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::vector<int> block;
    std::vector<int> score;
    int n;

    std::cin >> n;
    block.resize(n);
    score.resize(n);
    for (int i = 0; i < n; i++)
    {
        std::cin >> block[i];
    }
    score[0] = block[0];
    score[1] = block[0] + block[1];
    score[2] = std::max(score[1] + block[2], block[0] + block[2]);

    for (int i = 3; i < n; i++)
    {
        score[i] = std::max(std::max(score[i-3],score[i-2]),score[i-1]) + block[i];
    }
    std::cout << score[n - 1] << '\n';
}