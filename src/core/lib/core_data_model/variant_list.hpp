#ifndef VARIANT_LIST_HPP
#define VARIANT_LIST_HPP

#include "i_list_model.hpp"
#include "i_item.hpp"
#include <memory>

namespace wgt
{
class VariantList
	: public IListModel
{
public:
	VariantList();
	virtual ~VariantList();

	// IListModel
	virtual IItem * item( size_t index ) const override;
	virtual size_t index( const IItem * item ) const override;

	virtual bool empty() const override;
	virtual size_t size() const override;
	virtual int columnCount() const override;

	bool canClear() const override;
	void clear() override;
	//

	typedef std::vector< std::unique_ptr< IItem > > Items;

	class ConstIterator
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef Items::const_iterator::difference_type difference_type;
		typedef const Variant value_type;
		typedef value_type * pointer;
		typedef value_type & reference;

		ConstIterator( const ConstIterator& rhs );
		ConstIterator& operator=( const ConstIterator& rhs );

		reference operator*( ) const;
		pointer operator->() const;
		ConstIterator & operator++( );
		ConstIterator operator++( int );
		bool operator==( const ConstIterator & other ) const;
		bool operator!=( const ConstIterator & other ) const;
		bool operator<( const ConstIterator & other ) const;
		difference_type operator-( const ConstIterator & other ) const;
		ConstIterator operator+(difference_type n) const;

	protected:
		ConstIterator() {}
		ConstIterator( const Items::const_iterator & iterator );

		const Items::const_iterator& iterator() const;

		std::unique_ptr<Items::const_iterator> iterator_;

		friend class VariantList;
	};

	class Iterator : public ConstIterator
	{
	public:
		typedef std::random_access_iterator_tag iterator_category;
		typedef Items::iterator::difference_type difference_type;
		typedef Variant value_type;
		typedef value_type * pointer;
		typedef value_type & reference;

		Iterator( const Iterator& rhs );
		Iterator& operator=( const Iterator& rhs );

		reference operator*( ) const;
		pointer operator->() const;
		Iterator & operator++( );
		Iterator operator++( int );
		Iterator operator+(difference_type n) const;

	private:
		Iterator( const Items::iterator & iterator );

		const Items::const_iterator& iterator() const;

		friend class VariantList;
	};

	void resize( size_t newSize );

	ConstIterator cbegin() const;
	ConstIterator cend() const;
	ConstIterator begin() const;
	ConstIterator end() const;
	Iterator begin();
	Iterator end();

	Iterator insert( const Iterator & position, const Variant & value );
	Iterator erase( const Iterator & position );
	Iterator erase( const Iterator & first, const Iterator & last );

	void emplace_back( Variant && value );
	void push_back( const Variant & value );
	void push_front( const Variant & value );
	Variant pop_back();
	Variant pop_front();
	const Variant & back() const;
	const Variant & front() const;

	Variant & operator[](size_t index);
	const Variant & operator[](size_t index) const;

private:
	VariantList( const VariantList& rhs );
	VariantList& operator=( const VariantList& rhs );

	Items items_;

	friend class Iterator;
};
} // end namespace wgt
#endif // VARIANT_LIST_HPP
