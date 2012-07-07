#ifndef FIREFLY_TIMER_HPP
#define FIREFLY_TIMER_HPP

// platform-specific headers
#ifdef WIN32
    #include <Windows.h>
#else
    #include <sys/time.h>
#endif

////////////////////////////////////////////////////////////////////////

namespace ff {

// base timer class

    class timer
    {
    public:
        timer();
        ~timer();

        void start();
        void stop();
        double elapsed();
        double elapsed_ms();
        double elapsed_micro();

    private:
        double m_start;
        double m_end;
        bool   m_running;

        #ifdef WIN32

        LARGE_INTEGER li_freq;
        LARGE_INTEGER li_start;
        LARGE_INTEGER li_end;

        #else

        timeval tv_start;
        timeval tv_end;

        #endif
    };

} // exiting namespace ff

#endif
