#include <firefly/debug/log.hpp>
#include <cassert>

// variety of log entry prefixes
string prefix[7] = { ": ", "# ", "", "* ", ">> ", "+ ", "- " };

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

        if (elapsed < 0)
        {
            int index = ((int)elapsed + 1) * -1;
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
		static string strbuf;
        if ( !valid() )
            return;

		strbuf.clear();
        char buffer[FF_LOG_MAX + 1];
		buffer[FF_LOG_MAX - 1] = '\0';
        va_list va;
        va_start(va, format);
        vsnprintf(buffer, FF_LOG_MAX, format, va);
        va_end(va);
		
		buffer[FF_LOG_MAX] = '\0';
		strbuf = buffer;
		if (buffer[FF_LOG_MAX - 1] != '\0') {
			strbuf += "...";
		}

        write(elapsed, strbuf);
    }


// writes the buffer to the file

    void log::flush()
    {
        if ( !valid() )
            return;

        ofstream logFile;

        if (!m_Created)
        {
            logFile.open(m_Filename.c_str(), ios::out | ios::binary);
            m_Created = true;
        }
        else
        {
            logFile.open(m_Filename.c_str(), ios::app | ios::binary);
        }

        assert(logFile.is_open());
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
