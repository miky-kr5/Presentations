#####################
# Atomic data types #
#####################

a = 89       # Integer
b = 0.5      # Float
c = 9 + 0.4j # Complex
d = True     # Boolean
e = None     # Nonetype

########################
# Composite data types #
########################

f = [1, '2', 3]    # List
g = (1, '2', 3)    # Tuple
h = {1: 'one',     # Dict
     'two': 2,
     3: 'Three'}
i = set([1, 2, 3]) # Set
j = frozenset(f)   # Frozen set

######################
# Control structures #
######################

# If
if 1 < 2:
    print("It's true!")
elif 2 < 1:
    print("It's false!")
else:
    print("It's neither!")

# For
for i in f:
    print(i)

# While
while a > 80:
    print(a)
    a -= 1

########################
# Function definitions #
########################

# No arguments

def fun():
    return 89

fun()

# Functions without a return statement return None

def non():
    pass

print(non())

# Positional arguments

def sum(a, b):
    return a + b

print(sum(1, 2))

# Keyword arguments

def divide(dividend = 1, divisor = 1):
    return dividend / divisor # Unsafe!!

print(divide())
print(divide(divisor = 2))
print(divide(dividend = 4, divisor = 2))
print(divide(divisor = 4, dividend = 2))

# Both argument types

def xnp(x, n, p = 1):
    # Keyword args MUST appear AFTER positional args!
    return (x * n) + p

# Variable length arguments

def varargs(*args):
    for a in args:
        print("Argument " + str(args.index(a)) + " is " + str(a))

varargs(1)
varargs(2, 3)
varargs(4, 5, 6)

# Variable keyword arguments

def varkwargs(**kwargs):
    for k in kwargs.keys():
        print('Argument "' + str(k) + '" is "' + str(kwargs[k]))

varkwargs(a = 1, b = 2, c = 3)

# Everything!

def allargs(a, b, c = None, *args, **kwargs):
    print("a is " + str(a))
    
    print("b is " + str(b))
    
    if c is None:
        print("c is None")
    else:
        print("c is Some")
        
    for a in args:
        print("Argument " + str(args.index(a)) + " is " + str(a))
        
    for k in kwargs.keys():
        print('Argument "' + str(k) + '" is "' + str(kwargs[k]))

allargs('a', 1, None, 2, 3, 4, q = "Hail", w = "Caesar!")
allargs(1, 2, c = 89)

# Nested functions

def outer(x):
    def inner(y):
        return x + y

    return inner(9)

print(outer(1))
