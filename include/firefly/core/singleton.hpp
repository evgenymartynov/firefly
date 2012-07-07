#ifndef FIREFLY_SINGLETON_HPP
#define FIREFLY_SINGLETON_HPP

#include <cassert>

////////////////////////////////////////////////////////////////////////

namespace ff {

// base template class

    template <typename T> class singleton
    {
        static T * ms_singleton;

    public:
        singleton()
        {
            assert( !ms_singleton );
            ms_singleton = static_cast<T*> (this);
        }

        ~singleton()
        {
            assert( ms_singleton );
            ms_singleton = 0;
        }

        static T & get_singleton()
        {
            assert( ms_singleton );
            return ( *ms_singleton );
        }

        static T * get_singleton_ptr()
        {
            assert( ms_singleton );
            return ( ms_singleton );
        }
    };

    template <typename T> T * singleton<T>::ms_singleton = 0;

} // exiting namespace ff

#endif
