#ifndef QRC_LOADER_HELPER_HPP
#define QRC_LOADER_HELPER_HPP

namespace wgt
{
static const char* QT_HOOK_ENV = "QT_HOOK_ENV";

enum QtHooks
{
	registerData = 0,
	unRegisterData = 1
};
typedef bool (*ResourceDataRegisterFunc)(int, const unsigned char*, const unsigned char*, const unsigned char*);
typedef bool (*ResourceDataUnregisterFunc)(int, const unsigned char*, const unsigned char*, const unsigned char*);
}
#endif // QRC_LOADER_HELPER_HPP
