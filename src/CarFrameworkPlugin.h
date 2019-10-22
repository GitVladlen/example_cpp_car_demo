#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class CarFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "CarFramework" )

    public:
        CarFrameworkPlugin();
        ~CarFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}