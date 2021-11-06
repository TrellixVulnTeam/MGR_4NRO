#pragma once

#include "Windows/windowApplication.h"
#include "dxDevice.h"
#include "timer.h"

namespace Real_Engine
{
	class DxApplication : public WindowApplication
	{
	public:
		explicit DxApplication(HINSTANCE hInstance,
			int wndWidth = Window::m_defaultWindowWidth,
			int wndHeight = Window::m_defaultWindowHeight,
			std::wstring wndTitle = L"DirectX Window");

	protected:
		int MainLoop() override;

		virtual void Update(const float dt) { }
		virtual void Render();

        Timer m_timer;
		DxDevice m_device;

	protected:
        dx_ptr<ID3D11RenderTargetView> m_renderTargetView;
		dx_ptr<ID3D11DepthStencilView> m_depthStencilView;
	};
}
