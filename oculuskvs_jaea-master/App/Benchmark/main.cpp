/*****************************************************************************/
/**
 *  @file   main.cpp
 *  @brief  Benchmark program.
 *  @author Insight, Inc.
 *
 * This code is based on KVS.ovulus/App/Benchmark/main.cpp.
 */

#define USE_STOCHASTIC_RENDERER

#include <kvs/Message>
#include <kvs/PolygonObject>
#ifdef USE_STOCHASTIC_RENDERER
#include <kvs/StochasticPolygonRenderer>
#else // USE_STOCHASTIC_RENDERER
#include <kvs/PolygonRenderer>
#endif // USE_STOCHASTIC_RENDERER
#include <kvs/Timer>

#include <Application.h>
#include <Screen.h>
#include <TouchController.h>

#include <iostream>
#include <fstream>


void setupPolygons
(kvs::ValueArray<kvs::Real32> *node,
 kvs::ValueArray<kvs::UInt32> *triangle,
 kvs::ValueArray<kvs::UInt8> *color,
 kvs::ValueArray<kvs::UInt8> *opacity,
 kvs::ValueArray<kvs::Real32> *normal
 ) {
  kvs::UInt32 node_id;
  kvs::Real32 crd[3][3];
  kvs::Real32 v01[3];
  kvs::Real32 v02[3];
  kvs::Real32 n[3];
  kvs::Real32 len;

  int ntriangles = triangle->size() / 3;
  int nnodes = node->size() / 3;

    // opacity
    opacity->allocate(1);
    opacity->at(0) = (kvs::UInt8)255;

    // color
    // single color
    color->allocate(3);
    color->at(0) = (kvs::UInt8)127;
    color->at(1) = (kvs::UInt8)127;
    color->at(2) = (kvs::UInt8)127;

    // kvs::Real32 min_x;
    // kvs::Real32 min_y;
    // kvs::Real32 min_z;
    // kvs::Real32 max_x;
    // kvs::Real32 max_y;
    // kvs::Real32 max_z;
    // kvs::Real32 size_x;
    // kvs::Real32 size_y;
    // kvs::Real32 size_z;
    // kvs::Real32 x;
    // kvs::Real32 y;
    // kvs::Real32 z;
    // kvs::UInt8 cx;
    // kvs::UInt8 cy;
    // kvs::UInt8 cz;

    // min_x = max_x = node->at(0);
    // min_y = max_y = node->at(1);
    // min_z = max_z = node->at(2);
    // for (int inode = 0; inode < nnodes; inode++) {
    //     x = node->at(inode*3);
    //     y = node->at(inode*3+1);
    //     z = node->at(inode*3+2);
    //     min_x = (min_x < x) ? min_x : x;
    //     min_y = (min_y < y) ? min_y : y;
    //     min_z = (min_z < z) ? min_z : z;
    //     max_x = (max_x > x) ? max_x : x;
    //     max_y = (max_y > y) ? max_y : y;
    //     max_z = (max_z > z) ? max_z : z;
    // }
    // size_x = max_x - min_x;
    // size_y = max_y - min_y;
    // size_z = max_z - min_z;

    // // Vertex Color
    // color->allocate(node->size());
    // for (int inode = 0; inode < nnodes; inode++) {
    //     x = node->at(inode*3);
    //     y = node->at(inode*3+1);
    //     z = node->at(inode*3+2);
    //     cx = (kvs::UInt8)(256.0 * (x-min_x) / size_x);
    //     cy = (kvs::UInt8)(256.0 * (y-min_y) / size_y);
    //     cz = (kvs::UInt8)(256.0 * (z-min_z) / size_z);
    //     color->at(inode*3) = cx;
    //     color->at(inode*3+1) = cy;
    //     color->at(inode*3+2) = cz;
    // }
    // //Vertex Color
    // color->allocate(node->size());
    // for (int inode = 0; inode < nnodes; inode++) {
    //   color->at(inode*3) = 127;
    //   color->at(inode*3+1) = 127;
    //   color->at(inode*3+2) = 127;
    // }

  // set normals
  // // Vertex Normal
  // normal->allocate (node->size());
  // for (int itri = 0; itri < ntriangles; itri++) {
  //   for (int inode = 0; inode < 3; inode++) {
  //     node_id = triangle->at (itri*3+inode);
  //     crd[inode][0] = node->at (node_id*3);
  //     crd[inode][1] = node->at (node_id*3+1);
  //     crd[inode][2] = node->at (node_id*3+2);
  //   }
    
  //   v01[0] = crd[1][0] - crd[0][0];
  //   v01[1] = crd[1][1] - crd[0][1];
  //   v01[2] = crd[1][2] - crd[0][2];
    
  //   v02[0] = crd[2][0] - crd[0][0];
  //   v02[1] = crd[2][1] - crd[0][1];
  //   v02[2] = crd[2][2] - crd[0][2];
    
  //   n[0] = v01[1]*v02[2] - v01[2]*v02[1];
  //   n[1] = v01[2]*v02[0] - v01[0]*v02[2];
  //   n[2] = v01[0]*v02[1] - v01[1]*v02[0];
    
  //   len = std::sqrt (n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);

  //   kvs::Real32 nx = n[0]/len;
  //   kvs::Real32 ny = n[1]/len;
  //   kvs::Real32 nz = n[2]/len;
  //   for (int inode = 0; inode < 3; inode++) {
  //     node_id = triangle->at (itri*3+inode);
  //     normal->at (node_id*3) += nx;
  //     normal->at (node_id*3+1) += ny;
  //     normal->at (node_id*3+2) += nz;
  //   }
  // }

  // for (int inode = 0; inode < nnodes; inode++) {
  //   kvs::Real32 nx = normal->at(inode*3);
  //   kvs::Real32 ny = normal->at(inode*3+1);
  //   kvs::Real32 nz = normal->at(inode*3+2);
  //   len = std::sqrt (nx*nx+ny*ny+nz*nz);
  //   normal->at(inode*3) = -nx / len;
  //   normal->at(inode*3+1) = -ny / len;
  //   normal->at(inode*3+2) = -nz / len;
  // }

  // Polygon Normal
  normal->allocate(triangle->size());
  for (int itri = 0; itri < ntriangles; itri++) {
    for (int inode = 0; inode < 3; inode++) {
      node_id = triangle->at (itri*3+inode);
      crd[inode][0] = node->at (node_id*3);
      crd[inode][1] = node->at (node_id*3+1);
      crd[inode][2] = node->at (node_id*3+2);
    }
    
    v01[0] = crd[1][0] - crd[0][0];
    v01[1] = crd[1][1] - crd[0][1];
    v01[2] = crd[1][2] - crd[0][2];
    
    v02[0] = crd[2][0] - crd[0][0];
    v02[1] = crd[2][1] - crd[0][1];
    v02[2] = crd[2][2] - crd[0][2];
    
    n[0] = v01[1]*v02[2] - v01[2]*v02[1];
    n[1] = v01[2]*v02[0] - v01[0]*v02[2];
    n[2] = v01[0]*v02[1] - v01[1]*v02[0];
    
    len = std::sqrt (n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);

    normal->at (itri*3) = -n[0]/len;
    normal->at (itri*3+1) = -n[1]/len;
    normal->at (itri*3+2) = -n[2]/len;
  }
}

