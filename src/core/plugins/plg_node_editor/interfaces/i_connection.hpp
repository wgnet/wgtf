#ifndef __I_CONNECTION_H__
#define __I_CONNECTION_H__

#include "core_reflection/reflected_object.hpp"
#include "core_data_model/i_list_model.hpp"

#include "i_slot.hpp"

namespace wgt
{
/*!
* \class IConnection
*
* \brief Interface is allowing to create connection between slots
*
* \author a_starunskiy
* \date May 2016
*/
class IConnection
{
    DECLARE_REFLECTED
public:
	IConnection(){}
    virtual ~IConnection(){}

    /*! Returns the unique connection Id
    @return unique connection id
    */
    virtual size_t Id() const = 0;

    /*! Returns the connections input slot
    @return input slot
    */
    virtual ISlot* Input() const = 0;

    /*! Returns the connection's output slot
    @return output slot
    */
    virtual ISlot* Output() const = 0;

    /*! Binds two slots
    @param outputSlot The slot which will be connected as output
    @param inputSlot The slot which will be connected as input
    @return true if two slots are successfully binded, false otherwise
    */
    virtual bool Bind(ObjectHandleT<ISlot> outputSlot, ObjectHandleT<ISlot> inputSlot) = 0;

    /*! Unbinds two slots
    @return true if two slots are succesfully unbinded, false otherwise
    */
    virtual bool UnBind() = 0;
};
} // end namespace wgt
#endif //__I_CONNECTION_H__
