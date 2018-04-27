#pragma once

namespace wgt
{
class IMetaCommandExecutable
{
public:
	virtual ~IMetaCommandExecutable()
	{
	}
	virtual void execute(void* pBase, const void* arguments) const = 0;
};

template <typename T>
class MetaCommandExecutableImpl : public IMetaCommandExecutable
{
public:
	typedef void (T::*CommandExecuteFunc)(const void*);

	MetaCommandExecutableImpl(CommandExecuteFunc commandExecuteFunc) : commandExecuteFunc_(commandExecuteFunc)
	{
	}

	~MetaCommandExecutableImpl()
	{
	}

	virtual void execute(void* pBase, const void* arguments) const
	{
		(((T*)pBase)->*commandExecuteFunc_)(arguments);
	}

private:
	CommandExecuteFunc commandExecuteFunc_;
};
}