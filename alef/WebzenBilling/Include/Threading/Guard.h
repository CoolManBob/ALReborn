#ifndef  __WBANetwork_Guard_H
#define  __WBANetwork_Guard_H

namespace WBANetwork
{
		template < class _T >
		class Guard
		{
		public:
					Guard( _T& lock )
						: m_monitorObj( &lock )
					{
						m_monitorObj->Lock();
					}

					~Guard()
					{
						m_monitorObj->Unlock();
					}

		private:
					_T*		m_monitorObj;
		};
}

#endif