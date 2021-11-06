#include "timer.h"

namespace Real_Engine
{
	Timer::Timer() :
		m_secondsPerCount(0.0), m_deltaTime(-1.0), m_baseTime(0),
		m_pausedTime(0), m_prevTime(0), m_currTime(0), m_stopped(false)
	{
		__int64 countsPerSec;
		GetFrequency(countsPerSec);
		m_secondsPerCount = 1.0 / (double)countsPerSec;
		__int64 currTime;
		GetTime(currTime);
		m_prevTime = currTime;
		m_currTime = currTime;
	}

	void Timer::Tick()
	{
		if (m_stopped)
		{
			m_deltaTime = 0.0;
			return;
		}

		__int64 currTime;
		GetTime(currTime);
		m_currTime = currTime;

		m_deltaTime = (m_currTime - m_prevTime)*m_secondsPerCount;

		m_prevTime = m_currTime;

		if (m_deltaTime < 0.0)
			m_deltaTime = 0.0;
	}

	double Timer::DeltaTime() const
	{
		return m_deltaTime;
	}

	void Timer::Reset()
	{
		__int64 currTime;
		GetTime(currTime);

		m_baseTime = currTime;
		m_prevTime = currTime;
		m_stopTime = 0;
		m_stopped = false;
	}

	void Timer::Stop()
	{
		if (!m_stopped)
		{
			__int64 currTime;
			GetTime(currTime);

			m_stopTime = currTime;
			m_stopped = true;
		}
	}

	void Timer::Start()
	{
		__int64 startTime;
		GetTime(startTime);

		if (m_stopped)
		{
			m_pausedTime += (startTime - m_stopTime);
			m_prevTime = startTime;
			m_stopTime = 0;
			m_stopped = false;
		}
	}

	double Timer::TotalTime() const
	{
		if (m_stopped)
			return (double)(((m_stopTime - m_pausedTime) - m_baseTime)*m_secondsPerCount);

		return (double)(((m_currTime - m_pausedTime) - m_baseTime)*m_secondsPerCount);
	}

	void Timer::GetTime(__int64& time)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&time);
	}

	void Timer::GetFrequency(__int64& countsPerSec)
	{
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	}
}
