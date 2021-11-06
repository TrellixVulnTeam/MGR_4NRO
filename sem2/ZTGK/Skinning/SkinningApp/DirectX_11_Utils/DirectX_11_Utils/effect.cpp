#include "effect.h"


namespace Real_Engine
{
	Effect::Effect(ID3DX11Effect* _effect, string _technique_name) :
		m_effect(_effect)
	{
		auto eff = m_effect->GetTechniqueByName(_technique_name.c_str());
		m_technique.reset(eff);
		m_technique->GetDesc(&m_desc);

		SetPass(0);
	}

	void Effect::Apply(const dx_ptr<ID3D11DeviceContext>& context) const
	{
		current_pass->Apply(0, context.get());
	}

	void Effect::SetPass(int index)
	{
		current_pass.reset(m_technique->GetPassByIndex(index));
	}

	void Effect::SetPass(string name)
	{
		current_pass.reset(m_technique->GetPassByName(name.c_str()));
	}

	void Effect::SetTechnique(string name)
	{
		auto eff = m_effect->GetTechniqueByName(name.c_str());

		m_technique.reset(eff);
	
		this->SetPass(0);
	}

	D3DX11_PASS_DESC Effect::GetPassDesc()
	{
		D3DX11_PASS_DESC passDesc;
		m_technique->GetPassByIndex(0)->GetDesc(&passDesc);
		return passDesc;
	}

	ID3DX11EffectMatrixVariable* Effect::GetMatrixVariable(string name)
	{
		return m_effect->GetVariableByName(name.c_str())->AsMatrix();
	}

	ID3DX11EffectVectorVariable* Effect::GetVectorVariable(string name)
	{
		return m_effect->GetVariableByName(name.c_str())->AsVector();
	}

	ID3DX11EffectScalarVariable* Effect::GetScalarVariable(string name)
	{
		return m_effect->GetVariableByName(name.c_str())->AsScalar();
	}

	ID3DX11EffectShaderResourceVariable* Effect::GetSRVariable(string name)
	{
		return m_effect->GetVariableByName(name.c_str())->AsShaderResource();
	}

	ID3DX11EffectVariable* Effect::GetVariable(string name)
	{
		return m_effect->GetVariableByName(name.c_str());
	}

	void Effect::SetVariable(const dx_ptr<ID3DX11EffectMatrixVariable>& variable, XMMATRIX value)
	{
		variable->SetMatrix(reinterpret_cast<float*>(&value));
	}

	void Effect::SetVariable(const dx_ptr<ID3DX11EffectMatrixVariable>& variable, const XMFLOAT4X4* value, int n)
	{
		variable->SetMatrixArray(reinterpret_cast<const float*>(value), 0, n);
	}

	void Effect::SetVariable(const dx_ptr<ID3DX11EffectMatrixVariable>& variable, const XMMATRIX* value, int n)
	{
		variable->SetMatrixArray(reinterpret_cast<const float*>(value), 0, n);
	}

	void Effect::SetVariable(const dx_ptr<ID3DX11EffectVectorVariable>& variable, XMVECTOR value)
	{
		variable->SetFloatVector(reinterpret_cast<float*>(&value));
	}

	void Effect::SetVariable(const dx_ptr<ID3DX11EffectVectorVariable>& variable, XMFLOAT3 value)
	{
		variable->SetFloatVector(reinterpret_cast<float*>(&value));
	}

	void Effect::SetVariable(const dx_ptr<ID3DX11EffectScalarVariable>& variable, float value)
	{
		variable->SetFloat(value);
	}

	void Effect::SetVariable(const dx_ptr<ID3DX11EffectScalarVariable>& variable, int value)
	{
		variable->SetInt(value);
	}

	void Effect::SetVariable(const dx_ptr<ID3DX11EffectScalarVariable>& variable, bool value)
	{
		variable->SetBool(value);
	}

	void Effect::SetVariable(const dx_ptr<ID3DX11EffectShaderResourceVariable>& variable, ID3D11ShaderResourceView* value)
	{
		variable->SetResource(value);
	}
}
