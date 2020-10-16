#include "Screen.h"
#include "InputDevice.h"
#include "ImGuiWidget.h"
#include "imgui.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_glut.h"
#include "imgui_internal.h"

#include <vector>
#include <algorithm>

void kvs::oculus::jaea::ImGuiEventWidget::addEventHandler (ImGuiWidgetEventHandler *e)
{
    auto finder = std::find(event_handler.begin(), event_handler.end(), e);
    if(finder == event_handler.end())
    {
        event_handler.push_back(e);
    }
}
void kvs::oculus::jaea::ImGuiEventWidget::removeEventHandler (ImGuiWidgetEventHandler *e)
{
    auto finder = std::find(event_handler.begin(), event_handler.end(), e);
    if(finder != event_handler.end())
    {
        event_handler.erase(finder);
    }
}
void kvs::oculus::jaea::ImGuiEventWidget::invokeEvent ()
{
    for (int i = 0; i < event_handler.size(); i++)
    {
        ImGuiWidgetEventHandler *h = event_handler[i];
        h->handleEvent (this);
    }
}


// ******************** ImGuiTextWidget ************************
kvs::oculus::jaea::ImGuiTextWidget::ImGuiTextWidget() : 
    kvs::oculus::jaea::ImGuiWidget::ImGuiWidget(), 
    m_text("")
{
}

kvs::oculus::jaea::ImGuiTextWidget::ImGuiTextWidget(std::string text) :
    kvs::oculus::jaea::ImGuiWidget::ImGuiWidget(), 
    m_text(text)
{
}
void kvs::oculus::jaea::ImGuiTextWidget::setText (std::string text)
{
    m_text = text;
}
std::string& kvs::oculus::jaea::ImGuiTextWidget::text () 
{
    return m_text;
}

void kvs::oculus::jaea::ImGuiTextWidget::draw()
{
#ifdef DEBUG_HUD
    std::cerr << "ImGuiTextWidget::draw(): text=" << m_text << std::endl;
#endif // DEBUG_HUD

    ImGui::Text(m_text.c_str());
}

// ******************** ImGuiIntValueWidget ************************
kvs::oculus::jaea::ImGuiIntValueWidget::ImGuiIntValueWidget() :
    kvs::oculus::jaea::ImGuiWidget::ImGuiWidget(), 
    m_value(0),
    m_min_value(0),
    m_max_value(100),
    m_delta(2),
    m_label("")
{
}

void kvs::oculus::jaea::ImGuiIntValueWidget::draw()
{
#ifdef DEBUG_HUD
    std::cerr << "ImGuiValueWidget::draw(): value=" << m_value << std::endl;
#endif // DEBUG_HUD

    // MEMO : 3rd parameter(step) must be 0 or negative value...
    // if step is positive, ImGui generate "-" and "+" buttons automatically,
    // and InputInt is NEVER focused because child widget (input field) is focused.
    ImGui::InputInt(m_label.c_str(), &m_value, 0, 0);

#ifdef DEBUG_HUD
    std::cerr << "ImGuiIntValueWidget::draw() : call ImGui::IsItemFocused()." << std::endl;
#endif // DEBUG_HUD

    bool focused = ImGui::IsItemFocused();

#ifdef DEBUG_HUD
    std::cerr << "ImGuiIntValueWidget::draw() : ImGui::IsItemFocused() = " << focused << std::endl;
#endif // DEBUG_HUD


    if (focused) {
        int prev_value = m_value;

#ifdef DEBUG_HUD
    std::cerr << "ImGuiIntValueWidget::draw() : call ImGui::SetItemDefaultFocus()." << std::endl;
#endif // DEBUG_HUD

        ImGui::SetItemDefaultFocus();

        ImGuiIO &io = ImGui::GetIO();
        float status_down = io.NavInputs[ImGuiNavInput_LStickDown];
        if (status_down > 0.0f) {
            m_value -= (int)(status_down * m_delta);
            if (m_value < m_min_value) {
                m_value = m_min_value;
            }
#ifdef DEBUG_HUD
            std::cerr << "FloatValueWidget : decrease value : " << prev_value << " -> " << m_value
                      << "(status_down=" << status_down << ")"
                      << std::endl;
#endif // DEBUG_HUD
        }
        float status_up = io.NavInputs[ImGuiNavInput_LStickUp];
        if (status_up > 0.0f) {
            m_value += (int)(status_up * m_delta);
            if (m_value > m_max_value) {
                m_value = m_max_value;
            }
#ifdef DEBUG_HUD
            std::cerr << "FloatValueWidget : increase value : " << prev_value << " -> " << m_value
                      << "(status_up=" << status_up << ")"
                      << std::endl;
#endif // DEBUG_HUD
        }
        
#ifdef DEBUG_HUD
        std::cerr << "IntValueWidget : update value : " << prev_value << " -> " << m_value << std::endl;
#endif // DEBUG_HUD
    }
}

// ******************** ImGuiFloatValueWidget ************************
kvs::oculus::jaea::ImGuiFloatValueWidget::ImGuiFloatValueWidget() :
    kvs::oculus::jaea::ImGuiWidget::ImGuiWidget(), 
    m_value(0.0f),
    m_min_value(-FLT_MAX),
    m_max_value(FLT_MAX),
    m_delta(0.1f),
    m_label("")
{
}

