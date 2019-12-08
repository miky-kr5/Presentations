################
# Simple class #
################

class Class(object):
    def __init__(self, a):
        self.a = a

    def method(self):
        return self.a

o = Class(1)
print(o.method())
print(o.a) # !

####################
# With inheritance #
####################

class Subclass(Class):
    def __init__(self, a, b):
        super(Subclass, self).__init__(a)
        self.b = b

    def method(self):
        return self.b
        
    def sub_method(self):
        return self.a

s = Subclass(1, 2)
print(s.method())
print(s.sub_method())
