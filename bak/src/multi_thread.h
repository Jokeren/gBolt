#include <pthread.h>

class Thread
{
	public:
		Thread() {/* empty */}
		virtual ~Thread() {/* empty */}

		/** Returns true if the thread was successfully started, false if there was an error starting the thread */
		bool start_internal_thread()
		{
			return (pthread_create(&_m_thread, NULL, internal_thread_func, this) == 0);
		}

		/** Will not return until the internal thread has exited. */
		void wait_for_exit()
		{
			(void) pthread_join(_m_thread, NULL);
		}

	protected:
		/** Implement this method in your subclass with the code you want your thread to run. */
		virtual void internal_thread_entry() = 0;

	private:
		static void * internal_thread_func(void *_this) {((Thread *)_this)->internal_thread_entry(); return NULL;}
		pthread_t _m_thread;
};
