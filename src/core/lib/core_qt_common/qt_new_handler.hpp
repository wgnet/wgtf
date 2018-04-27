#ifndef QT_NEW_HANDLER
#define QT_NEW_HANDLER

#include <set>

#define DECLARE_QT_MEMORY_HANDLER                                 \
	\
public:                                                           \
	void* operator new(size_t s)                                  \
	{                                                             \
		return ::operator new(s);                                 \
	}                                                             \
                                                                  \
	void operator delete(void* ptr)                               \
	{                                                             \
        if(!wgt::eraseItemQtInPlaceNewCollection(ptr))            \
        {                                                         \
            ::operator delete(ptr);                               \
        }                                                         \
	}                                                             \
                                                                  \
	void* operator new(size_t s, void* at)                        \
	{                                                             \
		wgt::insertItemQtInPlaceNewCollection(at);                \
		return at;                                                \
	}                                                             \
                                                                  \
	void operator delete(void* p, void*)                          \
	{                                                             \
        wgt::eraseItemQtInPlaceNewCollection(p);                  \
	}                                                             \

namespace wgt
{
void insertItemQtInPlaceNewCollection(void* ptr);
bool eraseItemQtInPlaceNewCollection(void* ptr);
} // end namespace wgt
#endif // QT_NEW_HANDLER
