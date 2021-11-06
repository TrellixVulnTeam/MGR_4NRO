#include "mesh.h"
#include <algorithm>

using namespace std;

namespace Real_Engine
{
	void Mesh::SetDrawBuffers(const dx_ptr<ID3D11DeviceContext>& context) const
	{
		ID3D11Buffer* newBuffer[] = { this->m_vertexBuffers.get() };

		context->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetVertexBuffers(0, 1U, newBuffer, &m_strides, &m_offsets);
	}

    void Mesh::SetInstancedDrawData(const dx_ptr<ID3D11DeviceContext>& context, const dx_ptr<ID3D11Buffer>& instancedBuffer, UINT stride, UINT offset) const
    {
        ID3D11Buffer* newBuffer[] = { this->m_vertexBuffers.get(), instancedBuffer.get() };
        UINT strides[] = { this->m_strides, stride };
        UINT offsets[] = { this->m_offsets, offset };

        context->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetVertexBuffers(0, 2U, newBuffer, strides, offsets);
    }

	void Mesh::Draw(const dx_ptr<ID3D11DeviceContext>& context, UINT indexCount, UINT indexStart) const
	{
		context->DrawIndexed(indexCount, indexStart, 0);
	}

	void Mesh::Draw(const dx_ptr<ID3D11DeviceContext>& context) const
	{
		context->DrawIndexed(this->m_groupStartIndex.back(), 0, 0);
	}

    void Mesh::DrawInstanced(const dx_ptr<ID3D11DeviceContext>& context, UINT indexCount, UINT instanceCount, UINT indexStart) const
    {
        context->DrawIndexedInstanced(indexCount, instanceCount, indexStart, 0, 0);
    }

	void Mesh::DrawInstanced(const dx_ptr<ID3D11DeviceContext>& context, UINT instanceCount) const
	{
		context->DrawIndexedInstanced(this->m_groupStartIndex.back(), instanceCount, 0, 0, 0);
	}

	Mesh::Mesh() { }

	Mesh::Mesh(dx_ptr<ID3D11Buffer>&& vbuffers, UINT&& vstrides, UINT&& voffsets,
		dx_ptr<ID3D11Buffer>&& indices, std::vector<UINT>& groupStartIndex)
	{
		this->m_groupStartIndex = groupStartIndex;

		this->m_indexBuffer = std::move(indices);
		this->m_vertexBuffers = std::move(vbuffers);
		this->m_strides = vstrides;
		this->m_offsets = voffsets;
	}

	Mesh::~Mesh()
	{
		Release();
	}

	void Mesh::Release()
	{
		m_vertexBuffers.reset();
		m_indexBuffer.reset();
	}

	Mesh& Mesh::operator=(Mesh&& right)
	{
		Release();

		m_vertexBuffers = move(right.m_vertexBuffers);
		m_indexBuffer = move(right.m_indexBuffer);
		m_strides = right.m_strides;
		m_offsets = right.m_offsets;
		m_groupStartIndex = right.m_groupStartIndex;

		right.Release();
		return *this;
	}

	Mesh::Mesh(Mesh&& right)
		: m_indexBuffer(move(right.m_indexBuffer)), m_vertexBuffers(move(right.m_vertexBuffers)), m_strides(move(right.m_strides)),
		m_offsets(move(right.m_offsets)), m_groupStartIndex(right.m_groupStartIndex)
	{
		right.Release();
	}
}
