#include "Screen.h"
#include <kvs/OpenGL>
#include <kvs/Platform>
#include <kvs/Background>
#include <kvs/Camera>
#include <kvs/ObjectManager>
#include <kvs/RendererManager>
#include <kvs/IDManager>
#include <kvs/RendererBase>
#include <kvs/PaintEvent>
#include <kvs/FrameEvent>
#include <kvs/glut/GLUT>

namespace {

inline kvs::Vec3 ToVec3(const OVR::Vector3f& v) {
    return kvs::Vec3(v.x, v.y, v.z);
}

inline OVR::Vector3f ToVec3(const kvs::Vec3& v) {
    return OVR::Vector3f(v.x(), v.y(), v.z());
}

inline kvs::Mat4 ToMat4(const OVR::Matrix4f& m) {
    return kvs::Mat4(m.M[0][0], m.M[0][1], m.M[0][2], m.M[0][3], m.M[1][0], m.M[1][1], m.M[1][2], m.M[1][3], m.M[2][0], m.M[2][1], m.M[2][2],
            m.M[2][3], m.M[3][0], m.M[3][1], m.M[3][2], m.M[3][3]);
}

inline OVR::Matrix4f ToMat4(const kvs::Mat3& m) {
    return OVR::Matrix4f(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);
}

inline OVR::Matrix4f ToMat4(const kvs::Mat4& m) {
    return OVR::Matrix4f(m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1],
            m[3][2], m[3][3]);
}
} // end of namespace <anon>

