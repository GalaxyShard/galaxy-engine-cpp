#pragma once
#include <string>
#include <unordered_map>
class Texture;
struct GlyphData
{
    //int rx, ry, rwidth, rheight, rxoff, ryoff, rxadv;
    float x, y, width, height, xoff, yoff, xadv;

    float xCoord0, yCoord0, xCoord1, yCoord1;
    //float x, y, xw, yh, xoff, yoff, xadv;
};
class Font
{
private:
    std::unique_ptr<Texture> fontTex;
    std::unordered_map<char, GlyphData> glyphs;
    friend class UIText;
    friend class Renderer;
public:
    static Font *defaultFont;

    Font(std::string folder);
    const GlyphData &get_char(char c);


    void operator=(const Font&) = delete;
    Font(const Font&) = delete;
};