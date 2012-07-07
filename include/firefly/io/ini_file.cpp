#include <firefly/io/ini_file.hpp>
#include <cassert>

////////////////////////////////////////////////////////////////////////

namespace ff {

// pulls out substring using a delimiter

    string GetSubString(istream & in, char delim, bool ignoreWhiteSpace,
                        bool clearToNextChar)
    {
        string result;
        char ch;

        // add characters until delimiter is found
        while (in.get(ch) && ch != delim)
        {
            if (ignoreWhiteSpace && is_whitespace(ch))
                continue;

            result.push_back(ch);
        }

        // strip whitespace to the next character
        if (clearToNextChar)
        {
            while (in.get(ch) && is_whitespace(ch));
            in.putback(ch);
        }

        return result;
    }


// extract string containing the ini value

    string GetValueString(istream & in, bool ignoreWhiteSpace)
    {
        string value;
        char ch;

        while (in.get(ch) && ch != END_LINE)
        {
            if (ch == COMMENT)
            {
                // if there is a comment, purge the remaining chars
                purge(in, END_LINE, false);
                break;
            }
            if (ch == QUOTEMARK)
            {
                // if there is a quote, pull the string WITH whitespace
                value = GetSubString(in, QUOTEMARK, false, false);
                break;
            }
            if (ignoreWhiteSpace && is_whitespace(ch))
                continue;

            // valid char found, store in the result
            value.push_back(ch);
        }

        // remove whitespace until the next valid char
        while (in.get(ch) && is_whitespace(ch));
        in.putback(ch);

        return value;
    }


// constructor

    ini_file::ini_file(const string & filePath)
        : m_Filename(filePath), m_pCurSection(NULL)
    {
    }


// destructor

    ini_file::~ini_file()
    {
        reset();
    }


// implement usage via file["Section"]["Key"]

    ini_file::KeyValueMap & ini_file::operator[](const string & section)
    {
        auto it = m_SectionMap.find(section);
        assert(it != m_SectionMap.end());
        return *(it->second);
    }


// clears the data stored in the ini_file object

    void ini_file::reset(bool wipeFilename)
    {
        for (auto it = m_SectionMap.begin();
                  it != m_SectionMap.end(); ++it)
        {
            if (it->second)
                delete it->second;
        }

        if (wipeFilename)
            m_Filename.clear();

        m_SectionMap.clear();
        m_pCurSection = NULL;
    }


// add section to ini_file

    bool ini_file::create(const string & section)
    {
        if (select(section))
            return false;

        m_pCurSection = new KeyValueMap;
        m_SectionMap[section] = m_pCurSection;
        return true;
    }


// change active section

    bool ini_file::select(const string & section)
    {
        auto it = m_SectionMap.find(section);
        if (it == m_SectionMap.end())
            return false;

        m_pCurSection = it->second;
        return true;
    }


// set a key/value pair for the current section

    bool ini_file::set(const string & key, const string & value)
    {
        if (!m_pCurSection)
            return false;

        (*m_pCurSection)[key] = value;
        return true;
    }


// pull the value for a key in the current section

    string ini_file::get(const string & key, const string & def)
    {
        if (!m_pCurSection)
            return def;

        auto it = m_pCurSection->find(key);
        if (it == m_pCurSection->end())
            return def;

        return it->second;
    }


// load the sections/key/value tuples from specified file

    bool ini_file::load(const string & filePath)
    {
        if (filePath.empty())
        {
            return false;
        }
        else if (!m_Filename.empty())
        {
            reset();
        }

        // store the filename
        m_Filename = filePath;

        ifstream file(m_Filename, std::ios_base::in);
        if (!file.is_open())
            return false;

        string temp;
        char ch;

        // ignore everything until the first section
        while (file.get(ch) && ch != SECTION_BEGIN);
        temp = GetSubString(file, SECTION_END);
        if (!temp.empty())
            create(temp);

        // read in data
        while (!file.eof())
        {
            // start scanning for possible entries
            if ((ch = file.peek()))
            {
                switch(ch)
                {
                case SECTION_BEGIN:
                    file.get();
                    temp = GetSubString(file, SECTION_END);
                    if (!temp.empty())
                        create(temp);
                    break;
                case COMMENT:
                    purge(file, END_LINE);
                    break;
                default:
                    temp = GetSubString(file, ASSIGNMENT);
                    if (!temp.empty())
                    {
                        string value = GetValueString(file);
                        set(temp, value);
                    }
                }
            }
        }
        return true;
    }


// save the current ini_file state to the actual file

    void ini_file::save(const string & filePath)
    {
        if (filePath.empty() || m_SectionMap.empty())
            return;

        m_Filename = filePath;
        ofstream out(m_Filename, std::ios_base::out);

        if (!out.is_open())
            return;

        for (auto it = m_SectionMap.begin();
             it != m_SectionMap.end(); ++it)
        {
            out << SECTION_BEGIN << it->first.c_str() << SECTION_END;
            out << END_LINE;
            auto s = it->second;
            for (auto kv = s->begin(); kv != s->end(); ++kv)
            {
                out << kv->first.c_str() << ASSIGNMENT;
                out << kv->second.c_str() << END_LINE;
            }
        }
    }

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////
