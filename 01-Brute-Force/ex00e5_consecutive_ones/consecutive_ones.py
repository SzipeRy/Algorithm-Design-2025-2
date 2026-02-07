def combination(n, solution, c, check):
    if (len(solution) < n):
        sol_a = solution + "0"
        combination(n, sol_a, c, check)
        sol_b = solution + "1"
        combination(n, sol_b, c, check)
    else:
        if (check in solution): print(solution)

n, c = [int(e) for e in input().split()]
solution = "";
check = "1"*c
combination(n, solution, c, check)