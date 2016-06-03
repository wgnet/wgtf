#include "sequence_list_adapter.hpp"

namespace wgt
{
SequenceListAdapter::SequenceListAdapter()
	: model_( nullptr )
{
}

SequenceListAdapter::~SequenceListAdapter()
{
}

QAbstractItemModel * SequenceListAdapter::model() const
{
	return model_;
}

QModelIndex SequenceListAdapter::adaptedIndex(int row, int column, const QModelIndex &parent) const
{
	if (model() == nullptr)
	{
		return QModelIndex();
	}

	if (sequence_.empty())
	{
		return model()->index( row, column, parent );
	}

	if (row < sequence_.size())
	{
		return model()->index( sequence_[row], column, parent );
	}
	return QModelIndex();
}

int SequenceListAdapter::rowCount(const QModelIndex &parent) const
{
	if (model() == nullptr)
	{
		return 0;
	}

	if (sequence_.empty())
	{
		return model()->rowCount( parent );
	}

	return sequence_.size();
}

QVariant SequenceListAdapter::getModel() const
{
	return QVariant::fromValue< QAbstractListModel * >( model_ );
}

void SequenceListAdapter::setModel( const QVariant &model )
{
	beginResetModel();
	disconnect();
	model_ = model.value< QAbstractListModel * >();
	connect();
	endResetModel();
}

QList< int > SequenceListAdapter::getSequence()
{
	return sequence_;
}

void SequenceListAdapter::setSequence( const QList< int > &sequence )
{
	sequence_ = sequence;
}

void SequenceListAdapter::onParentDataChanged(const QModelIndex &topLeft, 
	const QModelIndex &bottomRight, const QVector<int> &roles)
{
	if (sequence_.empty())
	{
		emit dataChanged( 
			createIndex(topLeft.row(), topLeft.column(), topLeft.internalPointer()), 
			createIndex(bottomRight.row(), bottomRight.column(), bottomRight.internalPointer()), 
			roles );
		return;
	}

	for (int i = topLeft.row(); i <= bottomRight.row(); ++i)
	{
		int index = 0;
		for (;;)
		{
			index = sequence_.indexOf( i, index );
			if (index < 0)
			{
				break;
			}

			emit dataChanged( 
				createIndex(index, topLeft.column()), 
				createIndex(index, bottomRight.column()), 
				roles );
			++index;
		}
	}
}
} // end namespace wgt
