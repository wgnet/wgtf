#ifndef I_QT_TYPE_CONVERTER_HPP
#define I_QT_TYPE_CONVERTER_HPP

#include "wg_types/binary_block.hpp"
#include <core_variant/variant.hpp>
#include <QVariant>

Q_DECLARE_METATYPE( std::shared_ptr< wgt::BinaryBlock > );

namespace wgt
{
class ObjectHandle;

/**
 *	Interface for converting custom C++ types to/from QVariant.
 */
class IQtTypeConverter
{
public:
	/**
	 *	Interface required by TypeConverterQueue.
	 */
	virtual bool toVariant( const QVariant & qVariant, Variant & o_variant ) const = 0;
	
	/**
	*	 Interface required by TypeConverterQueue.
	*/
	bool toScriptType(const Variant & variant, QVariant & o_qVariant, QObject* parent = nullptr ) const
	{
		return this->toQVariant( variant, o_qVariant, parent );
	}

	virtual bool toQVariant(const Variant & variant, QVariant & o_qVariant, QObject* parent = nullptr ) const = 0;

	virtual bool toQVariant( const ObjectHandle & object,
		QVariant & o_qVariant, QObject* parent = nullptr ) const
	{ return false; };
};

template< typename T, typename U = T >
class GenericQtTypeConverter : public IQtTypeConverter
{
public:
	bool toVariant( const QVariant & qVariant, Variant & o_variant ) const override
	{
		int typeId = qVariant.type();
		if (typeId == QVariant::UserType)
		{
			typeId = qVariant.userType();
		}

		if (typeId != qMetaTypeId<U>())
		{
			return false;
		}

		o_variant = static_cast<T>( qVariant.value<U>() );
		return true;
	}

	bool toQVariant( const Variant & variant, QVariant & o_qVariant, QObject* parent = nullptr ) const override
	{
		if (variant.typeIs< T >() == false)
		{
			return false;
		}
		U tmp;
		if (variant.tryCast( tmp ))
		{
			o_qVariant = QVariant::fromValue( tmp );
		}
		return true;
	}
};
} // end namespace wgt
#endif
