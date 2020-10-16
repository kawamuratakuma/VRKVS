#include "Screen.h"
#include "InputDevice.h"
//#include "ImGuiWidgetHandler.h"
#include "ImGuiWindow.h"
#include "ImGuiWidget.h"
#include "imgui.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_glut.h"
#include "imgui_internal.h"

#include <vector>
#include <algorithm>

kvs::oculus::jaea::ImGuiWindow::ImGuiWindow() : 
    focusIndex(0),
    m_visible(true),
    m_title(""),
    m_pos_x(0),
    m_pos_y(0),
    m_width(-1),
    m_height(-1)
{
}
kvs::oculus::jaea::ImGuiWindow::ImGuiWindow(std::string title) : 
    focusIndex(0),
    m_visible(true),
    m_title(title),
    m_pos_x(0),
    m_pos_y(0),
    m_width(-1),
    m_height(-1)
{
}

kvs::oculus::jaea::ImGuiWindow::~ImGuiWindow()
{
}

void kvs::oculus::jaea::ImGuiWindow::setTitle (std::string title)
{
    m_title = title;
}
std::string& kvs::oculus::jaea::ImGuiWindow::title () 
{
    return m_title;
}

int kvs::oculus::jaea::ImGuiWindow::indexOf (ImGuiWidget *widget)
{
    if (widget == nullptr)
    {
        return -1;
    }
    for (size_t i = 0; i < m_widgets.size(); i++)
    {
        if (m_widgets[i] == widget)
        {
            return (int)i;
        }
    }
    return -1;
}

void kvs::oculus::jaea::ImGuiWindow::addWidget(ImGuiWidget *widget)
{
    if(widget == nullptr)
    {
        return;
    }
    if (indexOf(widget) < 0)
    {
        // widget is not found in m_widgets
        m_widgets.push_back(widget);
    }
}
void kvs::oculus::jaea::ImGuiWindow::removeWidget (ImGuiWidget *widget)
{
    removeWidget(indexOf(widget));
}
void kvs::oculus::jaea::ImGuiWindow::removeWidget (int index)
{
    if (index < 0 || m_widgets.size() <= (size_t)index)
    {
        return;
    }

    m_widgets.erase(m_widgets.begin()+index);
}
void kvs::oculus::jaea::ImGuiWindow::draw ()
{
#ifdef DEBUG_HUD
    std::cerr << "ImGuiWindow::draw(): title=" << m_title << std::endl;
#endif // DEBUG_HUD

    if(!m_visible)
    {
#ifdef DEBUG_HUD
        std::cerr << "ImGuiWindow(" << m_title << ")::draw(): m_visible=false" << std::endl;
#endif // DEBUG_HUD

        return;
    }

    ImGui::SetNextWindowPos(ImVec2(m_pos_x, m_pos_y), ImGuiSetCond_Once);

#ifdef DEBUG_HUD
    std::cerr << "ImGuiWindow(" << m_title << ")::draw(): pos=(" << m_pos_x << ", " << m_pos_y << ")" << std::endl;
#endif // DEBUG_HUD

    if(m_width > 0 && m_height > 0)
    {
        ImGui::SetNextWindowSize(ImVec2(m_width, m_height), ImGuiSetCond_Once);

#ifdef DEBUG_HUD
        std::cerr << "ImGuiWindow(" << m_title << ")::draw(): size=(" << m_width << ", " << m_height << ")" << std::endl;
#endif // DEBUG_HUD

    }

    std::string title = m_title;
    if(m_focused)
    {
        ImGui::SetNextWindowFocus();
        title = "** " + m_title + " **";
    }
    ImGui::Begin(title.c_str());
    
#ifdef DEBUG_HUD
    std::cerr << "ImGuiWindow::draw(): draw window(" << m_title << ")" << std::endl;
#endif // DEBUG_HUD

#ifdef DEBUG_HUD
    std::cerr << "ImGuiWindow::draw(): ImGui::Begin();" << std::endl;
#endif // DEBUG_HUD

    for (int i = 0; i < m_widgets.size(); i++)
    {
        ImGuiWidget *widget = m_widgets[i];

        // Push/Pop item id to avoid to publish duplicated item id.
        ImGui::PushID(widget);
        widget->draw();
        ImGui::PopID();
    }
    ImGui::End();

#ifdef DEBUG_HUD
    std::cerr << "ImGuiWindow::draw(): ImGui::End();" << std::endl;
#endif // DEBUG_HUD
}

void kvs::oculus::jaea::ImGuiWindow::setPosition(int x, int y)
{
    setX(x);
    setY(y);
}
void kvs::oculus::jaea::ImGuiWindow::setX(int x)
{
    m_pos_x = x;
}
void kvs::oculus::jaea::ImGuiWindow::setY(int y)
{
    m_pos_y = y;
}
int kvs::oculus::jaea::ImGuiWindow::x()
{
    return m_pos_x;
}
int kvs::oculus::jaea::ImGuiWindow::y()
{
    return m_pos_y;
}

void kvs::oculus::jaea::ImGuiWindow::setSize(int w, int h)
{
    setWidth(w);
    setHeight(h);
}
void kvs::oculus::jaea::ImGuiWindow::setWidth(int w)
{
    m_width = w;
}
void kvs::oculus::jaea::ImGuiWindow::setHeight(int h)
{
    m_height = h;
}
int kvs::oculus::jaea::ImGuiWindow::width()
{
    return m_width;
}
int kvs::oculus::jaea::ImGuiWindow::height()
{
    return m_height;
}
void kvs::oculus::jaea::ImGuiWindow::setFocused(bool focused)
{
    m_focused = focused;
}
bool kvs::oculus::jaea::ImGuiWindow::isFocused()
{
    return m_focused;
}



