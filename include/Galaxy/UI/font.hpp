#pragma once
#include <string>
#include <unordered_map>
#include <Galaxy/Assets/assets.hpp>
class Texture;
struct GlyphData
{
    float x, y, width, height, xoff, yoff, xadv;

    float xCoord0, yCoord0, xCoord1, yCoord1;
};
class Font
{
private:
    AssetRef<Texture> fontTex;
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