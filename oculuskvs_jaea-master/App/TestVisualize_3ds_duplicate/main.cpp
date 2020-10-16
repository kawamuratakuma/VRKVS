/*****************************************************************************/
/**
 *  @file   main.cpp
 *  @brief  Example program to visualize 3ds file.
 *  @author Insight, Inc.
 */
#include <kvs/Version>

#include "TexturedPolygonObject.h"
#include "TexturedPolygonImporter.h"
#include "ThreeDS.h"
#include "StochasticTexturedPolygonRenderer.h"

#define USE_OCULUS
#ifdef USE_OCULUS
#include <Application.h>
#include <Screen.h>
#include <TouchController.h>
#else
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#endif

namespace KVSMain {
    // ポリゴンオブジェクトのコネクティビティを複製してポリゴン数を増やす
    kvs::jaea::TexturedPolygonObject* DuplicatePolygons(kvs::jaea::TexturedPolygonObject* texturedPolygonIn, int nDuplicate )
    {
        kvs::ValueArray<kvs::UInt32> connections = texturedPolygonIn->connections();
        
        int nPolygonsOld = connections.size() / 3;
        int nPolygonsNew = nPolygonsOld * nDuplicate;
        
        kvs::ValueArray<kvs::UInt32> connectionsNew(nPolygonsNew * 3);
        for(int j=0;j<nDuplicate;j++){
            for(int i=0;i<connections.size();i++){
                int p = connections[i];
                connectionsNew[i + j * connections.size()] = p;
            }
        }
        
        
        kvs::jaea::TexturedPolygonObject* texturedPolygonOut = new kvs::jaea::TexturedPolygonObject();
        texturedPolygonOut->deepCopy(*texturedPolygonIn);
        texturedPolygonOut->setConnections(connectionsNew);
        
        return texturedPolygonOut;
    }
    
    // ポリゴンオブジェクトを生成する関数
    kvs::jaea::TexturedPolygonObject* CreateTexturedPolygonObject( char *arg )
    {
        std::string filePath = arg;
        
        kvs::FileFormatBase* io = new kvs::jaea::ThreeDS();
        kvs::jaea::TexturedPolygonImporter* importer = nullptr;
        io->read(filePath);
        if(io->isSuccess()){
            std::cout << "loading " << io->filename() << " succeed." << std::endl;
            importer = new kvs::jaea::TexturedPolygonImporter();
            importer->exec(io);
        }else{
            std::cerr << "loading " << io->filename() << " failed." << std::endl;
        }
        
        delete io;
        
        return importer;
    }
}

int main(int argc, char *argv[])
{
    if(argc <= 1){
        std::cerr << "No arguments." << std::endl;
        return 0;
    }
    
#ifdef USE_OCULUS
    kvs::oculus::jaea::Application app(argc, argv);
    kvs::oculus::jaea::Screen screen(&app);
    
    kvs::oculus::jaea::TouchController event( &screen );
    event.setRotationFactor( 5000.0f );
    event.setTranslationFactor( 5000.0f );
    event.setScalingFactor(30000.0f);
#else    
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setTitle("Example program for kvs::StochasticPolygonRenderer");
#endif
    
    kvs::jaea::TexturedPolygonObject* tmpTexturedPolygon = KVSMain::CreateTexturedPolygonObject( argv[1] );
    if( tmpTexturedPolygon == nullptr ){
        return 0;
    }
    
    int nDuplicate = 15;
    kvs::jaea::TexturedPolygonObject* texturedPolygon = KVSMain::DuplicatePolygons(tmpTexturedPolygon, nDuplicate);
    std::cout << "NumPolygons=" << (tmpTexturedPolygon->connections().size()/3) << ", NumDuplicatedPolygons=" << (texturedPolygon->connections().size()/3) << std::endl;
    
    kvs::jaea::StochasticTexturedPolygonRenderer* renderer = new kvs::jaea::StochasticTexturedPolygonRenderer();
    renderer->setEnabledTextureLinear(true);
    renderer->setEnabledTextureMipmap(true);
    renderer->setEnabledTextureMipmapLinear(true);
    renderer->setEnabledTextureAnisotropy(true);
    
    screen.registerObject( texturedPolygon, renderer );
    
    screen.show();
    return app.run();
}
