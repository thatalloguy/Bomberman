#include "precomp.h"
#include "assetManager.h"

#include <stb_image.h>


//Global namespace
namespace
{
    GLTexture* _textures[11] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    Shader* _shaders[11] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };


}


bool bomberman::Assets::LoadAssets()
{
    bool success = true;

    //load all textures
    for (int i=0; i<TEXTURE_AMOUNT; i++)
    {
        printf("Loading texture: %s\n", texturePaths[i]);
        int w = 0, h = 0, c = 0;
        GLTexture* tex = nullptr;

        stbi_set_flip_vertically_on_load(false);
    	unsigned char* data = stbi_load(texturePaths[i], &w, &h, &c, 0);

        if (data) {
            tex = new GLTexture(w, h);
            tex->CopyFrom(data);
        }
        else {
            delete tex;
            FatalError("Could not load image");
            success = false;
        }

        stbi_image_free(data);

        _textures[i] = tex;
    }

    for (int i=0; i<SHADER_AMOUNT; i++)
    {
        _shaders[i] = new Shader(shaderPaths[i].vertPath, shaderPaths[i].fragPath, false);
    }

    return success;
}

void bomberman::Assets::UnloadAssets()
{
    for (int i=0; i<TEXTURE_AMOUNT; i++)
    {
        delete _textures[i];
    }

    for (int i = 0; i < SHADER_AMOUNT; i++)
    {
        delete _shaders[i];
    }
}

GLTexture* bomberman::Assets::GetTexture(TextureType type)
{
    return _textures[static_cast<int>(type)];
}

Shader* bomberman::Assets::GetShader(ShaderType type)
{
    return _shaders[static_cast<int>(type)];
}
