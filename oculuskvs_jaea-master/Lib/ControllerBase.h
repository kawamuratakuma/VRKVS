#pragma once
#include <kvs/EventListener>
#include "Screen.h"
#include "InputDevice.h"


namespace kvs
{

namespace oculus
{
namespace jaea
{

class ControllerBase : public kvs::EventListener
{
private:
    kvs::oculus::jaea::InputDevice m_input_device; ///< input device

public:
    ControllerBase( kvs::oculus::jaea::Screen* screen );

    virtual void initializeEvent() {}
    virtual void frameEvent() {}

protected:
    kvs::oculus::jaea::InputDevice& inputDevice() { return m_input_device; }

private:
    void onEvent( kvs::EventBase* event );
};

} // end of namespace jaea

} // end of namespace oculus

} // end of namespace kvs
