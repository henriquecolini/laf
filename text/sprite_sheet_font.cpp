// LAF Text Library
// Copyright (c) 2024-2025  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "text/sprite_sheet_font.h"

#include "os/sampling.h"

namespace text {

void SpriteSheetFont::setSize(const float size)
{
  ASSERT(m_defaultSize > 0.0f);

  os::Sampling sampling;
  if (m_antialias)
    sampling = os::Sampling(os::Sampling::Filter::Linear, os::Sampling::Mipmap::Nearest);

  const float scale = size / m_defaultSize;
  m_sheet = m_originalSheet->applyScale(scale, sampling);
  m_size = size;

  m_glyphs = m_originalGlyphs;
  for (auto& rc : m_glyphs)
    rc = gfx::Rect(gfx::RectF(rc) * scale);
}

void SpriteSheetFont::fromSurface(os::SurfaceRef& sur, float size)
{
  m_originalSheet = sur;
  m_sheet = sur;
  m_originalGlyphs.push_back(gfx::Rect()); // glyph index 0 is MISSING CHARACTER glyph
  m_originalGlyphs.push_back(gfx::Rect()); // glyph index 1 is NULL glyph

  os::SurfaceLock lock(sur.get());
  gfx::Rect bounds(0, 0, 1, 1);
  gfx::Rect glyphBounds;

  while (findGlyph(sur.get(), sur->width(), sur->height(), bounds, glyphBounds)) {
    m_originalGlyphs.push_back(glyphBounds);
    bounds.x += bounds.w;
  }

  // Clear the border of all glyphs to avoid bilinear interpolation
  // with those borders when drawing this font scaled/antialised.
  os::Paint p;
  p.blendMode(os::BlendMode::Clear);
  p.style(os::Paint::Stroke);
  for (gfx::Rect rc : m_originalGlyphs) {
    sur->drawRect(rc.enlarge(1), p);
  }

  m_glyphs = m_originalGlyphs;

  m_defaultSize = getCharBounds(' ').h;
  if (m_defaultSize <= 0.0f)
    m_defaultSize = 1.0f;
  m_size = m_defaultSize;

  if (size > 0.0f)
    setSize(size);
}

bool SpriteSheetFont::findGlyph(const os::Surface* sur,
                                int width,
                                int height,
                                gfx::Rect& bounds,
                                gfx::Rect& glyphBounds)
{
  gfx::Color keyColor = sur->getPixel(0, 0);

  while (sur->getPixel(bounds.x, bounds.y) == keyColor) {
    bounds.x++;
    if (bounds.x >= width) {
      bounds.x = 0;
      bounds.y += bounds.h;
      bounds.h = 1;
      if (bounds.y >= height)
        return false;
    }
  }

  gfx::Color firstCharPixel = sur->getPixel(bounds.x, bounds.y);

  bounds.w = 0;
  while ((bounds.x + bounds.w < width) &&
         (sur->getPixel(bounds.x + bounds.w, bounds.y) != keyColor)) {
    bounds.w++;
  }

  bounds.h = 0;
  while ((bounds.y + bounds.h < height) &&
         (sur->getPixel(bounds.x, bounds.y + bounds.h) != keyColor)) {
    bounds.h++;
  }

  // Using red color in the first pixel of the char indicates that
  // this glyph shouldn't be used as a valid one.
  if (firstCharPixel != kRedColor)
    glyphBounds = bounds;
  else
    glyphBounds = gfx::Rect();

  return !bounds.isEmpty();
}

} // namespace text
