import random as r

def qsort(l):
    if len(l) == 0:
        return []
    elif len(l) == 1:
        return l
    else:
        less = qsort([x for x in l[1:] if x <= l[0]])
        more = qsort([x for x in l[1:] if x > l[0]])
        return less + [l[0]] + more

a = [r.randint(0, 100) for x in xrange(100)]
b = qsort(a)
print a
print b
