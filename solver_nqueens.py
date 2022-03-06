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

    def fill_str_begin(self, arr):
        self._str_begin = "p cnf " + str(self._N * self._N) + " " + str(self._clauses)  + "\n"

    def write_out(self):
        sys.stdout.write(self._str_begin + self._rule)

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
    print("Usage: python3 " + sys.argv[0] + " _argN_ > name_input_file\n\tn must be greater then 3", file = sys.stderr)
    sys.exit(-1)

prg = Program(int(sys.argv[1]))

# each row, column and diagonal contain exactly one queen
get_combs_from_arr(prg._poss, prg)
get_combs_from_arr(np.transpose(prg._poss), prg)
get_combs_from_diag(prg._poss, 0, prg._N-1, prg)
get_combs_from_diag(np.transpose(prg._poss), 1, prg._N-1, prg)
get_combs_from_diag(np.fliplr(prg._poss), 0, prg._N-1, prg)
get_combs_from_diag(np.transpose(np.fliplr(prg._poss)), 1, prg._N-1, prg)

prg.fill_str_begin(prg._poss)
prg.write_out()
