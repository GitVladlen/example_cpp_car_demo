#include "NodeCreationHelper.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Engine/Engine.h"
#include "Engine/SurfaceImage.h"
#include "Engine/SurfaceSolidColor.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/Surface.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/StringHelper.h"
#include "Kernel/EntityHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"


namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ResourceImageDefaultPtr createImageResource( const ConstString & _resourceName, const ConstString & _fileGroupName, const FilePath & _filePath, const mt::vec2f & _maxSize )
        {
            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( _fileGroupName );

            const ConstString & codecType = CODEC_SERVICE()
                ->findCodecType( _filePath );

            mt::vec2f maxSize;

            if( _maxSize.x < 0.f || _maxSize.y < 0.f )
            {
                InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, _filePath, false, MENGINE_DOCUMENT_FUNCTION );

                MENGINE_ASSERTION_MEMORY_PANIC( stream, nullptr );

                ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
                    ->createDecoderT<ImageDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FUNCTION );

                MENGINE_ASSERTION_MEMORY_PANIC( imageDecoder, nullptr );

                if( imageDecoder->prepareData( stream ) == false )
                {
                    return nullptr;
                }

                const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

                maxSize.x = (float)dataInfo->width;
                maxSize.y = (float)dataInfo->height;
            }
            else
            {
                maxSize = _maxSize;
            }

            ResourceImageDefaultPtr resource = Helper::generateResourceImageDefault( MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, nullptr );

            resource->setName( _resourceName );

            ContentInterface * content = resource->getContent();

            content->setFilePath( _filePath );
            content->setFileGroup( fileGroup );
            content->setCodecType( codecType );

            resource->setSize( maxSize );

            mt::uv4f uv_image;
            mt::uv4f uv_alpha;

            if( resource->setup( _filePath, ConstString::none(), uv_image, uv_alpha, maxSize ) == false )
            {
                return nullptr;
            }

            return resource;
        }
        //////////////////////////////////////////////////////////////////////////
        ShapeQuadFixedPtr createSprite( const ConstString & _name, const ResourcePtr & _resource )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _resource, nullptr, "'%s' resource is NULL"
                , _name.c_str()
            );

            SurfaceImagePtr surface = Helper::generateSurfaceImage( MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( surface, nullptr );

            surface->setName( _name );
            surface->setResourceImage( _resource );

            ShapeQuadFixedPtr shape = Helper::generateShapeQuadFixed( MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( shape, nullptr );

            shape->setName( _name );
            shape->setSurface( surface );

            return shape;
        }
        //////////////////////////////////////////////////////////////////////////
        ShapeQuadFixedPtr createSolid( const ConstString & _name, const Color & _color, const mt::vec2f & _size )
        {
            SurfaceSolidColorPtr surface = Helper::generateSurfaceSolidColor( MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( surface, nullptr );

            surface->setName( _name );

            surface->setSolidColor( _color );
            surface->setSolidSize( _size );

            ShapeQuadFixedPtr shape = Helper::generateShapeQuadFixed( MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( shape, nullptr );

            shape->setName( _name );
            shape->setSurface( surface );

            return shape;
        }
        /////////////////////////////////////////////////////////////////////////
        HotSpotPolygonPtr createHotSpot( const ConstString & _name, const mt::vec2f & _size )
        {
            HotSpotPolygonPtr hotspot = Helper::generateHotSpotPolygon( MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( hotspot, nullptr );

            hotspot->setName( _name );

            Polygon polygon;

            polygon.append( { 0.f, 0.f } );
            polygon.append( { _size.x, 0.f } );
            polygon.append( { _size.x, _size.y } );
            polygon.append( { 0.f, _size.y } );

            hotspot->setPolygon( polygon );

            return hotspot;
        }
    }
}
