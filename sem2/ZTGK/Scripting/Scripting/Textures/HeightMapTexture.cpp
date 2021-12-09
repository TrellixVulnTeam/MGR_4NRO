#include "HeightMapTexture.h"

namespace duckApp
{
    HeightMapTexture::HeightMapTexture()
    {
        this->height_map_texture = new float*[this->texture_size];

        for (int i = 0; i < this->texture_size; i++)
            this->height_map_texture[i] = new float[this->texture_size];

        for (int x = 0; x < this->texture_size; x++)
            for (int y = 0; y < this->texture_size; y++)
                this->height_map_texture[x][y] = 0.0f;
    }

    //void HeightMapTexture::SetValue(int x, int y, float value)
    //{
    //    this->height_map_texture[x][y] = value;
    //}

    //float HeightMapTexture::GetValue(int x, int y)
    //{
    //    return this->height_map_texture[x][y];
    //}

    //int HeightMapTexture::GetSize()
    //{
    //    return this->texture_size;
    //}
}
