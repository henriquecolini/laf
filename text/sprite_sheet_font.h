// LAF Text Library
// Copyright (C) 2019-2025  Igara Studio S.A.
// Copyright (C) 2012-2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef LAF_TEXT_SPRITE_SHEET_FONT_H_INCLUDED
#define LAF_TEXT_SPRITE_SHEET_FONT_H_INCLUDED
#pragma once

#include "base/debug.h"
#include "base/ref.h"
#include "base/string.h"
#include "base/utf8_decode.h"
#include "gfx/rect.h"
#include "os/surface.h"
#include "text/font.h"
#include "text/font_metrics.h"
#include "text/typeface.h"

#include <vector>

namespace text {

class SpriteSheetFont : public Font {
  static constexpr auto kRedColor = gfx::rgba(255, 0, 0);

public:
  SpriteSheetFont() {}
  ~SpriteSheetFont() {}

  FontType type() override { return FontType::SpriteSheet; }

  TypefaceRef typeface() const override
  {
    return nullptr; // TODO impl
  }

  void setDescent(float descent) { m_descent = descent; }

  float metrics(FontMetrics* metrics) const override
  {
    // TODO impl

    if (metrics) {
      float descent = m_descent;
      if (m_descent > 0.0f && m_defaultSize > 0.0f && m_defaultSize != m_size)
        descent = m_descent * m_size / m_defaultSize;

      metrics->descent = descent;
      metrics->ascent = -m_size + descent;
      metrics->underlineThickness = 1.0f;
      metrics->underlinePosition = m_descent;
    }

    return lineHeight();
  }

  float defaultSize() const override { return m_defaultSize; }
  float size() const override { return m_size; }
  float lineHeight() const override { return m_size; }

  float textLength(const std::string& str) const override
  {
    base::utf8_decode decode(str);
    int x = 0;
    while (int chr = decode.next())
      x += getCharBounds(chr).w;
    return x;
  }

  float measureText(const std::string& str,
                    gfx::RectF* bounds,
                    const os::Paint* paint) const override
  {
    float w = textLength(str);
    if (bounds)
      *bounds = gfx::RectF(0, 0, w, lineHeight());
    return w;
  }

  bool isScalable() const override { return true; }

  void setSize(float size) override;

  bool antialias() const override { return m_antialias; }

  void setAntialias(bool antialias) override
  {
    m_antialias = antialias;
    setSize(m_size);
  }

  FontHinting hinting() const override { return FontHinting::None; }

  void setHinting(FontHinting hinting) override { (void)hinting; }

  glyph_t codePointToGlyph(const codepoint_t codepoint) const override
  {
    glyph_t glyph = codepoint - int(' ') + 2;
    if (glyph >= 0 && glyph < int(m_glyphs.size()) && !m_glyphs[glyph].isEmpty()) {
      return glyph;
    }
    else
      return 0;
  }

  gfx::RectF getGlyphBounds(glyph_t glyph) const override
  {
    if (glyph >= 0 && glyph < (int)m_glyphs.size())
      return gfx::RectF(0, 0, m_glyphs[glyph].w, m_glyphs[glyph].h);

    return getCharBounds(128);
  }

  float getGlyphAdvance(glyph_t glyph) const override { return getGlyphBounds(glyph).w; }

  gfx::RectF getGlyphBoundsOnSheet(glyph_t glyph) const
  {
    if (glyph >= 0 && glyph < (int)m_glyphs.size())
      return m_glyphs[glyph];

    return getCharBounds(128);
  }

  gfx::RectF getGlyphBoundsOutput(glyph_t glyph) const
  {
    gfx::RectF bounds = getGlyphBoundsOnSheet(glyph);
    return bounds * m_size;
  }

  os::Surface* sheetSurface() const { return m_sheet.get(); }

  gfx::Rect getCharBounds(codepoint_t cp) const
  {
    glyph_t glyph = codePointToGlyph(cp);
    if (glyph == 0)
      glyph = codePointToGlyph(128);

    if (glyph != 0)
      return m_glyphs[glyph];
    else
      return gfx::Rect();
  }

  static FontRef FromSurface(os::SurfaceRef& sur, float size)
  {
    auto font = base::make_ref<SpriteSheetFont>();
    font->fromSurface(sur, size);
    return font;
  }

private:
  void fromSurface(os::SurfaceRef& sur, float size);

  bool findGlyph(const os::Surface* sur,
                 int width,
                 int height,
                 gfx::Rect& bounds,
                 gfx::Rect& glyphBounds);

private:
  os::SurfaceRef m_originalSheet;
  os::SurfaceRef m_sheet;
  std::vector<gfx::Rect> m_originalGlyphs;
  std::vector<gfx::Rect> m_glyphs;
  float m_defaultSize = 0.0f;
  float m_size = 0.0f;
  float m_descent = 0.0f;
  bool m_antialias = false;
};

} // namespace text

#endif
