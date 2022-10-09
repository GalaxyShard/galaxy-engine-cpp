#include <Galaxy/UI/font.hpp>
#include <Galaxy/Renderer/texture.hpp>
#include <iostream>

#include <fstream>
#include <sstream>
Font *Font::defaultFont = 0;

static float xcoord(float pixel, float size) { return pixel / size + (1.f / (size * 2.f)); }
static float ycoord(float pixel, float size) { return xcoord(size-pixel, size); }

Font::Font(std::string folder)
{
    auto info = std::ifstream(folder+"/bitmapinfo.txt");
    fontTex = Texture::load(folder+"/bitmap.png", Texture::Pixel);

    float w = fontTex->get_width();
    float h = fontTex->get_height();

    std::string line, valueStr;
    std::getline(info, line);
    fontSize = std::stoi(line);

    while (std::getline(info, line)) {
        auto data = std::stringstream(line);
        int values[8];
        for (int i = 0; i < 8; ++i) {
            std::getline(data, valueStr, ',');
            values[i] = std::stoi(valueStr);
        }
        if (values[0]>255) continue;
        unsigned char id = values[0];

        auto &glyph = glyphs[id];
        glyph.x = values[1];
        glyph.y = values[2];
        glyph.width = values[3];
        glyph.height = values[4];
        glyph.xoff = values[5];
        glyph.yoff = values[6];
        glyph.xadv = values[7];

        glyph.xCoord0 = xcoord(glyph.x, w);
        glyph.yCoord0 = ycoord(glyph.y, h);
        glyph.xCoord1 = xcoord(glyph.x+glyph.width, w);
        glyph.yCoord1 = ycoord(glyph.y+glyph.height, h);
    }
}
GlyphData const& Font::get_char(char c)
{
    return glyphs[c];
}