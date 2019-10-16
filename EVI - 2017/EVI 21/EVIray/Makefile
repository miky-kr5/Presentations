CXX = g++
TARGET = ray
OBJECTS = main.o sampling.o camera.o plane.o sphere.o \
          phong_brdf.o directional_light.o point_light.o \
          spot_light.o sphere_area_light.o scene.o tracer.o \
          path_tracer.o whitted_tracer.o
DEPENDS = $(OBJECTS:.o=.d)
CXXFLAGS = -std=c++11 -pedantic -Wall -DGLM_FORCE_RADIANS -DUSE_CPP11_RANDOM -fno-builtin -pg
LDLIBS = -lfreeimage -ljson_spirit

.PHONY: all
all: CXXFLAGS += -O3 -DNDEBUG
all: $(TARGET)

.PHONY: debug
debug: CXXFLAGS += -g
debug: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

-include $(DEPENDS)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $*.cpp -o $*.o
	$(CXX) -MM $(CXXFLAGS) $*.cpp > $*.d

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJECTS) $(DEPENDS)
