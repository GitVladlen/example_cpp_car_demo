#pragma once

#include "Kernel/Factorable.h"

#include "Kernel/Node.h"

#include "Engine/ShapeQuadFixed.h"

#include "Config/Vector.h"

#include "Tire.h"

#include "Box2D/Box2D.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<TirePtr> VectorPtr;
    //////////////////////////////////////////////////////////////////////////
    class Car
        : public Factorable
    {
    public:
        Car();
        ~Car() override;

    public:
        void initialize( b2World * _world );
        void finalize();

        void update( int _controlState );

    public:
        bool attachToNode( const NodePtr & _node );

        const NodePtr & getNode() const;

    public:
        void moveBy( const mt::vec3f _distance );
        void moveTo( const mt::vec3f _position );
        void angleTo( float _angle );

    protected:
        NodePtr m_node;

        ShapeQuadFixedPtr m_bg;

        VectorizatorPtr m_vectorizator;

    protected:
        b2Body * m_body;
        VectorPtr m_tires;
        b2RevoluteJoint * flJoint, * frJoint;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Car> CarPtr;
    //////////////////////////////////////////////////////////////////////////
}