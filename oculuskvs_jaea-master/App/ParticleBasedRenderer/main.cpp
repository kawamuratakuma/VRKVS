/*****************************************************************************/
/**
 *  @file   main.cpp
 *  @author Naohisa Sakamoto
 *  @brief  Example program for kvs::ParticleBasedRenderer class.
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id$
 */
/*****************************************************************************/
//#include <kvs/glut/Application>
//#include <kvs/glut/Screen>
#include <kvs/HydrogenVolumeData>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/PointObject>
#include <kvs/CellByCellMetropolisSampling>
#include <kvs/ParticleBasedRenderer>
#include <kvs/TransferFunction>
#include <kvs/Vector3>

#include <Application.h>
#include <Screen.h>
#include <TouchController.h>

#include <iostream>
#include <string>


/*===========================================================================*/
/**
 *  @brief  Import structured volume object
 *  @param  argc [i] argument count
 *  @param  argv [i] argument values
 *  @return pointer to the imported structured volume object
 */
/*===========================================================================*/
kvs::StructuredVolumeObject* Import( int argc, char** argv )
{
    kvs::StructuredVolumeObject* object = NULL;
    if ( argc > 1 )
    {
        std::string filename( argv[1] );
        object = new kvs::StructuredVolumeImporter( argv[1] );
    }
    else
    {
        kvs::Vector3ui resolution( 32, 32, 32 );
        object = new kvs::HydrogenVolumeData( resolution );
    }

    return object;
}

/*===========================================================================*/
/**
 *  @brief  Main function.
 *  @param  argc [i] argument count
 *  @param  argv [i] argument values
 */
/*===========================================================================*/
int main( int argc, char** argv )
{
    kvs::oculus::jaea::Application app( argc, argv );

    kvs::StructuredVolumeObject* volume = Import( argc, argv );
    volume->print( std::cout );

    /* The image quality can be improved by increasing 'repetitions'. However,
     * the number of generated points will be increased depending on the
     * number of repetitions. Therefore, the 'repetitions' will be specified
     * appropriately by taking into account memory resources of CPU and GPU.
     */
    const size_t repetitions = 4;
    const float step = 0.5f;
    const kvs::TransferFunction tfunc( 256 );
    kvs::PointObject* object = new kvs::CellByCellMetropolisSampling( volume, repetitions, step, tfunc );
    //object->translate(kvs::Vec3(0,0,-10));
    //object->translate(kvs::Vec3(0,10,-20));
    object->print( std::cout << std::endl );
    delete volume;

    /* User can use following particle based renderers.
     *     (1) CPU renderer -> kvs::ParticleBasedRenderer
     *     (2) GPU renderer -> kvs::glsl::ParticleBasedRenderer
     *     (3) GPU renderer -> kvs::glsl::rits::ParticleBasedRenderer (Ritsumeikan Univ. version)
     */
    kvs::glsl::ParticleBasedRenderer* renderer = new kvs::glsl::ParticleBasedRenderer();
    renderer->setRepetitionLevel( repetitions );
    renderer->enableLODControl();

    kvs::oculus::jaea::Screen screen( &app );
    screen.setTitle("kvs::ParticleBasedRenderer");
    screen.registerObject( object, renderer );

    kvs::oculus::jaea::TouchController event( &screen );
    event.setRotationFactor( 5000.0f );
    event.setTranslationFactor( 5000.0f );
    event.setScalingFactor( 30000.0f );

    screen.show(false);

    return app.run();
}
