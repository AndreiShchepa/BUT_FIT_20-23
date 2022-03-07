# Andrei Shchapaniak, xshcha00

# Brief description of how the program works and how I came to this solution

# The main idea of this task is determine what rules are used in it.
# 3 rules whaich i used:
#   1. exactly one queen per row
#   2. exactly one queen per column
#   3. at most one queen on diagonal

# Before explaining functions I am goint to show, how I made logical formulas
# for "exactly one" and "at most one".

# Consider all possible ways in which it would be false that exactly one of the variables is true. 
# So that would be when they are either all false, or when two or more of them are true:
# (~X1 & ~X2 & ... & ~Xn) | (X1 & X2) | (X1 & X3) | (X2 & X3) | ... | (X(n-1) & Xn)
# We want a negation of this:
#   ~((~X1 & ~X2 & ... & ~Xn) | (X1 & X2) | (X1 & X3) | (X2 & X3) | ... | (X(n-1) & Xn)) = 
#   = (X1 | X2 | ... | Xn) & (~X1 | ~X2) & (~X1 | ~X3) & (~X2 | ~X3) & ... & (~X(n-1) | ~Xn)
# We get a Conjunctive Normal Form. That is actually what we need.

# We can do the same with "at most one", and we get finally next formula:
#   (~X1 | ~X2) & (~X1 | ~X3) & (~X2 | ~X3) & ... & (~X(n-1) | ~Xn)
# It is also in a Conjunctive Normal Form.

# Function get_combs_from_arr() creates a CNF formula for one queen per row (column).
# It takes as argument matrix (chessboard) and in loop creates formal for each row.
# For columns function takes a transpose of matrix. (Some tricks from linear algebra)
# 1  2  3  4                          1 5 9  13
# 5  6  7  8      ==(transpose)==     2 6 10 14
# 9  10 11 12     ==(transpose)==     3 7 11 15
# 13 14 15 16                         4 8 12 16

# Function get_combs_from_diag() creates a CNF formula for at most one queen per diagonal.
# It is so harder than working with rows and columns, however, tricks from linear algebra 
# and numpy library can help us.

# First, we traverse all possible diagonals of length greater than 1 from left to right. 
# Then all possible diagonals from right to left. 
# For this, a function np.fliplr() was used that returns the desired diagonal.

from itertools import combinations
import sys
import numpy as np

class Program():
    def __init__(self, N):
        self._clauses = 2*N
        self._rule = ""
        self._poss = self.__create_arr(N)
        self._N = N
        self._str_begin = ""

    def __create_arr(self, N):
        poss = list(map(str, list(range(1, N*N + 1))))
        poss = np.reshape(poss, (N, N))
        return poss

def get_combs_from_arr(arr, prg):
    for i in range(prg._N):
        prg._rule += " ".join(arr[i]) + " 0\n"
        combs = combinations(arr[i], 2)
        for j in combs:
            prg._clauses += 1
            prg._rule += "-"+j[0] + " -"+j[1] + " 0\n"

def get_combs_from_diag(arr, start, end, prg):
    for i in range(start, end):
        tmp = arr.diagonal(i)
        combs = combinations(tmp, 2)
        for j in combs:
            prg._clauses += 1
            prg._rule += "-"+j[0] + " -"+j[1] + " 0\n"

if len(sys.argv) != 2 or int(sys.argv[1]) < 4:
    print("Usage: python3 " + sys.argv[0] + \
            " _argN_\n\t_argN_ must be greater than 3", file = sys.stderr)
    sys.exit(-1)

prg = Program(int(sys.argv[1]))

get_combs_from_arr(prg._poss, prg)
get_combs_from_arr(np.transpose(prg._poss), prg)
get_combs_from_diag(prg._poss, 0, prg._N-1, prg)
get_combs_from_diag(np.transpose(prg._poss), 1, prg._N-1, prg)
get_combs_from_diag(np.fliplr(prg._poss), 0, prg._N-1, prg)
get_combs_from_diag(np.transpose(np.fliplr(prg._poss)), 1, prg._N-1, prg)

prg._str_begin = "p cnf " + str(prg._N ** 2) + " " + str(prg._clauses)  + "\n"
sys.stdout.write(prg._str_begin + prg._rule)
