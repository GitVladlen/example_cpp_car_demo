#include "Car.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Engine/Engine.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"

#include "NodeCreationHelper.h"

namespace Mengine
{
    Car::Car()
        : m_body( nullptr )
        , flJoint( nullptr )
        , frJoint( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Car::~Car()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Car::initialize( b2World * _world )
    {
        // node
        NodePtr node = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Node" ), MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_FATAL( node != nullptr );

        m_node = node;

        //create car body
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        m_body = _world->CreateBody( &bodyDef );
        m_body->SetAngularDamping( 3.f );

        b2Vec2 vertices[8];
        vertices[0].Set( 1.5f, 0.f );
        vertices[1].Set( 3.f, 2.5f );
        vertices[2].Set( 2.8f, 5.5f );
        vertices[3].Set( 1.f, 10.f );
        vertices[4].Set( -1.f, 10.f );
        vertices[5].Set( -2.8f, 5.5f );
        vertices[6].Set( -3.f, 2.5f );
        vertices[7].Set( -1.5f, 0.f );
        b2PolygonShape polygonShape;
        polygonShape.Set( vertices, 8 );
        m_body->CreateFixture( &polygonShape, 0.1f );//shape, density

        //prepare common joint parameters
        b2RevoluteJointDef jointDef;
        jointDef.bodyA = m_body;
        jointDef.enableLimit = true;
        jointDef.lowerAngle = 0.f;
        jointDef.upperAngle = 0.f;
        jointDef.localAnchorB.SetZero();//center of tire

        float maxForwardSpeed = 250.f;
        float maxBackwardSpeed = -40.f;
        float backTireMaxDriveForce = 300.f;
        float frontTireMaxDriveForce = 500.f;
        float backTireMaxLateralImpulse = 8.5f;
        float frontTireMaxLateralImpulse = 7.5f;

        //back left tire
        TirePtr tire = Helper::makeFactorableUnique<Tire>( _world );
        tire->setCharacteristics( maxForwardSpeed, maxBackwardSpeed, backTireMaxDriveForce, backTireMaxLateralImpulse );
        jointDef.bodyB = tire->getBody();
        jointDef.localAnchorA.Set( -3.f, 0.75f );
        _world->CreateJoint( &jointDef );
        tire->initializeView( m_node );
        m_tires.push_back( tire );

        //back right tire
        tire = Helper::makeFactorableUnique<Tire>( _world );
        tire->setCharacteristics( maxForwardSpeed, maxBackwardSpeed, backTireMaxDriveForce, backTireMaxLateralImpulse );
        jointDef.bodyB = tire->getBody();
        jointDef.localAnchorA.Set( 3.f, 0.75f );
        _world->CreateJoint( &jointDef );
        tire->initializeView( m_node );
        m_tires.push_back( tire );

        //front left tire
        tire = Helper::makeFactorableUnique<Tire>( _world );
        tire->setCharacteristics( maxForwardSpeed, maxBackwardSpeed, frontTireMaxDriveForce, frontTireMaxLateralImpulse );
        jointDef.bodyB = tire->getBody();
        jointDef.localAnchorA.Set( -3.f, 8.5f );
        flJoint = (b2RevoluteJoint *)_world->CreateJoint( &jointDef );
        tire->initializeView( m_node );
        m_tires.push_back( tire );

        //front right tire
        tire = Helper::makeFactorableUnique<Tire>( _world );
        tire->setCharacteristics( maxForwardSpeed, maxBackwardSpeed, frontTireMaxDriveForce, frontTireMaxLateralImpulse );
        jointDef.bodyB = tire->getBody();
        jointDef.localAnchorA.Set( 3.f, 8.5f );
        frJoint = (b2RevoluteJoint *)_world->CreateJoint( &jointDef );
        tire->initializeView( m_node );
        m_tires.push_back( tire );

        // bg
        ResourceImageDefaultPtr resourceImageCar = Helper::createImageResource(
            STRINGIZE_STRING_LOCAL( "Image_Car" )
            , STRINGIZE_STRING_LOCAL( "Downloads" )
            , STRINGIZE_FILEPATH_LOCAL( "images/car.png" )
            , { -1.f, -1.f }
        );

        const mt::vec2f & maxSize = resourceImageCar->getMaxSize();

        ShapeQuadFixedPtr spriteCar = Helper::createSprite( STRINGIZE_STRING_LOCAL( "Sprite_Car" ), resourceImageCar );

        mt::vec3f anchor( maxSize * mt::vec2f( 0.5f, 0.85f ), 0.f );

        spriteCar->setLocalOrigin( anchor );

        m_bg = spriteCar;

        m_node->addChild( spriteCar );

        // vectorizer
        VectorizatorPtr vectorizator = Helper::generateVectorizator( MENGINE_DOCUMENT_FUNCTION );

        Color colorLine( Helper::makeARGB8( 230, 230, 230, 0 ) );

        vectorizator->setLineColor( colorLine );
        vectorizator->setLineWidth( 3.f );

        vectorizator->moveTo( { vertices[7].x * UNITCOEF, vertices[7].y * UNITCOEF } );

        for( const b2Vec2 & vertice : vertices )
        {
            vectorizator->lineTo( { vertice.x * UNITCOEF, vertice.y * UNITCOEF } );
        }

        vectorizator->drawCircle( { 0.f, 0.f }, 2.f );

        m_vectorizator = vectorizator;

        m_node->addChild( vectorizator );
    }
    //////////////////////////////////////////////////////////////////////////
    void Car::finalize()
    {
        m_tires.clear();

        if( m_vectorizator != nullptr )
        {
            m_vectorizator->removeFromParent();
            m_vectorizator->release();

            m_vectorizator = nullptr;
        }

        if( m_bg != nullptr )
        {
            m_bg->removeFromParent();
            m_bg->release();

            m_bg = nullptr;
        }

        if( m_node != nullptr )
        {
            m_node->removeFromParent();
            m_node->release();

            m_node = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Car::update( int _controlState )
    {
        for( VectorPtr::size_type i = 0; i < m_tires.size(); i++ )
        {
            m_tires[i]->updateFriction();
        }
            
        for( VectorPtr::size_type i = 0; i < m_tires.size(); i++ )
        {
            m_tires[i]->updateDrive( _controlState );
        }

        for( VectorPtr::size_type i = 0; i < m_tires.size(); i++ )
        {
            m_tires[i]->updateView();
        }

        //control steering
        float lockAngle = 35.f * DEGTORAD;
        float turnSpeedPerSec = 160.f * DEGTORAD;//from lock to lock in 0.5 sec
        float turnPerTimeStep = turnSpeedPerSec / 60.0f;
        float desiredAngle = 0.f;

        switch( _controlState & (TDC_LEFT | TDC_RIGHT) )
        {
        case TDC_LEFT:  desiredAngle = lockAngle;  break;
        case TDC_RIGHT: desiredAngle = -lockAngle; break;
        default:;//nothing
        }

        float angleNow = flJoint->GetJointAngle();
        float angleToTurn = desiredAngle - angleNow;
        angleToTurn = b2Clamp( angleToTurn, -turnPerTimeStep, turnPerTimeStep );
        float newAngle = angleNow + angleToTurn;
        flJoint->SetLimits( newAngle, newAngle );
        frJoint->SetLimits( newAngle, newAngle );


        b2Vec2 position = m_body->GetPosition();
        float32 angle = m_body->GetAngle();

        m_vectorizator->setLocalPosition( { position.x * UNITCOEF, position.y * UNITCOEF, 0.f } );
        m_vectorizator->setLocalOrientationX( -angle );

        m_bg->setLocalPosition( { position.x * UNITCOEF, position.y * UNITCOEF, 0.f } );
        m_bg->setLocalOrientationX( 180 * DEGTORAD -angle );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Car::attachToNode( const NodePtr & _node )
    {
        if( _node == nullptr || m_node == nullptr )
        {
            return false;
        }

        _node->addChild( m_node );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & Car::getNode() const
    {
        return m_node;
    }
    //////////////////////////////////////////////////////////////////////////
    void Car::moveBy( const mt::vec3f _distance )
    {
        mt::vec3f position = m_node->getLocalPosition();

        position += _distance;

        m_node->setLocalPosition( position );
    }
    //////////////////////////////////////////////////////////////////////////
    void Car::moveTo( const mt::vec3f _position )
    {
        m_node->setLocalPosition( _position );
    }
    //////////////////////////////////////////////////////////////////////////
    void Car::angleTo( float _angle )
    {
        m_node->setLocalOrientationX( _angle );
    }
}