#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class CarFramework
        : public FrameworkBase
        , public Observable
    {
    public:
        CarFramework();
        ~CarFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
