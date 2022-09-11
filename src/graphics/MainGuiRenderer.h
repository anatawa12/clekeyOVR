//
// Created by anatawa12 on 8/11/22.
//

#ifndef CLEKEY_OVR_MAINGUIRENDERER_H
#define CLEKEY_OVR_MAINGUIRENDERER_H

#include <GL/glew.h>
#include "oglwrap/oglwrap.h"
#include "BackgroundRingRenderer.h"
#include "CursorCircleRenderer.h"
#include "../OVRController.h"
#include "FreetypeRenderer.h"
#include "RingRenderer.h"

class MainGuiRenderer {
public:
  static std::unique_ptr<MainGuiRenderer> create(int width, int height);

  void draw(const OVRController &controller, LeftRight side);

  int width, height;

  gl::Texture2D dest_textures[2];
  gl::Renderbuffer depth_buffer;
  gl::Framebuffer frame_buffer;

  std::unique_ptr<BackgroundRingRenderer> backgroundRingRenderer;
  std::unique_ptr<CursorCircleRenderer> cursorCircleRenderer;
  std::unique_ptr<FreetypeRenderer> ftRenderer;
  std::unique_ptr<RingRenderer> ringRenderer;
};

#endif //CLEKEY_OVR_MAINGUIRENDERER_H
