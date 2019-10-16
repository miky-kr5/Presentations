####################
# Generator object #
####################

def simple_generator():
    i = 0
    while(i < 10):
        yield i
        i += 1
    raise StopIteration

g = simple_generator()

for i in g:
    print(i)

try:
    print(next(g))
except StopIteration:
    print("Generator exhausted!")
else:
    print("Generator success!")
finally:
    print("At the end!")

####################
# Collection class #
####################

class Collection(object):
    def __init__(self):
        self.c = []

    def add(self, x):
        self.c.append(x)

    def __iter__(self):
        for i in self.c:
            yield i

c = Collection()
c.add(1)
c.add(2)
c.add(3)
for i in c:
    print(i)
