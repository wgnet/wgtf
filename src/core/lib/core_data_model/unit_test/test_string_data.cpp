#include "pch.hpp"
#include "test_string_data.hpp"
#include "test_data_model_objects.hpp"

namespace wgt
{
struct TestStringData::Implementation
{
	Implementation( TestStringData & self ) 
		: self_( self )
		, state_( STATE_LIST )
		, position_( 0 )
	{
	}

	void initialise( State state )
	{
		/*
		// Note: If raw data is modified, please take into consideration the filtering tests using
		//       positions with specific data expectations (e.g. "apple" should be in index 0).
		*/

		state_ = state;
		switch( state_ )
		{
		case STATE_LIST:
			{
				rawListData_ = "apple berry custard drama eggs fig grape hat igloo jam kangaroo lemon mango noodles ";
				rawListData_ += "orange pineapple queen rice star tribble upvote vine wine xray yoyo zebra ";
				
				position_ = 0;
				std::string dataString = getNextString();
				while (!dataString.empty())
				{
					std::string itemName = dataString.c_str();
					testList_.push_back( itemName );
					dataString = getNextString();
				}
			}
			break;

		case STATE_TREE:
			{	
				rawTreeData_ = "Animations Monsters anim_bear anim_mouse anim_rat Landscape anim_dirt anim_rock anim_water Dancing crazy_dance happy_dance fancy_dance ";
				rawTreeData_ += "Objects Generic object_01 objects_02 object_03 Magical object_04 object_05 object_06 Mechanical object_07 object_08 object_09 ";
				rawTreeData_ += "Models Enemies model_skeleton model_wizard model_ogre Small model_bat model_ant model_rat Large model_truck model_troll model_dinosaur ";
								
				position_ = 0;
				testTree_.initialise( &self_ );
			}
			break;

		default:
			{			
				BWUnitTest::unitTestInfo( "Data Model Unit Test - No Valid State Provided!\n" );
			}
		}
	}

	std::string getNextString()
	{
		std::string data;
		switch (state_)
		{
		case STATE_LIST:
			data = rawListData_;
			break;
		case STATE_TREE:
			data = rawTreeData_;
			break;
		default:
			return "";
		}

		size_t nextPosition = data.find( ' ', position_ );
		size_t count = nextPosition == std::string::npos ? std::string::npos : nextPosition - position_;
		std::string temp = data.substr( position_, count );
		position_ = nextPosition == std::string::npos ? 0 : nextPosition + 1;
		return temp;
	}
	
	TestStringData & self_;
	State state_;
	VariantList testList_;
	UnitTestTreeModel testTree_;
	std::string rawListData_;
	std::string rawTreeData_;
	size_t position_;
};

TestStringData::TestStringData()
	: impl_( new Implementation( *this ) )
{
}

TestStringData::~TestStringData()
{
	// Erase the list
	for (auto it = impl_->testList_.begin(); it != impl_->testList_.end();)
	{
		it = impl_->testList_.erase( it );
	}
}

void TestStringData::initialise( State state )
{
	impl_->initialise( state );
}

VariantList & TestStringData::getVariantList()
{
	return impl_->testList_;
}

UnitTestTreeModel & TestStringData::getTreeModel()
{
	return impl_->testTree_;
}

std::string TestStringData::getNextString()
{
	return impl_->getNextString();
}
} // end namespace wgt
