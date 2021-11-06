#ifndef EFFECT_H
#define EFFECT_H

#include "Microsoft/d3dx11Effect.h"
#include "dxptr.h"
#include <string>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

namespace Real_Engine
{
	class Effect
	{
	public:
		Effect() {};
		Effect(ID3DX11Effect* _effect, string _technique_name);

		void Apply(const dx_ptr<ID3D11DeviceContext>& context) const;
		void SetPass(int index);
		void SetPass(string name);
		void SetTechnique(string name);
		D3DX11_PASS_DESC GetPassDesc();

		int passCount() { return m_desc.Passes; }
		ID3DX11EffectTechnique* technique() { return m_technique.get(); }
		ID3DX11Effect* effect() { return m_effect.get(); }
	protected:
		ID3DX11EffectMatrixVariable* GetMatrixVariable(string name);
		ID3DX11EffectVectorVariable* GetVectorVariable(string name);
		ID3DX11EffectScalarVariable* GetScalarVariable(string name);
		ID3DX11EffectShaderResourceVariable* GetSRVariable(string name);
		ID3DX11EffectVariable* GetVariable(string name);

		void SetVariable(const dx_ptr<ID3DX11EffectMatrixVariable>& variable, XMMATRIX value);
		void SetVariable(const dx_ptr<ID3DX11EffectMatrixVariable>& variable, const XMFLOAT4X4* value, int n);
		void SetVariable(const dx_ptr<ID3DX11EffectMatrixVariable>& variable, const XMMATRIX* value, int n);
		void SetVariable(const dx_ptr<ID3DX11EffectVectorVariable>& variable, XMVECTOR value);
		void SetVariable(const dx_ptr<ID3DX11EffectVectorVariable>& variable, XMFLOAT3 value);
		void SetVariable(const dx_ptr<ID3DX11EffectScalarVariable>& variable, float value);
		void SetVariable(const dx_ptr<ID3DX11EffectScalarVariable>& variable, int value);
		void SetVariable(const dx_ptr<ID3DX11EffectScalarVariable>& variable, bool value);
		void SetVariable(const dx_ptr<ID3DX11EffectShaderResourceVariable>& variable, ID3D11ShaderResourceView* value);
		template<class T>
		void SetVariable(const dx_ptr<ID3DX11EffectVariable>& variable, T value)
		{
			variable->SetRawValue(&value, 0, sizeof(T));
		}

		template<class T>
		void SetVariable(const dx_ptr<ID3DX11EffectVariable>& variable, T* value, int size)
		{
			variable->SetRawValue(value, 0, size * sizeof(T));
		}

		dx_ptr<ID3DX11Effect> m_effect;
		dx_ptr<ID3DX11EffectTechnique> m_technique;
		dx_ptr<ID3DX11EffectPass> current_pass;
		D3DX11_TECHNIQUE_DESC m_desc;
	};
}

#endif /* EFFECT_H */
