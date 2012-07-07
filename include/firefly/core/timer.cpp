#include <firefly/core/timer.hpp>
#include <cstdlib>

////////////////////////////////////////////////////////////////////////

namespace ff {

// ctor

    timer::timer()
    {
        #ifdef WIN32

            QueryPerformanceFrequency(&li_freq);
            li_start.QuadPart = 0;
            li_end.QuadPart = 0;

        #else

            tv_start.tv_sec = tv_start.tv_usec = 0;
            tv_end.tv_sec = tv_end.tv_usec = 0;

        #endif

        m_running = false;
        m_start = 0;
        m_end = 0;
    }


// dtor

    timer::~timer()
    {
    }


// start the timer

    void timer::start()
    {
        m_running = true;

        #ifdef WIN32

            QueryPerformanceCounter(&li_start);

        #else

            gettimeofday(&tv_start, NULL);

        #endif
    }


// stop the timer

    void timer::stop()
    {
        m_running = false;

        #ifdef WIN32

            QueryPerformanceCounter(&li_end);

        #else

            gettimeofday(&tv_end, NULL);

        #endif
    }


// return timer count in seconds

    double timer::elapsed()
    {
        return this->elapsed_micro() * 0.000001;
    }


// return timer count in milliseconds

    double timer::elapsed_ms()
    {
        return this->elapsed_micro() * 0.001;
    }


// return timer count in microseconds

    double timer::elapsed_micro()
    {
        #ifdef WIN32

            if (m_running)
                QueryPerformanceCounter(&li_end);

            m_start = li_start.QuadPart * (1000000. / li_freq.QuadPart);
            m_end = li_end.QuadPart * (1000000.0 / li_freq.QuadPart);

        #else

            if (m_running)
                gettimeofday(&tv_end, NULL);

            m_start = (tv_start.tv_sec * 1000000.) + tv_start.tv_usec;
            m_end = (tv_end.tv_sec * 1000000.) + tv_end.tv_usec;

        #endif

        return (m_end - m_start);
    }

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////
