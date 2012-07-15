#ifndef FIREFLY_LOG_HPP
#define FIREFLY_LOG_HPP

#include <cstdarg>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <firefly/core/singleton.hpp>

using std::stringstream;
using std::ofstream;
using std::ios;
using std::string;
using std::vector;

#define FF_LOG_MAX 512

#define LOG_MAX_BUFFER        (50)
#define LOG_DEFAULT_FILENAME  "firefly.log"
#define LOG_CONFIG   -1
#define LOG_ERROR    -2
#define LOG_INTERNAL -3
#define LOG_WARNING  -4
#define LOG_EVENT    -5
#define LOG_LOAD     -6
#define LOG_UNLOAD   -7

////////////////////////////////////////////////////////////////////////

namespace ff {

    class log : public singleton<log>
    {
    public:
        log(string outFile = LOG_DEFAULT_FILENAME);
        ~log();

        void write(double elapsed, string & msg);
        void write(double elapsed, const char * format, ...);
        void set(string file) { m_Filename = file; m_Created = false; }
        void flush();

    private:
        bool valid() { return (!m_Filename.empty()); }

        vector<string> m_Entries;
        string         m_Filename;
        bool           m_Created;
    };

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

#define g_Log ff::log::get_singleton()

#endif
