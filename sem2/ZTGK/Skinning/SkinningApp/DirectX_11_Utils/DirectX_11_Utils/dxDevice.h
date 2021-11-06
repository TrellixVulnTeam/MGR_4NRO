#pragma once

#include "dxptr.h"
#include "dxstructures.h"
#include <vector>
#include <tuple>
#include "dxSwapChain.h"
#include "mesh.h"
#include "Microsoft/d3dx11Effect.h"
#include <string>

namespace Real_Engine
{
	class Window;

	class DxDevice
	{
	public:
		explicit DxDevice(const Window& window);
		DxDevice(DxDevice&& other)
			: m_device(std::move(other.m_device)), m_immediateContext(move(m_immediateContext)),
			m_swapChain(std::move(other.swapChain()))
		{ }

		DxDevice(const DxDevice& other)
			: m_swapChain(other.m_swapChain)
		{
			other.m_device->AddRef();
			m_device.reset(other.m_device.get());
			other.m_immediateContext->AddRef();
			m_immediateContext.reset(other.m_immediateContext.get());
		}

        void BeginRender(ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView);

		void ClearRenderTarget(ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView, const float(&color)[4] = { 0.0f, 0.0f, 0.0f, 0.0f }) const
		{
			m_immediateContext->ClearRenderTargetView(renderTargetView, color);
			m_immediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}

		dx_ptr<ID3D11Texture2D> CreateTexture(const Texture2DDescription& desc,
			const SubresourceData* data = nullptr) const;
		dx_ptr<ID3D11Texture2D> CreateTexture(const Texture2DDescription& desc,
			const SubresourceData& data) const
		{
			return CreateTexture(desc, &data);
		}
		//dx_ptr<ID3D11RenderTargetView> CreateRenderTargetView(const dx_ptr<ID3D11Texture2D>& backTexture) const;
		ID3D11RenderTargetView* CreateRenderTargetView(const dx_ptr<ID3D11Texture2D>& backTexture) const;
		//dx_ptr<ID3D11RenderTargetView> CreateRenderTargetView(const dx_ptr<ID3D11Texture2D>& backTexture, const D3D11_RENDER_TARGET_VIEW_DESC& desc) const;
		ID3D11RenderTargetView* CreateRenderTargetView(const dx_ptr<ID3D11Texture2D>& backTexture, const D3D11_RENDER_TARGET_VIEW_DESC& desc) const;
		dx_ptr<ID3D11DepthStencilView> CreateDepthStencilView(const dx_ptr<ID3D11Texture2D>& depthTexture, const DepthViewDescription& desc) const
		{
			return CreateDepthStencilView(depthTexture, &desc);
		}
		dx_ptr<ID3D11DepthStencilView> CreateDepthStencilView(const dx_ptr<ID3D11Texture2D>& depthTexture, const DepthViewDescription* desc = nullptr) const;
		dx_ptr<ID3D11DepthStencilView> CreateDepthStencilView(unsigned int width, unsigned int height) const;
		dx_ptr<ID3D11DepthStencilView> CreateDepthStencilView(SIZE size) const
		{
			return CreateDepthStencilView(size.cx, size.cy);
		}

		void SetViewport(ViewportDescription viewport);

        dx_ptr<ID3D11InputLayout> CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* layout, UINT layoutElements, const uint8_t* signature, size_t signatureSize) const;

		template<typename VERT>
		dx_ptr<ID3D11InputLayout> CreateInputLayout(const std::vector<BYTE>& vsByteCode) const
		{
			return CreateInputLayout(VERT::Layout, vsByteCode);
		}

