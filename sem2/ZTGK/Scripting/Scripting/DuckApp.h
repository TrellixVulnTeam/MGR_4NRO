#pragma once
#include <vector>
#include <memory>

#include <DirectXMath.h>
#include <mesh.h>
#include <constantBuffer.h>

#include "Geometry\Mesh\MeshRobot.h"
#include "Gk2BaseClasses\TexturedEffect.h"
#include "Gk2BaseClasses\Gk2ExampleBase.h"

#include "Water.h"
#include "GameObject.h"

/* TODO (4.01) Include Script Manager */
#include "ScriptManager.h"

using namespace mini;

namespace duckApp
{
    class DuckApp : public Gk2ExampleBase
    {
    public:
        explicit DuckApp(HINSTANCE hInstance);

    protected:
        void Update(const Clock& clock) override;
        void Render() override;

    private:
        static const DirectX::XMFLOAT4 LIGHT_POS;

        /* TODO (4.02) Create Script Manager Pointer Instance */
        std::unique_ptr<ScriptManager> script_manager;

        std::unique_ptr<GameObject> duck_instance;
        std::unique_ptr<Water> water_instance;

        Mesh wall_mesh;
        DirectX::XMFLOAT4X4 wall_mesh_matrix[6];
        

        DirectX::XMFLOAT4X4 m_projMtx;
        dx_ptr<ID3D11InputLayout> m_inputlayout;
        dx_ptr<ID3D11SamplerState> m_samplerWrap;
        dx_ptr<ID3D11SamplerState> m_samplerBorder;


        ConstantBuffer<DirectX::XMFLOAT4X4>
            m_cbWorldMtx,    //vertex shader constant buffer slot 0
            m_cbProjMtx;    //vertex shader constant buffer slot 2 & geometry shader constant buffer slot 0

        ConstantBuffer<DirectX::XMFLOAT4> m_cbCameraPos;        //vertex shader constant buffer for Camera Position for Water
        ConstantBuffer<DirectX::XMFLOAT4> m_cbLightPos;        //pixel shader constant buffer slot 1
        ConstantBuffer<DirectX::XMFLOAT4X4, 2> m_cbViewMtx;        //vertex shader constant buffer slot 1
        ConstantBuffer<DirectX::XMFLOAT4> m_cbSurfaceColor;        //pixel shader constant buffer slot 0
        ConstantBuffer<DirectX::XMFLOAT4X4> m_cbTexMtx;

        void DrawWalls();
        void DrawDuck();
        void DrawWater();

        Mesh box_mesh;
        XMFLOAT4X4 box_mesh_matrix;

        TexturedEffect textured_wall_effect;

        void UpdateCamera(DirectX::XMMATRIX viewMtx);
        void UpdateDuck(float dtime);
        void UpdateWater();

        void SetWorldMtx(DirectX::XMFLOAT4X4 mtx);
        void DrawMesh(const Mesh& m, DirectX::XMFLOAT4X4 worldMtx);

        void SetSurfaceColor(DirectX::XMFLOAT4 color);
    };
}
