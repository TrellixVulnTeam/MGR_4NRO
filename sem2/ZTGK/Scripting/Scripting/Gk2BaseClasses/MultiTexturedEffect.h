#pragma once

#include <effect.h>
#include <constantBuffer.h>
#include <DirectXMath.h>

using namespace mini;

namespace duckApp
{
    class MultiTexturedEffect : public StaticEffect<BasicEffect, VSConstantBuffers, PSSamplers, PSShaderResources>
    {
    public:
        enum VSConstantBufferSlots
        {
            WorldMtxSlot,
            ViewMtxSlot,
            ProjMtxSlot,
            TextureMtxSlot0,
            TextureMtxSlot1,
            CameraPositionSlot
        };

        enum PSSamplerSlots
        {
            TextureSamplerSlot
        };

        enum PSTextureSlots
        {
            TextureSlot0,
            TextureSlot1
        };

        MultiTexturedEffect() = default;

        MultiTexturedEffect(dx_ptr<ID3D11VertexShader>&& vs, dx_ptr<ID3D11PixelShader>&& ps, const ConstantBuffer<DirectX::XMFLOAT4X4>& cbWorld,
            const ConstantBuffer<DirectX::XMFLOAT4X4, 2>& cbView, const ConstantBuffer<DirectX::XMFLOAT4X4>& cbProj,
            const ConstantBuffer<DirectX::XMFLOAT4X4>& cbTextureMtx1, const ConstantBuffer<DirectX::XMFLOAT4X4>& cbTextureMtx2, const dx_ptr<ID3D11SamplerState>& textureSampler,
            const dx_ptr<ID3D11ShaderResourceView>& texture0, const dx_ptr<ID3D11ShaderResourceView>& texture1)
            : StaticEffect(BasicEffect(move(vs), move(ps)), VSConstantBuffers{ cbWorld, cbView, cbProj, cbTextureMtx1, cbTextureMtx2 },
                PSSamplers(textureSampler), PSShaderResources(texture0, texture1))
        { }

        void SetWorldMatrixBuffer(const ConstantBuffer<DirectX::XMFLOAT4X4>& buffer) { SetVSConstantBuffer(WorldMtxSlot, buffer); }
        void SetViewMatrixBuffer(const ConstantBuffer<DirectX::XMFLOAT4X4, 2>& buffer) { SetVSConstantBuffer(ViewMtxSlot, buffer); }
        void SetProjMatrixBuffer(const ConstantBuffer<DirectX::XMFLOAT4X4>& buffer) { SetVSConstantBuffer(ProjMtxSlot, buffer); }
        void SetTextureMatrixBuffer1(const ConstantBuffer<DirectX::XMFLOAT4X4>& buffer) { SetVSConstantBuffer(TextureMtxSlot0, buffer); }
        void SetTextureMatrixBuffer2(const ConstantBuffer<DirectX::XMFLOAT4X4>& buffer) { SetVSConstantBuffer(TextureMtxSlot1, buffer); }
        void SetTextureSampler(const dx_ptr<ID3D11SamplerState>& sampler) { SetPSSampler(TextureSamplerSlot, sampler); }
        void SetCameraPosition(const ConstantBuffer<DirectX::XMFLOAT4>& buffer) { SetVSConstantBuffer(CameraPositionSlot, buffer); }
        void SetTexture1(const dx_ptr<ID3D11ShaderResourceView>& texture) { SetPSShaderResource(TextureSlot0, texture); }
        void SetTexture2(const dx_ptr<ID3D11ShaderResourceView>& texture) { SetPSShaderResource(TextureSlot1, texture); }
        void SetTextures(const dx_ptr<ID3D11ShaderResourceView>& texture0, const dx_ptr<ID3D11ShaderResourceView>& texture1)
        {
            SetPSShaderResource(TextureSlot0, texture0);
            SetPSShaderResource(TextureSlot1, texture1);
        }
    };
}
