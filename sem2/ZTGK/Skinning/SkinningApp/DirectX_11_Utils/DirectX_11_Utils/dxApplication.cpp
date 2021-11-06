#include "dxApplication.h"
#include "dxstructures.h"
#include <iostream>
using namespace Real_Engine;

DxApplication::DxApplication(HINSTANCE hInstance, int wndWidth, int wndHeight, std::wstring wndTitle)
	: WindowApplication(hInstance, wndWidth, wndHeight, wndTitle), m_device(m_window)
{
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	auto windowSize = m_window.getClientSize();
	auto backBufferTexture = m_device.swapChain().GetBuffer();
	ViewportDescription viewport(windowSize.cx, windowSize.cy);
	m_device.SetViewport(viewport);
	m_depthStencilView = m_device.CreateDepthStencilView(windowSize);
	m_renderTargetView.reset(m_device.CreateRenderTargetView(backBufferTexture));
}

int DxApplication::MainLoop()
{
	MSG msg = { nullptr };

	while (msg.message != WM_QUIT)
	{

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_timer.Tick(); double deltaTime = m_timer.DeltaTime();
            
			Update((float)deltaTime); Render();

			m_device.swapChain().Present();
		}
	}
	return static_cast<int>(msg.wParam);
}

void DxApplication::Render() 
{ 
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_device.ClearRenderTarget(m_renderTargetView.get(), m_depthStencilView.get(), clearColor);
}
