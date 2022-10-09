//
// Created by anatawa12 on 8/11/22.
//

#include <include/core/SkCanvas.h>
#include <include/core/SkTextBlob.h>
#include <include/core/SkTypeface.h>
#include <include/core/SkFontMgr.h>
#include <modules/skshaper/include/SkShaper.h>
#include "MainGuiRenderer.h"
#include "glutil.h"
#include "../global.h"
#include <array>
#include <filesystem>

namespace {

constexpr float sin45deg = 0.70710678118655;

inline std::array<SkPoint, 8> calcOffsets(float size) {
  float axis = 0.75f * size;
  float diagonal = axis * sin45deg;
  return {
      SkPoint{.0f, -axis},
      SkPoint{+diagonal, -diagonal},
      SkPoint{+axis, .0f},
      SkPoint{+diagonal, +diagonal},
      SkPoint{.0f, +axis},
      SkPoint{-diagonal, +diagonal},
      SkPoint{-axis, .0f},
      SkPoint{-diagonal, -diagonal},
  };
}

void renderRingChars(SkCanvas *canvas, sk_sp<SkTypeface> face, SkPoint center, float size,
                     std::function<std::pair<const std::u8string &, glm::vec3>(int)> getChar) {
  float fontSize = size * 0.4f;
  auto offsets = calcOffsets(size);

  for (int i = 0; i < 8; ++i) {
    auto pair = getChar(i);

    // TODO: fix text does not rendered
    //auto text = SkTextBlob::MakeFromString((char *)pair.first.c_str(), SkFont(nullptr, fontSize));
    auto text = SkTextBlob::MakeFromString("hello", SkFont(nullptr, fontSize));

    auto textCenterPos = center + offsets[i];
    SkPaint textPaint;
    textPaint.setColor(SkColor4f{pair.second.r, pair.second.g, pair.second.b, 1.0f});
    //*
    textPaint.setColor(SkColor4f{pair.second.r, pair.second.g, pair.second.b, .5f});
    SkRect rect {
        .fLeft = textCenterPos.x() - fontSize / 2,
        .fTop = textCenterPos.y() - fontSize / 2,
        .fRight = textCenterPos.x() + fontSize / 2,
        .fBottom = textCenterPos.y() + fontSize / 2,
    };
    canvas->drawRect(rect, textPaint);
    //*/
    canvas->drawTextBlob(text.get(), textCenterPos.x(), textCenterPos.y(), textPaint);
    // */

    //renderer.addCenteredStringWithMaxWidth(
    //    pair.first, center + offsets[i], pair.second,
    //    {fontSize, fontSize}, fontSize,
    //    CenteredMode::Both);
  }
}

}

std::unique_ptr<MainGuiRenderer> MainGuiRenderer::create(glm::ivec2 size) {
  //auto ftRenderer = FreetypeRenderer::create();
  auto backgroundRingRenderer = BackgroundRingRenderer::create();
  auto cursorCircleRenderer = CursorCircleRenderer::create();

  std::cout << "loading fonts" << std::endl;
  for (const auto &entry: std::filesystem::directory_iterator(getResourcesDir() / "fonts")) {
    if (entry.path().extension() == ".otf" || entry.path().extension() == ".ttf") {
      if (SkFontMgr::RefDefault()->makeFromFile(entry.path().string().c_str())) {
        std::cout << "loaded font:" << entry.path() << std::endl;
      } else {
        std::cout << "load font failed:" << entry.path() << std::endl;
      }
      //ftRenderer->addFontType(entry.path().string().c_str());

    }
  }
  auto face = SkTypeface::MakeFromFile((getResourcesDir() / "fonts" / "NotoSansJP-Medium.otf").string().c_str());

  auto res = new MainGuiRenderer{
      .size = size,

      .backgroundRingRenderer = std::move(backgroundRingRenderer),
      .face = std::move(face),
      .cursorCircleRenderer = std::move(cursorCircleRenderer),
      //.ftRenderer = std::move(ftRenderer),
  };
  return std::unique_ptr<MainGuiRenderer>(res);
}

void MainGuiRenderer::drawRing(
    const KeyboardStatus &status,
    LeftRight side,
    bool alwaysShowInCircle,
    const RingOverlayConfig &config,
    SkSurface& surface
) {
  // clear to transparent
  surface.getCanvas()->clear(SK_ColorTRANSPARENT);
  check_gl_err("drawRing: clear");

  int8_t selectingCurrent = status.getSelectingOfCurrentSide(side);
  int8_t selectingOpposite = status.getSelectingOfOppositeSide(side);

  auto stickPos = status.getStickPos(side);

  auto center = SkPoint {float(surface.width()) / 2, float(surface.height()) / 2};
  auto radius = float(surface.width()) / 2;

  backgroundRingRenderer->draw(
      surface.getCanvas(),
      center,
      radius,
      {config.centerColor, 1},
      {config.backgroundColor, 1},
      {config.edgeColor, 1}
  );
  check_gl_err("drawRing: background");


  int lineStep = side == LeftRight::Left ? 8 : 1;
  int lineLen = side == LeftRight::Left ? 1 : 8;

  auto getColor = [selectingCurrent, &config](int idx) {
    return selectingCurrent == -1
           ? config.normalCharColor
           : idx == selectingCurrent
             ? config.selectingCharColor
             : config.unSelectingCharColor;
  };

  if (alwaysShowInCircle || selectingOpposite == -1) {
    auto offsets = calcOffsets(radius);
    for (int pos = 0; pos < 8; ++pos) {
      int colOrigin = lineStep * pos;
      auto ringColor = getColor(pos);
      auto ringSize = (pos == selectingCurrent ? 0.22f : 0.2f) * radius;
      renderRingChars(surface.getCanvas(), this->face, offsets[pos] + center, ringSize, [=](int idx) -> std::pair<const std::u8string &, glm::vec3> {
        return {status.method->getTable()[colOrigin + lineLen * idx], ringColor};
      });
    }
  } else {
    int lineOrigin = lineLen * selectingOpposite;
    renderRingChars(surface.getCanvas(), this->face, center, radius,
                    [=, &status, &getColor](auto idx) -> std::pair<const std::u8string &, glm::vec3> {
                      return {status.method->getTable()[lineOrigin + lineStep * idx], getColor(idx)};
                    });
  }

  cursorCircleRenderer->draw(surface.getCanvas(), center, radius, stickPos);

  SkPaint paint2;
  auto text = SkTextBlob::MakeFromString("Hello, Skia!", SkFont(face, 18));
  surface.getCanvas()->drawTextBlob(text.get(), 50, 25, paint2);

  check_gl_err("main gui rendering");
}

void MainGuiRenderer::drawCenter(
    const KeyboardStatus &status,
    const CompletionOverlayConfig &config,
    SkSurface& surface
) {
  auto canvas = surface.getCanvas();
  canvas->clear(SkColor4f{config.backgroundColor.r, config.backgroundColor.g, config.backgroundColor.b, 1.0f});

  auto text = SkTextBlob::MakeFromString((char*)status.method->getBuffer().c_str(), SkFont(face, float(surface.height()) * 0.5f));

  SkPaint textPaint;
  textPaint.setColor(Color4fFromVec3(config.inputtingCharColor));

  canvas->drawTextBlob(text.get(), float(surface.height()) * 0.15f, float(surface.height()) * 0.7f, textPaint);

  check_gl_err("main gui rendering");
}
