#pragma once
#include <string>
#include <Galaxy/Renderer/material.hpp>

class Texture
{
    public: enum DisplayMode { Pixel = 0x2600, Linear = 0x2601 }; // OpenGL
    private:
        unsigned int rendererID;
        int width, height, bpp; // bpp: bits per pixel

        //void init_texture(unsigned char *localBuffer, DisplayMode displayMode, unsigned int slot);
        void init_texture(unsigned char *localBuffer, DisplayMode displayMode);
        mutable unsigned int selectedSlot = -1U;
    public:
        Texture(const std::string &path, DisplayMode mode);
        ~Texture();

        // possibly remove from public code
        void bind();
        void unbind() const;

        inline unsigned int get_slot() const { return selectedSlot; }
        inline int get_width() const { return width; }
        inline int get_height() const { return height; }

        Texture(const Texture &) = delete;
        void operator=(const Texture &) = delete;
};