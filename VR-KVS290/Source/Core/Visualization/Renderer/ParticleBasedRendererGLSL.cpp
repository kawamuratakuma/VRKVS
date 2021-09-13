/*****************************************************************************/
/**
 *  @file   ParticleBasedRenderer.cpp
 *  @author Naohisa Sakamoto
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
#include "ParticleBasedRendererGLSL.h"
#include <cmath>
#include <kvs/OpenGL>
#include <kvs/PointObject>
#include <kvs/Camera>
#include <kvs/Light>
#include <kvs/Assert>
#include <kvs/Math>
#include <kvs/MersenneTwister>
#include <kvs/Xorshift128>


namespace
{

/*===========================================================================*/
/**
 *  @brief  Returns shuffled array.
 *  @param  values [in] value array
 *  @param  seed   [in] seed value for random number generator
 */
/*===========================================================================*/
template <int Dim, typename T>
kvs::ValueArray<T> ShuffleArray( const kvs::ValueArray<T>& values, kvs::UInt32 seed )
{
    KVS_ASSERT( Dim > 0 );
    KVS_ASSERT( values.size() % Dim == 0 );

    kvs::Xorshift128 rng; rng.setSeed( seed );
    kvs::ValueArray<T> ret;
    if ( values.unique() ) { ret = values; }
    else { ret = values.clone(); }

    T* p = ret.data();
    size_t size = ret.size() / Dim;

    for ( size_t i = 0; i < size; ++i )
    {
        size_t j = rng.randInteger() % ( i + 1 );
        for ( int k = 0; k < Dim; ++k )
        {
            std::swap( p[ i * Dim + k ], p[ j * Dim + k ] );
        }
    }
    return ret;
}

}


