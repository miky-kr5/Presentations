a = [1, 2, 3, 4, 5, 6, 7, 8, 9]

#################
# List indexing #
#################

# Classical position indexing

i = 0
while i < len(a):
    print(a[i])
    i += 1

# Negative indices

print(a[-1])
print(a[-2])
print(a[-3])

################
# List slicing #
################

# Elements between indices 3 and 7

print(a[3:7])

# Elements from index 5 onwards

print(a[5:])

# Elements from the start up to index 8

print(a[:8])

#######################
# List comprehensions #
#######################

# The first 10 square natural numbers

l = [x * x for x in range(1, 10)]
print(l)

# The first even square natural numbers

l = [x * x for x in range(1, 10) if (x * x) % 2 == 0]
print(l)

# Some numbers from the first list

l = [x for x in a if x > 2 and x < 7]
print(l)

#######
# zip #
#######

l = [x for x in zip([1, 2, 3, 4], ["a", "b", "c"], ["Hello", ",", "World", "!"])]
print(l)
