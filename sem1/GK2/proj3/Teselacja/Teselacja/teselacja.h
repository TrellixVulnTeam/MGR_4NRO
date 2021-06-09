#pragma once
#include <DirectXMath.h>

#include "dxApplication.h"
#include "mesh.h"

namespace mini::gk2
{
	//Position and color of a single light
	struct Light
	{
		//Light position coordinates [x, y, z, 1.0f]
		DirectX::XMFLOAT4 position;
		//Light color [r, g, b, 1.0f] - on
		//or [r, g, b, 0.0f] - off
		DirectX::XMFLOAT4 color;
	};

	//Lighting parameters (except surface color)
	struct Lighting
	{
		//ambient reflection color [r, g, b, a]
		DirectX::XMFLOAT4 ambientColor;
		//surface reflection coefficients [ka, kd, ks, m]
		DirectX::XMFLOAT4 surface;
		//Light positions and colors
		Light lights[3];
	};

	struct Parameters
	{
		int edgeTessFactor;
		int insideTessFactor;
		int useLOD;
		int displacementMapping;
	};
	class Teselacja : public DxApplication
	{
	public:
		using Base = DxApplication;

		explicit Teselacja(HINSTANCE hInstance);
	protected:
		void Update(const Clock& dt) override;
		void Render() override;

	private:
#pragma region CONSTANTS
		static const unsigned int BS_MASK;
		void CreateRenderStates();
#pragma endregion

#pragma region D3D Resources
		//Shader's constant buffer containing Local -> World matrix
		dx_ptr<ID3D11Buffer> m_cbWorld;
		//ConstantBuffer<DirectX::XMFLOAT4X4> m_cbWorld;
		//Shader's constant buffer containing World -> Camera and Camera -> World matrix
		dx_ptr<ID3D11Buffer> m_cbView;
		//ConstantBuffer<DirectX::XMFLOAT4X4, 2> m_cbView;
		//Shader's constant buffer containing projection matrix
		dx_ptr<ID3D11Buffer> m_cbProj;
		//ConstantBuffer<DirectX::XMFLOAT4X4> m_cbProj;
		//Shader's constant buffer containing lighting parameters (except surface color)
		dx_ptr<ID3D11Buffer> m_cbLighting;
		//ConstantBuffer<Lighting> m_cbLighting;
		dx_ptr<ID3D11Buffer> m_cbParameters;

		dx_ptr<ID3D11Buffer> m_cbMapMtx; //pixel shader constant buffer slot 2
		//Shader's constant buffer containing surface color
		dx_ptr<ID3D11Buffer> m_cbSurfaceColor;
		//ConstantBuffer<DirectX::XMFLOAT4> m_cbSurfaceColor;
		// 


		dx_ptr<ID3D11SamplerState> m_sampler;


		dx_ptr<ID3D11SamplerState> m_samplerWrap;
		dx_ptr<ID3D11SamplerState> m_samplerWrap_back;
		//Depth stencil state used for drawing billboards without writing to the depth buffer
		dx_ptr<ID3D11DepthStencilState> m_dssNoDepthWrite;
		dx_ptr<ID3D11DepthStencilState> m_dssDepthWrite;
		//Depth stencil state used to fill the stencil buffer
		dx_ptr<ID3D11DepthStencilState> m_dssStencilWrite;
		dx_ptr<ID3D11DepthStencilState> m_dssStencilWriteSh;
		//Depth stencil state used to perform stencil test when drawing mirrored scene
		dx_ptr<ID3D11DepthStencilState> m_dssStencilTest;
		dx_ptr<ID3D11DepthStencilState> m_dssStencilTestSh;
		dx_ptr<ID3D11DepthStencilState> m_dssStencilTestGreaterSh;
		//Depth stencil state used to perform stencil test when drawing mirrored billboards without writing to the depth buffer
		dx_ptr<ID3D11DepthStencilState> m_dssStencilTestNoDepthWrite;
		//Rasterizer state used to define front faces as counter-clockwise, used when drawing mirrored scene
		dx_ptr<ID3D11InputLayout> m_layout, m_wireIL;
		dx_ptr<ID3D11RasterizerState> m_rsWireframe;
		dx_ptr<ID3D11RasterizerState> m_rsCCW;
		dx_ptr<ID3D11RasterizerState> m_rsAll;
		dx_ptr<ID3D11RasterizerState> m_rsCCW_backSh;
		dx_ptr<ID3D11RasterizerState> m_rsCCW_frontSh;
		//Blend state used to draw dodecahedron faced with alpha blending.
		dx_ptr<ID3D11BlendState> m_bsAlpha;
		dx_ptr<ID3D11BlendState> m_bsAlphaInv;
		//Blend state used to draw billboards.
		dx_ptr<ID3D11BlendState> m_bsAdd;

		dx_ptr<ID3D11DepthStencilView> m_shadowDepthBuffer;

		dx_ptr<ID3D11ShaderResourceView> m_shadowMap;

		dx_ptr<ID3D11VertexShader> m_tessVS;
		dx_ptr<ID3D11VertexShader> m_wireVS;
		dx_ptr<ID3D11HullShader> m_tessHS;
		dx_ptr<ID3D11DomainShader> m_tessDS;
		dx_ptr<ID3D11PixelShader> m_tessPS;
		dx_ptr<ID3D11PixelShader> m_wirePS;
		dx_ptr<ID3D11Buffer> m_vertexBuffer;
		unsigned int m_vertexStride;
		unsigned int m_vertexCount;
		unsigned int patchN = 4, patchM = 4;
		Mesh m_wire;
		Parameters parameters;
		bool handled[4];
		bool fillWireframe = false, drawControlPolygon = false;
		int version = 1;

		dx_ptr<ID3D11ShaderResourceView> m_heightTexture;
		dx_ptr<ID3D11ShaderResourceView> m_normalsTexture;
		dx_ptr<ID3D11ShaderResourceView> m_diffuseTexture;
#pragma endregion

#pragma region Matrices
		DirectX::XMFLOAT4X4 m_projMtx;
#pragma endregion
		void SetWorldMtx(DirectX::XMFLOAT4X4 mtx);
		void DrawMesh(const Mesh& m, DirectX::XMFLOAT4X4 worldMtx);
		void UpdateCameraCB(DirectX::XMFLOAT4X4 cameraMtx);
		void Set1Light();
		void HandleKeyboard();
		void UpdateParameters();
		void SetInitialParameters();

		void SetShaders(const dx_ptr<ID3D11InputLayout>& il, const dx_ptr<ID3D11VertexShader>& vs, const dx_ptr<ID3D11PixelShader>& ps, const dx_ptr<ID3D11DomainShader>& ds, const dx_ptr<ID3D11HullShader>& hs);
		void SetShaders(const dx_ptr<ID3D11InputLayout>& il, const dx_ptr<ID3D11VertexShader>& vs, const dx_ptr<ID3D11PixelShader>& ps);
		void SetTexturesPS(std::initializer_list<ID3D11ShaderResourceView*> resList, const dx_ptr<ID3D11SamplerState>& sampler);
		void SetTexturesDS(std::initializer_list<ID3D11ShaderResourceView*> resList, const dx_ptr<ID3D11SamplerState>& sampler);
		void SetTextures(std::initializer_list<ID3D11ShaderResourceView*> resList) { SetTexturesPS(std::move(resList), m_sampler); }
		void SetVersion();

	};
}