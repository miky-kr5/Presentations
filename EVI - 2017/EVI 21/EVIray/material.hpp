#pragma once
#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glm/vec3.hpp>

#include "brdf.hpp"
#include "phong_brdf.hpp"

using glm::vec3;

class Material {
public:
  vec3 m_emission;   // Light emission color.
  vec3 m_diffuse;    // Diffuse color.
  vec3 m_specular;   // Specular reflection color.
  float m_rho;       // Reflection coefficient.
  float m_shininess; // Shininess.
  float m_ref_index; // Refraction index.
  bool m_refract;    // Refraction enabled.
  BRDF * m_brdf;     // BRDF.

  /**
   * Default material constructor.
   */
  Material(BRDF * _brdf = NULL):
    m_emission(vec3(0.0f)),
    m_diffuse(vec3(1.0f)),
    m_specular(vec3(1.0f)),
    m_rho(0.0f),
    m_shininess(89.0f),
    m_ref_index(1.0f),
    m_refract(false)
  {
    m_brdf = _brdf != NULL ? _brdf : static_cast<BRDF *>(new PhongBRDF());
  }

  ~Material() {
    delete m_brdf;
  }
  
  /**
   * Copy constructor.
   */
  Material(const Material & m) {
    m_diffuse = m.m_diffuse;
    m_specular = m.m_specular;
    m_rho = m.m_rho;
    m_shininess = m.m_shininess;
    m_ref_index = m.m_ref_index;
    m_refract = m.m_refract;
    m_brdf = m.m_brdf;
  }
};

#endif
