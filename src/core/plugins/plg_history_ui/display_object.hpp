#ifndef DISPLAY_OBJECT_HPP
#define DISPLAY_OBJECT_HPP

#include "core_command_system/command_instance.hpp"

namespace wgt
{
class DisplayObject
{
public:
    DisplayObject();
    void init( IDefinitionManager& defManager, const CommandInstancePtr & instance );
    ObjectHandle getDisplayData() const;
private:
	ObjectHandle data_;

};
} // end namespace wgt
#endif //DISPLAY_OBJECT_HPP
