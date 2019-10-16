################################
# Lambda (anonymous) functions #
################################

f = lambda x, y: x + y
print(f(1, 2))
print()

##########################
# Higher order functions #
##########################

# Map: Applies a function to many lists. Returns a generator.

for i in map(lambda x: x*x, [1, 2, 3, 4, 5, 6]):
    print(i)
print()

for i in map(f, [1, 2, 3, 4, 5, 6], [2, 3, 4, 5, 6, 7]):
    print(i)
print()

# Custom higher order function

def hof(function = lambda x: x, *args):
    for a in args:
        print(function(a))

hof(lambda s: s.upper(), "a", "b", "c")
print()
hof(lambda x: x is None, 1, ["a", "b"], None, {})
