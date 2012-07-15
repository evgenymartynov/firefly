#ifndef FF_HELPER_STRING_HPP
#define FF_HELPER_STRING_HPP

#include <string>
using std::string;

#define IS_DIGIT(ch) ( ch - '0' < 9 )
#define WILD_CARD '*'
#define WILD_CHAR '?'

////////////////////////////////////////////////////////////////////////

namespace ff {

// helper function prototypes

	bool	is_upper( const char * str );
	bool	is_lower( const char * str );
	string 	to_upper( const string & str );
	string 	to_lower( const string & str );

// templated string matching function

	template<class STRING>
	bool str_match(STRING pattern, STRING test)
	{
		unsigned int i;
		bool wild;
		const char * p = pattern.c_str();
		const char * t = test.c_str();

		if ( pattern == STRING() + WILD_CARD )
			return true;

	new_segment:
   
		wild = false;
		while ( p[0] == WILD_CARD )
		{
			wild = true;
			++p;
		}

	test_match:
   
		for ( i = 0; p[i] && ( p[i] != WILD_CARD ); ++i )
		{
			if ( p[i] != t[i] )
			{
				if ( !t[i] ) return false;
				if (  p[i] == WILD_CHAR ) continue;
				if ( !wild ) return false;
				++t;
				goto test_match;
			}
		}
   
		if ( p[i] == WILD_CARD )
		{
			p += i;
			t += i;
			goto new_segment;
		}
   
	   if ( !t[i] ) return true;
	   if (  i && p[i-1] == WILD_CARD) return true;
	   if ( !wild ) return false;
	   ++t;
	   goto test_match;
	}

#undef WILD_CARD
#undef WILD_CHAR

} // exiting namespace ff

////////////////////////////////////////////////////////////////////////

#endif