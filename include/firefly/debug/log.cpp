#include <firefly/debug/log.hpp>

// global instance of log
ff::log GlobalLog;

// variety of log entry prefixes
string prefix[5] = { ": ", "# ", "", "* ", ">> " };

////////////////////////////////////////////////////////////////////////

namespace ff {

// constructor

    log::log(string outFile)
    {
        m_Filename = outFile;
        m_Created = false;
    }


// destructor

    log::~log()
    {
        write(LOG_INTERNAL, "EOF");
        flush();
    }


// writes to the log buffer

    void log::write(double elapsed, string & msg)
    {
        if ( !valid() )
            return;

        static double lastTime = 0;
        stringstream buffer;

        // purge trailing newlines
        while(*msg.rbegin() == '\n')
        {
            msg.erase(msg.end() - 1);
        }

        if (elapsed < 0)
        {
            int index = (elapsed + 1) * -1;
            buffer << prefix[index] << msg;
        }
        else
        {
            if (elapsed < lastTime)
                elapsed = lastTime;
            else
                lastTime = elapsed;

            double temp = elapsed * .01666666667 * .01666666667;
            int hours = (int)temp;
            temp -= (double)hours;
            temp *= 60;

            int mins = (int)temp;
            temp -= (double)mins;
            temp *= 60;

            int secs = (int)temp;
            temp -= (double)secs;
            temp *= 1000;

            int ms = (int)temp;

            if (hours > 0)
                buffer << hours << ":";
            buffer << mins << ":" << secs << ":" << ms << " " << msg;
        }

        m_Entries.push_back(buffer.str());

        if (m_Entries.size() >= LOG_MAX_BUFFER || elapsed == LOG_ERROR)
            flush();
    }


// allows variable parameters like printf

    void log::write(double elapsed, const char * format, ...)
    {
        if ( !valid() )
            return;

        static string strbuf;
        char buffer[256];
        va_list va;
        va_start(va, format);
        vsnprintf(buffer, 256, format, va);
        va_end(va);
        write(elapsed, strbuf = buffer);
    }


// writes the buffer to the file

    void log::flush()
    {
        if ( !valid() )
            return;

        ofstream logFile;

        if (!m_Created)
        {
            logFile.open(m_Filename.c_str(), std::ios::out);
            m_Created = true;
        }
        else
        {
            logFile.open(m_Filename.c_str(), std::ios::app);
        }

        if (!logFile.is_open())
            return;

        if (!m_Entries.empty())
        {
            auto it = m_Entries.begin(), end = m_Entries.end();
            for (; it != end; ++it)
            {
                logFile << *it << '\n';
            }
        }

        m_Entries.clear();
    }

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////
