#include <iostream>
#include <queue>
#include <utility>

int const TARGET = 16777216;

// First Operation : x = (x + 1) % TARGET
// Second Operation : x = (x * 2) % TARGET
// If the result of operation == 0, return the number of operation that has been done

int main(void)
{
    int x;
    bool pass = false;
    std::queue<std::pair<int, int>> process;

    std::cin >> x;
    if (x == 0)
    {
        std::cout << 0 << '\n';
        return 0;
    }
    std::pair<int,int> tmp = std::make_pair(x, 0);
    process.push(tmp);
    while(true)
    {
        auto tmp = process.front();
        process.pop();
        if (tmp.first % TARGET == 0)
        {
            std::cout << tmp.second << '\n';
            break;
        }
        auto op1 = std::make_pair(tmp.first + 1, tmp.second + 1);
        process.push(op1);
        auto op2 = std::make_pair(tmp.first * 2, tmp.second + 1);
        process.push(op2);
    }

    return 0;
}