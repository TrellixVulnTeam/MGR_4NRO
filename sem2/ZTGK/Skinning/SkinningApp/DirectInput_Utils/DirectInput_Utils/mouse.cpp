#include "mouse.h"

using namespace std;
using namespace Real_Engine;

bool Mouse::GetState(MouseState& state) const
{
	return DeviceBase::GetState(sizeof(DIMOUSESTATE), reinterpret_cast<void*>(&state.m_state));
}
