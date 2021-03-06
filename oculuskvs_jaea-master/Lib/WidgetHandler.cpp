#include "WidgetHandler.h"

#include <kvs/RGBAColor>

kvs::oculus::jaea::WidgetHandler::WidgetHandler () : m_width(512), m_height(512), background(0,0,0,0)
{

}

kvs::oculus::jaea::WidgetHandler::WidgetHandler (int w, int h) : background(0,0,0,0)
{
    m_width = w;
    m_height = h;
}

void kvs::oculus::jaea::WidgetHandler::setLaserPoint(int x, int y)
{
    m_laserPoint_X = x;
    m_laserPoint_Y = y;
}
int kvs::oculus::jaea::WidgetHandler::getLaserPointX()
{
    return m_laserPoint_X;
}
int kvs::oculus::jaea::WidgetHandler::getLaserPointY()
{
    return m_laserPoint_Y;
}

void kvs::oculus::jaea::WidgetHandler::setBackgroundColor(kvs::RGBAColor c)
{
    background = c;
}
kvs::RGBAColor kvs::oculus::jaea::WidgetHandler::getBackgroundColor()
{
    return background;
}
int kvs::oculus::jaea::WidgetHandler::getTextureWidth()
{
    return m_width;
}
int kvs::oculus::jaea::WidgetHandler::getTextureHeight()
{
    return m_height;
}

bool kvs::oculus::jaea::WidgetHandler::initialize() {return true;}
bool kvs::oculus::jaea::WidgetHandler::destroy() {return true;}

void kvs::oculus::jaea::WidgetHandler::update() {}

