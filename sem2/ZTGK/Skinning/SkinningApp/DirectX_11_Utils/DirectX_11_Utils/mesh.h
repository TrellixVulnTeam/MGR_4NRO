#pragma once

#include "dxptr.h"
#include <vector>
#include <DirectXMath.h>
#include <D3D11.h>

namespace Real_Engine
{
	class Mesh
	{
	public:
		Mesh();
		Mesh(dx_ptr<ID3D11Buffer>&& vbuffers,
			UINT&& vstrides, UINT&& voffsets,
			dx_ptr<ID3D11Buffer>&& indices,
			std::vector<UINT>& groupStartIndex);

		Mesh(Mesh&& right);
		Mesh(const Mesh& right) = delete;
		void Release();
		~Mesh();

		Mesh& operator=(const Mesh& right) = delete;
		Mesh& operator=(Mesh&& right);

		void SetDrawBuffers(const dx_ptr<ID3D11DeviceContext>& context) const;
		void SetInstancedDrawData(const dx_ptr<ID3D11DeviceContext>& context, const dx_ptr<ID3D11Buffer>& instancedBuffer, UINT stride, UINT offset) const;

		void Draw(const dx_ptr<ID3D11DeviceContext>& context, UINT indexCount, UINT indexStart) const;
		void Draw(const dx_ptr<ID3D11DeviceContext>& context) const;
        void DrawInstanced(const dx_ptr<ID3D11DeviceContext>& context, UINT indexCount, UINT instanceCount, UINT indexStart) const;
		void DrawInstanced(const dx_ptr<ID3D11DeviceContext>& context, UINT instanceCount) const;

        std::vector<UINT> GetGroupsStartIndex() { return this->m_groupStartIndex; }

	private:

		dx_ptr<ID3D11Buffer> m_indexBuffer;
        dx_ptr<ID3D11Buffer> m_vertexBuffers;
		unsigned int m_strides;
		unsigned int m_offsets;
		std::vector<UINT> m_groupStartIndex;
	};
}

//#include <d3d11.h>
//#include <DirectXMath.h>
//#include "dxptr.h"
//
//namespace Real_Engine
//{
//    class Mesh
//    {
//    public:
//        Mesh();
//        Mesh(dx_ptr<ID3D11Buffer>&& vertexBuffer, UINT stride, UINT offset, dx_ptr<ID3D11Buffer>&& indexBuffer, UINT indexCount);
//        Mesh(Mesh&& right);
//        ~Mesh();
//
//        //Mesh& operator=(const Mesh& right);
//        Mesh& operator=(Mesh&& right);
//        void Render(const dx_ptr<ID3D11DeviceContext>& context) const;
//        void Release();
//
//    private:
//        dx_ptr<ID3D11Buffer> m_vertexBuffer;
//        dx_ptr<ID3D11Buffer> m_indexBuffer;
//        UINT m_stride;
//        UINT m_offset;
//        UINT m_indexCount;
//    };
//}
