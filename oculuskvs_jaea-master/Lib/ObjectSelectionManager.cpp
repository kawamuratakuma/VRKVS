/*
 * ObjectSelectionManager.cpp
 *
 *  Created on: 2019/04/26
 *      Author: nakamura
 */

#include <vector>
#include <kvs/Camera>
#include <kvs/IDManager>
#include <kvs/ObjectBase> 
#include <OVR_CAPI.h>
#include "ObjectSelectionManager.h"

namespace kvs {
namespace oculus {
namespace jaea {

ObjectSelectionManager::ObjectSelectionManager() :
    m_draw_beams(true),
    m_beam_length(1000.0f),
    m_selected_index(-1)
 {
}

ObjectSelectionManager::~ObjectSelectionManager() {
}

int ObjectSelectionManager::select (const Scene *scene) {
    ovrInputState input_state = m_hmd.inputState(ovrControllerType_Touch);
    ovrTrackingState tracking_state = m_hmd.trackingState();

    ovrPoseStatef handPoseState[] = {tracking_state.HandPoses[ovrHand_Left], tracking_state.HandPoses[ovrHand_Right]};
    ovrPosef handPose[] = {handPoseState[ovrHand_Left].ThePose, handPoseState[ovrHand_Right].ThePose};
    ovrVector3f handPosition[] = {handPose[ovrHand_Left].Position, handPose[ovrHand_Right].Position};
    ovrQuatf handQuat[] = {handPose[ovrHand_Left].Orientation, handPose[ovrHand_Right].Orientation};

    OVR::Vector3f v = OVR::Vector3f(0.0f, 0.0f, -1.0f);
    OVR::Matrix4f matHandQuat[] = {
        OVR::Matrix4f(handQuat[ovrHand_Left]),
        OVR::Matrix4f(handQuat[ovrHand_Right])
    };
    OVR::Vector3f dir[] = {
        matHandQuat[ovrHand_Left].Transform(v) * m_beam_length,
        matHandQuat[ovrHand_Right].Transform(v) * m_beam_length
    };

    if (m_draw_beams) {
        drawHandBeams(handPosition, dir);
    }

    // get left index trigger status
    float triggerL_curr = input_state.IndexTrigger[ovrHand_Left];
    float triggerL_prev= m_prev_trigger_status[ovrHand_Left];
    bool triggerL_released = (triggerL_prev > 0.5f && triggerL_curr < 0.5f);
    if (triggerL_released) {
        m_selected_index = findCrossingObjects(scene, handPosition[ovrHand_Left], dir[ovrHand_Left], m_selected_index);
    }

    // cancel selected_index
    float triggerR_curr = input_state.IndexTrigger[ovrHand_Right];
    float triggerR_prev= m_prev_trigger_status[ovrHand_Right];
    bool triggerR_released = (triggerR_prev > 0.5f && triggerR_curr < 0.5f);
    if (triggerL_released) {
        clearSelection();
    }

    m_prev_trigger_status[ovrHand_Left] = triggerL_curr;
    m_prev_trigger_status[ovrHand_Right] = triggerR_curr;

    return m_selected_index;
}

int ObjectSelectionManager::findCrossingObjects(const Scene *scene, ovrVector3f handPosition, OVR::Vector3f handDir, int prev_selected_kindex) {
    // TODO : implement me!
    return -1;
}

void ObjectSelectionManager::drawHandBeams (ovrVector3f pos[], OVR::Vector3f dir[]) {
    float pLX0 = pos[ovrHand_Left].x;
    float pLY0 = pos[ovrHand_Left].y;
    float pLZ0 = pos[ovrHand_Left].z;
    float pRX0 = pos[ovrHand_Right].x;
    float pRY0 = pos[ovrHand_Right].y;
    float pRZ0 = pos[ovrHand_Right].z;

    float pLX1 = pLX0 + dir[ovrHand_Left].x;
    float pLY1 = pLY0 + dir[ovrHand_Left].y;
    float pLZ1 = pLZ0 + dir[ovrHand_Left].z;
    float pRX1 = pRX0 + dir[ovrHand_Right].x;
    float pRY1 = pRY0 + dir[ovrHand_Right].y;
    float pRZ1 = pRZ0 + dir[ovrHand_Right].z;

    glEnable(GL_DEPTH_TEST);

    glPointSize(20);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(pLX0, pLY0, pLZ0);
    glColor3f(1.0f, 0.8f, 0.8f);
    glVertex3f(pLX1, pLY1, pLZ1);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(pRX0, pRY0, pRZ0);
    glColor3f(0.8f, 0.8f, 1.0f);
    glVertex3f(pRX1, pRY1, pRZ1);
    glColor3f(0.0f, 1.0f, 0.0f);
    //glVertex3f(lookat.x, lookat.y, lookat.z);
    //glVertex3f(lx, ly, lz);
    glEnd();

    glLineWidth(5);
    glBegin(GL_LINES);
    glColor4f(1.0f, 0.5f, 0.0f, 1.0f);
    glVertex3f(pLX0, pLY0, pLZ0);
    glColor4f(1.0f, 0.8f, 0.8f, 1.0f);
    glVertex3f(pLX1, pLY1, pLZ1);
    glColor4f(1.0f, 0.5f, 0.0f, 1.0f);
    glVertex3f(pRX0, pRY0, pRZ0);
    glColor4f(1.0f, 0.8f, 0.8f, 1.0f);
    glVertex3f(pRX1, pRY1, pRZ1);
    glEnd();
}

} /* namespace jaea */
} /* namespace oculus */
} /* namespace kvs */
