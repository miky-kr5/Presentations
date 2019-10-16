#pragma once
#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <FreeImage.h>
#include <glm/vec3.hpp>

#include "ray.hpp"

using glm::vec3;

class Environment {
public:
  Environment(vec3 bckg = vec3(1.0f)): m_bckg_color(bckg) { }

  ~Environment() { }

  vec3 get_color(Ray & r) {
    return m_bckg_color;
  }

private:
  vec3 m_bckg_color;
};

#endif
