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

    std::string line;
    while(std::getline(info, line))
    {
        if (line.substr(0, 5) == "char ")
        {
            auto charData = std::stringstream(line.substr(5));
            std::string name, val;
            unsigned char id=0;
            while(std::getline(charData, val, ' '))
            {
                size_t pos = val.find('=');
                assert(pos != std::string::npos && "Error reading font data");

                name = val.substr(0, pos);
                val = val.substr(pos+1);
                
                int value = std::stoi(val);
                if (name == "id")
                {
                    unsigned int id_local = std::stoul(val);
                    if (id_local > 255) break;
                    id = (unsigned char)id_local;
                }
                else if (name == "x") glyphs[id].x = value;
                else if (name == "y") glyphs[id].y = value;
                else if (name == "width") glyphs[id].width = value;
                else if (name == "height") glyphs[id].height = value;
                else if (name == "xoffset") glyphs[id].xoff = value;
                else if (name == "yoffset") glyphs[id].yoff = value;
                else if (name == "xadvance") glyphs[id].xadv = value;
                
            }
            auto &glyph = glyphs[id];
            glyph.xCoord0 = xcoord(glyph.x, w);
            glyph.yCoord0 = ycoord(glyph.y, h);
            glyph.xCoord1 = xcoord(glyph.x+glyph.width, w);
            glyph.yCoord1 = ycoord(glyph.y+glyph.height, h);
        }
    }
}
GlyphData const& Font::get_char(char c)
{
    return glyphs[c];
}