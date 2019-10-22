#include "CarFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "CarFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( CarFramework, Mengine::CarFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    CarFrameworkPlugin::CarFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CarFrameworkPlugin::~CarFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool CarFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "CarFramework" ), Helper::makeFrameworkFactory<CarFramework>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CarFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "CarFramework" ) );
    }
}