kvs::PolygonObject *read_pch (const char *filename) {
  kvs::Timer read_timer;
  read_timer.start();

  std::ifstream ifs (filename);

  int nnodes;
  int ntriangles;

  

  ifs >> nnodes;
  kvs::ValueArray<kvs::Real32> node (nnodes * 3);
  for (int inode = 0; inode < nnodes; inode++) {
    ifs >> node[inode*3];
    ifs >> node[inode*3+1];
    ifs >> node[inode*3+2];
  }

  ifs >> ntriangles;
  kvs::ValueArray<kvs::UInt32> triangle (ntriangles * 3);
  for (int itri = 0; itri < ntriangles; itri++) {
    ifs >> triangle[itri*3];
    ifs >> triangle[itri*3+1];
    ifs >> triangle[itri*3+2];
  }
  ifs.close ();

  read_timer.stop();

  kvs::Timer normal_timer;
  normal_timer.start();

  kvs::ValueArray<kvs::UInt8> color;
  kvs::ValueArray<kvs::UInt8> opacity;
  kvs::ValueArray<kvs::Real32> normal;
  setupPolygons (&node, &triangle, &color, &opacity, &normal);

  normal_timer.stop();


  kvs::Timer object_timer;
  object_timer.start();

  kvs::PolygonObject *poly = new kvs::PolygonObject ();
  poly->setCoords (node);

  poly->setConnections (triangle);
  poly->setColors (color);
  poly->setOpacities (opacity);
  poly->setNormals (normal);

  poly->setPolygonType (kvs::PolygonObject::Triangle);
  //poly->setColorType (kvs::PolygonObject::PolygonColor);
  poly->setColorType (kvs::PolygonObject::VertexColor);
  poly->setNormalType (kvs::PolygonObject::PolygonNormal);
  //poly->setNormalType (kvs::PolygonObject::VertexNormal);

  object_timer.stop();

  std::cerr << "TIMER: file_read " << read_timer.msec() << " msec." << std::endl;
  std::cerr << "TIMER: calc_norm " << normal_timer.msec() << " msec." << std::endl;
  std::cerr << "TIMER: build_obj " << object_timer.msec() << " msec." << std::endl;
  
  return poly;
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

    kvs::PolygonObject* object = NULL;
    if ( argc > 1 ) {
        object = read_pch( argv[1] );
    } else {
    }

    if ( !object )
    {
        kvsMessageError( "Cannot create a polygon object." );
        return false;
    }

    kvs::oculus::jaea::Screen screen( &app );
    //screen.setGeometry( 0, 0, 2000, 1000 );

#ifdef USE_STOCHASTIC_RENDERER
  kvs::StochasticPolygonRenderer* renderer = new kvs::StochasticPolygonRenderer();
  screen.registerObject( object, renderer );
#else // USE_STOCHASTIC_RENDERER
    bool glsl = true;
    //bool glsl = false;
    if ( glsl )
    {
        kvs::glsl::PolygonRenderer* renderer = new kvs::glsl::PolygonRenderer();
        screen.registerObject( object, renderer );
    }
    else
    {
        kvs::PolygonRenderer* renderer = new kvs::PolygonRenderer();
        screen.registerObject( object, renderer );
    }
#endif // USE_STOCHASTIC_RENDERER

    screen.setTitle( "Benchmark" );

    kvs::oculus::jaea::TouchController event( &screen );

    event.setRotationFactor( 5000.0f );
    event.setTranslationFactor( 5000.0f );
    event.setScalingFactor( 30000.0f );

    screen.show(false);

    return app.run();
}
