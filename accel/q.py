# Main paper: http://www.ncbi.nlm.nih.gov/pmc/articles/PMC4346101/

from sympy import symbols
from sympy.matrices import Matrix


a1, b1, c1 = symbols('a1 b1 c1')
a2, d2 = symbols('a2 d2')
a3, b3, c3 = symbols('a3 b3 c3')


def V(q):
    q0, q1, q2, q3 = q
    return Matrix([q0, q1, q2, q3])


def M(q):
    q0, q1, q2, q3 = q
    return Matrix([[q0, -q1, -q2, -q3], [q1, q0, -q3, q2],
                   [q2, q3, q0, -q1], [q3, -q2, q1, q0]])

M3 = M((a3, b3, c3, 0))
M2 = M((a2, 0, 0, d2))
Q1 = V((a1, b1, c1, 0))

Q = M3 * M2 * Q1

Q0, Q1, Q2, Q3 = Q

print(Q0)
print(Q1)
print(Q2)
print(Q3)