void kvs::oculus::jaea::ImGuiFloatValueWidget::draw()
{
#ifdef DEBUG_HUD
    std::cerr << "ImGuiValueWidget::draw(): value=" << m_value << std::endl;
#endif // DEBUG_HUD

    ImGui::InputFloat(m_label.c_str(), &m_value);

#ifdef DEBUG_HUD
    std::cerr << "ImGuiFloatValueWidget::draw() : call ImGui::IsItemFocused()." << std::endl;
#endif // DEBUG_HUD

    bool focused = ImGui::IsItemFocused();

#ifdef DEBUG_HUD
    std::cerr << "ImGuiFloatValueWidget::draw() : ImGui::IsItemFocused() = " << focused << std::endl;
#endif //  DEBUG_HUD

    if (focused) {
        float prev_value = m_value;

#ifdef DEBUG_HUD
    std::cerr << "ImGuiFloatValueWidget::draw() : call ImGui::SetItemDefaultFocus()." << std::endl;
#endif // DEBUG_HUD

        ImGui::SetItemDefaultFocus();

        ImGuiIO &io = ImGui::GetIO();
        if (io.NavInputs[ImGuiNavInput_LStickDown] > 0.0f) {
            m_value -= io.NavInputs[ImGuiNavInput_LStickDown] * m_delta;
            if (m_value < m_min_value) {
                m_value = m_min_value;
            }
            //std::cerr << "FloatValueWidget : decrease value : " << prev_value << " -> " << m_value << std::endl;
        }
        if (io.NavInputs[ImGuiNavInput_LStickUp] > 0.0f) {
            m_value += io.NavInputs[ImGuiNavInput_LStickUp] * m_delta;
            if (m_value > m_max_value) {
                m_value = m_max_value;
            }
            //std::cerr << "FloatValueWidget : increase value : " << prev_value << " -> " << m_value << std::endl;
        }
#ifdef DEBUG_HUD
        std::cerr << "FloatValueWidget : update value : " << prev_value << " -> " << m_value << std::endl;
#endif // DEBUG_HUD
    }
}

// ******************** ImGuiFloatSliderWidget ************************
kvs::oculus::jaea::ImGuiFloatSliderWidget::ImGuiFloatSliderWidget() :
    kvs::oculus::jaea::ImGuiWidget::ImGuiWidget(), 
    m_value(0.0f),
    m_min_value(0.0f),
    m_max_value(1.0f),
    m_delta(0.01f),
    m_label("")
{
}

void kvs::oculus::jaea::ImGuiFloatSliderWidget::draw()
{
#ifdef DEBUG_HUD
    std::cerr << "ImGuiValueWidget::draw(): value=" << m_value << std::endl;
#endif // DEBUG_HUD

    ImGui::SliderFloat(m_label.c_str(), &m_value, m_min_value, m_max_value, "%.5f", 1.0f);
    //ImGui::SliderFloat(m_label.c_str(), &m_value, m_min_value, m_max_value, "%.5f", m_delta);

#ifdef DEBUG_HUD
    std::cerr << "ImGuiFloatSliderWidget::draw() : call ImGui::IsItemFocused()." << std::endl;
#endif // DEBUG_HUD

    bool focused = ImGui::IsItemFocused();

#ifdef DEBUG_HUD
    std::cerr << "ImGuiFloatSliderWidget::draw() : ImGui::IsItemFocused() = " << focused << std::endl;
#endif // DEBUG_HUD

    if (focused) {

#ifdef DEBUG_HUD
    std::cerr << "ImGuiFloatSliderWidget::draw() : call ImGui::SetItemDefaultFocus() = " << focused << std::endl;
#endif // DEBUG_HUD

        ImGui::SetItemDefaultFocus();

        float prev_value = m_value;

        ImGuiIO &io = ImGui::GetIO();
        if (io.NavInputs[ImGuiNavInput_LStickLeft] > 0.0f) {
            m_value -= io.NavInputs[ImGuiNavInput_LStickLeft] * m_delta;
            if (m_value < m_min_value) {
                m_value = m_min_value;
            }
            //std::cerr << "FloatSliderWidget : decrease value : " << prev_value << " -> " << m_value << std::endl;
        }
        if (io.NavInputs[ImGuiNavInput_LStickRight] > 0.0f) {
            m_value += io.NavInputs[ImGuiNavInput_LStickRight] * m_delta;
            if (m_value > m_max_value) {
                m_value = m_max_value;
            }
            //std::cerr << "FloatSliderWidget : increase value : " << prev_value << " -> " << m_value << std::endl;
        }
#ifdef DEBUG_HUD
        std::cerr << "FloatSliderWidget : update value : " << prev_value << " -> " << m_value << std::endl;
#endif // DEBUG_HUD
    }
}

// ************************ ImGuiButtonWidget *****************************
kvs::oculus::jaea::ImGuiButtonWidget::ImGuiButtonWidget(std::string text) : 
    kvs::oculus::jaea::ImGuiEventWidget(), 
    m_text(text)
{
}
void kvs::oculus::jaea::ImGuiButtonWidget::setText (std::string text)
{
    m_text = text;
}
std::string& kvs::oculus::jaea::ImGuiButtonWidget::text () 
{
    return m_text;
}


void kvs::oculus::jaea::ImGuiButtonWidget::draw()
{
#ifdef DEBUG_HUD
    std::cerr << "ImGuiButtonWidget::draw(): text=" << m_text << std::endl;
#endif // DEBUG_HUD

    bool clicked = ImGui::Button(m_text.c_str());
    if (clicked)
    {
        this->invokeEvent();
    }
}

kvs::oculus::jaea::ImGuiWidget::ImGuiWidget()
{}
kvs::oculus::jaea::ImGuiWidget::~ImGuiWidget()
{}
