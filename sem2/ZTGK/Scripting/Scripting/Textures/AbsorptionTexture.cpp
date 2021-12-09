#include "AbsorptionTexture.h"

#include <cmath>
#include <algorithm>

namespace duckApp
{
    AbsorptionTexture::AbsorptionTexture()
    {
        /* Allocate Mamory */
        this->absorption_texture = new float*[this->texture_size];

        for (int i = 0; i < this->texture_size; i++)
            this->absorption_texture[i] = new float[this->texture_size];

        /* Calculate Distance Data */
        float half_size_value = this->texture_size / 2.0f;

        for (int x = 0; x < this->texture_size; x++)
            for (int y = 0; y < this->texture_size; y++)
            {
                float posX = x / static_cast<float>(this->texture_size - 1);
                float posY = y / static_cast<float>(this->texture_size - 1);
        
                float left = posX;
                float right = 1.0f - posX;
                float top = posY;
                float bottom = 1.0f - posY;
        
                float leftDistance = std::max(left, std::max(top, bottom));
                float rightDistance = std::max(right, std::max(top, bottom));
                float topDistance = std::max(top, std::max(left, right));
                float bottomDistance = std::max(bottom, std::max(left, right));
        
                float l = std::min(std::min(leftDistance, rightDistance), std::min(topDistance, bottomDistance));
                float absorption_value = 0.95f * std::min(1.0f, l * 5.0f);
        
                this->absorption_texture[x][y] = absorption_value;
            }
    }

    int AbsorptionTexture::GetSize()
    {
        return this->texture_size;
    }

    float AbsorptionTexture::GetValue(int x, int y)
    {
        return this->absorption_texture[x][y];
    }
}


