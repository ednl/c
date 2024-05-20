from itertools import combinations
from math import ceil

k = 5       # number of unique tetrahedral numbers
S = 343867  # that sum to S

N = ceil(pow(S * 6, 1/3))
index = list(range(N))
tetra = [0] * N
for n in range(N):
    tetra[n] = n * (n + 1) * (n + 2) // 6

sol = 0  # solutions found
for c in combinations(index, k):
    t = [tetra[i] for i in c]
    if (sum(t) == S):
        sol += 1
        print(f'{sol}: index={c} tetra={t} sum={S}')

print(f'Found {sol} unique combinations of tetrahedral numbers that sum to {S}.')
