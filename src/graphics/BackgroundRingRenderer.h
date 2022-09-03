//
// Created by anatawa12 on 2022/09/03.
//

#ifndef CLEKEY_OVR_BACKGROUNDRINGRENDERER_H
#define CLEKEY_OVR_BACKGROUNDRINGRENDERER_H


#include <GL/glew.h>
#include "oglwrap/oglwrap.h"

class BackgroundRingRenderer {
public:
  static BackgroundRingRenderer create();

  void draw(
      float x1, float y1,
      float x2, float y2,
      glm::vec4 center = glm::vec4(0.83, 0.83, 0.83, 1.0),
      glm::vec4 background = glm::vec4(0.686, 0.686, 0.686, 1.0),
      glm::vec4 edge = glm::vec4(1.0, 1.0, 1.0, 1.0)
  );

  gl::Program program;
  gl::VertexAttrib vertexPositionAttrib;
  gl::VertexArray vertexArray;
  gl::ArrayBuffer vertexBuffer;

  // transform
  gl::Uniform<glm::mat3> uMat;
  // colors
  gl::Uniform<glm::vec4> uCenter;
  gl::Uniform<glm::vec4> uBackground;
  gl::Uniform<glm::vec4> uEdge;
};


#endif //CLEKEY_OVR_BACKGROUNDRINGRENDERER_H
