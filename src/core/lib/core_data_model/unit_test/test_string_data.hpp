#ifndef TEST_STRING_DATA_HPP
#define TEST_STRING_DATA_HPP
#include "pch.hpp"

#include "core_data_model/variant_list.hpp"
#include <memory>
#include <vector>

namespace wgt
{
class UnitTestTreeModel;

class TestStringData
{
public:
	enum State
	{
		STATE_LIST,
		STATE_TREE
	};

	TestStringData();
	~TestStringData();
	void initialise(State state);
	VariantList& getVariantList();
	UnitTestTreeModel& getTreeModel();
	std::string getNextString();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

/* Tree data is populated as follows:
Animations
    Monsters
        anim_bear
        anim_mouse
        anim_rat
    Landscape
        anim_dirt
        anim_rock
        anim_water
    Dancing
        crazy_dance
        happy_dance
        fancy_dance
Objects
    Generic
        object_01
        objects_02
        object_03
    Magical
        object_04
        object_05
        object_06
    Mechanical
        object_07
        object_08
        object_09
Models
    Enemies
        model_skeleton
        model_wizard
        model_ogre
    Small
        model_bat
        model_ant
        model_rat
    Large
        model_truck
        model_troll
        model_dinosaur
*/
} // end namespace wgt
#endif // TEST_STRING_DATA_HPP
