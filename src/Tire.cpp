#include "Tire.h"

namespace Mengine
{
    Tire::Tire( b2World * world )
    {
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        m_body = world->CreateBody( &bodyDef );

        b2PolygonShape polygonShape;
        polygonShape.SetAsBox( 0.5f, 1.25f );
        b2Fixture * fixture = m_body->CreateFixture( &polygonShape, 1 );//shape, density
        fixture->SetUserData( new CarTireFUD() );

        m_body->SetUserData( this );

        m_currentTraction = 1;
    }
    //////////////////////////////////////////////////////////////////////////
    Tire::~Tire()
    {
        m_body->GetWorld()->DestroyBody( m_body );
    }
    //////////////////////////////////////////////////////////////////////////
    void Tire::initializeView( const NodePtr & _node )
    {
        VectorizatorPtr vectorizator = Helper::generateVectorizator( MENGINE_DOCUMENT_FUNCTION );

        Color colorLine( Helper::makeARGB8( 230, 230, 230, 0 ) );
        Color colorFill( Helper::makeARGB8( 45, 45, 45, 255 ) );

        vectorizator->setLineColor( colorLine );
        vectorizator->setLineWidth( 3.f );

        mt::vec2f halfSize( 0.5f * UNITCOEF, 1.25f * UNITCOEF );

        vectorizator->beginFill( colorFill );
        vectorizator->drawRect( { -halfSize.x, -halfSize.y  }, halfSize.x * 2.f, halfSize.y * 2.f);
        vectorizator->endFill();

        vectorizator->drawCircle( { 0.f, 0.f }, 2.f );

        m_vectorizator = vectorizator;

        _node->addChild( vectorizator );
    }
    //////////////////////////////////////////////////////////////////////////
    void Tire::finilizeView()
    {
        if( m_vectorizator != nullptr )
        {
            m_vectorizator->removeFromParent();
            m_vectorizator->release();

            m_vectorizator = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Tire::updateView()
    {
        b2Vec2 position = m_body->GetPosition();
        float32 angle = m_body->GetAngle();

        m_vectorizator->setLocalPosition( { position.x * UNITCOEF, position.y * UNITCOEF, 0.f } );
        m_vectorizator->setLocalOrientationX( -angle );

        //LOGGER_MESSAGE( "%4.2f %4.2f %4.2f", position.x, position.y, angle );
    }
    //////////////////////////////////////////////////////////////////////////
    void Tire::setCharacteristics( float maxForwardSpeed, float maxBackwardSpeed, float maxDriveForce, float maxLateralImpulse )
    {
        m_maxForwardSpeed = maxForwardSpeed;
        m_maxBackwardSpeed = maxBackwardSpeed;
        m_maxDriveForce = maxDriveForce;
        m_maxLateralImpulse = maxLateralImpulse;
    }
    //////////////////////////////////////////////////////////////////////////
    void Tire::addGroundArea( GroundAreaFUD * ga )
    {
        m_groundAreas.insert( ga ); updateTraction();
    }
    //////////////////////////////////////////////////////////////////////////
    void Tire::removeGroundArea( GroundAreaFUD * ga )
    {
        m_groundAreas.erase( ga ); updateTraction();
    }
    //////////////////////////////////////////////////////////////////////////
    void Tire::updateTraction()
    {
        if( m_groundAreas.empty() )
            m_currentTraction = 1;
        else
        {
            //find area with highest traction
            m_currentTraction = 0;
            std::set<GroundAreaFUD *>::iterator it = m_groundAreas.begin();
            while( it != m_groundAreas.end() )
            {
                GroundAreaFUD * ga = *it;
                if( ga->frictionModifier > m_currentTraction )
                    m_currentTraction = ga->frictionModifier;
                ++it;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    b2Vec2 Tire::getLateralVelocity()
    {
        b2Vec2 currentRightNormal = m_body->GetWorldVector( b2Vec2( 1, 0 ) );
        return b2Dot( currentRightNormal, m_body->GetLinearVelocity() ) * currentRightNormal;
    }
    //////////////////////////////////////////////////////////////////////////
    b2Vec2 Tire::getForwardVelocity()
    {
        b2Vec2 currentForwardNormal = m_body->GetWorldVector( b2Vec2( 0, 1 ) );
        return b2Dot( currentForwardNormal, m_body->GetLinearVelocity() ) * currentForwardNormal;
    }
    //////////////////////////////////////////////////////////////////////////
    void Tire::updateFriction()
    {
        //lateral linear velocity
        b2Vec2 impulse = m_body->GetMass() * -getLateralVelocity();
        if( impulse.Length() > m_maxLateralImpulse )
            impulse *= m_maxLateralImpulse / impulse.Length();
        m_body->ApplyLinearImpulse( m_currentTraction * impulse, m_body->GetWorldCenter(), true );

        //angular velocity
        m_body->ApplyAngularImpulse( m_currentTraction * 0.1f * m_body->GetInertia() * -m_body->GetAngularVelocity(), true );

        //forward linear velocity
        b2Vec2 currentForwardNormal = getForwardVelocity();
        float currentForwardSpeed = currentForwardNormal.Normalize();
        float dragForceMagnitude = -2 * currentForwardSpeed;
        m_body->ApplyForce( m_currentTraction * dragForceMagnitude * currentForwardNormal, m_body->GetWorldCenter(), true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Tire::updateDrive( int controlState )
    {

        //find desired speed
        float desiredSpeed = 0;
        switch( controlState & (TDC_UP | TDC_DOWN) )
        {
        case TDC_UP:   desiredSpeed = m_maxForwardSpeed;  break;
        case TDC_DOWN: desiredSpeed = m_maxBackwardSpeed; break;
        default: return;//do nothing
        }

        //find current speed in forward direction
        b2Vec2 currentForwardNormal = m_body->GetWorldVector( b2Vec2( 0, 1 ) );
        float currentSpeed = b2Dot( getForwardVelocity(), currentForwardNormal );

        //apply necessary force
        float force = 0;
        if( desiredSpeed > currentSpeed )
            force = m_maxDriveForce;
        else if( desiredSpeed < currentSpeed )
            force = -m_maxDriveForce;
        else
            return;
        m_body->ApplyForce( m_currentTraction * force * currentForwardNormal, m_body->GetWorldCenter(), true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Tire::updateTurn( int controlState )
    {
        float desiredTorque = 0;
        switch( controlState & (TDC_LEFT | TDC_RIGHT) )
        {
        case TDC_LEFT:  desiredTorque = 15;  break;
        case TDC_RIGHT: desiredTorque = -15; break;
        default:;//nothing
        }
        m_body->ApplyTorque( desiredTorque, true );
    }
    //////////////////////////////////////////////////////////////////////////
    const b2Vec2 & Tire::getPosition()
    {
        return m_body->GetPosition();
    }
    //////////////////////////////////////////////////////////////////////////
    float32 Tire::getAngle()
    {
        return m_body->GetAngle();
    }
    //////////////////////////////////////////////////////////////////////////
    b2Body * Tire::getBody() const
    {
        return m_body;
    }
}