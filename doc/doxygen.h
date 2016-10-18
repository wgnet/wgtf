// \file prevent lint detector warning
// dummy file to trick doxygen into documenting shared pointers
namespace std {
//! Shared pointer dummy
template<class T>
class shared_ptr
{
     T *p; //!< Pointer
};

//! Unique pointer dummy
template<class T>
class unique_ptr
{
     T *p; //!< Pointer
}
}

/**
* @defgroup plugins Plugins
* A list of plugins available
*/

/**
* @defgroup coreplugins Core Plugins
* A list of plugins which most plugins rely on
*/

/**
* @defgroup wgcontrols WG Controls
* A list of QML controls available
*/