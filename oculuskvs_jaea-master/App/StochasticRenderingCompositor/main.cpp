/*****************************************************************************/
/**
 *  @file   main.cpp
 *  @author Naohisa Sakamoto
 *  @brief  Example program for kvs::StochasticTetrahedraRenderer class.
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
#include <kvs/glut/TransferFunctionEditor>
#include <kvs/glut/Slider>
#include <kvs/glut/CheckBox>

#include <kvs/PolygonObject>
#include <kvs/PointObject>
#include <kvs/HydrogenVolumeData>
#include <kvs/StructuredVolumeObject>

#include <kvs/ExternalFaces>
#include <kvs/OrthoSlice>
#include <kvs/CellByCellMetropolisSampling>
#include <kvs/TransferFunction>

#include <kvs/UnstructuredVolumeImporter>

#include <kvs/StochasticPolygonRenderer>
#include <kvs/StochasticTetrahedraRenderer>
#include <kvs/ParticleBasedRenderer>
#include <kvs/StochasticRenderingCompositor>

#include <kvs/Scene>
#include <kvs/Vector3>
#include <kvs/ObjectManager>
#include <kvs/RendererManager>

#include <Application.h>
#include <Screen.h>
#include <TouchController.h>

#include <iostream>

#if 0
/*===========================================================================*/
/**
 *  @brief  Import unstructured volume object
 *  @param  argc [i] argument count
 *  @param  argv [i] argument values
 *  @return pointer to the imported structured volume object
 */
/*===========================================================================*/
kvs::UnstructuredVolumeObject* CreateCube( kvs::Vector3ui div, kvs::Vector3f size, float radius )
{
    kvs::Vector3f center = size / 2.0f;

    kvs::UnstructuredVolumeObject* obj = new kvs::UnstructuredVolumeObject();
    obj->setLabel("Cube");
    obj->setCellTypeToTetrahedra();

    std::vector<float> crds;
    std::vector<float> vals;
    for (int x = 0; x <= div.x(); x++) {
        float fx = ((float)x/(float)div.x()) * size.x();
        for (int y = 0; y <= div.y(); y++) {
            float fy = ((float)y/(float)div.y()) * size.y();
            for (int z = 0; z <= div.z(); z++) {
                float fz = ((float)z/(float)div.z()) * size.z();

                crds.push_back(fx);
                crds.push_back(fy);
                crds.push_back(fz);

                float distance = 1.0f - ((center - kvs::Vector3f(fx,fy,fz)).length())/radius;
                vals.push_back(distance);
            }
        }
    }
    obj->setVeclen(1);
    obj->setValues(kvs::VolumeObjectBase::Values(kvs::ValueArray<float>(vals)));
    obj->updateMinMaxValues();

    obj->setNumberOfNodes(crds.size());
    obj->setCoords(kvs::VolumeObjectBase::Coords(crds));
    obj->updateMinMaxCoords();

    int index_hex2tet[6][4] = {
        {0, 6, 1, 5},
        {0, 6, 2, 1},
        {0, 6, 3, 2},
        {0, 6, 7, 3},
        {0, 6, 4, 7},
        {0, 6, 5, 4}
    };

    std::vector<kvs::UInt32> cons;
    int dx = div.y() * div.z();
    int dy = div.z();
    int dz = 1;
    for (int x0 = 0; x0 < div.x(); x0++) {
        int x1 = x0 + 1;
        for (int y0 = 0; y0 < div.y(); y0++) {
            int y1 = y0 + 1;
            for (int z0 = 0; z0 < div.z(); z0++) {
                int z1 = z0 + 1;

                int hexa[] = {
                    dx*x0 + dy*y0 + dz*z0, // [0] x0, y0, z0
                    dx*x1 + dy*y0 + dz*z0, // [1] x1, y0, z0
                    dx*x1 + dy*y1 + dz*z0, // [2] x1, y1, z0
                    dx*x0 + dy*y1 + dz*z0, // [3] x0, y1, z0
                    dx*x0 + dy*y0 + dz*z1, // [4] x0, y0, z1
                    dx*x1 + dy*y0 + dz*z1, // [5] x1, y0, z1
                    dx*x1 + dy*y1 + dz*z1, // [6] x1, y1, z1
                    dx*x0 + dy*y1 + dz*z1, // [7] x0, y1, z1
                };
                for (int itet = 0; itet < 6; itet++) {
                    for (int ivert = 0; ivert < 4; ivert++) {
                        cons.push_back(hexa[index_hex2tet[itet][ivert]]);
                    }
                }
            }
        }
    }
    obj->setNumberOfCells(div.x() * div.y() * div.z() * 6);
    obj->setConnections(kvs::UnstructuredVolumeObject::Connections(cons));

    return obj;
}