		template<unsigned int N>
		dx_ptr<ID3D11InputLayout> CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC (&layout)[N],
			const std::vector<BYTE>& vsByteCode) const
		{
			return CreateInputLayout(layout, N, vsByteCode);
		}

		dx_ptr<ID3D11InputLayout> CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* layout,
															 unsigned int layoutElements,
															 const std::vector<BYTE>& vsByteCode) const;

		template<class T>
		dx_ptr<ID3D11Buffer> CreateVertexBuffer(unsigned int N) const
		{
			auto desc = BufferDescription::VertexBufferDescription(N * sizeof(T));
			desc.Usage = D3D11_USAGE_DYNAMIC; //since no data is provided assume buffer data will be frequently modified
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			return CreateBuffer(desc);
		}

		template<class T>
		dx_ptr<ID3D11Buffer> CreateVertexBuffer(const T* buffer, size_t count)
		{
			return CreateBuffer(
				BufferDescription::VertexBufferDescription(static_cast<UINT>(sizeof(T)*count)),
				reinterpret_cast<const void*>(buffer));
		}

		template<class T>
		dx_ptr<ID3D11Buffer> CreateVertexBuffer(const std::vector<T>& vertices) const
		{
			assert(vertices.size() * sizeof(T) <= UINT_MAX);
			return CreateBuffer(
				BufferDescription::VertexBufferDescription(static_cast<UINT>(vertices.size() * sizeof(T))),
				reinterpret_cast<const void*>(vertices.data()));
		}

		template<class T, unsigned int N>
		dx_ptr<ID3D11Buffer> CreateVertexBuffer(const T (&vertices)[N]) const
		{
			assert(N * sizeof(T) <= UINT_MAX);
			return CreateBuffer(BufferDescription::VertexBufferDescription(static_cast<UINT>(N * sizeof(T))),
				reinterpret_cast<const void*>(vertices));
		}

		template<class T>
		dx_ptr<ID3D11Buffer> CreateIndexBuffer(size_t N) const
		{
			auto desc = BufferDescription::IndexBufferDescription(N * sizeof(T));
			desc.Usage = D3D11_USAGE_DYNAMIC; //since no data is provided assume buffer data will be frequently modified
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			return CreateBuffer(desc);
		}

		template<class T>
		dx_ptr<ID3D11Buffer> CreateIndexBuffer(const std::vector<T>& indices) const
		{
			assert(indices.size() * sizeof(T) <= UINT_MAX);
			return CreateBuffer(BufferDescription::IndexBufferDescription(static_cast<UINT>(indices.size() * sizeof(T))),
				reinterpret_cast<const void*>(indices.data()));
		}

		template<class T, unsigned int N>
		dx_ptr<ID3D11Buffer> CreateIndexBuffer(const T (&indices)[N]) const
		{
			assert(N * sizeof(T) <= UINT_MAX);
			return CreateBuffer(BufferDescription::IndexBufferDescription(static_cast<UINT>(N * sizeof(T))),
				reinterpret_cast<const void*>(indices));
		}

		dx_ptr<ID3D11Buffer> CreateBuffer(const BufferDescription& desc, const void* pData = nullptr) const;
		dx_ptr<ID3D11BlendState> CreateBlendState(const BlendDescription& desc = {}) const;
		dx_ptr<ID3D11DepthStencilState> CreateDepthStencilState(const DepthStencilDescription& desc = {}) const;
		
		dx_ptr<ID3D11RasterizerState> CreateRasterizerState(const RasterizerDescription& desc = { }) const;
		dx_ptr<ID3D11SamplerState> CreateSamplerState(const SamplerDescription& desc = { }) const;
		dx_ptr<ID3D11ShaderResourceView> CreateShaderResourceView(const std::wstring& texPath) const;
		dx_ptr<ID3D11ShaderResourceView> CreateShaderResourceView(const dx_ptr<ID3D11Texture2D>& texture,
			const ShaderResourceViewDescription& desc) const
		{
			return CreateShaderResourceView(texture, &desc);
		}
		dx_ptr<ID3D11ShaderResourceView> CreateShaderResourceView(const dx_ptr<ID3D11Texture2D>& texture,
			const ShaderResourceViewDescription* desc = nullptr) const;

		template<typename IDX, typename VERTS>
        std::tuple<dx_ptr<ID3D11Buffer>,dx_ptr<ID3D11Buffer>> CreateMesh(const std::vector<IDX>& indices, const std::vector<VERTS>& vertices) const
		{
			//if (indices.size() == 0) return { };

            dx_ptr<ID3D11Buffer> vertexArray = CreateVertexBuffer(vertices);
            dx_ptr<ID3D11Buffer> indexArray = CreateIndexBuffer(indices);

            return { std::move(vertexArray) , std::move(indexArray) };
		}

        ID3DX11Effect* LoadEffect(const std::wstring& fileName);

        //template<typename IDX, typename VERT>
        //Mesh CreateMesh(const std::vector<IDX>& indices, const std::vector<VERT>& vertices) const
        //{
        //    if (indices.size() == 0) return {};
        //    dx_ptr<ID3D11Buffer> vb = CreateVertexBuffer(vertices);
        //    dx_ptr<ID3D11Buffer> ib = CreateIndexBuffer(indices);

        //    return Mesh{ std::move(vb), sizeof(VERT), 0, std::move(ib), static_cast<UINT>(indices.size()) };
        //}

		ID3D11Device* operator->() const { return m_device.get(); }
		operator ID3D11Device*() const { return m_device.get(); }
		const dx_ptr<ID3D11DeviceContext>& context() const { return m_immediateContext; }
		const DxSwapChain& swapChain() const { return m_swapChain; }

	private:

		dx_ptr<ID3D11Device> m_device;
		dx_ptr<ID3D11DeviceContext> m_immediateContext;
		DxSwapChain m_swapChain;
		ViewportDescription viewport;
	};
}
