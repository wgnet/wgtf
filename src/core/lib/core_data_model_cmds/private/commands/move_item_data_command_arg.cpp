#include "move_item_data_command_arg.hpp"
#include "core_data_model/abstract_item_model.hpp"

namespace wgt
{


MoveItemDataCommandArgument::MoveItemDataCommandArgument( ) :
	  pModel_( nullptr )
	, startPos_( -1 )
	, direction_( Direction::ROW )
	, count_( -1 )
	, startParent_( nullptr )
	, endParent_( nullptr )
{
}

void MoveItemDataCommandArgument::setModel( AbstractItemModel & model )
{
	pModel_ = &model;
}


void MoveItemDataCommandArgument::setStartPos( int startPos )
{
	startPos_ = startPos;
}

void MoveItemDataCommandArgument::setDirection( Direction direction )
{
	direction_ = direction;
}

void MoveItemDataCommandArgument::setEndPos( int endPos )
{
	endPos_ = endPos;
}

void MoveItemDataCommandArgument::setCount( int count )
{
	count_ = count;
}

void MoveItemDataCommandArgument::setStartParent( const AbstractItem * startParent )
{
	startParent_ = startParent;
}

void MoveItemDataCommandArgument::setEndParent( const AbstractItem * endParent )
{
	endParent_ = endParent;
}


} // end namespace wgt
