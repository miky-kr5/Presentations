#pragma once
#ifndef TRACER_HPP
#define TRACER_HPP

#include <vector>

#include <glm/glm.hpp>

#include "ray.hpp"
#include "scene.hpp"

using std::vector;
using glm::vec2;
using glm::vec3;

// Intersection displacement bias.
extern const float BIAS;

// Default background color.
extern const vec3 BCKG_COLOR;

/**
 * Base class for ray tracers.
 */
class Tracer {
public:
  unsigned int m_max_depth; // Max recursion depth.

  Tracer(unsigned int max_depth = 5): m_max_depth(max_depth) { }

  virtual ~Tracer() { }

  /**
   * Recursively trace a ray into a scene, up until the max recursion depth.
   */
  virtual vec3 trace_ray(Ray & r, Scene * s, unsigned int rec_level) const = 0;

protected:
  /**
   * Fresnel law for refractions.
   */
  float fresnel(const vec3 & i, const vec3 & n, const float ir1, const float ir2) const;
};

#endif