namespace kvs
{

namespace glsl
{

/*===========================================================================*/
/**
 *  @brief  Constructs a new ParticleBasedRenderer class.
 */
/*===========================================================================*/
ParticleBasedRenderer::ParticleBasedRenderer():
    StochasticRendererBase( new Engine() )
{
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new ParticleBasedRenderer class.
 *  @param  m [in] initial modelview matrix
 *  @param  p [in] initial projection matrix
 *  @param  v [in] initial viewport
 */
/*===========================================================================*/
ParticleBasedRenderer::ParticleBasedRenderer( const kvs::Mat4& m, const kvs::Mat4& p, const kvs::Vec4& v ):
    StochasticRendererBase( new Engine( m, p, v ) )
{
}

/*===========================================================================*/
/**
 *  @brief  Returns true if the particle shuffling is enabled
 *  @return true, if the shuffling is enabled
 */
/*===========================================================================*/
bool ParticleBasedRenderer::isEnabledShuffle() const
{
    return static_cast<const Engine&>( engine() ).isEnabledShuffle();
}

/*===========================================================================*/
/**
 *  @brief  Returns true if the particle zooming is enabled
 *  @return true, if the zooming is enabled
 */
/*===========================================================================*/
bool ParticleBasedRenderer::isEnabledZooming() const
{
    return static_cast<const Engine&>( engine() ).isEnabledZooming();
}

/*===========================================================================*/
/**
 *  @brief  Sets enable-flag for the particle shuffling.
 *  @param  enable [in] enable-flag
 */
/*===========================================================================*/
void ParticleBasedRenderer::setEnabledShuffle( const bool enable )
{
    static_cast<Engine&>( engine() ).setEnabledShuffle( enable );
}

/*===========================================================================*/
/**
 *  @brief  Sets enable-flag for the particle zooming.
 *  @param  enable [in] enable-flag
 */
/*===========================================================================*/
void ParticleBasedRenderer::setEnabledZooming( const bool enable )
{
    static_cast<Engine&>( engine() ).setEnabledZooming( enable );
}

/*===========================================================================*/
/**
 *  @brief  Enable the particle shuffling.
 */
/*===========================================================================*/
void ParticleBasedRenderer::enableShuffle()
{
    static_cast<Engine&>( engine() ).enableShuffle();
}

/*===========================================================================*/
/**
 *  @brief  Enable the particle zooming.
 */
/*===========================================================================*/
void ParticleBasedRenderer::enableZooming()
{
    static_cast<Engine&>( engine() ).enableZooming();
}

/*===========================================================================*/
/**
 *  @brief  Disable the particle shuffling.
 */
/*===========================================================================*/
void ParticleBasedRenderer::disableShuffle()
{
    static_cast<Engine&>( engine() ).disableShuffle();
}

/*===========================================================================*/
/**
 *  @brief  Disable the particle zooming.
 */
/*===========================================================================*/
void ParticleBasedRenderer::disableZooming()
{
    static_cast<Engine&>( engine() ).disableZooming();
}

/*===========================================================================*/
/**
 *  @brief  Returns the initial modelview matrix.
 *  @param  initial modelview matrix
 */
/*===========================================================================*/
const kvs::Mat4& ParticleBasedRenderer::initialModelViewMatrix() const
{
    return static_cast<const Engine&>( engine() ).initialModelViewMatrix();
}

/*===========================================================================*/
/**
 *  @brief  Returns the initial projection matrix.
 *  @param  initial projection matrix
 */
/*===========================================================================*/
const kvs::Mat4& ParticleBasedRenderer::initialProjectionMatrix() const
{
    return static_cast<const Engine&>( engine() ).initialProjectionMatrix();
}

/*===========================================================================*/
/**
 *  @brief  Returns the initial viewport.
 *  @param  initial viewport
 */
/*===========================================================================*/
const kvs::Vec4& ParticleBasedRenderer::initialViewport() const
{
    return static_cast<const Engine&>( engine() ).initialViewport();
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new Engine class.
 */
/*===========================================================================*/
ParticleBasedRenderer::Engine::Engine():
    m_has_normal( false ),
    m_enable_shuffle( true ),
    m_enable_zooming( true ),
    m_random_index( 0 ),
    m_initial_modelview( kvs::Mat4::Zero() ),
    m_initial_projection( kvs::Mat4::Zero() ),
    m_initial_viewport( kvs::Vec4::Zero() ),
    m_initial_object_depth( 0 ),
    m_vbo_manager( NULL )
{
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new Engine class.
 *  @param  m [in] initial modelview matrix
 *  @param  p [in] initial projection matrix
 *  @param  v [in] initial viewport
 */
/*===========================================================================*/
ParticleBasedRenderer::Engine::Engine( const kvs::Mat4& m, const kvs::Mat4& p, const kvs::Vec4& v ):
    m_has_normal( false ),
    m_enable_shuffle( true ),
    m_random_index( 0 ),
    m_initial_modelview( m ),
    m_initial_projection( p ),
    m_initial_viewport( v ),
    m_initial_object_depth( 0 ),
    m_vbo_manager( NULL )
{
}

/*===========================================================================*/
/**
 *  @brief  Destroys the Engine class.
 */
/*===========================================================================*/
ParticleBasedRenderer::Engine::~Engine()
{
    if ( m_vbo_manager ) delete [] m_vbo_manager;
}

/*===========================================================================*/
/**
 *  @brief  Releases the GPU resources.
 */
/*===========================================================================*/
void ParticleBasedRenderer::Engine::release()
{
    KVS_ASSERT( m_vbo_manager );

    if ( m_shader_program.isCreated() )
    {
        m_shader_program.release();
        for ( size_t i = 0; i < repetitionLevel(); i++ ) m_vbo_manager[i].release();
        if (m_vbo_manager){
            delete [] m_vbo_manager;
            m_vbo_manager = NULL;
        }
    }
}

/*===========================================================================*/
/**
 *  @brief  Create shaders, VBO, and framebuffers.
 *  @param  point [in] pointer to the point object
 *  @param  camera [in] pointer to the camera
 *  @param  light [in] pointer to the light
 */
/*===========================================================================*/
void ParticleBasedRenderer::Engine::create( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    kvs::PointObject* point = kvs::PointObject::DownCast( object );
    m_has_normal = point->normals().size() > 0;
    if ( !m_has_normal ) setEnabledShading( false );

    // Create resources.
    attachObject( object );
    createRandomTexture();
    this->create_shader_program();
    this->create_buffer_object( point );

    // Initial values for calculating the object depth.
    if ( kvs::Math::IsZero( m_initial_modelview[3][3] ) )
    {
        m_initial_modelview = kvs::OpenGL::ModelViewMatrix();


        std::cerr << "ParticleBasedRenderer::Engine::create(): m_initial_modelview[3][3] is zero." << std::endl;
    } else {
        std::cerr << "ParticleBasedRenderer::Engine::create(): m_initial_modelview[3][3] is NOT zero." << std::endl;

    }
    std::cerr << std::showpos << std::scientific << "     " << m_initial_modelview[0][0] << "  " << m_initial_modelview[0][1] << "  " << m_initial_modelview[0][2] << "  " << m_initial_modelview[0][3] << std::endl;
    std::cerr << std::showpos << std::scientific << "     " << m_initial_modelview[1][0] << "  " << m_initial_modelview[1][1] << "  " << m_initial_modelview[1][2] << "  " << m_initial_modelview[1][3] << std::endl;
    std::cerr << std::showpos << std::scientific << "     " << m_initial_modelview[2][0] << "  " << m_initial_modelview[2][1] << "  " << m_initial_modelview[2][2] << "  " << m_initial_modelview[2][3] << std::endl;
    std::cerr << std::showpos << std::scientific << "     " << m_initial_modelview[3][0] << "  " << m_initial_modelview[3][1] << "  " << m_initial_modelview[3][2] << "  " << m_initial_modelview[3][3] << std::endl;


    if ( kvs::Math::IsZero( m_initial_projection[3][3] ) )
    {
        m_initial_projection = kvs::OpenGL::ProjectionMatrix();


        std::cerr << "ParticleBasedRenderer::Engine::create(): m_initial_projection[3][3] is zero." << std::endl;
    } else {
        std::cerr << "ParticleBasedRenderer::Engine::create(): m_initial_projection[3][3] is NOT zero." << std::endl;

    }
    std::cerr << std::showpos << std::scientific << "     " << m_initial_projection[0][0] << "  " << m_initial_projection[0][1] << "  " << m_initial_projection[0][2] << "  " << m_initial_projection[0][3] << std::endl;
    std::cerr << std::showpos << std::scientific << "     " << m_initial_projection[1][0] << "  " << m_initial_projection[1][1] << "  " << m_initial_projection[1][2] << "  " << m_initial_projection[1][3] << std::endl;
    std::cerr << std::showpos << std::scientific << "     " << m_initial_projection[2][0] << "  " << m_initial_projection[2][1] << "  " << m_initial_projection[2][2] << "  " << m_initial_projection[2][3] << std::endl;
    std::cerr << std::showpos << std::scientific << "     " << m_initial_projection[3][0] << "  " << m_initial_projection[3][1] << "  " << m_initial_projection[3][2] << "  " << m_initial_projection[3][3] << std::endl;



    if ( kvs::Math::IsZero( m_initial_viewport[2] ) )
    {
        const float dpr = camera->devicePixelRatio();
        const float framebuffer_width = camera->windowWidth() * dpr;
        const float framebuffer_height = camera->windowHeight() * dpr;
        m_initial_viewport[2] = framebuffer_width;
        m_initial_viewport[3] = framebuffer_height;

        std::cerr << "ParticleBasedRenderer::Engine::create(): viewport[2] is zero." << std::endl;
        std::cerr << "     dpr       = " << dpr << std::endl;
        std::cerr << "     fb_width  = " << dpr << std::endl;
        std::cerr << "     fb_height = " << dpr << std::endl;

    } else {
        std::cerr << "ParticleBasedRenderer::Engine::create(): viewport[2] is NOT zero." << std::endl;
    }
    std::cerr << "     m_initial_viewport =( " << m_initial_viewport[0] << ", " << m_initial_viewport[1] << ", " << m_initial_viewport[2] << ", " << m_initial_viewport[3] << ")" << std::endl;

    const kvs::Vec4 I( point->objectCenter(), 1.0f );
    const kvs::Vec4 O = m_initial_projection * m_initial_modelview * I;
    m_initial_object_depth = O.z();
}

/*===========================================================================*/
/**
 *  @brief  Update.
 *  @param  point [in] pointer to the point object
 *  @param  camera [in] pointer to the camera
 *  @param  light [in] pointer to the light
 */
/*===========================================================================*/
void ParticleBasedRenderer::Engine::update( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    const float dpr = camera->devicePixelRatio();
    const float framebuffer_width = camera->windowWidth() * dpr;
    m_initial_viewport[2] = static_cast<float>( framebuffer_width );
}

/*===========================================================================*/
/**
 *  @brief  Setup.
 *  @param  point [in] pointer to the point object
 *  @param  camera [in] pointer to the camera
 *  @param  light [in] pointer to the light
 */
/*===========================================================================*/
void ParticleBasedRenderer::Engine::setup( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    // The repetition counter must be reset here.
    resetRepetitions();

    kvs::OpenGL::Enable( GL_DEPTH_TEST );
    kvs::OpenGL::Enable( GL_VERTEX_PROGRAM_POINT_SIZE );
    m_random_index = m_shader_program.attributeLocation("random_index");

    const kvs::Mat4 M = kvs::OpenGL::ModelViewMatrix();
    const kvs::Mat4 P = kvs::OpenGL::ProjectionMatrix();
    m_shader_program.bind();
    m_shader_program.setUniform( "ModelViewMatrix", M );
    m_shader_program.setUniform( "ProjectionMatrix", P );
    m_shader_program.unbind();
}

/*===========================================================================*/
/**
 *  @brief  Draw an ensemble.
 *  @param  point [in] pointer to the point object
 *  @param  camera [in] pointer to the camera
 *  @param  light [in] pointer to the light
 */
/*===========================================================================*/
void ParticleBasedRenderer::Engine::draw( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    kvs::PointObject* point = kvs::PointObject::DownCast( object );
    KVS_ASSERT( repetitionCount()  < repetitionLevel());
    if (repetitionCount() >= repetitionLevel()){
        return;
    }
    kvs::VertexBufferObjectManager::Binder bind1( m_vbo_manager[ repetitionCount() ] );
    kvs::ProgramObject::Binder bind2( m_shader_program );
    kvs::Texture::Binder bind3( randomTexture() );
    {
        // 20210616 add by insight
        kvs::OpenGL::WithEnabled dd( GL_DEPTH_TEST );
        kvs::OpenGL::WithEnabled dp( GL_VERTEX_PROGRAM_POINT_SIZE );

        const kvs::Mat4& m0 = m_initial_modelview;
        const float scale0 = kvs::Vec3( m0[0][0], m0[1][0], m0[2][0] ).length();
        const float width0 = m_initial_viewport[2];
        const float height0 = m_initial_viewport[3];

        const kvs::Mat4 m = kvs::OpenGL::ModelViewMatrix();
        const float scale = kvs::Vec3( m[0][0], m[1][0], m[2][0] ).length();
        const float dpr = camera->devicePixelRatio();
        const float width = camera->windowWidth() * dpr;
        const float height = camera->windowHeight() * dpr;

        const float Cr = ( width / width0 ) * ( height / height0 );
        const float Cs = scale / scale0;
        const float D0 = m_initial_object_depth;
        const float object_scale = Cr * Cs * dpr;
        const float object_depth = object_scale * D0;
        m_shader_program.setUniform( "object_scale", object_scale );
        m_shader_program.setUniform( "object_depth", object_depth );
        m_shader_program.setUniform( "random_texture", 0 );
        m_shader_program.setUniform( "random_texture_size_inv", 1.0f / randomTextureSize() );
        m_shader_program.setUniform( "screen_scale", kvs::Vec2( width * 0.5f, height * 0.5f ) );

        const size_t nvertices = point->numberOfVertices();
        const size_t rem = nvertices % repetitionLevel();
        const size_t quo = nvertices / repetitionLevel();
        const size_t count = quo + ( repetitionCount() < rem ? 1 : 0 );
        kvs::OpenGL::EnableVertexAttribArray( m_random_index );
        kvs::OpenGL::VertexAttribPointer( m_random_index, 2, GL_UNSIGNED_SHORT, GL_FALSE, 0, (GLubyte*)NULL + 0 );
        m_vbo_manager[ repetitionCount() ].drawArrays( GL_POINTS, 0, count );
        kvs::OpenGL::DisableVertexAttribArray( m_random_index );
    }
}

/*===========================================================================*/
/**
 *  @brief  Creates shader program.
 */
/*===========================================================================*/
void ParticleBasedRenderer::Engine::create_shader_program()
{
    kvs::ShaderSource vert( "PBR_zooming.vert" );
    kvs::ShaderSource frag( "PBR_zooming.frag" );

    if ( isEnabledShading() )
    {
        switch ( shader().type() )
        {
        case kvs::Shader::LambertShading: frag.define("ENABLE_LAMBERT_SHADING"); break;
        case kvs::Shader::PhongShading: frag.define("ENABLE_PHONG_SHADING"); break;
        case kvs::Shader::BlinnPhongShading: frag.define("ENABLE_BLINN_PHONG_SHADING"); break;
        default: break; // NO SHADING
        }

        if ( kvs::OpenGL::Boolean( GL_LIGHT_MODEL_TWO_SIDE ) == GL_TRUE )
        {
            frag.define("ENABLE_TWO_SIDE_LIGHTING");
        }
    }

    if ( isEnabledZooming() )
    {
        vert.define("ENABLE_PARTICLE_ZOOMING");
        frag.define("ENABLE_PARTICLE_ZOOMING");
    }

    m_shader_program.build( vert, frag );
    m_shader_program.bind();
    m_shader_program.setUniform( "shading.Ka", shader().Ka );
    m_shader_program.setUniform( "shading.Kd", shader().Kd );
    m_shader_program.setUniform( "shading.Ks", shader().Ks );
    m_shader_program.setUniform( "shading.S",  shader().S );
    m_shader_program.unbind();
}

/*===========================================================================*/
/**
 *  @brief  Creates buffer objects.
 *  @param  point [in] pointer to the point object
 */
/*===========================================================================*/
void ParticleBasedRenderer::Engine::create_buffer_object( const kvs::PointObject* point )
{
    KVS_ASSERT( point->coords().size() == point->colors().size() );

    kvs::ValueArray<kvs::Real32> coords = point->coords();
    kvs::ValueArray<kvs::UInt8> colors = point->colors();
    kvs::ValueArray<kvs::Real32> normals = point->normals();
    if ( m_enable_shuffle )
    {
        kvs::UInt32 seed = 12345678;
        coords = ::ShuffleArray<3>( point->coords(), seed );
        colors = ::ShuffleArray<3>( point->colors(), seed );
        if ( m_has_normal ) { normals = ::ShuffleArray<3>( point->normals(), seed ); }
    }

    if ( !m_vbo_manager ) m_vbo_manager = new kvs::VertexBufferObjectManager [ repetitionLevel() ];

    const size_t nvertices = point->numberOfVertices();
    const size_t rem = nvertices % repetitionLevel();
    const size_t quo = nvertices / repetitionLevel();

    std::cerr << "ParticleBasedRenderer::Engine::create_buffer_object():" << std::endl;
    std::cerr << "     repeat_level = " << repetitionLevel() << std::endl;
    std::cerr << "           nverts = " << nvertices << std::endl;
    std::cerr << "              rem = " << rem << std::endl;
    std::cerr << "              quo = " << quo << std::endl;
    std::cerr << "     m_has_normal = " << m_has_normal << std::endl;


    for ( size_t i = 0; i < repetitionLevel(); i++ )
    {
        const size_t count = quo + ( i < rem ? 1 : 0 );
        const size_t first = quo * i + kvs::Math::Min( i, rem );

        kvs::VertexBufferObjectManager::VertexBuffer vertex_array;
        vertex_array.type = GL_FLOAT;
        vertex_array.size = count * sizeof( kvs::Real32 ) * 3;;
        vertex_array.dim = 3;
        vertex_array.pointer = coords.data() + first * 3;
        m_vbo_manager[i].setVertexArray( vertex_array );

        kvs::VertexBufferObjectManager::VertexBuffer color_array;
        color_array.type = GL_UNSIGNED_BYTE;
        color_array.size = count * sizeof( kvs::UInt8 ) * 3;
        color_array.dim = 3;
        color_array.pointer = colors.data() + first * 3;
        m_vbo_manager[i].setColorArray( color_array );

        if ( m_has_normal )
        {
            kvs::VertexBufferObjectManager::VertexBuffer normal_array;
            normal_array.type = GL_FLOAT;
            normal_array.size = count * sizeof( kvs::Real32 ) * 3;
            normal_array.dim = 3;
            normal_array.pointer = normals.data() + first * 3;
            m_vbo_manager[i].setNormalArray( normal_array );
        }

        m_vbo_manager[i].create();
    }
}

} // end of glsl

} // end of kvs
