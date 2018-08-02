#include "Shader.h"
#include "jgl.h"

namespace jgl {

    Shader *Shader::active;

    Shader::Shader() : Shader(0) {}
    Shader::Shader(GLuint p) : frag(0), vert(0), program(p) {}
    Shader::Shader(GLuint v, GLuint f) : frag(f), vert(v), program(createProgram()) {}
    Shader::Shader(const jutil::String &v, const jutil::String &f) : Shader(createShader(GL_VERTEX_SHADER, v), createShader(GL_FRAGMENT_SHADER, f)) {}
    Shader::Shader(GLuint v, const jutil::String &f) : Shader(v, createShader(GL_FRAGMENT_SHADER, f)) {}
    Shader::Shader(const jutil::String &v, GLuint f) : Shader(createShader(GL_VERTEX_SHADER, v), f) {}
    GLuint Shader::getProgram() const {
        return program;
    }
    GLuint Shader::createShader(GLenum eShaderType, const jutil::String &strShaderPath) {

        jutil::String strShaderFile = "", line = "";
        size_t max_file_len = 5000;
        strShaderFile.reserve(max_file_len);
        line.reserve(max_file_len);
        jutil::File shaderFile(strShaderPath, jutil::File::Mode::READ);
        while (!shaderFile.eof()) {
            shaderFile >> line;
            strShaderFile += line;
        }

        char strProto[strShaderFile.size() + 1];
        strShaderFile.array(strProto);
        const char *strFileData = strProto;
        strProto[strShaderFile.size()] = '\0';
        GLuint shader = glCreateShader(eShaderType);
        glShaderSource(shader, 1, &strFileData, NULL);

        glCompileShader(shader);

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint infoLogLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar *strInfoLog = new GLchar[infoLogLength + 1];
            glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

            const char *strShaderType = NULL;
            switch(eShaderType) {
                case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
                case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
                case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
            }

            jutil::err << "Compile failure in " << strShaderType << "shader: \n" << strInfoLog << jutil::endl;
            delete[] strInfoLog;
        }

        return shader;
    }
    GLuint Shader::createProgram() {
        GLuint program = glCreateProgram();

        glAttachShader(program, vert);
        glAttachShader(program, getDefaultVertexShader());
        glAttachShader(program, frag);
        glAttachShader(program, getDefaultFragmentShader());

        glLinkProgram(program);

        GLint status;
        glGetProgramiv (program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            GLint infoLogLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar *strInfoLog = new GLchar[infoLogLength + 1];
            glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
            jutil::err << "Link failure: " << strInfoLog << jutil::endl;
            delete[] strInfoLog;
        }

        glDetachShader(program, vert);
        glDetachShader(program, getDefaultVertexShader());
        glDetachShader(program, frag);
        glDetachShader(program, getDefaultFragmentShader());
        return program;
    }

    bool Shader::hasUniform(const char *cstrName) const {
        return (glGetUniformLocation(program, cstrName) == -1);
    }

    GLuint Shader::getUniformID(const char *cstrName) const {
        return glGetUniformLocation(program, cstrName);
    }

    void Shader::setActive(Shader *s) {
        active = s;
        if (s) glUseProgram(s->program);
        else glUseProgram(0);
    }
    Shader *Shader::getActive() {
        return active;
    }
    Shader::~Shader() {}
    Shader::Shader(const Shader &s) {
        *this = s;
    }
    Shader::Shader(Shader &&s) {
        *this = jutil::move(s);
    }
    Shader &Shader::operator=(const Shader &s) {
        frag = s.frag;
        vert = s.vert;
        program = s.program;
        return *this;
    }
    Shader &Shader::operator=(Shader &&s) {
        frag = s.frag;
        vert = s.vert;
        program = s.program;

        s.frag = 0;
        s.vert = 0;
        s.program = 0;
        return *this;
    }
}
