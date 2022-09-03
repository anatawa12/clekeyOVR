//
// Created by anatawa12 on 8/11/22.
//

#ifndef CLEKEY_OVR_GLUTIL_H
#define CLEKEY_OVR_GLUTIL_H

#include "GL/glew.h"
#include "oglwrap/oglwrap.h"

inline gl::Program compile_shader_program(const char *vertex_shader_src, const char *fragment_shader_src) {
  gl::Shader vertex(gl::kVertexShader);
  gl::Shader fragment(gl::kFragmentShader);
  vertex.set_source(vertex_shader_src);
  vertex.compile();
  fragment.set_source(fragment_shader_src);
  fragment.compile();
  gl::Program program(vertex, fragment);
  program.link();
  return program;
}

#define check_gl_err(func) check_gl_err_impl(__LINE__, func)

inline void check_gl_err_impl(int line, const char *func) {
  GLenum err;
  while ((err = glGetError())) {
    std::cerr << "err #" << line;
    if (func && *func) {
      std::cerr << "(" << func << ")";
    }
    std::cerr << ": 0x" << std::hex << err << std::dec << ": " << gluErrorString(err) << std::endl;
  }
}

#endif //CLEKEY_OVR_GLUTIL_H