namespace kvs {
namespace oculus {
namespace jaea {

Screen::Screen(kvs::oculus::jaea::Application* app) :
        kvs::glut::Screen(app),
        m_widget_handler(nullptr),
        m_mirror_buffer_type(DistortedBothEyeImage),
        m_enableManipulator(UseNoManipulator) {
    static bool flag = true;
    if (flag) {
        flag = false;
        if (m_hmd.create()) {
            const int desktop_width = glutGet(GLUT_SCREEN_WIDTH);
            const int desktop_height = glutGet(GLUT_SCREEN_HEIGHT);

            const int hmd_width = m_hmd.resolution().w;
            const int hmd_height = m_hmd.resolution().h;

            const float ratio = static_cast<float>(desktop_width) / hmd_width;
            const int screen_width = (ratio >= 1.0f) ? hmd_width : desktop_width;
            const int screen_height = (ratio >= 1.0f) ? hmd_height : static_cast<int>(hmd_height * ratio);
            setSize(screen_width, screen_height);
        }
    }

    scene()->camera()->setProjectionTypeToFrustum();
}

Screen::~Screen() {
    m_hmd.destroy();
}

bool Screen::isEnabledManipulator() {
    return (m_enableManipulator == Screen::UseLeftManipulator) || (m_enableManipulator == Screen::UseRightManipulator);
}
void Screen::setEnabledManipulator(Screen::ManipulatorUseFlag flag) {
    this->m_enableManipulator = flag;
}

void Screen::show(const bool fullscreen) {
    kvs::glut::Screen::show();
    if (fullscreen) {
        kvs::glut::Screen::showFullScreen();
    }
}

void Screen::setWidgetHandler(kvs::oculus::jaea::WidgetHandler *handler) {
#ifdef DEBUG_HUD
    std::cerr << "Screen::setWidgethandler() : handler=" << handler << std::endl;
#endif // DEBUG_HUD

    m_widget_handler = handler;
    //m_hmd.setWidgetHandler(handler);
}

void Screen::initializeEvent() {
#ifdef DEBUG_HUD
std::cerr << "Screen::initializeEvent() : start" << std::endl;
#endif //DEBUG_HUD

    // Configure tracking.
    const kvs::UInt32 trac_caps = ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position;
    if (!m_hmd.configureTracking(trac_caps, 0)) {
        kvsMessageError("Failed tracking configuration");
    }

    // Configure rendering.
    if (!m_hmd.configureRendering()) {
        kvsMessageError("Failed rendering configuration.");
    }

#ifdef DEBUG_HUD
    std::cerr << "Screen::initializeEvent() : m_hmd.configureRendering() finished." << std::endl;
#endif //DEBUG_HUD

    kvs::glut::Screen::initializeEvent();

#ifdef DEBUG_HUD
    std::cerr << "Screen::initializeEvent() : kvs::glut::Screen::initializeEvent() finished." << std::endl;
#endif //DEBUG_HUD
    
    if (m_widget_handler != nullptr) {
        // ImGui initialize functions must be called after initialize glut.
        m_hmd.setWidgetHandler(m_widget_handler);
    }

    m_objectSelectionManager.setHeadMountDisplay(m_hmd);
}

void Screen::defaultPaintEvent() {
    if (scene()->objectManager()->hasObject()) {
        const int size = scene()->IDManager()->size();
        for (int index = 0; index < size; index++) {
            kvs::IDManager::IDPair id = scene()->IDManager()->id(index);
            kvs::ObjectBase* object = scene()->objectManager()->object(id.first);
            kvs::RendererBase* renderer = scene()->rendererManager()->renderer(id.second);
            if (object->isShown()) {
                kvs::OpenGL::PushMatrix();
                scene()->updateGLModelingMatrix(object);
                renderer->exec(object, scene()->camera(), scene()->light());
                kvs::OpenGL::PopMatrix();
            }
        }
    } else {
        scene()->updateGLModelingMatrix();
    }
}


void Screen::paintEvent() {
    static kvs::Int64 frame_index = 0;
    static OVR::Vector3f initial_hp;
    static OVR::Vector3f initial_ep[2];

    //bool retryCreate = false;

    ovrSessionStatus sessionStatus;
    ovr_GetSessionStatus(m_hmd.handler(), &sessionStatus);
    if (sessionStatus.ShouldQuit) {
        // Because the application is requested to quit, should not request retry
        //retryCreate = false;
        //break;
        // TODO : implement shutdown event.
    }
    if (sessionStatus.ShouldRecenter)
    {
        ovr_RecenterTrackingOrigin(m_hmd.handler());
    }

    if (sessionStatus.IsVisible) {
        const double frame_timing = m_hmd.frameTiming(frame_index);
        const ovrTrackingState& ts = m_hmd.trackingState(frame_timing);
        const kvs::Vec4 current_viewport = kvs::OpenGL::Viewport();

        {
            // call frame event listeners
            kvs::FrameEvent event;
            BaseClass::eventHandler()->notify( &event );
        }

        kvs::OpenGL::WithPushedMatrix p(GL_MODELVIEW);
        p.loadIdentity();

        m_hmd.beginFrame(frame_index);

        scene()->updateGLLightParameters();

        // preserve camera status
        const kvs::Vec3 camera_position = scene()->camera()->position();
        const kvs::Vec3 camera_lookat = scene()->camera()->lookAt();
        const kvs::Vec3 camera_upvector = scene()->camera()->upVector();
        const OVR::Vector3f position0 = ::ToVec3(camera_position);
        const OVR::Vector3f lookat0 = ::ToVec3(camera_lookat);
        const OVR::Vector3f upvector0 = ::ToVec3(camera_upvector);
        const OVR::Matrix4f rotation0 = ::ToMat4(scene()->camera()->xform().rotation());

        const size_t neyes = ovrEye_Count;
        for (size_t eye = 0; eye < neyes; eye++) {
            // bind eye render target
            m_hmd.eyeRenderTexture(eye)->SetAndClearRenderSurface();

            const ovrEyeRenderDesc& render_desc = m_hmd.renderDesc(eye);
            const ovrPosef& eye_pose = m_hmd.eyePose(eye);
            const ovrRecti& viewport = m_hmd.viewport(eye);

            // Set viewport.
            const ovrVector2i pos = viewport.Pos;
            const ovrSizei size = viewport.Size;
            kvs::OpenGL::SetViewport(pos.x, pos.y, size.w, size.h);

            // Set projection matrix.
            const float aspect = static_cast<float>(size.w) / size.h;
            const float front = scene()->camera()->front();
            scene()->camera()->setWindowSize(size.w, size.h);
            if (aspect >= 1.0f) {
                scene()->camera()->setTop(render_desc.Fov.UpTan * front);
                scene()->camera()->setBottom(-render_desc.Fov.DownTan * front);
                scene()->camera()->setLeft(-render_desc.Fov.LeftTan * front / aspect);
                scene()->camera()->setRight(render_desc.Fov.RightTan * front / aspect);
            } else {
                scene()->camera()->setTop(render_desc.Fov.UpTan * front * aspect);
                scene()->camera()->setBottom(-render_desc.Fov.DownTan * front * aspect);
                scene()->camera()->setLeft(-render_desc.Fov.LeftTan * front);
                scene()->camera()->setRight(render_desc.Fov.RightTan * front);
            }
            scene()->updateGLProjectionMatrix();

            // Set viewing matrix.
            if (frame_index == 0) {
                initial_hp = ts.HeadPose.ThePose.Position;
                initial_ep[eye] = eye_pose.Position;
            }

            if (frame_index > 0) {
                // OVR::Vector3f shift_value(0.0f, -0.7f,0.0f);

                const float scale = 10.0f;
                const OVR::Matrix4f R0 = rotation0;
                const OVR::Matrix4f R = R0 * OVR::Matrix4f(ts.HeadPose.ThePose.Orientation);
                OVR::Vector3f PP = ts.HeadPose.ThePose.Position;
                PP -= initial_hp;

                //std::cerr << "HeadPose.Pos=(" << ts.HeadPose.ThePose.Position.x << "," << ts.HeadPose.ThePose.Position.y << "," << ts.HeadPose.ThePose.Position.z << ")" << std::endl;

                const OVR::Vector3f T = R0.Transform(PP) * scale;
                // const OVR::Vector3f T = R0.Transform(ts.HeadPose.ThePose.Position) * scale;
                const OVR::Vector3f upvector = R.Transform(upvector0);
                const OVR::Vector3f forward = R.Transform(lookat0 - position0);
                //const OVR::Vector3f position = ts.HeadPose.ThePose.Position;
                OVR::Vector3f EP = eye_pose.Position;
                EP -= initial_ep[eye];

                //std::cerr << "EyePose.Pos=(" << eye_pose.Position.x << "," << eye_pose.Position.y << "," << eye_pose.Position.z << ")" << std::endl;

                const OVR::Vector3f position = position0 + T + R.Transform( EP ) * scale;
                //const OVR::Vector3f position = position0 + T + R.Transform( eye_pose.Position ) * scale;
                const OVR::Vector3f lookat = position + forward;
                scene()->camera()->setPosition(::ToVec3(position), ::ToVec3(lookat), ::ToVec3(upvector));
            }
            scene()->updateGLViewingMatrix();

            // Setup the modeling matrix and render the objects.
            if (m_enable_default_paint_event) {
                this->defaultPaintEvent();
            } else {
                kvs::PaintEvent event;
                BaseClass::eventHandler()->notify( &event );
            }
            // draw beams & select object
            // TODO : fix drawing procedure to consider scale value (even if scale value is not set to 1.0)
            //int selectedObjectIndex = m_objectSelectionManager.select(scene());

            // unbind eye render target
            m_hmd.eyeRenderTexture(eye)->UnsetRenderSurface();
            m_hmd.eyeRenderTexture(eye)->Commit();
        }

        // recover camera position
        scene()->camera()->setPosition(camera_position, camera_lookat, camera_upvector);

        if (m_widget_handler != nullptr) {
#ifdef DEBUG_HUD
        std::cerr << "Screen::paintEvent(): call m_hmd.updateHUD()" << std::endl;
#endif // DEBUG_HUD
            m_hmd.updateHUD();
        }

        m_hmd.endFrame(frame_index);

        //std::cout << "# x26" << std::endl;
        // Render to the frame buffer.
        kvs::OpenGL::SetViewport(current_viewport);
        switch (m_mirror_buffer_type) {
        case LeftEyeImage:
            m_hmd.renderToMirrorLeftEyeImage();
            break;
        case RightEyeImage:
            m_hmd.renderToMirrorRightEyeImage();
            break;
        case BothEyeImage:
            m_hmd.renderToMirrorBothEyeImage();
            break;
        case DistortedBothEyeImage:
            m_hmd.renderToMirrorDistortedBothEyeImage();
            break;
        default:
            break;
        }

        // Redraw.
        //kvs::OpenGL::Finish();
        kvs::OpenGL::Flush();
        redraw();

        frame_index++;
    }

    {
        // debug
        ovrErrorInfo info;
        ovr_GetLastErrorInfo(&info);
    }
}

// void Screen::paintEvent() {
//     static kvs::Int64 frame_index = 0;

//     bool retryCreate = false;

//     ovrSessionStatus sessionStatus;
//     {
//         ovr_GetSessionStatus(m_hmd.handler(), &sessionStatus);
//         if (sessionStatus.ShouldQuit) {
//             // Because the application is requested to quit, should not request retry
//             retryCreate = false;
//             //break;
//         }
//         if (sessionStatus.ShouldRecenter)
//             ovr_RecenterTrackingOrigin(m_hmd.handler());
//     }

//     //std::cout << "############## " << (sessionStatus.IsVisible == ovrTrue) << std::endl;

//     if (sessionStatus.IsVisible) {
//         //std::cout << "# x1" << std::endl;
//         //const kvs::Int64 frame_index = 0;
//         const double frame_timing = m_hmd.frameTiming(frame_index);
//         const ovrTrackingState& ts = m_hmd.trackingState(frame_timing);
//         const kvs::Vec4 current_viewport = kvs::OpenGL::Viewport();

//         //std::cout << "# x2" << std::endl;
//         kvs::OpenGL::WithPushedMatrix p( GL_MODELVIEW);
//         p.loadIdentity();
//         {
//             //std::cout << "# x3" << std::endl;
//             m_hmd.beginFrame(frame_index);

//             //std::cout << "# x4" << std::endl;
//             //scene()->background()->apply();
//             scene()->updateGLLightParameters();

//             //std::cout << "# x5" << std::endl;
//             const kvs::Vec3 camera_position = scene()->camera()->position();
//             const kvs::Vec3 camera_lookat = scene()->camera()->lookAt();
//             const kvs::Vec3 camera_upvector = scene()->camera()->upVector();

//             //std::cout << "# x6" << std::endl;
//             const OVR::Vector3f position0 = ::ToVec3(camera_position);
//             const OVR::Vector3f lookat0 = ::ToVec3(camera_lookat);
//             const OVR::Vector3f upvector0 = ::ToVec3(camera_upvector);
//             const OVR::Matrix4f rotation0 = ::ToMat4(scene()->camera()->xform().rotation());
//             // Call ovr_GetRenderDesc each frame to get the ovrEyeRenderDesc, as the returned values (e.g. HmdToEyePose) may change at runtime.
//             /*ovrEyeRenderDesc eyeRenderDesc[2];
//              eyeRenderDesc[0] = ovr_GetRenderDesc(m_hmd.handler(), ovrEye_Left,
//              m_hmd.defaultEyeFov(0));
//              eyeRenderDesc[1] = ovr_GetRenderDesc(m_hmd.handler(), ovrEye_Right,
//              m_hmd.defaultEyeFov(1));

//              // Get eye poses, feeding in correct IPD offset
//              ovrPosef EyeRenderPose[2];
//              ovrPosef HmdToEyePose[2] = { eyeRenderDesc[0].HmdToEyePose,
//              eyeRenderDesc[1].HmdToEyePose };

//              double sensorSampleTime; // sensorSampleTime is fed into the layer later
//              ovr_GetEyePoses(m_hmd.handler(), frame_index, ovrTrue, HmdToEyePose,
//              EyeRenderPose, &sensorSampleTime);
//              */

//             //std::cout << "# x7" << std::endl;
//             const size_t neyes = ovrEye_Count;
//             //std::cout << "## neyes=" << neyes << std::endl;
//             //for (size_t eye = 1; eye >=0; eye--) {
//             for (size_t eye = 0; eye < neyes; eye++) {
//                 //std::cout << "# x8" << std::endl;
//                 {
//                     m_hmd.eyeRenderTexture(eye)->SetAndClearRenderSurface();
//                 }
//                 //std::cout << "# x9" << std::endl;
//                 {
//                     //int tmp = kvs::OpenGL::Integer( GL_FRAMEBUFFER_BINDING);
//                     //std::cout << "##A i=" << eye << " : framebuffer==" << tmp << std::endl;
//                 }

//                 //std::cout << "# x10" << std::endl;
//                 const ovrEyeRenderDesc& render_desc = m_hmd.renderDesc(eye);
//                 const ovrPosef& eye_pose = m_hmd.eyePose(eye);
//                 const ovrRecti& viewport = m_hmd.viewport(eye);
//                 //std::cout << "## i=" << i << " : render_desc=" << render_desc. << std::endl;
//                 //std::cout << "## i=" << i << " : eye_pose=" << eye_pose.Position.x
//                 //        << ", " << eye_pose.Position.y << ", "
//                 //        << eye_pose.Position.z << ", " << std::endl;
//                 //std::cout << "## i=" << i << " : viewport=" << viewport.Pos.x
//                 //        << ", " << viewport.Pos.y << ", " << viewport.Size.w << ", "
//                 //        << viewport.Size.h << ", " << std::endl;

//                 //std::cout << "# x11" << std::endl;
//                 // Set viewport.
//                 const ovrVector2i pos = viewport.Pos;
//                 const ovrSizei size = viewport.Size;
//                 kvs::OpenGL::SetViewport(pos.x, pos.y, size.w, size.h);

//                 //std::cout << "# x12" << std::endl;
//                 // Set projection matrix.
//                 const float aspect = static_cast<float>(size.w) / size.h;
//                 const float front = scene()->camera()->front();
//                 scene()->camera()->setWindowSize(size.w, size.h);
//                 //std::cout << "# x13" << std::endl;
//                 if (aspect >= 1.0f) {
//                     scene()->camera()->setTop(render_desc.Fov.UpTan * front);
//                     scene()->camera()->setBottom(-render_desc.Fov.DownTan * front);
//                     scene()->camera()->setLeft(-render_desc.Fov.LeftTan * front / aspect);
//                     scene()->camera()->setRight(render_desc.Fov.RightTan * front / aspect);
//                 } else {
//                     scene()->camera()->setTop(render_desc.Fov.UpTan * front * aspect);
//                     scene()->camera()->setBottom(-render_desc.Fov.DownTan * front * aspect);
//                     scene()->camera()->setLeft(-render_desc.Fov.LeftTan * front);
//                     scene()->camera()->setRight(render_desc.Fov.RightTan * front);
//                 }
//                 scene()->updateGLProjectionMatrix();

//                 //std::cout << "# x14" << std::endl;
//                 // Set viewing matrix.
//                 //const float scale = 10.0f;
//                 const float scale = 1.0f;
//                 const OVR::Matrix4f R0 = rotation0;
//                 const OVR::Matrix4f R = R0 * OVR::Matrix4f(ts.HeadPose.ThePose.Orientation);
//                 const OVR::Vector3f T = R0.Transform(ts.HeadPose.ThePose.Position) * scale;
//                 const OVR::Vector3f upvector = R.Transform(upvector0);
//                 const OVR::Vector3f forward = R.Transform(lookat0 - position0);
//                 //const OVR::Vector3f position = position0 + T + R.Transform(eye_pose.Position) * scale;
//                 const OVR::Vector3f position = ts.HeadPose.ThePose.Position;
//                 const OVR::Vector3f lookat = position + forward;
//                 scene()->camera()->setPosition(::ToVec3(position), ::ToVec3(lookat), ::ToVec3(upvector));
//                 scene()->updateGLViewingMatrix();

//                 const OVR::Matrix4f rotationCurr = ::ToMat4(scene()->camera()->xform().rotation());
//                 ovrInputState input_state = m_hmd.inputState(ovrControllerType_Touch);
//                 ovrTrackingState tracking_state = m_hmd.trackingState();
//                 ovrHandType_ handType = (m_enableManipulator == Screen::UseLeftManipulator) ? ovrHand_Left : ovrHand_Right;
//                 ovrPoseStatef handPoseStateL = tracking_state.HandPoses[ovrHand_Left];
//                 ovrPoseStatef handPoseStateR = tracking_state.HandPoses[ovrHand_Right];
//                 ovrPosef handPoseL = handPoseStateL.ThePose;
//                 ovrPosef handPoseR = handPoseStateR.ThePose;
//                 ovrVector3f handPositionL = handPoseL.Position;
//                 ovrVector3f handPositionR = handPoseR.Position;
//                 ovrQuatf handQuatL = handPoseL.Orientation;
//                 ovrQuatf handQuatR = handPoseR.Orientation;
//                 //const OVR::Vector3f positionL = position0 + R.Transform(handPositionL) * scale;
//                 //const OVR::Vector3f positionR = position0 + R.Transform(handPositionR) * scale;
//                 const OVR::Vector3f positionL = handPositionL;
//                 const OVR::Vector3f positionR = handPositionR;
//                 //const OVR::Vector3f positionL =  OVR::Matrix4f(ts.HeadPose.ThePose.Orientation).Transform(handPositionL) * scale;
//                 //const OVR::Vector3f positionR =  OVR::Matrix4f(ts.HeadPose.ThePose.Orientation).Transform(handPositionR) * scale;
//                 {
//                     glEnable(GL_DEPTH_TEST);
//                     glPointSize(10);
//                     glBegin(GL_POINTS);
//                     glColor3f(1.0f, 0.0f, 0.0f);
//                     glVertex3f(handPositionL.x, handPositionL.y, handPositionL.z);
//                     glColor3f(0.0f, 0.0f, 1.0f);
//                     glVertex3f(positionR.x, positionR.y, positionR.z);
//                     glColor3f(0.0f, 1.0f, 0.0f);
//                     glVertex3f(lookat.x, lookat.y, lookat.z);
//                     glEnd();
//                 }

//                 char str_R0[1024];
//                 R0.ToString(str_R0, 1024);
//                 char str_R[1024];
//                 R.ToString(str_R, 1024);
//                 char str_tmp[1024];
//                 OVR::Matrix4f(ts.HeadPose.ThePose.Orientation).ToString(str_tmp, 1024);
//                 //std::cout << "# x14.1 " << scene()->camera()->viewingMatrix() << std::endl;
//                 //std::cout << "# str_R0  " << str_R0 << std::endl;
//                 //std::cout << "# str_tmp " << str_tmp << std::endl;
//                 //std::cout << "# x14.3 " << str_R << std::endl;
//                 //std::cout << "# position0           : " << position0.x << ", " << position0.y << ", " << position0.z << std::endl;
//                 //std::cout << "# head                : " << ts.HeadPose.ThePose.Position.x << ", " << ts.HeadPose.ThePose.Position.y << ", "
//                 //        << ts.HeadPose.ThePose.Position.z << std::endl;
//                 //std::cout << "# head T              : " << T.x << ", " << T.y << ", " << T.z << std::endl;
//                 //std::cout << "# position            : " << position.x << ", " << position.y << ", " << position.z << std::endl;
//                 std::cout << "# handPositionL       : " << handPositionL.x << ", " << handPositionL.y << ", " << handPositionL.z << std::endl;
//                 std::cout << "# handPositionR       : " << handPositionR.x << ", " << handPositionR.y << ", " << handPositionR.z << std::endl;
//                 //std::cout << "# positionL           : " << positionL.x << ", " << positionL.y << ", " << positionL.z << std::endl;
//                 //std::cout << "# positionR           : " << positionR.x << ", " << positionR.y << ", " << positionR.z << std::endl;
//                 //std::cout << "# up                  : " << upvector.x << ", " << upvector.y << ", " << upvector.z << std::endl;
//                 //std::cout << "# forward             : " << forward.x << ", " << forward.y << ", " << forward.z << std::endl;
//                 std::cout << "# lookat              : " << lookat.x << ", " << lookat.y << ", " << lookat.z << std::endl;
//                 //std::cout << "# eye_pose Position   : " << eye_pose.Position.x << ", " << eye_pose.Position.y << ", " << eye_pose.Position.z
//                 //        << std::endl;
//                 //std::cout << "# eye_pose Orientation: " << eye_pose.Orientation.x << ", " << eye_pose.Orientation.y << ", " << eye_pose.Orientation.z
//                 //        << ", " << eye_pose.Orientation.w << std::endl;

//                 /*if (m_enableManipulator != Screen::UseNoManipulator) {
//                  // TODO : re-implement me!

//                  const OVR::Matrix4f Rh = R;
//                  const OVR::Vector3f Th = R0.Transform(position0 - handPosition) * scale;
//                  //const OVR::Vector3f ph = position0 + Th + Rh.Transform( handPosition ) * scale;

//                  // ovrVector3f ph;
//                  // ph.x = handPosition.x - eye_pose.Position.x;
//                  // ph.y = handPosition.y - eye_pose.Position.y;
//                  // ph.z = handPosition.z - eye_pose.Position.z;
//                  ovrVector3f ph;
//                  ph.x = handPosition.x;
//                  ph.y = handPosition.y;
//                  ph.z = handPosition.z;

//                  if (eye == 0) {
//                  std::cerr << " eye[" << eye << "] " << "Pos(" << eye_pose.Position.x << ", " << eye_pose.Position.y << ", "
//                  << eye_pose.Position.z << ")" << "Quat(" << eye_pose.Orientation.x << ", " << eye_pose.Orientation.y << ", "
//                  << eye_pose.Orientation.z << ", " << eye_pose.Orientation.w << ")" << std::endl;
//                  std::cerr << "hand[" << ((m_enableManipulator == Screen::UseLeftManipulator) ? "L" : "R") << "] " << "Pos(" << handPosition.x
//                  << ", " << handPosition.y << ", " << handPosition.z << "), " << "Quat(" << handQuat.x << ", " << handQuat.y << ", "
//                  << handQuat.z << ", " << handQuat.w << ")" << " -> (" << ph.x << "," << ph.y << "," << ph.z << ")" << std::endl;
//                  }

//                  glPointSize(10);
//                  glBegin(GL_POINTS);
//                  glColor3f(1.0f, 0.0f, 0.0f);
//                  glVertex3f(ph.x, ph.y, ph.z);
//                  glEnd();
//                  glFlush();
//                  }*/

//                 //std::cout << "# x15" << std::endl;
//                 // Setup the modeling matrix and render the objects.
//                 if (scene()->objectManager()->hasObject()) {
//                     //std::cout << "# x16" << std::endl;
//                     const int size = scene()->IDManager()->size();
//                     //std::cout << "##$*** size=" << size << std::endl;
//                     // {
//                     //int tmp = kvs::OpenGL::Integer( GL_FRAMEBUFFER_BINDING);
//                     //std::cout << "##B i=" << eye << " : framebuffer==" << tmp << std::endl;
//                     // }
//                     //std::cout << "# x17" << std::endl;
//                     for (int index = 0; index < size; index++) {
//                         kvs::IDManager::IDPair id = scene()->IDManager()->id(index);
//                         kvs::ObjectBase* object = scene()->objectManager()->object(id.first);
//                         kvs::RendererBase* renderer = scene()->rendererManager()->renderer(id.second);
//                         if (object->isShown()) {
//                             //std::cout << "##$$$$ i=" << i << " : index=" << index
//                             //        << std::endl;
//                             kvs::OpenGL::PushMatrix();

//                             // {
//                             //float x[16];
//                             //object->xform().toArray(x);
//                             //std::cout << "# x17.1 " << x[0] << ", " << x[1] << ", " << x[2] << ", " << x[3] << ", " << std::endl << x[4] << ", "
//                             //        << x[5] << ", " << x[6] << ", " << x[7] << ", " << std::endl << x[8] << ", " << x[9] << ", " << x[10] << ", "
//                             //        << x[11] << ", " << std::endl << x[12] << ", " << x[13] << ", " << x[14] << ", " << x[15] << ", "
//                             //        << std::endl;
//                             // }

//                             scene()->updateGLModelingMatrix(object);
//                             renderer->exec(object, scene()->camera(), scene()->light());
//                             kvs::OpenGL::PopMatrix();
//                         }
//                     }
//                     //std::cout << "# x18" << std::endl;
//                 } else {
//                     //std::cout << "# x19" << std::endl;
//                     scene()->updateGLModelingMatrix();
//                 }

//                 //std::cout << "# x20" << std::endl;
//                 {
//                     m_hmd.eyeRenderTexture(eye)->UnsetRenderSurface();
//                     m_hmd.eyeRenderTexture(eye)->Commit();
//                 }
//                 //std::cout << "# x21" << std::endl;
//             }

//             scene()->camera()->setPosition(camera_position, camera_lookat, camera_upvector);

//             //std::cout << "# x23" << std::endl;
//             if (m_widget_handler != nullptr) {
// #ifdef DEBUG_HUD
//             std::cerr << "Screen::paintEvent(): call m_hmd.updateHUD()" << std::endl;
// #endif // DEBUG_HUD
//                 m_hmd.updateHUD();
//             }

//             //std::cout << "# x24" << std::endl;
//             m_hmd.endFrame(frame_index);
//             //frame_index++;
//             //std::cout << "# x25" << std::endl;
//         }

//         //std::cout << "# x26" << std::endl;
//         // Render to the frame buffer.
//         kvs::OpenGL::SetViewport(current_viewport);
//         switch (m_mirror_buffer_type) {
//         case LeftEyeImage:
//             m_hmd.renderToMirrorLeftEyeImage();
//             break;
//         case RightEyeImage:
//             m_hmd.renderToMirrorRightEyeImage();
//             break;
//         case BothEyeImage:
//             m_hmd.renderToMirrorBothEyeImage();
//             break;
//         case DistortedBothEyeImage:
//             m_hmd.renderToMirrorDistortedBothEyeImage();
//             break;
//         default:
//             break;
//         }

//         //std::cout << "# x27" << std::endl;
//         // Callback the registered paint events.
//         kvs::PaintEvent event;
//         BaseClass::eventHandler()->notify(&event);

//         // Redraw.
//         //std::cerr << "# x28 : call kvs::OpenGL::Finish()" << std::endl;
//         //kvs::OpenGL::Finish();
//         //std::cerr << "# x28 : call kvs::OpenGL::Flush()" << std::endl;
//         kvs::OpenGL::Flush();

//         //std::cerr << "# x28.1 : call redraw()" << std::endl;

//         redraw();

//         //std::cerr << "# x29 : end of if(sessionStatus.IsVisible) block" << std::endl;

//         frame_index++;
//     }

//     {
//         // debug
//         ovrErrorInfo info;
//         ovr_GetLastErrorInfo(&info);
//         //std::cerr << "# 30: " << info.ErrorString << std::endl;
//     }

//     //std::cerr << "# x31 : end of paintEvent()" << std::endl;
// }

}// end of namespace jaea

} // end of namespace oculus

} // end of namespace kvs
