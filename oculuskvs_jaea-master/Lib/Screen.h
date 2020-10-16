#pragma once
#include <kvs/glut/Screen>
#include <kvs/FrameBufferObject>
#include "Application.h"
#include "HeadMountedDisplay.h"
#include "WidgetHandler.h"
#include "ObjectSelectionManager.h"

namespace kvs
{
namespace oculus
{
namespace jaea
{

class Screen : public kvs::glut::Screen
{
public:
    typedef kvs::glut::Screen BaseClass;

    enum MirrorBufferType
    {
        LeftEyeImage = 0,
        RightEyeImage,
        BothEyeImage,
        DistortedBothEyeImage
    };

    enum ManipulatorUseFlag
    {
        UseNoManipulator = 0,
        UseLeftManipulator,
        UseRightManipulator,
    };

private:
    kvs::oculus::jaea::HeadMountedDisplay m_hmd; ///< Oculus HMD
    MirrorBufferType m_mirror_buffer_type; ///< mirror buffer type

    kvs::oculus::jaea::WidgetHandler *m_widget_handler;
    ManipulatorUseFlag m_enableManipulator;

    ObjectSelectionManager m_objectSelectionManager;

protected:
    virtual void defaultPaintEvent();

public:
    Screen( kvs::oculus::jaea::Application* app = 0 );
    virtual ~Screen();

    bool isEnabledManipulator();
    void setEnabledManipulator(ManipulatorUseFlag flag);

    void setWidgetHandler (kvs::oculus::jaea::WidgetHandler *handler);

    const kvs::oculus::jaea::HeadMountedDisplay& headMountedDisplay() const { return m_hmd; }
    void setMirrorBufferType( const MirrorBufferType type ) { m_mirror_buffer_type = type; }
    void setMirrorBufferTypeToLeftEyeImage() { this->setMirrorBufferType( LeftEyeImage ); }
    void setMirrorBufferTypeToRightEyeImage() { this->setMirrorBufferType( RightEyeImage ); }
    void setMirrorBufferTypeToBothEyeImage() { this->setMirrorBufferType( BothEyeImage ); }
    void setMirrorBufferTypeToDistortedBothEyeImage() { this->setMirrorBufferType( DistortedBothEyeImage ); }

    virtual void show( const bool fullscreen = true );
    virtual void initializeEvent();
    virtual void paintEvent();
};

} // end of namespace jaea

} // end of namespace oculus

} // end of namespace kvs
