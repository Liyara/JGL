#ifndef JGL_SHADER_H
#define JGL_SHADER_H

#include "Dependencies.h"
#include "Resource.h"

#define SHADER_OP_START \
    bool __changed_ = false;\
    Shader *__sAct_;\
    if (getActive() != this) {\
        __sAct_ = getActive();\
        setActive(this);\
        __changed_ = true;\
    }

#define SHADER_OP_END if (__changed_) setActive(__sAct_);

#define JGL_SET_UNIFORM_VALS_STR(type, name, queue) SHADER_OP_START \
    switch(queue.getLength()) {\
    case 1: glUniform1##type(glGetUniformLocation(_id, name), queue[0]); break;\
    case 2: glUniform2##type(glGetUniformLocation(_id, name), queue[0], queue[1]); break;\
    case 3: glUniform3##type(glGetUniformLocation(_id, name), queue[0], queue[1], queue[2]); break;\
    case 4: glUniform4##type(glGetUniformLocation(_id, name), queue[0], queue[1], queue[2], queue[3]); break;\
    default: break;\
    }\
SHADER_OP_END

#define JGL_SET_UNIFORM_MATRIX_STR(size, str, matrix) SHADER_OP_START \
    switch(size) {\
    case 2: glUniformMatrix2fv(glGetUniformLocation(_id, str), 1, GL_TRUE, matrix); break;\
    case 3: glUniformMatrix3fv(glGetUniformLocation(_id, str), 1, GL_TRUE, matrix); break;\
    case 4: glUniformMatrix4fv(glGetUniformLocation(_id, str), 1, GL_TRUE, matrix); break;\
    default: break;\
    } \
SHADER_OP_END

#define JGL_SET_UNIFORM_VALS(type, location, queue) SHADER_OP_START \
    switch(queue.getLength()) {\
    case 1: glUniform1##type(location, queue[0]); break;\
    case 2: glUniform2##type(location, queue[0], queue[1]); break;\
    case 3: glUniform3##type(location, queue[0], queue[1], queue[2]); break;\
    case 4: glUniform4##type(location, queue[0], queue[1], queue[2], queue[3]); break;\
    default: break;\
    } \
SHADER_OP_END

#define JGL_SET_UNIFORM_MATRIX(size, location, matrix) SHADER_OP_START \
    switch(size) {\
    case 2: glUniformMatrix2fv(location, 1, GL_TRUE, matrix); break;\
    case 3: glUniformMatrix3fv(location, 1, GL_TRUE, matrix); break;\
    case 4: glUniformMatrix4fv(location, 1, GL_TRUE, matrix); break;\
    default: break;\
    } \
SHADER_OP_END

namespace jgl {

    enum ShaderFileType : uint32_t {
        FRAGMENT    = 0x8B30,
        GEOMETRY    = 0x8DD9,
        VERTEX      = 0x8B31,
    };

    class ShaderFile : public Resource {
    public:
        ShaderFile(ShaderFileType, const jutil::File&);
        ShaderFile(ShaderFileType, const jutil::String&);
        ShaderFile(const ShaderFile&);

        const ShaderFileType &shaderType() const;

        virtual ~ShaderFile();
    private:
        Resource &generate() override;
        Resource &destroy() override;
        ShaderFileType fType;
        jutil::String fileData;
    };

    class Shader : public Resource {
    public:
        Shader(const jutil::Queue<ShaderFile*>&);
        Shader(const Shader&);

        const jutil::Queue<ShaderFile*> &getShaderFiles() const;
        jutil::Queue<ShaderFile*> getShaderFiles(ShaderFileType) const;

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

        bool hasUniform(const char*) const;
        GLuint getUniformID(const char*) const;

        static void setActive(Shader*);
        static Shader *getActive();
        virtual ~Shader();
    private:
        jutil::Queue<ShaderFile*> files;
        static Shader *active;

        Resource &generate() override;
        Resource &destroy() override;
    };
}

#endif // JGL_SHADER_H
