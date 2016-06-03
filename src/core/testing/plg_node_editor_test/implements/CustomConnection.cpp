#include "CustomConnection.h"
#include "core_logging/logging.hpp"

namespace wgt
{
CustomConnection::CustomConnection() 
: m_inputSlot(nullptr)
, m_outputSlot(nullptr)
, isConnected(false)
{
    m_id = reinterpret_cast<size_t>(this);
}

CustomConnection::~CustomConnection()    
{
    UnBind();
}

ISlot* CustomConnection::Input() const
{
    return m_inputSlot;
}

ISlot* CustomConnection::Output() const
{
    return m_outputSlot;
}

bool CustomConnection::Bind(ObjectHandleT<ISlot> outputSlot, ObjectHandleT<ISlot> inputSlot)
{
    bool result = false;
    if (outputSlot == nullptr || inputSlot == nullptr)
    {
        NGT_ERROR_MSG("Input arguments are null\n");
        return result;
    }

    if (isConnected)
    {
        NGT_ERROR_MSG("Connection is already connected\n");
        return result;
    }

    while (true)
    {
        result = outputSlot->CanConnect(inputSlot);

        if (!result)
            break;

        result = inputSlot->CanConnect(outputSlot);
        
        if (!result)
            break;

        if (outputSlot->Connect(m_id, inputSlot) && inputSlot->Connect(m_id, outputSlot))
        {
            m_inputSlot = inputSlot.get();
            m_outputSlot = outputSlot.get();
            isConnected = true;
        }            
        else
        {
            result = false;
            NGT_ERROR_MSG("Failed to connect input and output slots\n");

            outputSlot->Disconnect(m_id, inputSlot);
            inputSlot->Disconnect(m_id, outputSlot);
        }

        break;
    }

    return result;
}

bool CustomConnection::UnBind()
{
    bool result = true;

    while (true)
    {
        if (!isConnected)
        {
            NGT_WARNING_MSG("Connection is not connected\n");
            break;
        }            

        if (!m_inputSlot->Disconnect(m_id, m_outputSlot))
        {
            result = false;
            NGT_ERROR_MSG("Failed to disconnect input slot with output slot\n");
            break;
        }            

        if (!m_outputSlot->Disconnect(m_id, m_inputSlot))
        {
            result = false;
            NGT_ERROR_MSG("Failed to disconnect output slot with input slot\n");
            break;
        }
            
        m_inputSlot = nullptr;
        m_outputSlot = nullptr;
        isConnected = false;
        break;
    }

    return result;
}
} // end namespace wgt
