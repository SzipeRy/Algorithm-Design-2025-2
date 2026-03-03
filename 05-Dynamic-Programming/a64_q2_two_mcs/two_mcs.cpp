#include <iostream>
#include <vector>

int kadane(std::vector<int> array, int start, int end, int &bestStart, int &bestEnd);

int main(void)
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int n;
    int bestStart, bestEnd;
    std::vector<int> array;

    std::cin >> n;
    array.resize(n);
    for (int i = 0; i < n; i++)
    {
        std::cin >> array[i];
    }
    int out1 = kadane(array, 0, array.size() - 1, bestStart, bestEnd);
    // std::cout << bestEnd << '\n';
    int out2 = kadane(array, bestEnd+1, array.size()-1, bestStart, bestEnd);

    // std::cout << out1 << ' ' << out2 << '\n';
    std::cout << out1 + out2 << '\n';
}

int kadane(std::vector<int> array, int start, int end, int &bestStart, int &bestEnd)
{
    if (start >= array.size() || end < 0 || start < 0) return 0;
    int suf = array[start];
    int mss = array[start];
    int tmp = start;
    for (int i = start + 1; i <= end; i++)
    {
        if (suf + array[i] < array[i])
        {
            suf = array[i];
            tmp = i;
        }
        else
        {
            suf += array[i];
        }
        if (suf > mss)
        {
            mss = suf;
            bestStart = tmp;
            bestEnd = i;
        }
    }
    return mss;
}