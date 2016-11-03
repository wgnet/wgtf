#ifndef ASSET_LIST_MODEL_HPP
#define ASSET_LIST_MODEL_HPP

#include "core_data_model/i_list_model.hpp"

#include <memory>

namespace wgt
{
class IAssetObjectItem;

class AssetListModel : public IListModel
{
public:
	// Typedefs
	typedef std::vector<std::unique_ptr<IAssetObjectItem>> Items;

public:
	AssetListModel();
	virtual ~AssetListModel();

	// IListModel Implementation
	virtual IItem* item(size_t index) const override;
	virtual size_t index(const IItem* item) const override;

	virtual bool empty() const override;
	virtual size_t size() const override;
	virtual int columnCount() const override;

	virtual bool canClear() const override;
	virtual void clear() override;
	//

	// Iterators
	class ConstIterator
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef Items::const_iterator::difference_type difference_type;
		typedef const IAssetObjectItem value_type;
		typedef value_type* pointer;
		typedef value_type& reference;

		ConstIterator(const ConstIterator& rhs)
		    : iterator_(new Items::const_iterator(*rhs.iterator_))
		{
		}
		ConstIterator& operator=(const ConstIterator& rhs)
		{
			if (this != &rhs)
			{
				iterator_.reset(new Items::const_iterator(*rhs.iterator_));
			}
			return *this;
		}

		reference operator*() const
		{
			return *operator->();
		}
		pointer operator->() const
		{
			auto item = (*this->iterator_)->get();
			return item;
		}

		ConstIterator& operator++()
		{
			++(*iterator_);
			return *this;
		}
		ConstIterator operator++(int)
		{
			ConstIterator tmp = *this;
			++(*this);
			return tmp;
		}
		bool operator==(const ConstIterator& other) const
		{
			return *iterator_ == *other.iterator_;
		}
		bool operator!=(const ConstIterator& other) const
		{
			return !(*this == other);
		}
		bool operator<(const ConstIterator& other) const
		{
			return *iterator_ < *other.iterator_;
		}
		difference_type operator-(const ConstIterator& other) const
		{
			return *iterator_ - *other.iterator_;
		}
		ConstIterator operator+(difference_type n) const
		{
			return *iterator_ + n;
		}

	protected:
		ConstIterator()
		{
		}
		ConstIterator(const Items::const_iterator& iterator)
		    : iterator_(new Items::const_iterator(iterator))
		{
		}

		const Items::const_iterator& iterator() const
		{
			return *iterator_;
		}

		std::unique_ptr<Items::const_iterator> iterator_;

		friend class AssetListModel;
	};

	class Iterator : public ConstIterator
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef Items::iterator::difference_type difference_type;
		typedef IAssetObjectItem value_type;
		typedef value_type* pointer;
		typedef value_type& reference;

		Iterator(const Iterator& rhs)
		    : ConstIterator()
		{
			this->iterator_.reset(new Items::const_iterator(rhs.iterator()));
		}
		Iterator& operator=(const Iterator& rhs)
		{
			if (this != &rhs)
			{
				this->iterator_.reset(new Items::const_iterator(*rhs.iterator_));
			}

			return *this;
		}

		reference operator*() const
		{
			return *operator->();
		}
		pointer operator->() const
		{
			auto item = (*this->iterator_)->get();
			return item;
		}

		Iterator& operator++()
		{
			++(*this->iterator_);
			return *this;
		}
		Iterator operator++(int)
		{
			Iterator tmp = *this;
			++(*this);
			return tmp;
		}
		Iterator operator+(difference_type n) const
		{
			*this->iterator_ += n;
			return *this;
		}

	private:
		Iterator(const Items::iterator& iterator)
		    : ConstIterator()
		{
			this->iterator_.reset(new Items::const_iterator(iterator));
		}

		const Items::const_iterator& iterator() const
		{
			return *(this->iterator_.get());
		}

		friend class AssetListModel;
	};

	// List manipulation and usage functions
	virtual void resize(size_t newSize);
	Iterator insert(const Iterator& position, IAssetObjectItem* value);
	Iterator erase(const Iterator& position);
	Iterator erase(const Iterator& first, const Iterator& last);

	void push_back(IAssetObjectItem* value);
	void push_front(IAssetObjectItem* value);
	const IAssetObjectItem& back() const;
	const IAssetObjectItem& front() const;

	IAssetObjectItem& operator[](size_t index);
	const IAssetObjectItem& operator[](size_t index) const;

private:
	AssetListModel(const AssetListModel& rhs);
	AssetListModel& operator=(const AssetListModel& rhs);

	Items items_;
};
} // end namespace wgt
#endif
