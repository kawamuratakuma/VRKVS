#pragma once
#include <kvs/glut/Application>


namespace kvs
{

namespace oculus
{

namespace jaea
{

class Application : public kvs::glut::Application
{
public:
    Application( int argc, char** argv );

    virtual int run();
    virtual void quit();
};

} // end of namespace jaea

} // end of namepace oculus

} // end of namespace kvs
