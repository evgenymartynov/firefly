#ifndef FIREFLY_INIFILE_HPP
#define FIREFLY_INIFILE_HPP

#include <map>
#include <string>
#include <fstream>
#include <sstream>

using std::map;
using std::string;
using std::istream;
using std::ostream;
using std::ifstream;
using std::ofstream;
using std::stringstream;

#define COMMENT       '#'
#define ASSIGNMENT    '='
#define SECTION_BEGIN '['
#define SECTION_END   ']'
#define QUOTEMARK     '"'
#define END_LINE      '\n'

////////////////////////////////////////////////////////////////////////

namespace ff {

// returns if a char is considered white space

    inline bool is_whitespace(char ch)
    {
        return (ch == ' '  ||
                ch == '\n' ||
                ch == '\t' ||
                ch == '\v' ||
                ch == '\f' ||
                ch == '\r');
    }

// purges all characters from a stream until a specified char

    inline void purge(istream & in, char target,
                      bool stripChar = true)
    {
        char ch;
        while (in.get(ch) && ch != target);
        if (!stripChar)
            in.putback(ch);
    }

// pulls a substring out of a string marked by a delimiter

    string GetSubString(istream & in,
                        char delim,
                        bool ignoreWhiteSpace = true,
                        bool clearToNextChar = true);

// cleans up file input and outputs the ini value in a formatted string

    string GetValueString(istream & in, bool ignoreWhiteSpace = true);

// template to convert between standard types

    template<typename OUT_T, typename IN_T>
    static OUT_T convert(IN_T value)
    {
        stringstream ss;
        OUT_T result;
        ss << value;
        ss >> result;
        return result;
    }

// base class for handling ini files

    class ini_file
    {
    private:
        typedef map<string, string>        KeyValueMap;
        typedef map<string, KeyValueMap*>  SectionMap;

        string        m_Filename;
        KeyValueMap * m_pCurSection;
        SectionMap    m_SectionMap;

    public:
        ini_file(const string & filePath = string());
        ~ini_file();

        KeyValueMap & operator[](const string & section);

        // section functions
        bool create(const string & section);
        bool select(const string & section);

        // set VALUE for a KEY
        bool set(const string & key, const string & value);

        template<class V>
        bool set(const string & key, const V & value)
            { return set(key, convert<string>(value)); }

        // get VALUE from KEY
        string get(const string & key, const string & def = string());

        template<class V>
        V get(const string & key, const V & def = V())
            { return convert<V>(get(key, convert<string>(def))); }

        // file helper functions
        bool load(const string & filePath = string());
        void save(const string & filePath = string());
        void reset(bool wipeFilename = true);
    };

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

#endif
