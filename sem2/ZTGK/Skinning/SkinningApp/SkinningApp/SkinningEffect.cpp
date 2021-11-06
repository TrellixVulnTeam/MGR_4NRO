#include "SkinningEffect.h"



namespace SkinningApp
{
	const std::string SkinningEffect::SKINNING_TECH = "SkinningTech";
	
	SkinningEffect::SkinningEffect(ID3DX11Effect* effect, string technique_name, XMMATRIX proj_matrix) :Effect(effect, technique_name)
	{
		//TODO: 02. Zainicjować zmienne przetrzymujące wskaźniki do zmiennych w buforze 
		m_ProjMatrix.reset(GetMatrixVariable("projMatrix"));
		m_ViewMatrix.reset(GetMatrixVariable("viewMatrix"));
		m_WorldMatrix.reset(GetMatrixVariable("worldMatrix"));
		m_BoneMatrix.reset(GetMatrixVariable("boneMatrix"));
		m_DirectionalLight.reset(GetVariable("dirLight"));
		m_Material.reset(GetVariable("mat"));
		m_EyePosition.reset(GetVectorVariable("eyePos"));

		UpdateProjMatrix(proj_matrix);
	}

	//TODO: 03. Uzupełnić poniższe funkcje służące do aktualizowania zmiennych w buforze
	void SkinningEffect::UpdateProjMatrix(XMMATRIX value)
	{
		SetVariable(m_ProjMatrix, value);
	}

	void SkinningEffect::UpdateViewMatrix(XMMATRIX value)
	{
		SetVariable(m_ViewMatrix, value);
	}

	void SkinningEffect::UpdateWorldMatrix(XMMATRIX value)
	{
		SetVariable(m_WorldMatrix, value);
	}

	void SkinningEffect::UpdateBoneMatrix(XMMATRIX* value, int count)
	{
		SetVariable(m_BoneMatrix, value,count);
	}

	void SkinningEffect::UpdateDirectionalLight(std::vector<DirectionalLight> value)
	{
		SetVariable(m_DirectionalLight, value.data(),static_cast<int>(value.size()));
	}

	void SkinningEffect::UpdateMaterial(Material value)
	{
		SetVariable(m_Material, value);
	}

	void SkinningEffect::UpdateEyePosition(XMVECTOR value)
	{
		SetVariable(m_EyePosition, value);
	}
}