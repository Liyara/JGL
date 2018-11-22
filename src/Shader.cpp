#include "Shader.h"
#include "jgl.h"

namespace jgl {

    const jutil::String shaderVersion = "440 core";

    Shader *Shader::active;


    ShaderFile::ShaderFile(ShaderFileType t, const jutil::File &file) : Resource(ResourceType::SHADER), fType(t) {
        fileData = "";
        jutil::String line = "";
        size_t max_file_len = 5000;
        fileData.reserve(max_file_len);
        line.reserve(max_file_len);
        while (!file.eof()) {
            file >> line;
            fileData += line;
        }

        generate();
    }
    ShaderFile::ShaderFile(ShaderFileType t, const jutil::String &str) : Resource(ResourceType::SHADER),  fType(t), fileData(str) {
        generate();
    }
    ShaderFile::ShaderFile(const ShaderFile &f) : Resource(f._type, f._id), fType(f.fType), fileData(f.fileData) {
        acquire();
    }
    ShaderFile::~ShaderFile() {
        release();
    }
    Resource &ShaderFile::generate() {
        fileData.replace(jutil::String("#version jgl vertex"), jutil::String(
            "#version " + shaderVersion + "\n"
            "#include jglVertexShader\n"
        ));
        fileData.replace(jutil::String("#include jglVertexShader"), jutil::String(
            "vec4 jglGetVertexInput();\n"
            "vec2 worldToTexture(vec2);\n"
            "vec4 jglVertexShader();\n"
            "float jglGetAspectRatio();\n"
            "vec2 pixelsToScreen(vec2, vec2);\n"
            "vec2 screenToPixels(vec2, vec2);\n"
            "mat4 jglTranslationMatrix(vec2, vec2, vec2);\n"
            "mat4 jglRotationMatrix(float);\n"
            "mat4 jglScalingMatrix(vec2);\n"
            "mat4 jglProjectionMatrix(float);\n"
            "vec4 jglTransformVertex(vec4, mat4, mat4, mat4, mat4);\n"
            "vec2 jglGetTextureCoordinates();\n"
            "layout (std140, binding = 0) uniform JGLVertexDrawData {\n"
            "    float jglObjectRotation;\n"
            "    vec2 jglWindowSize;\n"
            "    vec2 jglObjectPosition;\n"
            "    vec2 jglObjectSize;\n"
            "    vec2 jglObjectOrigin;\n"
            "    vec3 jglCameraPosition;\n"
            "};\n"
        ));
        fileData.replace(jutil::String("#version jgl fragment"), jutil::String(
            "#version " + shaderVersion + "\n"
            "#include jglFragmentShader\n"
        ));
        fileData.replace(jutil::String("#include jglFragmentShader"), jutil::String(
            "vec4 fromTexture(uint, vec2);\n"
            "vec4 jglFragmentApplyLighting(vec4, int);\n"
            "vec4 sampleTextures();\n"
            "vec4 jglFragmentShader();\n"
            "layout (std140, binding = 1) uniform JGLFragmentDrawData {\n"
            "    uint jglTextureCount;\n"
            "    int jglLightingMode;\n"
            "    int jglLightCount;\n"
            "    uint blendingMode;\n"
            "    vec4 jglObjectColor;\n"
            "    vec3 jglCameraNormal;\n"
            "};\n"
        ));

        char strProto[fileData.size() + 1];
        fileData.array(strProto);
        const char *strFileData = strProto;
        strProto[fileData.size()] = '\0';
        _id = glCreateShader(static_cast<GLenum>(fType));
        glShaderSource(_id, 1, &strFileData, NULL);

        glCompileShader(_id);

        GLint status;
        glGetShaderiv(_id, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint infoLogLength;
            glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar *strInfoLog = new GLchar[infoLogLength + 1];
            glGetShaderInfoLog(_id, infoLogLength, NULL, strInfoLog);

            const char *strShaderType = NULL;
            switch(fType) {
                case VERTEX: strShaderType = "vertex"; break;
                case GEOMETRY: strShaderType = "geometry"; break;
                case FRAGMENT: strShaderType = "fragment"; break;
            }

            jutil::err << "Compile failure in " << strShaderType << " shader: \n" << strInfoLog << jutil::endl;
            delete[] strInfoLog;
        }
        acquire();
        return *this;
    }
    Resource &ShaderFile::destroy() {
        glDeleteShader(_id);
        return *this;
    }

    const ShaderFileType &ShaderFile::shaderType() const {
        return fType;
    }

    Shader::Shader(const jutil::Queue<ShaderFile*> &q) : Resource(ResourceType::PROGRAM), files(q) {
        generate();
    }
    Shader::Shader(const Shader &s) :  Resource(s._type, s._id), files(s.files) {
        acquire();
    }

    const jutil::Queue<ShaderFile*> &Shader::getShaderFiles() const {
        return files;
    }
    jutil::Queue<ShaderFile*> Shader::getShaderFiles(ShaderFileType t) const {
        jutil::Queue<ShaderFile*> r;
        r.reserve(files.size());
        for (auto &i: files) {
            if (i->shaderType() == t) r.insert(i);
        }
        return r;
    }


    bool Shader::hasUniform(const char *cstrName) const {
        return (glGetUniformLocation(_id, cstrName) == -1);
    }

    GLuint Shader::getUniformID(const char *cstrName) const {
        return glGetUniformLocation(_id, cstrName);
    }

    void Shader::setActive(Shader *s) {
        active = s;
        if (s) glUseProgram(s->_id);
        else glUseProgram(0);
    }

    Shader *Shader::getActive() {
        return active;
    }

    Shader::~Shader() {
        release();
    }

    Resource &Shader::generate() {
        _id = glCreateProgram();

        glAttachShader(_id, getDefaultVertexShader()->id());
        glAttachShader(_id, getDefaultFragmentShader()->id());

        for (auto &i: files) glAttachShader(_id, i->id());

        glLinkProgram(_id);

        GLint status;
        glGetProgramiv (_id, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            GLint infoLogLength;
            glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar *strInfoLog = new GLchar[infoLogLength + 1];
            glGetProgramInfoLog(_id, infoLogLength, NULL, strInfoLog);
            jutil::err << "Link failure: " << strInfoLog << jutil::endl;
            delete[] strInfoLog;
        }

        glDetachShader(_id, getDefaultVertexShader()->id());
        glDetachShader(_id, getDefaultFragmentShader()->id());

        for (auto &i: files) glDetachShader(_id, i->id());
        acquire();
        return *this;
    }
    Resource &Shader::destroy() {
        glDeleteProgram(_id);
        return *this;
    }
}
