/*****************************************************************************/
/**
 *  @file   main.cpp
 *  @brief  Example program for kvs::ArrowGlyph with HUD.
 *  @author Insight, Inc.
 *
 * This code is based on KVS.ovulus/App/ArrowGlyph/main.cpp.
 */
#include <kvs/Message>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/ArrowGlyph>
#include <kvs/TornadoVolumeData>
#include <Application.h>
#include <Screen.h>
#include <TouchController.h>
#include "ImGuiWidgetHandler.h"
#include "ImGuiWindow.h"
#include "ImGuiWidget.h"

#include <sstream>

// #include <GL/GL.h>
// #include "imgui.h"
// #include "imgui_internal.h"
// #include "imgui_impl_opengl2.h"
// #include "imgui_impl_glut.h"

class Button00Handler : public kvs::oculus::jaea::ImGuiWidgetEventHandler
{
    protected:
    kvs::oculus::jaea::ImGuiTextWidget *m_text;
    int click_count;

    void handleEvent(kvs::oculus::jaea::ImGuiWidget *w)
    {
        click_count++;
        std::cerr << "Button0-0 clicked! (" << click_count << ")" << std::endl;

        std::stringstream sstrm;
        sstrm << "Button0-0 clicked! (" << click_count << ")";
        m_text->setText(sstrm.str());
    }

    public:
    Button00Handler(kvs::oculus::jaea::ImGuiTextWidget *text)
     : click_count(0), m_text(text) {}
};

class Button10Handler : public kvs::oculus::jaea::ImGuiWidgetEventHandler
{
    protected:
    void handleEvent(kvs::oculus::jaea::ImGuiWidget *w)
    {
        std::cerr << "Button1-0 clicked!" << std::endl;
    }
};

class Button11Handler : public kvs::oculus::jaea::ImGuiWidgetEventHandler
{
    protected:
    void handleEvent(kvs::oculus::jaea::ImGuiWidget *w)
    {
        std::cerr << "Button1-1 clicked!" << std::endl;
    }
};

kvs::oculus::jaea::ImGuiWidgetHandler createWidgetHandler(kvs::oculus::jaea::Screen *screen)
{
    kvs::oculus::jaea::ImGuiTextWidget *text0 = new kvs::oculus::jaea::ImGuiTextWidget("[0] Text Widget!");
    kvs::oculus::jaea::ImGuiButtonWidget *button0 = new kvs::oculus::jaea::ImGuiButtonWidget("[0] Button!");
    button0->addEventHandler(new Button00Handler(text0));
    kvs::oculus::jaea::ImGuiButtonWidget *button0a = new kvs::oculus::jaea::ImGuiButtonWidget("[0a] Button!");
    kvs::oculus::jaea::ImGuiFloatValueWidget *floatWidget0 = new kvs::oculus::jaea::ImGuiFloatValueWidget();
    floatWidget0->setValue(0.1);
    kvs::oculus::jaea::ImGuiFloatSliderWidget *floatWidget1 = new kvs::oculus::jaea::ImGuiFloatSliderWidget();
    floatWidget1->setValue(10.1);
    floatWidget1->setMinValue(0.0);
    floatWidget1->setMinValue(100.0);
    kvs::oculus::jaea::ImGuiIntValueWidget *intWidget0 = new kvs::oculus::jaea::ImGuiIntValueWidget();
    intWidget0->setValue(10);

    kvs::oculus::jaea::ImGuiWindow *window0 = new kvs::oculus::jaea::ImGuiWindow("HUD[0]");
    window0->setSize(200, 200);
    window0->addWidget(floatWidget0);
    window0->addWidget(button0);
    window0->addWidget(floatWidget1);
    window0->addWidget(intWidget0);
    window0->addWidget(button0a);
    window0->addWidget(text0);

    kvs::oculus::jaea::ImGuiTextWidget *text1 = new kvs::oculus::jaea::ImGuiTextWidget("[1] Text Widget!");
    kvs::oculus::jaea::ImGuiButtonWidget *button10 = new kvs::oculus::jaea::ImGuiButtonWidget("[1-0] Button!");
    button10->addEventHandler(new Button10Handler());
    kvs::oculus::jaea::ImGuiButtonWidget *button11 = new kvs::oculus::jaea::ImGuiButtonWidget("[1-1] Button!!!");
    button11->addEventHandler(new Button11Handler());

    kvs::oculus::jaea::ImGuiWindow *window1 = new kvs::oculus::jaea::ImGuiWindow("HUD[1]");
    window1->setSize(200, 100);
    window1->setPosition(300, 100);
    window1->addWidget(text1);
    window1->addWidget(button10);
    window1->addWidget(button11);

    kvs::oculus::jaea::ImGuiWidgetHandler wh(screen, 512, 512);
    wh.addWindow(window0);
    wh.addWindow(window1);

    return wh;
}

/*===========================================================================*/
/**
 *  @brief  Main function.
 *  @param  argc [i] argument counter
 *  @param  argv [i] argument values
 */
/*===========================================================================*/
int main( int argc, char** argv )
{
    kvs::oculus::jaea::Application app( argc, argv );

    kvsMessageDebug("HUDTest01!!");

    /* Read volume data from the specified data file. If the data file is not
     * specified, tornado volume data is created by using kvs::TornadoVolumeData class.
     */
    kvs::StructuredVolumeObject* object = NULL;
    if ( argc > 1 ) object = new kvs::StructuredVolumeImporter( std::string( argv[1] ) );
    else            object = new kvs::TornadoVolumeData( kvs::Vector3ui( 8, 8, 8 ) );

    if ( !object )
    {
        kvsMessageError( "Cannot create a structured volume object." );
        return false;
    }

    // Create an arrow glyph renderer.
    kvs::ArrowGlyph* glyph = new kvs::ArrowGlyph();
    if ( !glyph )
    {
        kvsMessageError( "Cannot creat an arrow glyph.");
        return false;
    }

    // Set properties.
    const kvs::TransferFunction transfer_function( 256 );
    const kvs::ArrowGlyph::ArrowType type = kvs::ArrowGlyph::TubeArrow;
    glyph->setTransferFunction( transfer_function );
    glyph->setType( type );

    kvs::oculus::jaea::Screen screen( &app );
    screen.registerObject( object, glyph );
    screen.setTitle( "kvs::ArrowGlyph" );
    screen.setEnabledManipulator(kvs::oculus::jaea::Screen::UseRightManipulator);

    kvs::oculus::jaea::ImGuiWidgetHandler ghandler = createWidgetHandler(&screen);

    kvs::oculus::jaea::TouchController event( &screen );
    // event.setRotationFactor( 500.0f );
    // event.setTranslationFactor( 500.0f );
    // event.setScalingFactor( 3000.0f );
    event.setRotationFactor( 5000.0f );
    event.setTranslationFactor( 5000.0f );
    event.setScalingFactor( 30000.0f );

    screen.show(false);

    return app.run();
}
