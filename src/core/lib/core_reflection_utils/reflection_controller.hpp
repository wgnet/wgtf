#ifndef REFLECTION_CONTROLLER_HPP
#define REFLECTION_CONTROLLER_HPP

#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
class ICommandManager;

class ReflectionController : public Implements<IReflectionController>
{
public:
	ReflectionController();
	virtual ~ReflectionController();

	virtual void init(ICommandManager& commandManager);
	virtual void fini();

	Variant getValue(const PropertyAccessor& pa) override;
	void setValue(const PropertyAccessor& pa, const Variant& data) override;
	Variant invoke(const PropertyAccessor& pa, const ReflectedMethodParameters& parameters) override;
	void insert(const PropertyAccessor& pa, const Variant& key, const Variant& value) override;
	void erase(const PropertyAccessor& pa, const Variant& key) override;
	void customCommand(ExecuteFunc executeFunc, UndoFunc undoFunc, const std::string& description) override;
	void setEditor(IEditor* editor) override;
	void setCustomSetValue(CustomSetValueFunc setFunc) override;
	void setCustomErase(CustomEraseFunc eraseFunc) override;
	void setCustomInsert(CustomInsertFunc insertFunc) override;

private:
	class Impl;
	std::unique_ptr<Impl> impl_;
};
} // end namespace wgt
#endif // COMMAND_SYSTEM_REFLECTION_PROPERTY_SETTER_HPP
