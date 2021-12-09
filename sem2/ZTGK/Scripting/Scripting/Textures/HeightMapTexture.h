#pragma once

namespace duckApp
{
    class HeightMapTexture
    {
    public:
        HeightMapTexture();

        inline void SetValue(int x, int y, float value)
        {
            this->height_map_texture[x][y] = value;
        }

        inline int GetSize()
        {
            return this->texture_size;
        }
        
        inline float GetValue(int x, int y)
        {
            return this->height_map_texture[x][y];
        }
        
    private:
        static const unsigned int texture_size = 256;
        float **height_map_texture;
    };
}

