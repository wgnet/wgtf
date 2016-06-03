#ifndef __CUSTOM_CONNECTION_H__
#define __CUSTOM_CONNECTION_H__

#include "plugins/plg_node_editor/interfaces/i_connection.hpp"

namespace wgt
{
class CustomConnection : public Implements<IConnection>
{
    DECLARE_REFLECTED
public:
    CustomConnection();
    virtual ~CustomConnection();

    size_t Id() const override { return m_id; }
    ISlot* Input() const override;
    ISlot* Output() const override;
    bool Bind(ObjectHandleT<ISlot> outputSlot, ObjectHandleT<ISlot> inputSlot) override;
    bool UnBind() override;
private:
    size_t m_id;
    ISlot *m_inputSlot;
    ISlot *m_outputSlot;

    bool isConnected;
};
} // end namespace wgt
#endif //__CUSTOM_CONNECTION_H__
