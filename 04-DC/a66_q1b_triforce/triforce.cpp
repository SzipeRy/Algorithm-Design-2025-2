#include <iostream>
#include <vector>

int n;
int p;
std::vector<std::vector<int>> array;

int solve(int rstart, int rend, int cstart, int cend);

int main(void)
{
    std::ios_base::sync_with_stdio(false); std::cin.tie(NULL);
    for (int i = 0; i < 3; i++)
    {
        std::cin >> n;
        array.clear();
        array.resize(n, std::vector<int>(n));
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < n; k++)
            {
                std::cin >> p;
                array[j][k] = p;
            }
        }
        std::cout << solve(0,n,0,n) << '\n';
    }
}

int solve(int rstart, int rend, int cstart, int cend)
{
    int rmid = (rstart + rend) >> 1;
    int cmid = (cstart + cend) >> 1;

    if (rend - rstart == 2 && cend - cstart == 2)
    {
        if (array[rend-1][cend-1] != 0) return 0;
        if (array[rstart][cstart] == 0 || array[rstart][cmid] == 0 || array[rmid][cstart] == 0) return 0;
        if (array[rstart][cstart] == array[rstart][cmid]) return array[rstart][cstart];
        if (array[rstart][cstart] == array[rmid][cstart]) return array[rstart][cstart];
        if (array[rstart][cmid] == array[rmid][cstart]) return array[rstart][cmid];
        return 0;
    }

    // check the bottom right
    for (int i = rmid; i < rend; i++)
    {
        for (int j = cmid; j < cend; j++)
        {
            if (array[i][j] != 0) return 0;
        }
    }

    int up_left = solve(rstart, rmid, cstart, cmid);
    int up_right = solve(rstart, rmid, cmid, cend);
    int bot_left = solve(rmid, rend, cstart, cmid);

    if (up_left == 0 || up_right == 0 || bot_left == 0) return 0;
    if (up_left == up_right) return up_left;
    if (up_left == bot_left) return up_left;
    if (up_right == bot_left) return up_right;
    return 0;
}