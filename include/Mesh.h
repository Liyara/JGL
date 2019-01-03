#ifndef JGL_MESH_H
#define JGL_MESH_H

#include "Resource.h"
#include <Maths.h>
#include <Container/Map.hpp>
#include <Container/Set.hpp>
#include <Core/NonCopyable.h>

namespace jgl {

    typedef jutil::Queue<jml::Vertex> VertexField;
    typedef jutil::Set<3, jml::Vertex> VertexFace;
    typedef jutil::Queue<jml::LineSegment> LineMesh;

    class Mesh : public Resource, public jutil::NonCopyable {
    public:
        Mesh(VertexField);
        Mesh();

        Mesh &build(const VertexField&);

        bool valid() const;
        size_t vertexCount() const;

        const jutil::Queue<VertexFace> &getFaces() const;


        static jutil::Queue<VertexFace> generateFaces(VertexField);
        static bool vertexIsPrincipal(jml::Vertex*, VertexField*);
        static LineMesh connectPoints(VertexField&);

        static jml::Vertex *nextVertex(jml::Vertex*, VertexField*);
        static jml::Vertex *previousVertex(jml::Vertex*, VertexField*);

        virtual ~Mesh();

    protected:
        Resource &generate() override;
        Resource &destroy() override;

    private:
        jutil::Queue<VertexFace> faces;
        jutil::Queue<float> vertexData;
    };

}

#endif // JGL_MESH_H
