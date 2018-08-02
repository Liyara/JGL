#ifndef JGL_SHADER_H
#define JGL_SHADER_H

#include "Dependencies.h"

#define JGL_SET_UNIFORM_VALS_STR(type, name, queue) switch(queue.getLength()) {\
    case 1: glUniform1##type(glGetUniformLocation(program, name), queue[0]); break;\
    case 2: glUniform2##type(glGetUniformLocation(program, name), queue[0], queue[1]); break;\
    case 3: glUniform3##type(glGetUniformLocation(program, name), queue[0], queue[1], queue[2]); break;\
    case 4: glUniform4##type(glGetUniformLocation(program, name), queue[0], queue[1], queue[2], queue[3]); break;\
    default: break;\
}

#define JGL_SET_UNIFORM_MATRIX_STR(size, str, matrix) switch(size) {\
    case 2: glUniformMatrix2fv(glGetUniformLocation(program, str), 1, GL_TRUE, matrix); break;\
    case 3: glUniformMatrix3fv(glGetUniformLocation(program, str), 1, GL_TRUE, matrix); break;\
    case 4: glUniformMatrix4fv(glGetUniformLocation(program, str), 1, GL_TRUE, matrix); break;\
    default: break;\
}

#define JGL_SET_UNIFORM_VALS(type, location, queue) switch(queue.getLength()) {\
    case 1: glUniform1##type(location, queue[0]); break;\
    case 2: glUniform2##type(location, queue[0], queue[1]); break;\
    case 3: glUniform3##type(location, queue[0], queue[1], queue[2]); break;\
    case 4: glUniform4##type(location, queue[0], queue[1], queue[2], queue[3]); break;\
    default: break;\
}

#define JGL_SET_UNIFORM_MATRIX(size, location, matrix) switch(size) {\
    case 2: glUniformMatrix2fv(location, 1, GL_TRUE, matrix); break;\
    case 3: glUniformMatrix3fv(location, 1, GL_TRUE, matrix); break;\
    case 4: glUniformMatrix4fv(location, 1, GL_TRUE, matrix); break;\
    default: break;\
}

namespace jgl {
    class Shader {
    public:
        Shader();
        Shader(GLuint);
        Shader(GLuint, GLuint);
        Shader(const jutil::String&, const jutil::String&);
        Shader(GLuint, const jutil::String&);
        Shader(const jutil::String&, GLuint);
        Shader(const Shader&);
        Shader(Shader&&);
        Shader &operator=(const Shader&);
        Shader &operator=(Shader&&);

        template <typename T>
        Shader &setUniform(const char *str, const T &v) {
            return setUniform(str, jml::Vector<T, 1>({v}));
        }

        template <size_t s, typename T>
        Shader &setUniform(const char *str, const jml::Vector<T, s> &v) {
            if constexpr(jutil::IsFloatingPoint<T>::Value) {
                if constexpr(sizeof(T) <= 4) {
                    JGL_SET_UNIFORM_VALS_STR(f, str, v);
                } else {
                    JGL_SET_UNIFORM_VALS_STR(d, str, v);
                }
            } else {
                if constexpr(jutil::IsUnsigned<T>::Value) {
                    JGL_SET_UNIFORM_VALS_STR(ui, str, v);
                } else {
                    JGL_SET_UNIFORM_VALS_STR(i, str, v);
                }
            }
            return *this;
        }

        template<size_t s>
        Shader &setUniform(const char *str, jml::Matrix<float, s, s> &m) {
            float mat[s * s];
            m.array(mat);
            JGL_SET_UNIFORM_MATRIX_STR(s, str, mat);
            return *this;
        }

        template <typename T>
        Shader &setUniform(GLuint location, const T &v) {
            return setUniform(location, jml::Vector<T, 1>({v}));
        }

        template <size_t s, typename T>
        Shader &setUniform(GLuint location, const jml::Vector<T, s> &v) {
            if constexpr(jutil::IsFloatingPoint<T>::Value) {
                if constexpr(sizeof(T) <= 4) {
                    JGL_SET_UNIFORM_VALS(f, location, v);
                } else {
                    JGL_SET_UNIFORM_VALS(d, location, v);
                }
            } else {
                if constexpr(jutil::IsUnsigned<T>::Value) {
                    JGL_SET_UNIFORM_VALS(ui, location, v);
                } else {
                    JGL_SET_UNIFORM_VALS(i, location, v);
                }
            }
            return *this;
        }

        template<size_t s>
        Shader &setUniform(GLuint location, jml::Matrix<float, s, s> &m) {
            float mat[s * s];
            m.array(mat);
            JGL_SET_UNIFORM_MATRIX(s, location, mat);
            return *this;
        }

        static void setActive(Shader*);
        static Shader *getActive();

        bool hasUniform(const char*) const;
        GLuint getUniformID(const char*) const;

        GLuint getProgram() const;
        GLuint getFragmentShader() const;
        GLuint getVertexShader() const;

        virtual ~Shader();
    private:

        GLuint createShader(GLenum, const jutil::String&);
        GLuint createProgram();
        GLuint frag, vert, program;
        static Shader *active;

    };
}

#endif // JGL_SHADER_H
