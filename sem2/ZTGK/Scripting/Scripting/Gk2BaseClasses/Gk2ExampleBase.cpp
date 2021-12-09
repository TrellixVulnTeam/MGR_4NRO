#include "gk2ExampleBase.h"

using namespace DirectX;

namespace mini
{
    Gk2ExampleBase::Gk2ExampleBase(HINSTANCE hInstance, UINT width, UINT height, std::wstring title)
        : DxApplication(hInstance, width, height, title), m_inputDevice(hInstance),
        m_mouse(m_inputDevice.CreateMouseDevice(m_window.getHandle())),
        m_keyboard(m_inputDevice.CreateKeyboardDevice(m_window.getHandle())),
        m_camera(XMFLOAT3(0.0f, 0.0f, 0.0f), 0.01f, 25.0f, 5)
    { }

    bool Gk2ExampleBase::HandleCameraInput(double dt)
    {
        MouseState mstate;
        if (m_mouse.GetState(mstate))
        {
            auto d = mstate.getMousePositionChange();
            if (mstate.isButtonDown(0))
            {
                float tmp = d.y*ROTATION_SPEED;
                float angle = m_camera.getYAngle();
                if (m_camera.getXAngle() + tmp < -XM_PIDIV2)
                    tmp = -XM_PIDIV2 - m_camera.getXAngle();
                else
                    if (m_camera.getXAngle() + tmp > XM_PIDIV2)
                        tmp = XM_PIDIV2 - m_camera.getXAngle();
                m_camera.Rotate(tmp, d.x*ROTATION_SPEED);
            }
            else if (mstate.isButtonDown(1))
                m_camera.Zoom(d.y * ZOOM_SPEED);
        }
        KeyboardState kstate;
        if (!m_keyboard.GetState(kstate))
            return false;

        float distance = 3.0f * (float)dt;
        if (kstate.isKeyDown(30/*A*/))
            m_camera.MoveTarget(XMFLOAT3{ -distance, 0, 0 });
        if (kstate.isKeyDown(32/*D*/))
            m_camera.MoveTarget(XMFLOAT3{ distance, 0, 0 });
        if (kstate.isKeyDown(17/*W*/))
            m_camera.MoveTarget(XMFLOAT3{ 0, distance, 0 });
        if (kstate.isKeyDown(31/*S*/))
            m_camera.MoveTarget(XMFLOAT3{ 0, -distance, 0 });
        if (kstate.isKeyDown(16/*Q*/))
            m_camera.MoveTarget(XMFLOAT3{ 0, 0, distance });
        if (kstate.isKeyDown(18/*E*/))
            m_camera.MoveTarget(XMFLOAT3{ 0, 0, -distance });

        return true;
    }
}
