#pragma once
#include "windowApplication.h"
#include "dxDevice.h"
#include <DirectXMath.h>
using namespace DirectX;
class DxApplication : public mini::WindowApplication {
public:
	explicit DxApplication(HINSTANCE hInstance);
	std::vector<XMFLOAT2> CreateTriangleVertices();
protected:
	int MainLoop() override;
	bool ProcessMessage(mini::WindowMessage& msg) override;
private:
	void Render();
	void Update();
	void Draw(XMFLOAT4X4* modelMtx);
	void SetViewMtx();

	DxDevice m_device;
	mini::dx_ptr<ID3D11RenderTargetView> m_backBuffer;
	mini::dx_ptr<ID3D11DepthStencilView> m_depthBuffer;
	mini::dx_ptr<ID3D11Buffer> m_vertexBuffer;
	mini::dx_ptr<ID3D11VertexShader> m_vertexShader;
	mini::dx_ptr<ID3D11PixelShader> m_pixelShader;
	mini::dx_ptr<ID3D11InputLayout> m_layout;

	struct VertexPositionColor {DirectX::XMFLOAT3 position, color;};
	static std::vector<VertexPositionColor> CreateCubeVertices();
	static std::vector<unsigned short> CreateCubeIndices();
	mini::dx_ptr<ID3D11Buffer> m_indexBuffer;
	XMFLOAT4X4 m_modelMtx, m_viewMtx, m_projMtx;
	XMFLOAT4X4 m_modelMtx2;
	mini::dx_ptr<ID3D11Buffer> m_cbMVP;
	int l_yPos, r_yPos;
	float v_angle = -30.f, v_dist = 10.f;
};
