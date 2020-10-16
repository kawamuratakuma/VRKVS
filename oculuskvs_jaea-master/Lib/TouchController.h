#pragma once
#include "ControllerBase.h"
#include "Screen.h"


namespace kvs
{

namespace oculus
{
namespace jaea
{

class TouchController : public kvs::oculus::jaea::ControllerBase
{
private:
    bool m_is_grabbed;
    bool m_both_is_grabbed;
    float m_touch_distance;
    float m_rotation_factor;
    float m_translation_factor;
    float m_scaling_factor;
    kvs::Vec3 m_rot_start_pos_right;

public:
    TouchController( kvs::oculus::jaea::Screen* screen );

    float rotationFactor() const { return m_rotation_factor; }
    float translationFactor() const { return m_translation_factor; }
    float scalingFactor() const { return m_scaling_factor; }
    void setRotationFactor( const float factor ) { m_rotation_factor = factor; }
    void setTranslationFactor( const float factor ) { m_translation_factor = factor; }
    void setScalingFactor( const float factor ) { m_scaling_factor = factor; }

    virtual void initializeEvent();
    virtual void frameEvent();
};

} // end of namespace jaea

} // end of namespace oculus

} // end of namespace kvs
