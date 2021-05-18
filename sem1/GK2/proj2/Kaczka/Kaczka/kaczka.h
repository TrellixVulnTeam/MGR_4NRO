#pragma once
#include <DirectXMath.h>
#define WATER_N 256

#include "dxApplication.h"
#include "mesh.h"

namespace mini::gk2
{
	class Kaczka : public DxApplication
	{
	public:
		using Base = DxApplication;

		explicit Kaczka(HINSTANCE hInstance);

	protected:
		void Update(const Clock& dt) override;
		void Render() override;

	private:
#pragma region CONSTANTS
		static const unsigned int BS_MASK;
		void CreateRenderStates();

		static const DirectX::XMFLOAT3 SHEET_POS;
		static const float SHEET_SIZE;
		static constexpr unsigned int MAP_SIZE = 1024;
		static const DirectX::XMFLOAT4 SHEET_COLOR;

		static const float WALL_SIZE;
		static const DirectX::XMFLOAT3 WALLS_POS;

		static const float DUCK_SCALING;
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

		dx_ptr<ID3D11Buffer> m_cbMapMtx; //pixel shader constant buffer slot 2
		//Shader's constant buffer containing surface color
		dx_ptr<ID3D11Buffer> m_cbSurfaceColor;
		//ConstantBuffer<DirectX::XMFLOAT4> m_cbSurfaceColor;
		dx_ptr<ID3D11Buffer> m_cbPlane;
		//ConstantBuffer<DirectX::XMFLOAT4> m_cbSurfaceColor;
		// 


		dx_ptr<ID3D11SamplerState> m_sampler;


		dx_ptr<ID3D11VertexShader>m_envVS, m_duckVS;
		dx_ptr<ID3D11PixelShader> m_envPS, m_duckPS;
		dx_ptr<ID3D11InputLayout> m_envIL, m_duckIL;

		dx_ptr<ID3D11SamplerState> m_samplerWrap;
		dx_ptr<ID3D11SamplerState> m_samplerWrap_back;
		//Wall mesh
		Mesh m_wall;
		//Wall mesh
		Mesh m_sheet;
		Mesh m_duck;
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
		dx_ptr<ID3D11RasterizerState> m_rsCCW;
		dx_ptr<ID3D11RasterizerState> m_rsCCW_backSh;
		dx_ptr<ID3D11RasterizerState> m_rsCCW_frontSh;
		//Blend state used to draw dodecahedron faced with alpha blending.
		dx_ptr<ID3D11BlendState> m_bsAlpha;
		dx_ptr<ID3D11BlendState> m_bsAlphaInv;
		//Blend state used to draw billboards.
		dx_ptr<ID3D11BlendState> m_bsAdd;

		dx_ptr<ID3D11DepthStencilView> m_shadowDepthBuffer;

		dx_ptr<ID3D11ShaderResourceView> m_shadowMap;

		dx_ptr<ID3D11Texture2D> waterTex;
		dx_ptr<ID3D11ShaderResourceView> m_waterTexture;
		dx_ptr<ID3D11ShaderResourceView> m_envTexture;
		dx_ptr<ID3D11ShaderResourceView> m_duckTexture;

		dx_ptr<ID3D11VertexShader> m_waterVS;
		dx_ptr<ID3D11PixelShader> m_waterPS;
		dx_ptr<ID3D11InputLayout> m_waterIL;
#pragma endregion

#pragma region Matrices
		DirectX::XMFLOAT4X4 m_projMtx;
		DirectX::XMMATRIX m_wallsMtx[6];
		DirectX::XMMATRIX m_sheetMtx;
		DirectX::XMMATRIX m_revSheetMtx;
#pragma endregion
		float d[WATER_N][WATER_N];
		float heights[WATER_N][WATER_N];
		float heightsOld[WATER_N][WATER_N];
		float xPos, yPos, t, lastXPos, lastYPos;
		DirectX::XMFLOAT2 deBoor0, deBoor1, deBoor2, deBoor3;
		DirectX::XMFLOAT4 bernsteinXs, bernsteinYs;
		std::vector<BYTE> normalMap;
		void SetWorldMtx(DirectX::XMFLOAT4X4 mtx);
		void DrawMesh(const Mesh& m, DirectX::XMFLOAT4X4 worldMtx);
		void UpdateCameraCB(DirectX::XMFLOAT4X4 cameraMtx);
		void UpdatePlaneCB(DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 dir);
		void CreateWallsMtx();
		void DrawWalls();
		void CreateSheetMtx();
		void DrawSheet(bool colors);


		void SetShaders(const dx_ptr<ID3D11InputLayout>& il,const dx_ptr<ID3D11VertexShader>& vs, const dx_ptr<ID3D11PixelShader>& ps);
		void SetTextures(std::initializer_list<ID3D11ShaderResourceView*> resList, const dx_ptr<ID3D11SamplerState>& sampler);
		void SetTextures(std::initializer_list<ID3D11ShaderResourceView*> resList) { SetTextures(std::move(resList), m_sampler); }
		
		void RandomDrops();
		DirectX::XMFLOAT4 DeBoorToBernstein(float a, float b, float c, float d);
		DirectX::XMFLOAT2 RandPoint();
		void SimDuck(float dt);
		void InitializeWater();
		void UpdateHeights(float dt);
		void InitializeSimDuck();
	};
}