from math import pi, pow, sin, sqrt
from random import seed, random

def veclen(vec):
    return vec[1] - vec[0]

def randseg(vec):
    return vec[0] + random() * veclen(vec)

def f(x):
    return pow(x, x) * sin(x)

def g(x):
    return sin(x) * 4

# Area of interest
span = [[2, pi], [0, 7]]
rect = veclen(span[0]) * veclen(span[1])
maxerr = 0.005

# Init
seed()
chunk = 10000
hit = 0
count = 0
mean = 0
m2 = 0

while True:
    for _ in range(chunk):
        count += 1
        # Probe point inside square
        x = randseg(span[0])
        y = randseg(span[1])
        # Evaluate hit or miss
        if y <= f(x) and y >= g(x):
            hit += 1
        # Currently estimated value
        val = hit / count * rect
        # Update running mean and unscaled variance
        delta = val - mean
        mean += delta / count
        m2 += delta * (val - mean)
    # Scale variance & take square root for standard deviation
    std = sqrt(m2 / count)
    if std <= maxerr:
        break

print('N={}: area = {:.3f} +/- {:.3f}'.format(count, mean, std))
