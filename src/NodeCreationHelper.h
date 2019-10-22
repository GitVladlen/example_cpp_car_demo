#include "Kernel/Resource.h"

#include "Engine/HotSpotPolygon.h"
#include "Engine/ShapeQuadFixed.h"
#include "Engine/ResourceImageDefault.h"

namespace Mengine
{
    namespace Helper
    {
        // resources
        ResourceImageDefaultPtr createImageResource( const ConstString & _resourceName, const ConstString & _fileGroupName, const FilePath & _filePath, const mt::vec2f & _maxSize );
        // nodes
        ShapeQuadFixedPtr createSolid( const ConstString & _name, const Color & _color, const mt::vec2f & _size );
        ShapeQuadFixedPtr createSprite( const ConstString & _name, const ResourcePtr & _resource );
        HotSpotPolygonPtr createHotSpot( const ConstString & _name, const mt::vec2f & _size );
        
    }
}
