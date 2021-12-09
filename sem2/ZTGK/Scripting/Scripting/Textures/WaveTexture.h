#pragma once

#include <array>

#include <dxptr.h>
#include <dxDevice.h>

#include "../Geometry/Vector.h"

using namespace mini;
using namespace DirectX;

namespace duckApp
{
    class WaveTexture
    {
    public:
    public:
        WaveTexture() = default;
        WaveTexture(DxDevice m_device);

        void Update(DxDevice m_device);
        void SetValue(int x, int y, Vector value);
        unsigned int GetSize() { return 256; }

        dx_ptr<ID3D11ShaderResourceView> GetTexture(DxDevice m_device);

    private:
        unsigned int texture_stride;
        unsigned int txture_size;

        std::array<BYTE, 256 * 256 * 4> texture_data;

        dx_ptr<ID3D11Texture2D> texture;
        dx_ptr<ID3D11ShaderResourceView> texture_resource_view;
    };
}
