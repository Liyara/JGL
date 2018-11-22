#ifndef JGL_MESH_H
#define JGL_MESH_H

#include "Resource.h"
#include <Maths.h>
#include <Container/Map.hpp>

namespace jgl {

    /*class Vertex;

    typedef jutil::Queue<jml::Vertex> VertexField;
    typedef jutil::Tuple<Vertex, Vertex, Vertex> VertexFace;

    class Vertex : public jml::Vertex {
    public:
        using jml::Vertex::Vertex;
        virtual ~Vertex();
    private:
        jutil::Map<size_t, jutil::Queue<float> > attributes;
    };

    struct Face {
        Face(const VertexField&, size_t = 0);
        Face(const VertexFace&, size_t = 0);

        VertexFace vertices;
    };

    class Mesh {
    public:
        Mesh(VertexField verts) {
            Face face;
            for (size_t i = 0; i < verts.size(); ++i) {
                if (face.vertices.size() < 3) {
                    face.vertices.insert(verts[i]);
                } else {
                    faces.insert(face);
                }
            }
        }
    private:
        jutil::Queue<Face> faces;
    };*/

    /*typedef

    class Mesh {
    public:
        Mesh();
        Mesh(const VertexField&);
        Mesh(const Mesh&);
        Mesh (Mesh&&);

        Mesh &operator=(const Mesh&);
        Mesh &operator=(Mesh&&);

        Mesh &clear();
        Mesh &addVertex(const Vertex&);
        Mesh &addVertex(const Vertex&, size_t);
        Mesh &addVertices(const VertexField&);
        Mesh &addVertices(const VertexField&, size_t);
        Mesh &addProperty(size_t, const jutil::Queue<jutil::Queue<float> >&);
        jutil::Queue<float> getVertexProperty(size_t, size_t);
        Mesh &setVertexProperty(size_t, const jutil::Queue<float>&);

        size_t vertices() const;
        const VertexField &getVertices() const;
    private:
        Resource &generate() override;
        Resource &release() override;
        VertexField vertices;
        jutil::Map<size_t, size_t> propertyIdnentities;
        jutil::Map<Vertex*, jutil::Queue<float> > properties;
    };*/
}

#endif // JGL_MESH_H
