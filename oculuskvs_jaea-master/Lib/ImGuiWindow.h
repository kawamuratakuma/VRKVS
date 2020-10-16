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
            class ImGuiWidget;

            class ImGuiWindow
            {
                private:
                std::vector<ImGuiWidget *> m_widgets;
                int focusIndex;
                bool m_visible;
                std::string m_title;
                int m_pos_x;
                int m_pos_y;
                int m_width;
                int m_height;
                bool m_focused;

                int indexOf (ImGuiWidget *widget);

                public:
                ImGuiWindow();
                ImGuiWindow(std::string title);
                ~ImGuiWindow();

                void setTitle(std::string title);
                std::string& title();

                void addWidget(ImGuiWidget *widget);
                void removeWidget (ImGuiWidget *widget);
                void removeWidget (int index);

                void setFocused(bool focus);
                bool isFocused();

                void setPosition(int x, int y);
                void setX(int x);
                void setY(int y);
                int x();
                int y();

                void setSize(int w, int h);
                void setWidth(int w);
                void setHeight(int h);
                int width();
                int height();

                void setVisible (bool visible) {m_visible = visible;}
                bool isVisible () { return m_visible;}
                void draw ();
            };
        } // namespace jaea
    } // namespace oculus
} // namespace kvs



