#pragma once

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>
#include <memory>
#include "diptr.h"
#include "dxptr.h"
#include "keyboard.h"
#include "mouse.h"

namespace Real_Engine
{
	class DiDevice
	{
	public:
		DiDevice(HINSTANCE hInst = nullptr);

		void Initialize(HINSTANCE hInst);

		di_ptr CreateInputDevice(HWND hWnd, const GUID& deviceGuid, const DIDATAFORMAT& dataFormat) const;
		inline Keyboard CreateKeyboardDevice(HWND hWnd) const
		{
			return Keyboard(CreateInputDevice(hWnd, GUID_SysKeyboard, c_dfDIKeyboard));
		}
		inline Mouse CreateMouseDevice(HWND hWnd) const
		{
			return Mouse(CreateInputDevice(hWnd, GUID_SysMouse, c_dfDIMouse));
		}

	private:
		
        Real_Engine::dx_ptr<IDirectInput8> m_device;
	};
}

