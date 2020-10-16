#include "MirrorBufferChangeEvent.h"
#include "Screen.h"
#include <kvs/KeyEvent>


namespace kvs
{

namespace oculus
{
namespace jaea
{

MirrorBufferChangeEvent::MirrorBufferChangeEvent():
    m_left_eye_image_key( kvs::Key::One ),
    m_right_eye_image_key( kvs::Key::Two ),
    m_both_eye_image_key( kvs::Key::Three ),
    m_distorted_both_eye_image_key( kvs::Key::Four )
{
}

void MirrorBufferChangeEvent::update( kvs::KeyEvent* event )
{
    kvs::oculus::jaea::Screen* oculus_screen = static_cast<kvs::oculus::jaea::Screen*>( screen() );
    if ( event->key() == m_left_eye_image_key ) oculus_screen->setMirrorBufferTypeToLeftEyeImage();
    else if ( event->key() == m_right_eye_image_key ) oculus_screen->setMirrorBufferTypeToRightEyeImage();
    else if ( event->key() == m_both_eye_image_key ) oculus_screen->setMirrorBufferTypeToBothEyeImage();
    else if ( event->key() == m_distorted_both_eye_image_key ) oculus_screen->setMirrorBufferTypeToDistortedBothEyeImage();
}

} // end of namespace jaea

} // end of namespace oculus

} // end of namespace kvs