kvs::UnstructuredVolumeObject* Import( int argc, char** argv )
{
    kvs::UnstructuredVolumeObject* object = NULL;
    if ( argc > 1 )
    {
        std::string filename( argv[1] );
        object = new kvs::UnstructuredVolumeImporter( filename );
    }
    else
    {
        kvs::Vector3ui division( 32, 32, 32 );
        kvs::Vector3f resolution( 32.0f, 32.0f, 32.0f );
        float radius = 20.0f;
        object = CreateCube(division, resolution, radius);
    }

    return object;
}
#endif

/*===========================================================================*/
/**
 *  @brief  Transfer function editor.
 */
/*===========================================================================*/
class TransferFunctionEditor : public kvs::glut::TransferFunctionEditor
{
public:

    TransferFunctionEditor( kvs::glut::Screen* screen ):
        kvs::glut::TransferFunctionEditor( screen ){}

    void apply()
    {
        typedef kvs::StochasticTetrahedraRenderer Renderer;
        kvs::Scene* scene = static_cast<kvs::glut::Screen*>( screen() )->scene();
        Renderer* renderer = static_cast<Renderer*>( scene->rendererManager()->renderer( "Renderer" ) );
        renderer->setTransferFunction( transferFunction() );
        screen()->redraw();
    }
};

/*===========================================================================*/
/**
 *  @brief  LOD check box.
 */
/*===========================================================================*/
class LODCheckBox : public kvs::glut::CheckBox
{
    kvs::StochasticRenderingCompositor* m_compositor;

public:

    LODCheckBox( kvs::glut::Screen* screen, kvs::StochasticRenderingCompositor* compositor ):
        kvs::glut::CheckBox( screen ),
        m_compositor( compositor )
    {
        setMargin( 10 );
        setCaption( "Level-of-Detail" );
    }

    void stateChanged()
    {
        m_compositor->setEnabledLODControl( state() );
        screen()->redraw();
    }
};

/*===========================================================================*/
/**
 *  @brief  Opacity slider.
 */
/*===========================================================================*/
class OpacitySlider : public kvs::glut::Slider
{
public:

    OpacitySlider( kvs::glut::Screen* screen ):
        kvs::glut::Slider( screen )
    {
        setWidth( 150 );
        setMargin( 10 );
        setCaption( "Opacity" );
    }

    void valueChanged()
    {
        typedef kvs::PolygonObject Object;
        kvs::Scene* scene = static_cast<kvs::glut::Screen*>( screen() )->scene();
        Object* object1 = Object::DownCast( scene->objectManager()->object( "Polygon" ) );
        Object* object2 = new Object();
        object2->shallowCopy( *object1 );
        object2->setName( "Polygon" );
        object2->setOpacity( int( value() * 255 + 0.5 ) );
        scene->objectManager()->change( "Polygon", object2 );
    }
};

/*===========================================================================*/
/**
 *  @brief  Repetition slider.
 */
/*===========================================================================*/
class RepetitionSlider : public kvs::glut::Slider
{
    kvs::StochasticRenderingCompositor* m_compositor;

public:

    RepetitionSlider( kvs::glut::Screen* screen, kvs::StochasticRenderingCompositor* compositor ):
        kvs::glut::Slider( screen ),
        m_compositor( compositor )
    {
        setWidth( 150 );
        setMargin( 10 );
        setCaption( "Repetition" );
    }

