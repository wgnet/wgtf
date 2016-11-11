#ifndef I_ENV_SYSTEM
#define I_ENV_SYSTEM

#include <memory>

namespace wgt
{
struct ECGUID
{
	uint32_t a;
	uint32_t b;
	uint32_t c;
	uint32_t d;

	bool operator==(const ECGUID& other) const
	{
		return a == other.a && b == other.b && c == other.c && d == other.d;
	}
};

class IEnvComponent
{
public:
	virtual ~IEnvComponent()
	{
	}
	virtual const ECGUID& getGUID() const = 0;
};

class IEnvState
{
public:
	virtual ~IEnvState()
	{
	}
	typedef std::unique_ptr<IEnvComponent> IEnvComponentPtr;
	virtual void add(IEnvComponentPtr ec) = 0;
	virtual IEnvComponentPtr remove(const ECGUID& guid) = 0;
	virtual IEnvComponent* query(const ECGUID& guid) const = 0;
	virtual const char* description() const = 0;
	virtual int id() const = 0;
};

class IEnvEventListener
{
public:
	virtual ~IEnvEventListener(){};
	virtual void onAddEnv(IEnvState* state){};
	virtual void onRemoveEnv(IEnvState* state){};
	virtual void onSelectEnv(IEnvState* state){};
	virtual void onSaveEnvState(IEnvState* state){};
	virtual void onLoadEnvState(IEnvState* state){};
	virtual void onDeselectEnv(){};
};

class IEnvManager
{
public:
	enum
	{
		INVALID_ID = -1
	};

	virtual ~IEnvManager()
	{
	}
	virtual void registerListener(IEnvEventListener* listener) = 0;
	virtual void deregisterListener(IEnvEventListener* listener) = 0;

	virtual int addEnv(const char* description) = 0;
	virtual void removeEnv(int id) = 0;
	virtual void selectEnv(int id) = 0;
	virtual void deSelectEnv(int id) = 0;
	virtual void saveEnvState(int id) = 0;
	virtual void loadEnvState(int id) = 0;
};

#define DEFINE_EC_GUID                             \
	\
public:                                            \
	static const ECGUID GUID;                      \
	virtual const ECGUID& getGUID() const override \
	{                                              \
		return GUID;                               \
	}
#define DECLARE_EC_GUID(CL, A, B, C, D) const ECGUID CL::GUID = { A, B, C, D }

#define ENV_STATE_ADD(CL, var) \
	auto var = new CL;         \
	state->add(IEnvState::IEnvComponentPtr(var))
#define ENV_STATE_REMOVE(CL, var)             \
	auto __u_ptr__ = state->remove(CL::GUID); \
	auto var = static_cast<CL*>(__u_ptr__.get())
#define ENV_STATE_QUERY(CL, var)                         \
	auto var = static_cast<CL*>(state->query(CL::GUID)); \
	assert(var)
} // end namespace wgt
#endif // I_ENV_SYSTEM
