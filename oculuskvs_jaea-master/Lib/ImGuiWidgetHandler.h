/* ImGuiWidgetHandler.h */
#pragma once

#include "Screen.h"
#include "InputDevice.h"
#include "WidgetHandler.h"

#include <vector>

namespace kvs
{
    namespace oculus
    {
        namespace jaea
        {
            class ImGuiWindow;

            class ImGuiWidgetHandler : public WidgetHandler
            {
                private:
                InputDevice m_input_device;
                kvs::oculus::jaea::Screen *m_screen;

                std::vector<ImGuiWindow *> m_windows;

                float m_prev_trigger_state_left;
                float m_prev_trigger_state_right;

                int prev_buttons;
                ovrVector2f prev_dpad;
                ovrVector2f prev_inputStick;

                int focusIndex;
                int indexOf(ImGuiWindow *window);

                public:
                ImGuiWidgetHandler(Screen *screen, int w = 512, int h = 512);
                bool initialize();
                bool destroy();
                virtual void update();

                void addWindow (ImGuiWindow *window);
                void removeWindow(ImGuiWindow *window);
                void removeWindow(int index);

                int getWindowFocusIndex ();
                void setWindowFocus (ImGuiWindow *window);
                void setWindowFocus (int index);
                void setNextWindowFocus ();
                void setPrevWindowFocus ();
            };
        } // namespace jaea
    } // namespace oculus
} // namespace kvs



