#pragma once

#include "effect.h"
#include "dxptr.h"
#include <DirectXMath.h>
#include "Structures.h"
#include <vector>


using namespace DirectX;
using namespace Real_Engine;

namespace SkinningApp
{
	class SkinningEffect : public Effect
	{
	public:
		static const std::string SKINNING_TECH;

		SkinningEffect() {};
		SkinningEffect(ID3DX11Effect* effect, string technique_name, XMMATRIX proj_matrix);

		void UpdateProjMatrix(XMMATRIX value);
		void UpdateViewMatrix(XMMATRIX value);
		void UpdateWorldMatrix(XMMATRIX value);
		void UpdateBoneMatrix(XMMATRIX* value, int count);
		void UpdateDirectionalLight(std::vector<DirectionalLight> value);
		void UpdateMaterial(Material value);
		void UpdateEyePosition(XMVECTOR value);
	protected:
		dx_ptr<ID3DX11EffectMatrixVariable> m_ProjMatrix;
		dx_ptr<ID3DX11EffectMatrixVariable> m_ViewMatrix;
		dx_ptr<ID3DX11EffectMatrixVariable> m_WorldMatrix;
		dx_ptr<ID3DX11EffectVariable> m_DirectionalLight;
		dx_ptr<ID3DX11EffectVariable> m_Material;
		dx_ptr<ID3DX11EffectVectorVariable> m_EyePosition;
		dx_ptr<ID3DX11EffectMatrixVariable> m_BoneMatrix;
	};
}