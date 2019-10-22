#pragma once

#include "Kernel/DummySceneEventReceiver.h"

#include "Engine/ShapeQuadFixed.h"

#include "Config/Vector.h"

#include "GOAP/GOAP.h"

#include "Car.h"

#include "Box2D\Box2D.h"

#include "Tire.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ShapeQuadFixedPtr> VectorShapeQuadFixedPtr;
    typedef Vector<GOAP::ChainPtr> VectorChainPtr;
    //////////////////////////////////////////////////////////////////////////
    class CarEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        CarEventReceiver();
        ~CarEventReceiver() override;

    public:
        void onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        bool setupBackground();
        bool setupCar();

    protected:
        void clearSprites();
    
    protected:
        // Box2D /////////////////////////////////////////////////////////////////
        void initializeBox2D();
        void finalizeBox2D();

        void runBox2D();
        void runBox2DKeysTaskChain();
        void runBox2DWorldTaskChain();

        void onKeyDown( int32_t _key );
        void onKeyUp( int32_t _key );

        void stepBox2D();
        //////////////////////////////////////////////////////////////////////////

    protected:
        Scene * m_scene;

        VectorShapeQuadFixedPtr m_sprites;

        CarPtr m_car;

        VectorChainPtr m_taskChains;
    
    protected:
        // Box2D /////////////////////////////////////////////////////////////////
        b2World * m_world;

        int32_t m_controlState;
        
        TirePtr m_tire;
        //////////////////////////////////////////////////////////////////////////
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<CarEventReceiver> MySceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}