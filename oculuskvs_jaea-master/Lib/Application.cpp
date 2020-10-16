#include "Application.h"
#include "OVR.h"
#include <kvs/Message>
#include <cstdlib>


namespace kvs
{

namespace oculus
{

namespace jaea
{

Application::Application( int argc, char** argv ):
    kvs::glut::Application( argc, argv )
{
    std::cerr << "kvs::oculus::jaea::Application::Application() : start" << std::endl;

    static bool flag = true;
    if ( flag )
    {
        flag = false;
        if ( !kvs::oculus::jaea::Initialize() )
        {
            kvsMessageError( "Cannot initialize LibOVR." );
            std::exit( EXIT_FAILURE );
        }
        std::cerr << "kvs::oculus::jaea::Application::Application() : finish to initialize libOVR." << std::endl;
    }

    std::cerr << "kvs::oculus::jaea::Application::Application() : end" << std::endl;
}

int Application::run()
{
//    return true;
    std::cerr << "kvs::oculus::jaea::Application::run() : start" << std::endl;

    return kvs::glut::Application::run();
}

void Application::quit()
{
    kvs::oculus::jaea::Shutdown();
    kvs::glut::Application::quit();
}

} // end of namespace jaea

} // end of namespace oculus

} // end of namespace kvs
