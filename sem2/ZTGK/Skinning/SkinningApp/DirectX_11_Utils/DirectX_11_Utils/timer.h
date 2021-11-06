#include <Windows.h>

namespace Real_Engine
{
	class Timer
	{
	public:
		Timer();

		double TotalTime() const;
		double DeltaTime() const;

		void Reset();
		void Start();
		void Stop();
		void Tick();

	private:
		double m_secondsPerCount;
		double m_deltaTime;

		__int64 m_baseTime;
		__int64 m_pausedTime;
		__int64 m_stopTime;
		__int64 m_prevTime;
		__int64 m_currTime;

		bool m_stopped;

		void GetTime(__int64& time);
		void GetFrequency(__int64& countsPerSec);
	};
}
