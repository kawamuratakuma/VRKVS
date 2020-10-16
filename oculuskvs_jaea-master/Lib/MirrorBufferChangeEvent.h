#pragma once
#include <kvs/KeyPressEventListener>

namespace kvs
{

namespace oculus
{
namespace jaea
{

class MirrorBufferChangeEvent : public kvs::KeyPressEventListener
{
private:
    int m_left_eye_image_key;
    int m_right_eye_image_key;
    int m_both_eye_image_key;
    int m_distorted_both_eye_image_key;

public:
    MirrorBufferChangeEvent();

    void setLeftEyeImageKey( const int key ) { m_left_eye_image_key = key; }
    void setRightEyeImageKey( const int key ) { m_right_eye_image_key = key; }
    void setBothEyeImageKey( const int key ) { m_both_eye_image_key = key; }
    void setDistortedBothEyeImageKey( const int key ) { m_distorted_both_eye_image_key = key; }
    void update( kvs::KeyEvent* event );
};

} // end of namespace jaea

} // end of namespace oculus

} // end of namespace kvs
