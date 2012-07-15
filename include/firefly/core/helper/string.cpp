#include <firefly/core/helper/string.hpp>
#include <cassert>
#include <algorithm>

////////////////////////////////////////////////////////////////////////

namespace ff {

// returns true if all characters are upper case

	bool is_upper( const char * str )
	{
		assert (str != NULL);
		const char * end = str + strlen(str);
		return ( std::find_if(str, end, isupper) == end );
	}


// returns true if all characters are lower case

	bool is_lower( const char * str )
	{
		assert (str != NULL);
		const char * end = str + strlen(str);
		return ( std::find_if(str, end, islower) == end );
	}


// converts all characters of std::string to uppercase

	string to_upper( const string & str )
	{
		string result = str;
		auto ch = result.begin(), end = result.end();

		for ( ; ch != end; ++ch )
		{
			*ch = (char)toupper(*ch);
		}

		return result;
	}


// converts all characters of std::string to lowercase

	string to_lower( const string & str )
	{
		string result = str;
		auto ch = result.begin(), end = result.end();

		for ( ; ch != end; ++ch )
		{
			*ch = (char)tolower(*ch);
		}
	
		return result;
	}

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////