#ifndef MACRO_OBJECT_HPP
#define MACRO_OBJECT_HPP

#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_variant/variant.hpp"
#include "core_data_model/variant_list.hpp"
#include "command_instance.hpp"
#include "core_data_model/i_tree_model.hpp"

namespace wgt
{
class ICommandManager;
class IDefinitionManager;
class MacroObject;
class ReflectedPropertyCommandArgument;
class ReflectedMethodCommandParameters;
class IReflectionController;

class MacroEditObject
{
	DECLARE_REFLECTED

public:
	void init(size_t count);
	const ObjectHandle& getCommandArgument(size_t id) const;
	const ObjectHandle& getCommandArgController(size_t id) const;
	size_t getArgCount() const
	{
		return args_.size();
	}
	void setCommandHandlers(size_t id, const ObjectHandle& controller, const ObjectHandle& arg);

	void resolveDependecy(size_t command, const std::vector<CommandInstance*>& instances);

private:
	std::vector<ObjectHandle> args_;
	std::vector<ObjectHandle> controllers_;
};

typedef std::map<std::wstring, RefObjectId> EnumMap;

class ReflectedPropertyCommandArgumentController
{
	DECLARE_REFLECTED
public:
	ReflectedPropertyCommandArgumentController();

	void init(size_t subCommandIdx, ObjectHandle arguments, IDefinitionManager* defMngr);

	void setValue(const std::string& value);
	std::string getValue() const;

	void setPropertyPath(const std::string& value);
	const char* getPropertyPath() const;

	void getObject(int* o_EnumValue) const;
	void setObject(const int& o_EnumValue);
	int getDependencyOffset()
	{
		return dependencyOffset_;
	}
	void resolve(const std::vector<CommandInstance*>& instances);
	void generateObjList(std::map<int, std::wstring>* o_enumMap) const;

private:
	ReflectedPropertyCommandArgument* getArgumentObj() const;

	ObjectHandle arguments_;
	IDefinitionManager* defMngr_;

	mutable EnumMap enumMap_;
	size_t subCommandIdx_;
	int dependencyOffset_;
};

class MethodParam
{
	DECLARE_REFLECTED
public:
	MethodParam() : value_(nullptr)
	{
	}
	void init(Variant* v)
	{
		value_ = v;
	}

	void setValue(const std::string& value);
	std::string getValue() const;

private:
	Variant* value_;
};

class ReflectedMethodCommandParametersController
{
	DECLARE_REFLECTED
public:
	ReflectedMethodCommandParametersController();

	void init(ObjectHandle parameters, IDefinitionManager* defMngr);

	void setMethodPath(const std::string& value);
	const char* getMethodPath() const;

	void getObject(int* o_EnumValue) const;
	void setObject(const int& o_EnumValue);
	void generateObjList(std::map<int, std::wstring>* o_enumMap) const;

private:
	ReflectedMethodCommandParameters* getParamObj() const;

	std::vector<ObjectHandle> methodParams_;

	ObjectHandle paramsObj_;
	IDefinitionManager* defMngr_;

	mutable EnumMap enumMap_;
};

class MacroObject
{
	DECLARE_REFLECTED
public:
	MacroObject();

	void init(ICommandManager& commandSystem, IDefinitionManager& defManager, IReflectionController* controller,
	          const char* cmdId);

	bool validateArgsObject(const ObjectHandle& obj) const;
	bool setArgumentObject(const ObjectHandle& args);
	bool setArgumentObjectForCommand(size_t idx, const ObjectHandle& args);

	ObjectHandle executeMacro() const;
	ObjectHandle executeMacro(const ObjectHandle& contextObject) const;
	const ITreeModel* getTreeModel() const;

	void serialize(ISerializer& serializer) const;
	void deserialize(ISerializer& serializer);

private:
	void bindMacroArgumenets();
	ObjectHandle createController(size_t idx, const ObjectHandle& args) const;

	std::pair<ObjectHandle, ObjectHandle> bind(size_t idx, ReflectedPropertyCommandArgument* rpca) const;
	std::pair<ObjectHandle, ObjectHandle> bind(size_t idx, ReflectedMethodCommandParameters* rmcp) const;

	ICommandManager* commandSystem_;
	IDefinitionManager* pDefManager_;
	IReflectionController* controller_;

	std::string cmdId_;
	std::string macroName_;

	ObjectHandle argsEdit_;
	mutable std::shared_ptr<ITreeModel> treeModel_;
};
} // end namespace wgt
#endif // MACRO_OBJECT_HPP
