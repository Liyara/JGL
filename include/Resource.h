#ifndef JGL_RESOURCE_H
#define JGL_RESOURCE_H

#include <Core/integers.h>

namespace jgl {

    typedef uint32_t ResourceID;

    enum ResourceType : uint8_t {
        UNINITIALIZED,
        TEXTURE,        // Image.h          0
        SHADER,         // Shader.h         0
        BUFFER,         // Mesh.h           1
        QUERY,          // Query.h          2
        PROGRAM,        // Shader.h         0
        SAMPLER,        //                  4
        SYNC,           // Synchronizer.h   3
        VERTEX_ARRAY,   //                  4
        RENDER_BUFFER,  // Display.h        1
        FRAME_BUFFER    // Screen.h         0

        ,__RESOURCE_TYPE_SIZE__
    };

    ResourceType &operator++(ResourceType&);

    class Resource {
    public:

        Resource(ResourceType, ResourceID = 0);

        virtual Resource &generate() = 0;
        virtual Resource &destroy() = 0;

        bool acquire();
        bool release();

        const ResourceID &id() const;
        const ResourceType &type() const;

        virtual ~Resource();

    protected:
        ResourceID _id;
        const ResourceType _type;
    };
}

#endif // JGL_RESOURCE_H
