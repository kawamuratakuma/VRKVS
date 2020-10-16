#pragma once
#ifndef LIB_OBJECTSELECTIONMANAGER_H_
#define LIB_OBJECTSELECTIONMANAGER_H_

#include <kvs/Scene>
#include "HeadMountedDisplay.h"

namespace kvs {
namespace oculus {
namespace jaea {

class ObjectSelectionManager {
private:
    HeadMountedDisplay m_hmd;

    bool m_draw_beams;
    int m_selected_index;
    float m_beam_length;
    float m_prev_trigger_status[2];

    //vector<int> m_selected_indices;

public:
    virtual ~ObjectSelectionManager();
    ObjectSelectionManager();

    // parameters for beam drawing
    void setDrawsBeams(bool draw_beams) {m_draw_beams = draw_beams;}
    bool drawsBeams() {return m_draw_beams;}
    void setBeamLength (float length) {m_beam_length = length;}
    float beamLength () {return m_beam_length;}

    void setHeadMountDisplay(HeadMountedDisplay hmd) {
        m_hmd = hmd;
    }

    int select (const Scene *scene);
    int selectedIndex () {return m_selected_index;}
    void clearSelection(void) {m_selected_index = -1;}

private:
    int findCrossingObjects(const Scene *scene, ovrVector3f handPosition, OVR::Vector3f handDir, int prev_selected_kindex);
    void drawHandBeams (ovrVector3f pos[], OVR::Vector3f dir[]);
};

} /* namespace jaea */
} /* namespace oculus */
} /* namespace kvs */

#endif /* LIB_OBJECTSELECTIONMANAGER_H_ */
