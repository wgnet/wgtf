#ifndef REFLECTED_METHOD_PARAMETERS_HPP
#define REFLECTED_METHOD_PARAMETERS_HPP

#include <vector>

#include "reflection_dll.hpp"

namespace wgt
{
class Variant;
class Collection;

class REFLECTION_DLL ReflectedMethodParameters
{
public:
	typedef Variant value_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef const value_type const_value_type;
	typedef const_value_type* const_pointer;
	typedef const_value_type& const_reference;

	ReflectedMethodParameters();
	ReflectedMethodParameters(const Variant& variant);
	ReflectedMethodParameters(const ReflectedMethodParameters& rhs);
	virtual ~ReflectedMethodParameters();

	ReflectedMethodParameters& operator=(const ReflectedMethodParameters& rhs);

	ReflectedMethodParameters& operator,(const Variant& variant);
	operator Collection();

	bool empty() const;
	size_t size() const;

	void clear();

	void push_back(const Variant& variant);

	reference operator[](size_t index);
	const_reference operator[](size_t index) const;

	struct REFLECTION_DLL const_iterator
	{
		friend class ReflectedMethodParameters;
		typedef std::random_access_iterator_tag iterator_category;
		typedef long long difference_type;

		const_reference operator*() const;
		const_pointer operator->() const;
		const_iterator operator++(int);
		const_iterator& operator++();
		const_iterator operator+(difference_type n) const;
		difference_type operator-(const const_iterator& rhs) const;
		bool operator==(const const_iterator& rhs) const;
		bool operator!=(const const_iterator& rhs) const;
		bool operator<(const const_iterator& rhs) const;

	protected:
		const_iterator(const ReflectedMethodParameters& collection, size_t index);

		ReflectedMethodParameters& collection_;
		size_t index_;
	};

	struct REFLECTION_DLL iterator : public const_iterator
	{
		friend class ReflectedMethodParameters;
		typedef std::random_access_iterator_tag iterator_category;
		typedef long long difference_type;

		reference operator*() const;
		pointer operator->() const;
		iterator operator++(int);
		iterator& operator++();
		iterator operator+(difference_type n) const;

	private:
		iterator(ReflectedMethodParameters& collection, size_t index);
	};

	const_iterator cbegin() const;
	const_iterator cend() const;
	iterator begin();
	iterator end();

private:
	struct Implementation;
	Implementation* impl_;
};
} // end namespace wgt
#endif // REFLECTED_METHOD_PARAMETERS_HPP
