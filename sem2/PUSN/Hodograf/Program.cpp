#include "Program.h"
Program::Program()
{

}

void Program::AddValues(float t, float y, float dy, float ddy)
{
	_y.AddPoint(t, y);
	_dy.AddPoint(t, dy);
	_ddy.AddPoint(t, ddy);
	_state.AddPoint(y, dy);
}

Window::Window()
{
}

ScrollingBuffer::ScrollingBuffer(int max_size)
{
		MaxSize = max_size;
		Offset = 0;
		Data.reserve(MaxSize);
}
void ScrollingBuffer::AddPoint(float x, float y) {
	if (Data.size() < MaxSize)
		Data.push_back(ImVec2(x, y));
	else {
		Data[Offset] = ImVec2(x, y);
		Offset = (Offset + 1) % MaxSize;
	}
}

void ScrollingBuffer::Erase() {
	if (Data.size() > 0) {
		Data.shrink(0);
		Offset = 0;
	}
}