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
class ImGuiEventWidget;

// button click handler
class ImGuiWidgetEventHandler
{
    protected:
    virtual void handleEvent (ImGuiWidget *widget) = 0;

    friend class ImGuiEventWidget;
};

class ImGuiWidget
{
    protected:
    //bool m_focused;

    public:
    ImGuiWidget();
    virtual ~ImGuiWidget();
    virtual void draw() = 0;
};

class ImGuiEventWidget : public ImGuiWidget
{
    protected:
    std::vector<ImGuiWidgetEventHandler *> event_handler;

    public:
    void addEventHandler (ImGuiWidgetEventHandler *e);
    void removeEventHandler(ImGuiWidgetEventHandler *e);
    void invokeEvent();
};

class ImGuiIntValueWidget : public ImGuiWidget
{
    protected:
    int m_value;
    int m_min_value;
    int m_max_value;
    int m_delta;
    std::string m_label;

    public:
    ImGuiIntValueWidget();
    int value() { return m_value;}
    void setValue(int v) {m_value = v;}
    int minValue() { return m_min_value;}
    void setMinValue(int v) {m_min_value = v;}
    int maxValue() { return m_max_value;}
    void setMaxValue(int v) {m_max_value = v;}
    int delta() { return m_delta;}
    void setDelta(int v) {m_delta = v;}
    std::string label() { return m_label;}
    void setLabel(std::string v) {m_label = v;}
    void draw();
};

class ImGuiFloatValueWidget : public ImGuiWidget
{
    protected:
    float m_value;
    float m_min_value;
    float m_max_value;
    float m_delta;
    std::string m_label;

    public:
    ImGuiFloatValueWidget();
    float value() { return m_value;}
    void setValue(float v) {m_value = v;}
    float minValue() { return m_min_value;}
    void setMinValue(float v) {m_min_value = v;}
    float maxValue() { return m_max_value;}
    void setMaxValue(float v) {m_max_value = v;}
    float delta() { return m_delta;}
    void setDelta(float v) {m_delta = v;}
    std::string label() { return m_label;}
    void setLabel(std::string v) {m_label = v;}
    void draw();
};

class ImGuiFloatSliderWidget : public ImGuiWidget
{
    protected:
    float m_value;
    float m_min_value;
    float m_max_value;
    float m_delta;
    std::string m_label;

    public:
    ImGuiFloatSliderWidget();
    float value() { return m_value;}
    void setValue(float v) {m_value = v;}
    float minValue() { return m_min_value;}
    void setMinValue(float v) {m_min_value = v;}
    float maxValue() { return m_max_value;}
    void setMaxValue(float v) {m_max_value = v;}
    float delta() { return m_delta;}
    void setDelta(float v) {m_delta = v;}
    std::string label() { return m_label;}
    void setLabel(std::string v) {m_label = v;}
    void draw();
};

class ImGuiTextWidget : public ImGuiWidget
{
    private:
    std::string m_text;

    public:
    ImGuiTextWidget();
    ImGuiTextWidget(std::string text);
    void setText (std::string text);
    std::string& text ();
    void draw();
};

class ImGuiButtonWidget : public ImGuiEventWidget
{
    private:
    std::string m_text;
    // TODO : 
    //ImGuiCallback callback;

    public:
    //ImGuiButtonWidget();
    ImGuiButtonWidget(std::string text = "");
    void setText (std::string text);
    std::string& text ();
    void draw();
};

} // namespace jaea
} // namespace oculus
} // namespace kvs



