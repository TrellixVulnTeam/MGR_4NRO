#include "WaveTexture.h"

#include <dxstructures.h>

using namespace mini::utils;

namespace duckApp
{
    WaveTexture::WaveTexture(DxDevice m_device)
    {
        unsigned int texture_bpp = 4u;
        unsigned int texture_width = 256u;
        unsigned int texture_height = 256u;

        texture_stride = texture_width * texture_bpp;
        txture_size = texture_stride * texture_height;

        Texture2DDescription texture_description = Texture2DDescription(texture_width, texture_height);

        texture_description.BindFlags |= D3D11_BIND_RENDER_TARGET;
        texture_description.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

        this->texture = m_device.CreateTexture(texture_description);
        this->texture_resource_view = m_device.CreateShaderResourceView(this->texture);

        BYTE *value_pointer = this->texture_data.data();
    }

    void WaveTexture::SetValue(int x, int y, Vector value)
    {
        BYTE *value_pointer = this->texture_data.data();

        value_pointer += (256 * 4 * y + 4 * x);

        *(value_pointer++) = static_cast<BYTE>(255 * (value.x + 1) / 2);
        *(value_pointer++) = static_cast<BYTE>(255 * (value.y + 1) / 2);
        *(value_pointer++) = static_cast<BYTE>(255 * (value.z + 1) / 2);
        *(value_pointer++) = static_cast<BYTE>(255);
    }

    void WaveTexture::Update(DxDevice m_device)
    {
        m_device.context()->UpdateSubresource(this->texture.get(), 0, nullptr, texture_data.data(), texture_stride, txture_size);
    }

    dx_ptr<ID3D11ShaderResourceView> WaveTexture::GetTexture(DxDevice m_device)
    {
        return m_device.CreateShaderResourceView(this->texture);
    }
}
