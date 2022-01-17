#ifndef  __WBANetwork_Thread_H
#define  __WBANetwork_Thread_H

namespace WBANetwork
{
	class Thread
	{
	public:
				Thread();
		virtual ~Thread();

				HANDLE			Start();
				bool			Terminate( DWORD exitCode );
				bool			WaitForTerminate( DWORD timeout = INFINITE );

	protected:
		virtual	void			Run() = 0;

				HANDLE			m_handleThread;
				unsigned int	m_threadID;

	private:
		static unsigned int __stdcall	Runner( LPVOID parameter );
	};
}

#endif