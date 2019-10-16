#pragma once
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ray.hpp"

using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::normalize;
using glm::lookAt;
using glm::inverse;

// Camera vector disposition:
//   up^
//     |
//     |
//  eye)-------->* look
//
class Camera {
public:
  vec3 m_eye;  // Eye position.
  vec3 m_look; // Look-up position.
  vec3 m_up;   // Up vector.

  Camera(vec3 _e = vec3(0.0f), vec3 _l = vec3(0.0f, 0.0f, -1.0f), vec3 _u = vec3(0.0f, 1.0f, 0.0f)):
    m_eye(_e),
    m_look(_l),
    m_up(normalize(_u)),
    m_inv_view_matrix(inverse(lookAt(_e, _l, normalize(_u))))
  { }

  /**
   * Restores the view matrix undoing any transformation.
   */
  void reset();

  /**
   * Translate the eye position.
   */
  void translate(vec3 t);

  /**
   * Rotate view direction around left vector (cross product of up vector and view direction).
   */
  void pitch(float angle);

  /**
   * Rotate view direction around up vector.
   */
  void yaw(float angle);

  /**
   * Rotate up vector around view direction.
   */
  void roll(float angle);

  /**
   * Apply the inverse view matrix to a ray to take it from view space to world space.
   */
  void view_to_world(Ray & r) const;

private:
  // Inverse view matrix.
  mat4 m_inv_view_matrix;
};

#endif
