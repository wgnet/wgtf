#ifndef QRC_LOADER_HPP
#define QRC_LOADER_HPP
#include "qrc_loader_helper.hpp"
#include "env_pointer.hpp"
#include "core_common/assert.hpp"

// Macro provide qrc resources loading for other plugins in WGTF, and a plugin which use this macro needs to link to
// QtCore
// and this plugin need to be loaded before other plugins which want to load qrc resources
#ifdef QT_NAMESPACE
#define WGT_INIT_QRC_LOADER                                                                                  \
	\
namespace QT_NAMESPACE                                                                                       \
	\
{                                                                                                     \
		bool qRegisterResourceData(int, const unsigned char*, const unsigned char*, const unsigned char*);   \
		bool qUnregisterResourceData(int, const unsigned char*, const unsigned char*, const unsigned char*); \
	\
}                                                                                                     \
	\
using namespace QT_NAMESPACE;                                                                                \
	\
namespace wgt                                                                                                \
	\
{                                                                                                     \
		namespace wgt_qrc_loader                                                                             \
		{                                                                                                    \
		class StaticInitializer                                                                              \
		{                                                                                                    \
		public:                                                                                              \
			StaticInitializer()                                                                              \
			{                                                                                                \
				static uintptr_t qtHooks[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };                             \
				qtHooks[registerData] = reinterpret_cast<uintptr_t>(qRegisterResourceData);                  \
				qtHooks[unRegisterData] = reinterpret_cast<uintptr_t>(qUnregisterResourceData);              \
				setPointer(QT_HOOK_ENV, &qtHooks[0]);                                                        \
			}                                                                                                \
		};                                                                                                   \
		static StaticInitializer dummy;                                                                      \
		}                                                                                                    \
	\
}
#else
#define WGT_INIT_QRC_LOADER                                                                          \
	\
bool qRegisterResourceData(int, const unsigned char*, const unsigned char*, const unsigned char*);   \
	\
bool qUnregisterResourceData(int, const unsigned char*, const unsigned char*, const unsigned char*); \
	\
\
namespace wgt                                                                                        \
	\
{                                                                                             \
		namespace wgt_qrc_loader                                                                     \
		{                                                                                            \
		class StaticInitializer                                                                      \
		{                                                                                            \
		public:                                                                                      \
			StaticInitializer()                                                                      \
			{                                                                                        \
				static uintptr_t qtHooks[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };                     \
				qtHooks[registerData] = reinterpret_cast<uintptr_t>(qRegisterResourceData);          \
				qtHooks[unRegisterData] = reinterpret_cast<uintptr_t>(qUnregisterResourceData);      \
				setPointer(QT_HOOK_ENV, &qtHooks[0]);                                                \
			}                                                                                        \
		};                                                                                           \
		static StaticInitializer dummy;                                                              \
		}                                                                                            \
	\
}
#endif

// Macro used in plugins which want to load qrc resources without link to QtCore.
// Plugins which use this macro assuming that there is a plugin which has already used WGT_INIT_QRC_LOADER
// and has been loaded before application loading this plugin
#define WGT_INIT_QRC_RESOURCE                                                                                        \
	\
namespace wgt                                                                                                        \
	\
{                                                                                                             \
		\
uintptr_t**                                                                                                          \
		getQtHooks()                                                                                                 \
		\
{                                                                                                         \
			static uintptr_t** s_qtHooks = nullptr;                                                                  \
			if (s_qtHooks == nullptr)                                                                                \
			{                                                                                                        \
				s_qtHooks = getPointerT<uintptr_t*>(QT_HOOK_ENV);                                                    \
				TF_ASSERT(s_qtHooks != nullptr);                                                                        \
			}                                                                                                        \
			return s_qtHooks;                                                                                        \
		\
}                                                                                                         \
	\
}                                                                                                             \
	\
bool qRegisterResourceData(int id, const unsigned char* qt_resource_struct, const unsigned char* qt_resource_name,   \
	                       const unsigned char* qt_resource_data)                                                    \
	\
{                                                                                                             \
		auto hooks = wgt::getQtHooks();                                                                              \
		if (hooks == nullptr)                                                                                        \
		{                                                                                                            \
			return false;                                                                                            \
		}                                                                                                            \
		auto func = reinterpret_cast<wgt::ResourceDataRegisterFunc>(hooks[wgt::registerData]);                       \
		if (func == nullptr)                                                                                         \
		{                                                                                                            \
			return false;                                                                                            \
		}                                                                                                            \
		return func(id, qt_resource_struct, qt_resource_name, qt_resource_data);                                     \
	\
}                                                                                                             \
	\
\
bool qUnregisterResourceData(int id, const unsigned char* qt_resource_struct, const unsigned char* qt_resource_name, \
	                         const unsigned char* qt_resource_data)                                                  \
	\
{                                                                                                             \
		auto hooks = wgt::getQtHooks();                                                                              \
		if (hooks == nullptr)                                                                                        \
		{                                                                                                            \
			return false;                                                                                            \
		}                                                                                                            \
		auto func = reinterpret_cast<wgt::ResourceDataUnregisterFunc>(hooks[wgt::unRegisterData]);                   \
		if (func == nullptr)                                                                                         \
		{                                                                                                            \
			return false;                                                                                            \
		}                                                                                                            \
		return func(id, qt_resource_struct, qt_resource_name, qt_resource_data);                                     \
	\
}

#endif // QRC_LOADER_HPP
