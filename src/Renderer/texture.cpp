#include <Galaxy/Renderer/texture.hpp>
#include <Galaxy/Renderer/material.hpp>
#include <Galaxy/Renderer/shader.hpp>
#include <Galaxy/Assets/assets.hpp>

#include <gldebug.hpp>
#include <External/stb_image.h>
#include <unordered_map>

#if USE_GLFM
static constexpr unsigned int MAX_SLOTS = 8;
#else
static constexpr unsigned int MAX_SLOTS = 16;
#endif
static constexpr unsigned int NO_SLOT = -1U;
static unsigned int nextSlot = 0;
static auto boundTextures = std::make_unique<std::unordered_map<int, Texture*>>();

void Texture::init_texture(unsigned char *localBuffer, DisplayMode displayMode)
{
    GLCall(glGenTextures(1, &rendererID));
    // system: have a map of all slots, if one is empty then
    // bind there, otherwise bind with the id of the last used texture
    bind();
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, displayMode));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, displayMode));

/*
    performance notes: https://stackoverflow.com/questions/42425281/understanding-binding-activating-texture-performance-penalty-in-opengl
*/

    /* source: https://learnopengl.com/Getting-started/Textures
    When sampling past texture coordinates 0~1
        GL_REPEAT repeats the image, basically tiling
        GL_MIRRORED_REPEAT repeats, but each image is mirrored
        GL_CLAMP_TO_EDGE makes the last pixels stretch
        GL_CLAMP_TO_BORDER sets the pixels outside the image to a color
    */
    #if USE_GLFM
        #define GL_RGBA8 GL_RGBA
    #endif
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    // internal format is how it is stored, format is the format the texture data is given in
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));

    if (localBuffer) stbi_image_free(localBuffer);
}
Texture::Texture(const std::string &path, DisplayMode mode)
{
    // OpenGL expects images to start from the bottom left corner, normal images start from top left
    stbi_set_flip_vertically_on_load(1);

    auto fileContents = Assets::file_contents(path.c_str());
    auto image = (const unsigned char*)fileContents.str;

    unsigned char *localBuffer = stbi_load_from_memory(image, fileContents.length, &width, &height, &bpp, 4);
    assert(localBuffer && "Failed to load image");
    init_texture(localBuffer, mode);
}
Texture::~Texture()
{
    if (selectedSlot != NO_SLOT && boundTextures)
        boundTextures->erase(selectedSlot);

    GLCall(glDeleteTextures(1, &rendererID));
}
AssetRef<Texture> Texture::load(const std::string &path, DisplayMode mode)
{
    if (AssetRef<Texture>::is_loaded(path))
        return AssetRef<Texture>::get_loaded(path);
    return AssetRef<Texture>(new Texture(path, mode), path);
}
void Texture::bind()
{
    if (selectedSlot != NO_SLOT)
    {
        if (selectedSlot != nextSlot-1)
            GLCall(glActiveTexture(GL_TEXTURE0 + selectedSlot));
        
        return;
    }
    selectedSlot = nextSlot;
    GLCall(glActiveTexture(GL_TEXTURE0 + selectedSlot));
    GLCall(glBindTexture(GL_TEXTURE_2D, rendererID));

    if (boundTextures->count(selectedSlot))
    {
        (*boundTextures)[selectedSlot]->selectedSlot = NO_SLOT;
    }
    (*boundTextures)[selectedSlot] = this;

    ++nextSlot;
    if (nextSlot == MAX_SLOTS) nextSlot = 0;
}
void Texture::unbind() const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + selectedSlot));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
    selectedSlot = NO_SLOT;
}