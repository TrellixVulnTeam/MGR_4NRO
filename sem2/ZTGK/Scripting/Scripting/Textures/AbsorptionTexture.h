#pragma once

namespace duckApp
{
    class AbsorptionTexture
    {
    public:
        AbsorptionTexture();

        int GetSize();
        float GetValue(int x, int y);

    private:
        static const unsigned int texture_size = 256;
        float **absorption_texture;
    };
}
