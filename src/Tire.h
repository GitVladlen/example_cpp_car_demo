#pragma once

#include "Engine/Engine.h"
#include "Engine/Vectorizator.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/Factorable.h"

#include "Box2D/Box2D.h"

#include <set>

#ifndef DEGTORAD
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define UNITCOEF 23.f
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum
    {
        TDC_LEFT = 0x1,
        TDC_RIGHT = 0x2,
        TDC_UP = 0x4,
        TDC_DOWN = 0x8
    };
    //////////////////////////////////////////////////////////////////////////
    //types of fixture user data
    enum fixtureUserDataType
    {
        FUD_CAR_TIRE,
        FUD_GROUND_AREA
    };

    //a class to allow subclassing of different fixture user data
    class FixtureUserData
    {
        fixtureUserDataType m_type;
    protected:
        FixtureUserData( fixtureUserDataType type ) : m_type( type )
        {
        }
    public:
        virtual fixtureUserDataType getType()
        {
            return m_type;
        }
        virtual ~FixtureUserData()
        {
        }
    };

    //class to allow marking a fixture as a car tire
    class CarTireFUD : public FixtureUserData
    {
    public:
        CarTireFUD() : FixtureUserData( FUD_CAR_TIRE )
        {
        }
    };

    //class to allow marking a fixture as a ground area
    class GroundAreaFUD : public FixtureUserData
    {
    public:
        float frictionModifier;
        bool outOfCourse;

        GroundAreaFUD( float fm, bool ooc ) : FixtureUserData( FUD_GROUND_AREA )
        {
            frictionModifier = fm;
            outOfCourse = ooc;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    class Tire
        : public Factorable
    {
    public:
        Tire( b2World * world );
        ~Tire() override;

    public:
        void initializeView( const NodePtr & _node );
        void finilizeView();
        void updateView();

    public:
        void setCharacteristics( float maxForwardSpeed, float maxBackwardSpeed, float maxDriveForce, float maxLateralImpulse );

        void addGroundArea( GroundAreaFUD * ga );
        void removeGroundArea( GroundAreaFUD * ga );

        void updateTraction();

        b2Vec2 getLateralVelocity();
        b2Vec2 getForwardVelocity();

        void updateFriction();
        void updateDrive( int controlState );
        void updateTurn( int controlState );

        const b2Vec2 & getPosition();
        float32 getAngle();

        b2Body * getBody() const;

    protected:
        b2Body * m_body;
        float m_maxForwardSpeed;
        float m_maxBackwardSpeed;
        float m_maxDriveForce;

        float m_maxLateralImpulse;
        std::set<GroundAreaFUD *> m_groundAreas;
        float m_currentTraction;

        VectorizatorPtr m_vectorizator;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Tire> TirePtr;
    //////////////////////////////////////////////////////////////////////////
}