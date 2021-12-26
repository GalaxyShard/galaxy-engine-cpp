#include <Galaxy/UI/font.hpp>
#include <Galaxy/Renderer/texture.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
Font *Font::defaultFont;
//static float coord(int pixel, float size) { return pixel / size + (1.f / (size * 2.f)); }
//static float coord(int pixel, float size) { return (pixel*2 + 1)/(size*2); }

static float xcoord(float pixel, float size) { return pixel / size + (1.f / (size * 2.f)); }
static float ycoord(float pixel, float size) { return xcoord(size-pixel, size); }

Font::Font(std::string folder)
{
    auto info = std::ifstream(folder+"/bitmapinfo.txt");
    fontTex = std::make_unique<Texture>(folder+"/bitmap.png", Texture::Pixel);

    float w = fontTex->get_width();
    float h = fontTex->get_height();

    std::string line;
    while(std::getline(info, line))
    {
        if (line.substr(0, 5) == "char ")
        {
            auto charData = std::stringstream(line.substr(5));
            std::string name, val;
            unsigned char id;
            while(std::getline(charData, val, ' '))
            {
                size_t pos = val.find('=');
                assert(pos != std::string::npos);

                name = val.substr(0, pos);
                val = val.substr(pos+1);
                
                int value = std::stoi(val);

    //float x = xcoord(data.x, w);
    //float y = ycoord(data.y, h);
    //float xw = xcoord(data.xw, w);
    //float yh = ycoord(data.yh, h);
                if (name == "id")
                {
                    int id_local = std::stoi(val);
                    if (id_local > UCHAR_MAX) break;
                    id = id_local;
                }

                else if (name == "x") glyphs[id].x = value;
                else if (name == "y") glyphs[id].y = value;
                //else if (name == "width") glyphs[id].xw = value+glyphs[id].x-1;
                //else if (name == "height") glyphs[id].yh = value+glyphs[id].y-1;
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

            //glyph.x = xcoord(glyph.x, w);
            //glyph.y = ycoord(glyph.y, h);
            //glyph.xw = xcoord(glyph.xw, w);
            //glyph.yh = ycoord(glyph.yh, h);
            //glyph.xadv = xcoord(glyph.xadv, w);
        }
    }
    //for (auto glyph : glyphs)
    //{
    //    //printf("%d\n", glyph.);
    //    std::cout << "char: \'"
    //        << (char)glyph.first << "\', id:"
    //        << (int)glyph.first << ", x:"
    //        << glyph.second.x << ", y:"
    //        << glyph.second.y << ", xw:"
    //        << glyph.second.xw << ", yh:"
    //        << glyph.second.yh << ", xoff:"
    //        << glyph.second.xoff << ", yoff:"
    //        << glyph.second.yoff << ", xadv:"
    //        << glyph.second.xadv << "" << std::endl;
    //}
}
GlyphData const& Font::get_char(char c)
{
    return glyphs[c];
}