#pragma once

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>

namespace mini
{
    struct KeyboardState
    {
        static const unsigned int STATE_TAB_LENGTH = 256;
        static const BYTE KEY_MASK = 0x80;

        BYTE m_keys[STATE_TAB_LENGTH];

        KeyboardState()
        {
            ZeroMemory(m_keys, STATE_TAB_LENGTH*sizeof(char));
        }

        KeyboardState(const KeyboardState& other) = default;

        KeyboardState& operator=(const KeyboardState& other) = default;

        bool isKeyDown(BYTE keyCode) const
        {
            return 0 != (m_keys[keyCode] & KEY_MASK);
        }

        bool isKeyUp(BYTE keyCode) const
        {
            return 0 == (m_keys[keyCode] & KEY_MASK);
        }

        bool operator[](BYTE keyCode) const
        {
            return 0 != (m_keys[keyCode] & KEY_MASK);
        }
    };
}