    void valueChanged()
    {
        m_compositor->setRepetitionLevel( int( value() + 0.5 ) );
        screen()->redraw();
    }
};

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

    std::cerr << "main(): construct Application()" << std::endl;

    //kvs::UnstructuredVolumeObject* volume_object = new kvs::UnstructuredVolumeImporter( argv[1] );
    //kvs::UnstructuredVolumeObject* volume_object = Import( argc, argv);
    kvs::Vector3ui resolution( 32, 32, 32 );
    kvs::StructuredVolumeObject* volume = new kvs::HydrogenVolumeData( resolution );

    volume->print( std::cout );

    std::cerr << "main(): create volume object" << std::endl;

    /* The image quality can be improved by increasing 'repetitions'. However,
     * the number of generated points will be increased depending on the
     * number of repetitions. Therefore, the 'repetitions' will be specified
     * appropriately by taking into account memory resources of CPU and GPU.
     */
    const size_t repetitions = 4;
    const float step = 0.5f;
    const kvs::TransferFunction tfunc( 256 );
    kvs::PointObject* point_object = new kvs::CellByCellMetropolisSampling( volume, repetitions, step, tfunc );
    //object->translate(kvs::Vec3(0,0,-10));
    //object->translate(kvs::Vec3(0,10,-20));
    point_object->print( std::cout << std::endl );
    //delete volume;

    /* User can use following particle based renderers.
     *     (1) CPU renderer -> kvs::ParticleBasedRenderer
     *     (2) GPU renderer -> kvs::glsl::ParticleBasedRenderer
     *     (3) GPU renderer -> kvs::glsl::rits::ParticleBasedRenderer (Ritsumeikan Univ. version)
     */
    kvs::glsl::ParticleBasedRenderer* point_renderer = new kvs::glsl::ParticleBasedRenderer();
    //point_renderer->setRepetitionLevel( repetitions );
    //point_renderer->enableLODControl();

    std::cerr << "main(): create volume renderer" << std::endl;

   /* Extract orthogonal planes by using OrthoSlice class.
     *    p: plane position.
     *    a: axis of the orthogonal plane.
     *    t: transfer function.
     */
    const float p = volume->resolution().z() * 0.5f;
    //const kvs::OrthoSlice::AlignedAxis a = kvs::OrthoSlice::ZAxis;
    const kvs::OrthoSlice::AlignedAxis a = kvs::OrthoSlice::YAxis;
    const kvs::TransferFunction t( 256 );
    kvs::PolygonObject* polygon_object = new kvs::OrthoSlice( volume, p, a, t );
    if ( !polygon_object )
    {
        kvsMessageError( "Cannot create a polygon object." );
        delete volume;
        return( false );
    }

	delete volume;
	
    polygon_object->setName( "Slice" );
    //polygon_object->setColor( kvs::RGBColor::White() );
    //polygon_object->setOpacity( 128 );
    polygon_object->print( std::cout << std::endl );

    std::cerr << "main(): create polygon object" << std::endl;

    kvs::StochasticPolygonRenderer* polygon_renderer = new kvs::StochasticPolygonRenderer();
    //polygon_renderer->setPolygonOffset( 0.001f );

    std::cerr << "main(): create polygon renderer" << std::endl;

    kvs::oculus::jaea::Screen screen( &app );

    std::cerr << "main(): create Screen" << std::endl;

    screen.setTitle("Example program for kvs::StochasticRenderingCompositor");
    screen.registerObject( point_object, point_renderer );
    screen.registerObject( polygon_object, polygon_renderer );
    screen.show(false);

    std::cerr << "main(): show screen" << std::endl;

    kvs::StochasticRenderingCompositor compositor( screen.scene() );

    std::cerr << "main(): create compositor" << std::endl;

    //compositor.setRepetitionLevel( 50 );
    compositor.setRepetitionLevel( 5 );
    compositor.enableLODControl();
    screen.setEvent( &compositor );

    std::cerr << "main(): add compositor to screen as event listener" << std::endl;

    kvs::oculus::jaea::TouchController event( &screen );
    event.setRotationFactor( 5000.0f );
    event.setTranslationFactor( 5000.0f );
    event.setScalingFactor( 30000.0f );


    // TransferFunctionEditor editor( &screen );
    // editor.setVolumeObject( volume_object );
    // editor.show();

    // std::cerr << "main(): create transfer function editor" << std::endl;

    // LODCheckBox checkbox( &screen, &compositor );
    // checkbox.setPosition( 0, 0 );
    // checkbox.setState( true );
    // checkbox.show();

    // std::cerr << "main(): create LOD checkbox" << std::endl;

    // OpacitySlider opacity( &screen );
    // opacity.setPosition( checkbox.x(), checkbox.y() + checkbox.height() );
    // opacity.setValue( 0.5 );
    // opacity.setRange( 0, 1 );
    // opacity.show();

    // std::cerr << "main(): create opacity slider" << std::endl;

    // RepetitionSlider repetition( &screen, &compositor );
    // repetition.setPosition( opacity.x(), opacity.y() + opacity.height() );
    // repetition.setValue( 50 );
    // repetition.setRange( 1, 100 );
    // repetition.show();

    // std::cerr << "main(): create repetition slider" << std::endl;

    return app.run();
}
