#include "jgl.h"

namespace jgl {
        Mesh::Mesh(VertexField f) : Resource(BUFFER) {
            build(f);
        }

        const jutil::Queue<VertexFace> &Mesh::getFaces() const {
            return faces;
        }

        jutil::Queue<VertexFace> Mesh::generateFaces(VertexField &points) {
            jutil::Queue<VertexFace> r;
            size_t index = 0;
            while (points.size() > 3) {
                bool found = false;
                jml::Vertex *it;
                for (it = points.begin();; ++it) {
                    //jutil::out << "=============================================================================================" << jutil::endl;
                    if (vertexIsPrincipal(it, &points)) {
                        found = true;
                        break;
                    }
                    if (nextVertex(it, &points) == points.begin()) break;
                }
                if (!found) {
                    jutil::out << index << jutil::endl;
                    getCore()->errorHandler(0xefe, "Attempt to build invalid Mesh.");
                    return {};
                }
                r.insert(VertexFace{*it, *(previousVertex(it, &points)), *(nextVertex(it, &points))});
                points.erase(it);
                ++index;
            }
            r.insert(VertexFace{points[0], points[1], points[2]});
            return r;
        }

        bool Mesh::vertexIsPrincipal(jml::Vertex *vert, VertexField *field) {
            jml::LineSegment testSegment(*previousVertex(vert, field), *nextVertex(vert, field));
            LineMesh lines = connectPoints(*field);
            bool e = jml::endpointIntersectionEnabled();

            jml::Vertex tsm = testSegment.midPoint(), tsmd = {tsm.x() + 1, tsm.y()};

            jml::Ray testRay(tsm, tsmd);

            size_t rayIntersection = 0;

            VertexField endpointsOnRay;
            jml::Vertex inter;

            for (auto &i: lines) {
                jml::setEndpointIntersectionEnabled(true);
                //jutil::out << "Testing between ray {" << testRay.startingPoint() << " -> " << testRay.endingPoint() << "} and line segment {" << i.startingPoint() << " -> " << i.endingPoint() << "}" << jutil::endl;;
                if (testRay.intersects(i, &inter) && (endpointsOnRay.empty() || !endpointsOnRay.find(inter))) {
                    ++rayIntersection;
                    endpointsOnRay.insert(inter);
                    //jutil::out << "\t Hit detected." << jutil::endl;
                } //else jutil::out << "\t No hit detected." << jutil::endl;
                jml::setEndpointIntersectionEnabled(false);
                if (i.intersects(testSegment)) {
                    return false;
                }
            }

            //jutil::out << "No line intersections. Ray " << testRay.startingPoint() << " hit edge " << rayIntersection << " times." << jutil::endl;


            jml::setEndpointIntersectionEnabled(e);


            return (rayIntersection % 2 != 0);
        }

        LineMesh Mesh::connectPoints(VertexField &f) {
            LineMesh lines;
            for (auto it = f.begin();; it = nextVertex(it, &f)) {
                lines.insert(jml::LineSegment(*it, *nextVertex(it, &f)));
                if (nextVertex(it, &f) == &(f.first())) break;
            }
            return lines;
        }

        jml::Vertex *Mesh::nextVertex(jml::Vertex *currentVertex, VertexField *field) {
            if (currentVertex == &(field->last())) {
                return &field->first();
            } else return currentVertex + 1;
        }

        jml::Vertex *Mesh::previousVertex(jml::Vertex *currentVertex, VertexField *field) {
            if (currentVertex == &(field->first())) {
                return &field->last();
            } else return currentVertex - 1;
        }

        Mesh::Mesh() : Resource(BUFFER) {}

        Mesh &Mesh::build(VertexField f) {
            if (!valid()) {
                faces = generateFaces(f);
            } else getCore()->errorHandler(0xefa, "Attempt to re-build immutable type Mesh.");
            if (valid()) {
                generate();
                acquire();
            }
        }

        Resource &Mesh::generate() {
            glGenBuffers(1, &_id);

            vertexData.clear();
            vertexData.reserve(vertexCount() * 4);

            for (auto &i: faces) {
                for (auto &ii: i) {
                    for (auto &iii: ii) {
                        vertexData.insert(iii);
                    }
                }
            }

            glBindBuffer(GL_ARRAY_BUFFER, _id);

            glBufferData(GL_ARRAY_BUFFER, vertexCount() * 4 * sizeof(float), vertexData.begin(), GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        size_t Mesh::vertexCount() const {
            return faces.size() * 3;
        }

        Resource &Mesh::destroy() {
            glDeleteBuffers(1, &_id);
        }

        Mesh::~Mesh() {
            if (valid()) release();
        }

        bool Mesh::valid() const {
            return !faces.empty();
        }
}
