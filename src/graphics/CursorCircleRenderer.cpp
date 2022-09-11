//
// Created by anatawa12 on 2022/09/03.
//

#include "CursorCircleRenderer.h"
#include "glutil.h"

std::unique_ptr<CursorCircleRenderer> CursorCircleRenderer::create() {
  gl::Program program = std::move(compile_shader_program(
      "#version 330 core\n"
      "layout(location = 0) in vec2 position;\n"
      "uniform vec2 uCenter;"
      "uniform vec2 uSize;"
      "out vec2 xy;\n"
      "void main() {\n"
      "    gl_Position.xy = position * uSize + uCenter;\n"
      "    xy = position * 2;\n"
      "}\n",
      "#version 330 core\n"
      "in vec2 xy;\n"
      "// uniforms\n"
      "uniform vec4 uStickColor;\n"
      "uniform vec2 uStickPos;\n"
      "// Ouput data\n"
      "out vec4 color;\n"
      "\n"
      "void main() {\n"
      "    vec2 diff = xy - uStickPos / 3;\n"
      "    float len_sqrt = dot(diff, diff);\n"
      "    color = len_sqrt < (0.25 * 0.25) ? uStickColor : vec4(0, 0, 0, 0);\n"
      "}\n"
  ));
  gl::VertexAttrib vertexPositionAttrib(program, "position");

  gl::VertexArray vertexArray;
  gl::ArrayBuffer vertexBuffer;

  static const GLfloat g_vertex_buffer_data[] = {
      -.5f, -.5f,
      +.5f, -.5f,
      +.5f, +.5f,

      -.5f, -.5f,
      +.5f, +.5f,
      -.5f, +.5f,
  };
  gl::Bind(vertexBuffer);
  vertexBuffer.data(sizeof(g_vertex_buffer_data), g_vertex_buffer_data, gl::kStaticDraw);

  gl::Bind(program);
  // transform
  gl::Uniform<glm::vec2> uCenter(program, "uCenter");
  gl::Uniform<glm::vec2> uSize(program, "uSize");
  // colors
  gl::Uniform<glm::vec4> uStickColor(program, "uStickColor");
  gl::Uniform<glm::vec2> uStickPos(program, "uStickPos");

  auto res = new CursorCircleRenderer{
      .program = std::move(program),
      .vertexPositionAttrib = std::move(vertexPositionAttrib),
      .vertexArray = std::move(vertexArray),
      .vertexBuffer = std::move(vertexBuffer),
      .uCenter = std::move(uCenter),
      .uSize = std::move(uSize),
      .uStickColor = std::move(uStickColor),
      .uStickPos = std::move(uStickPos),
  };
  return std::unique_ptr<CursorCircleRenderer>(res);
}

void CursorCircleRenderer::draw(
    glm::vec2 center,
    glm::vec2 size,
    glm::vec2 stick,
    glm::vec4 color
) {
  gl::Bind(vertexArray);
  gl::Use(program);

  uCenter.set(center);
  uSize.set(size);
  uStickPos.set(stick);
  uStickColor.set(color);

  vertexPositionAttrib.enable();
  gl::Bind(vertexBuffer);
  vertexPositionAttrib.pointer(2, gl::kFloat, false, 0, nullptr);
  gl::DrawArrays(gl::kTriangles, 0, 6);
  vertexPositionAttrib.disable();

  check_gl_err("drawing background gui");
}