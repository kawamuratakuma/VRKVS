/* Lib/WidgetHandler.h */
#pragma once

#include <kvs/RGBAColor>
//#include "HeadMountedDisplay.h"
//#include "Oculus.h"
//#include "OVR.h"


namespace kvs
{
    namespace oculus
    {
        namespace jaea
        {
            class WidgetHandler
            {
            protected:
                int m_width;
                int m_height;

                kvs::RGBAColor background;

                int m_laserPoint_X;
                int m_laserPoint_Y;

            public:
                WidgetHandler ();
                WidgetHandler (int w, int h);
                virtual bool initialize();
                virtual bool destroy();
                virtual void update();

                virtual void setBackgroundColor(kvs::RGBAColor c);
                virtual kvs::RGBAColor getBackgroundColor();
                virtual int getTextureWidth();
                virtual int getTextureHeight();
                
                virtual void setLaserPoint(int x, int y);
                virtual int getLaserPointX();
                virtual int getLaserPointY();


            };
        }
    }
}

