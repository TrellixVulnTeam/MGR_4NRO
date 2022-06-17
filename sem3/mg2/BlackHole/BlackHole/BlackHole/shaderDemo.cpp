#include "shaderDemo.h"

using namespace mini;
using namespace gk2;
using namespace DirectX;
using namespace std;
using namespace utils;

ShaderDemo::ShaderDemo(HINSTANCE hInst) : GK2ShaderDemoBase(hInst)
{
	//Shader Variables
	m_variables.AddSemanticVariable("modelMtx", VariableSemantic::MatM);
	m_variables.AddSemanticVariable("viewProjMtx", VariableSemantic::MatVP);
	m_variables.AddSemanticVariable("viewInvMtx", VariableSemantic::MatVInv);
	//m_variables.AddSemanticVariable("camX", VariableSemantic::Vec4CamRight);
	//m_variables.AddSemanticVariable("camY", VariableSemantic::Vec4CamUp);
	//m_variables.AddSemanticVariable("camDir", VariableSemantic::Vec4CamDir);
	//m_variables.AddSemanticVariable("camPos", VariableSemantic::Vec4CamPos);
	m_variables.AddSemanticVariable("vpInv", VariableSemantic::MatVPInv);

	XMFLOAT3 holePos = { 0.0f, 0.0f, 1 };
	m_variables.AddGuiVariable("cam_holePos", holePos, -10, 10);
	m_variables.AddGuiVariable("holePos_e", 9.0f, 3.0f, 12.0f, 1.0f);
	m_variables.AddGuiVariable("mass", 1.0f, 1.0f, 10.0f);
	m_variables.AddGuiVariable("mass_e", 7.0f, 3.0f, 12.0f, 1.0f);

	m_variables.AddSemanticVariable("mvpMtx", VariableSemantic::MatMVP);


	//Models
	XMFLOAT4X4 modelMtx;
	auto envModel = addModelFromString("hex 0 0 0 1.73205");
	XMStoreFloat4x4(&modelMtx, XMMatrixScaling(20, 20, 20));
	model(envModel).applyTransform(modelMtx);

	auto screenSize = m_window.getClientSize();
	m_variables.AddSampler(m_device, "samp");
	m_variables.AddTexture(m_device, "envMap", L"textures/cubemapt.dds");
	//m_variables.AddTexture(m_device, "envMap", L"textures/cubemapt.dds");
	m_variables.AddSemanticVariable("projInvMtx", VariableSemantic::MatPInv);

	m_variables.AddSemanticVariable("viewportDim", VariableSemantic::Vec2ViewportDims);

	auto passEnv = addPass(L"envVS.cso", L"envPS.cso", getDefaultRenderTarget());
	addModelToPass(passEnv, envModel);

	RasterizerDescription rs;
	rs.CullMode = D3D11_CULL_NONE;
	addRasterizerState(passEnv, RasterizerDescription(true));
}
