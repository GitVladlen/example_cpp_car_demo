#include "CarEventReceiver.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Engine/Engine.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/StringHelper.h"

#include "Tasks/TaskGlobalKeyPress.h"
#include "Tasks/TaskTransformationTranslateTime.h"
#include "Tasks/TaskLocalDelay.h"

#include "NodeCreationHelper.h"



namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    CarEventReceiver::CarEventReceiver()
        : m_world( nullptr )

        , m_controlState( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CarEventReceiver::~CarEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void CarEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void CarEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDestroy [%s]"
            , m_scene->getName().c_str()
        );

        for( const GOAP::ChainPtr & tc : m_taskChains )
        {
            tc->cancel();
        }

        m_taskChains.clear();

        this->clearSprites();

        this->finalizeBox2D();
        
        m_scene = nullptr;
    }    
    //////////////////////////////////////////////////////////////////////////
    void CarEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        if( this->setupBackground() == false )
        {   
            LOGGER_ERROR("Fail to setup Background");

            return;
        }

        if( this->setupCar() == false )
        {
            LOGGER_ERROR( "Fail to setup Car" );

            return;
        }

        this->initializeBox2D();
    }
    //////////////////////////////////////////////////////////////////////////
    void CarEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        this->runBox2D();
    }
    //////////////////////////////////////////////////////////////////////////
    bool CarEventReceiver::setupBackground()
    {
        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        mt::vec2f size;

        resolution.calcSize( size );

        Color color( Helper::makeARGB8( 132, 121, 137, 255 ) );

        ShapeQuadFixedPtr spriteBackground = Helper::createSolid( STRINGIZE_STRING_LOCAL( "Sprite_Background" ), color, size );

        m_sprites.emplace_back( spriteBackground );

        m_scene->addChild( spriteBackground );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CarEventReceiver::setupCar()
    {
        

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CarEventReceiver::clearSprites()
    {
        for( const ShapeQuadFixedPtr & sprite : m_sprites )
        {
            sprite->removeFromParent();
            sprite->release();
        }

        m_sprites.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void CarEventReceiver::initializeBox2D()
    {
        b2Vec2 gravity( 0.0f, 0.0f );

        m_world = new b2World( gravity );

        //m_tire = Helper::makeFactorableUnique<Tire>( m_world );

        //if( m_tire == nullptr )
        //{
        //    LOGGER_ERROR( "Fail to create tire" );

        //    return;
        //}

        //m_tire->setCharacteristics( 100.f, -20.f, 150.f, 4.f );

        //m_tire->initializeView( NodePtr::from( m_scene ) );

        CarPtr car = Helper::makeFactorableUnique<Car>();

        if( car == nullptr )
        {
            LOGGER_ERROR( "Fail to create car" );

            return;
        }

        m_car = car;

        m_car->initialize( m_world );

        bool isAttached = m_car->attachToNode( NodePtr::from( m_scene ) );

        if( isAttached == false )
        {
            LOGGER_ERROR( "Fail to attach car node to scene" );

            return ;
        }

        m_controlState = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void CarEventReceiver::finalizeBox2D()
    {
        if( m_tire != nullptr )
        {
            m_tire->finilizeView();

            m_tire = nullptr;
        }

        if( m_car != nullptr )
        {
            m_car->finalize();

            m_car = nullptr;
        }

        if( m_world != nullptr )
        {
            // By deleting the world, we delete the bomb, mouse joint, etc.
            delete m_world;

            m_world = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void CarEventReceiver::runBox2D()
    {
        LOGGER_MESSAGE( "RUN BOX2D" );

        this->runBox2DKeysTaskChain();
        
        this->runBox2DWorldTaskChain();
    }
    //////////////////////////////////////////////////////////////////////////
    void CarEventReceiver::runBox2DKeysTaskChain()
    {
        GOAP::SourcePtr source = GOAP::Helper::makeSource();

        auto parallels = source->addParallel<4>();

        parallels[0]->addWhile( [this]( const GOAP::SourcePtr _scope_left )
        {
            _scope_left->addTask<TaskGlobalKeyPress>( KC_LEFT, true, nullptr );
            _scope_left->addFunction( this, &CarEventReceiver::onKeyDown, TDC_RIGHT );

            _scope_left->addTask<TaskGlobalKeyPress>( KC_LEFT, false, nullptr );
            _scope_left->addFunction( this, &CarEventReceiver::onKeyUp, TDC_RIGHT );

            return true;
        } );

        parallels[1]->addWhile( [this]( const GOAP::SourcePtr _scope_right )
        {
            _scope_right->addTask<TaskGlobalKeyPress>( KC_RIGHT, true, nullptr );
            _scope_right->addFunction( this, &CarEventReceiver::onKeyDown, TDC_LEFT );

            _scope_right->addTask<TaskGlobalKeyPress>( KC_RIGHT, false, nullptr );
            _scope_right->addFunction( this, &CarEventReceiver::onKeyUp, TDC_LEFT );

            return true;
        } );

        parallels[2]->addWhile( [this]( const GOAP::SourcePtr _scope_up )
        {
            _scope_up->addTask<TaskGlobalKeyPress>( KC_UP, true, nullptr );
            _scope_up->addFunction( this, &CarEventReceiver::onKeyDown, TDC_UP );

            _scope_up->addTask<TaskGlobalKeyPress>( KC_UP, false, nullptr );
            _scope_up->addFunction( this, &CarEventReceiver::onKeyUp, TDC_UP );

            return true;
        } );

        parallels[3]->addWhile( [this]( const GOAP::SourcePtr _scope_down )
        {
            _scope_down->addTask<TaskGlobalKeyPress>( KC_DOWN, true, nullptr );
            _scope_down->addFunction( this, &CarEventReceiver::onKeyDown, TDC_DOWN );

            _scope_down->addTask<TaskGlobalKeyPress>( KC_DOWN, false, nullptr );
            _scope_down->addFunction( this, &CarEventReceiver::onKeyUp, TDC_DOWN );

            return true;
        } );

        GOAP::ChainPtr chain = GOAP::Helper::makeChain( source );
        chain->run();

        m_taskChains.push_back( chain );
    }
    //////////////////////////////////////////////////////////////////////////
    void CarEventReceiver::runBox2DWorldTaskChain()
    {
        GOAP::SourcePtr source = GOAP::Helper::makeSource();

        source->addWhile( [this]( const GOAP::SourcePtr _scope )
        {
            _scope->addFunction( this, &CarEventReceiver::stepBox2D );
            _scope->addTask<TaskLocalDelay>( 0.f );

            return true;
        } );

        GOAP::ChainPtr chain = GOAP::Helper::makeChain( source );
        chain->run();

        m_taskChains.push_back( chain );
    }
    //////////////////////////////////////////////////////////////////////////
    void CarEventReceiver::onKeyDown( int32_t _key )
    {
        m_controlState |= _key;
    }
    //////////////////////////////////////////////////////////////////////////
    void CarEventReceiver::onKeyUp( int32_t _key )
    {
        m_controlState &= ~_key;
    }
    //////////////////////////////////////////////////////////////////////////
    void CarEventReceiver::stepBox2D()
    {
        // update tire
        //m_tire->updateFriction();
        //m_tire->updateDrive( m_controlState );
        //m_tire->updateTurn( m_controlState );

        //m_tire->updateView();

        m_car->update( m_controlState );

        // step
        float32 timeStep = 1.0f / 60.0f;
        int32 velocityIterations = 6;
        int32 positionIterations = 2;

        // Instruct the world to perform a single step of simulation.
        // It is generally best to keep the time step and iterations fixed.
        m_world->Step( timeStep, velocityIterations, positionIterations );       
    }
};