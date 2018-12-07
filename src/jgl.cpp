#include "jgl.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#include <GLFW/glfw3native.h>

namespace jgl {

    /*const char *FRAGMENT_SHADER = R"glsl(
        #version 400 core

        #define M_PI 3.1415926535897932384626433832795

        #define JGL_BLENDMODE_STACK     0u
        #define JGL_BLENDMODE_COMBINE   1u
        #define JGL_BLENDMODE_MAP       2u

        #define JGL_CHANNEL_NONE        0u
        #define JGL_CHANNEL_RED         1u
        #define JGL_CHANNEL_GREEN       2u
        #define JGL_CHANNEL_BLUE        3u
        #define JGL_CHANNEL_ALPHA       4u

        //in vec2 jglTexCoords0[24];
        //in vec2 jglTexCoords1[24];

        in vec2 jglVertexAsTexture;

        /*#load textures

        #ifdef JGL_BINDLESS_TEXTURES

            #extension GL_ARB_bindless_texture : require

        #elif defined JGL_BOUND_TEXTURES

            #define NUM_TEX_ARRAYS JGL_BOUND_TEXTURES / 12
            #define NUM_SEGMENTS NUM_TEX_ARRAYS * 3

            uniform sampler2D minimumTextureSlots[12];

            #if NUM_TEX_ARRAYS > 1
                uniform sampler2D standardTextureSlots[12];
            #endif

            #if NUM_TEX_ARRAYS > 2
                uniform sampler2D extendedTextureSlots[12];
            #endif

            #if NUM_TEX_ARRAYS > 3
                uniform sampler2D maximumTextureSlots[12];
            #endif

            uniform sampler2D maps[NUM_SEGMENTS];

            vec4 getTextureFragment(uint i, vec2 coords) {
                #if NUM_TEX_ARRAYS == 1
                    return texture(minimumTextureSlots[i], coords);
                #elif NUM_TEX_ARRAYS > 1
                    if (i < 24u) return texture(minimumTextureSlots[i], coords);
                    else if (i < 24u) return texture(standardTextureSlots[i - 12u], coords);
                    #if NUM_TEX_ARRAYS > 2
                        else if (i < 36u) return texture(extendedTextureSlots[i - 24u], coords);
                        #if NUM_TEX_ARRAYS > 3
                            else return texture(maximumTextureSlots[i - 36u], coords);
                        #else
                            else return vec4(1.0);
                        #endif
                    #else
                        else return vec4(1.0);
                    #endif
                #endif
            }

        #else

            #error "Unsupported HW!"

        #endif | /

        struct LightSource {
            vec2 pos;
            float intensity;
            vec3 c;
        };

        struct Material {
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            float shine;
        };

        /*struct TextureLayer {
            vec2 position;
            vec2 size;
            vec2 imageSize;
            vec2 factor;
            uvec2 controller;
            float rotation;
            uint vertexMode;
            uint mapChannel;
        };

        struct TextureSegment {
            TextureLayer layers[4];
            uint sep;
            uint link;
            uint connect;
        }; | /

        uniform Material material;


        ///MAX 128 lights in scene
        uniform LightSource lights[128];

        layout (std140) uniform JGLVertexDrawData {
            float jglObjectRotation;
            vec2 jglWindowSize;
            vec2 jglObjectPosition;
            vec2 jglObjectSize;
            vec2 jglObjectOrigin;
            vec3 jglCameraPosition;
        };

        layout (std140) uniform JGLFragmentDrawData {
            uint jglTextureCount;
            int jglLightingMode;
            int jglLightCount;
            uint blendingMode;
            vec4 jglObjectColor;
            vec3 jglCameraNormal;
        };

        /*layout (std140) uniform TextureSegmentData {
            TextureSegment segments[0xf];
        };

        #ifdef JGL_BINDLESS_TEXTURES

        layout (std140) uniform TexturePool {
            sampler2D bindlessTextures[JGL_BINDLESS_TEXTURES];
            sampler2D maps[JGL_BINDLESS_TEXTURES / 4];
        };

        vec4 getTextureFragment(uint i, vec2 coords) {
            return texture(bindlessTextures[i], coords);
        }

        #endif

        vec4 jglFragmentApplyLighting(vec4 initColor, int lightingMode) {

            float rand = gl_FragCoord.x * 353.0 + gl_FragCoord.y * 769.0;
            float noise = fract(rand / 991.0) / 256.0;
            vec3 begColor;
            begColor.rgb = vec3(0.0, 0.0, 0.0);
            float ambience = 0.02;
            vec3 ambientLight = ambience * material.ambient;
            for (int i = 0; i < jglLightCount; ++i) {

                vec3 passColor;
                passColor.rgb = vec3(initColor.r, initColor.g, initColor.b);
                vec3 fc = vec3(gl_FragCoord.x - (jglWindowSize.x / 2.0), gl_FragCoord.y - (jglWindowSize.y / 2.0), gl_FragCoord.z);

                vec3 lightPos = vec3(lights[i].pos, 1.0f);
                lightPos.x += jglCameraPosition.x;
                lightPos.y += jglCameraPosition.y;
                vec3 lc = lights[i].c;

                float mR = ambience + noise;

                lc.r = max(lc.r, mR);
                lc.g = max(lc.g, mR);
                lc.b = max(lc.b, mR);


                float intensity = lights[i].intensity * 100;
                float iA = 1 / sqrt(intensity);
                float iB = 1 / intensity;
                float d = distance(lightPos, fc);
                float attenuation = 1.0 / (1.0 + (iA * d) + (iB * pow(d, 2)));
                vec3 diffuse = lc * (attenuation * initColor.rgb);

                vec3 viewDir = normalize(vec3(lightPos.x, lightPos.y, 1.0) - fc);
                //vec3 lightDir = normalize(lightPos - fc);
                //vec3 reflectDir = reflect(-lightDir, jglCameraNormal);
                float spec = pow(max(dot(viewDir, jglCameraNormal), 0.0),  max(1.0 - material.shine, 0.0));
                vec3 specular = lc * (spec * material.specular);

                vec3 finalColor;

                if (lightingMode == 2) {
                    finalColor = (specular + noise) + (diffuse + noise);
                } else if (lightingMode == 1) {
                    float t = 0.5;
                    float m1 = sqrt(pow(passColor.r, 2) + pow(passColor.g, 2) + pow(passColor.b, 2));
                    float m2 = sqrt(pow(lc.r, 2) + pow(lc.g, 2) + pow(lc.b, 2));
                    float m = sqrt(pow(m1, 2) + pow(m2, 2));
                    vec3 baseColor;
                    baseColor.rgb = vec3(
                        sqrt((1 - t) * pow(passColor.r, 2) + t * pow(lc.r, 2)) * m,
                        sqrt((1 - t) * pow(passColor.g, 2) + t * pow(lc.g, 2)) * m,
                        sqrt((1 - t) * pow(passColor.b, 2) + t * pow(lc.b, 2)) * m
                    );
                    finalColor = (baseColor * diffuse);
                }

                begColor += finalColor;
            }

            return vec4(((ambientLight + noise) + begColor), initColor.a);

        }

        vec2 getTextureVertex(uint i) {
            if (i < 24) {
                return jglTexCoords0[i];
            } else {
                return jglTexCoords1[i - 24];
            }
        }

        vec4 blendTexels(vec4 texelA, vec4 texelB, uint blender) {
            switch (blender) {
            case JGL_BLENDMODE_STACK:
                float alpha = 1.0 - ((1.0 - texelA.a) * (1.0 - texelB.a));
                vec3 rgb = (texelA.rgb * (1.0 - texelB.a)) + (texelB.rgb * texelB.a);
                return vec4(rgb, alpha);
            case JGL_BLENDMODE_COMBINE:
                return (texelA + texelB) / 2.0;
            }
        }

        vec4 blendSegment(vec4 samples[4], uint filled, uint blendingMode) {
            float light = 1.0;
            vec3 lightRGB = samples[0].rgb;
            vec4 additiveFrag = vec4(0.0);
            for (uint i = 0; i < filled; ++i) {
                light *= 1.0 - samples[i].a;
                additiveFrag += samples[i];
                if (i > 0u) {
                    lightRGB *= 1.0 - samples[i].a;
                    lightRGB += samples[i].rgb * samples[i].a;
                }
            }
            if (blendingMode == JGL_BLENDMODE_STACK) return vec4(lightRGB, 1.0 - light);
            else if (blendingMode == JGL_BLENDMODE_COMBINE) return additiveFrag / filled;
        }

        vec4 sampleTextures() {
            //phease 0
            uint tIndex = 0;
            vec4 results[12];
            vec4 finalTexel;
            uint segmentsLoaded = 0;
            uint textured = 0u;
            vec2 texCoords;
            for (uint i = 0; i < 12; ++i) {
                vec4 samples[4];
                uint slotsLoaded = 0;
                for (uint ii = 0; ii < 4; ++ii) {
                    if (tIndex >= jglTextureCount) break;
                    texCoords = getTextureVertex(tIndex);
                    if (texCoords.x > -0.0001 && texCoords.y > -0.0001 && texCoords.x < 1.0001 && texCoords.y < 1.0001) textured = 1u;
                    samples[ii] = getTextureFragment(tIndex, fract(getTextureVertex(tIndex)));
                    ++slotsLoaded;
                    ++tIndex;
                    if (tIndex == segments[i].sep) break;
                }
                if (slotsLoaded > 0u) {
                    if (segments[i].link < JGL_BLENDMODE_MAP) results[i] = blendSegment(samples, slotsLoaded, segments[i].link);
                    else {
                        vec4 mapSample = texture(maps[segmentsLoaded], jglVertexAsTexture);
                        results[i].rgb = (samples[0].rgb * mapSample.r) + (samples[1].rgb * mapSample.g) + (samples[2].rgb * mapSample.b);
                        results[i].a = mapSample.a;
                    }
                    ++segmentsLoaded;
                } else break;
            }

            //phase 1
            if (textured == 0u) return jglObjectColor;
            if (segmentsLoaded > 0u) {
                finalTexel = results[0];
                for (uint i = 1; i < segmentsLoaded; ++i) {
                    finalTexel = blendTexels(finalTexel, results[i], segments[i].connect);
                }
                return finalTexel;
            } else return jglObjectColor;
        } | /

        vec4 jglFragmentShader() {

            /*vec4 fragColor;
            if (jglTextureCount > 0u) fragColor = sampleTextures();
            else fragColor = jglObjectColor;

            return fragColor; | /

            //return texture(bindlessTextures[0], jglVertexAsTexture);

            return vec4(1.0);

        }

    )glsl";

    const char *VERTEX_SHADER = R"glsl(
        #version 400 core

        #define JGL_VERTEXMODE_MANUAL       0u
        #define JGL_VERTEXMODE_RELATIVE     1u
        #define JGL_VERTEXMODE_ABSOLUTE     2u

        #define JGL_MAXIMUM_TEXTURE_UNITS   48u

        #define JGL_SIZE_CONTROLLER         0u
        #define EXTERNAL_SIZE_CONTROLLER    1u

        layout(location=0) in vec4 jglVertexCoordInput;
        layout(location=1) in vec2 jglTexCoordInput;

        //out vec2 jglTexCoords0[24];
        //out vec2 jglTexCoords1[24];

        out vec4 jglCoords;
        out vec2 jglVertexAsTexture;

        /*struct TextureLayer {
            vec2 position;
            vec2 size;
            vec2 imageSize;
            vec2 factor;
            uvec2 controller;
            float rotation;
            uint vertexMode;
            uint mapChannel;
        };

        struct TextureSegment {
            TextureLayer layers[4];
            uint sep;
            uint link;
            uint connect;
        }; | /

        layout (std140) uniform JGLVertexDrawData {
            float jglObjectRotation;
            vec2 jglWindowSize;
            vec2 jglObjectPosition;
            vec2 jglObjectSize;
            vec2 jglObjectOrigin;
            vec3 jglCameraPosition;
        };

        layout (std140) uniform JGLFragmentDrawData {
            uint jglTextureCount;
            int jglLightingMode;
            int jglLightCount;
            uint blendingMode;
            vec4 jglObjectColor;
            vec3 jglCameraNormal;
        };

       /* layout (std140) uniform TextureSegmentData {
            TextureSegment segments[0xf];
        }; | /

        vec4 jglGetVertexInput() {
            return jglVertexCoordInput;
        }

        vec2 worldToTexture(vec2 vA) {
            return vec2((vA.x / 2.0f) + 0.5f, ((vA.y / 2.0f) + 0.5f) + (vA.y * -1.0f));
        }

        vec2 textureToWorld(vec2 vA) {
            return vec2((vA.x * 2.0f) - 1.0f, ((vA.y * 2) - 1) * -1);
        }

        float jglGetAspectRatio() {
            return jglWindowSize.x / jglWindowSize.y;
        }

        vec2 pixelsToScreen(vec2 pixels, vec2 screen) {
            return vec2(pixels.x / screen.x, pixels.y / screen.y);
        }

        vec2 screenToPixels(vec2 screen, vec2 window) {
            return vec2(screen.x * window.x, screen.y * window.y);
        }

        mat4 jglTranslationMatrix(vec2 offset, vec2 cameraTranslation, vec2 screenSize) {

            float yRatio = screenSize.y / (screenSize.x / 2.0);

            return mat4(
                vec4(1, 0, 0, (offset.x + cameraTranslation.x) * 2),
                vec4(0, 1, 0, -(offset.y + cameraTranslation.y) * yRatio),
                vec4(0, 0, 1, 0),
                vec4(0, 0, 0, 1)
            );

        }

        mat4 jglRotationMatrix(float rotation) {
            return mat4(
                vec4(cos(-rotation), -sin(-rotation), 0, 0),
                vec4(sin(-rotation), cos(-rotation), 0, 0),
                vec4(0, 0, 1, 0),
                vec4(0, 0, 0, 1)
            );
        }

        mat4 jglScalingMatrix(vec2 size) {
            return mat4(
                vec4(size.x, 0, 0, 0),
                vec4(0, size.y, 0, 0),
                vec4(0, 0, 1, 0),
                vec4(0, 0, 0, 1)
            );
        }

        mat4 jglProjectionMatrix(float aspect) {
            return mat4(
                vec4(1, 0, 0, 0),
                vec4(0, 2.0 / (aspect + aspect), 0, 0),
                vec4(0, 0, -1, 0),
                vec4(0, 0, 0, 1)
            );
        }

        vec4 jglTransformVertex(vec4 vertex, mat4 projection, mat4 scaling, mat4 rotation, mat4 translation) {

            vec2 origin = -pixelsToScreen(jglObjectOrigin * (jglObjectSize / 2.0), jglWindowSize);

            return vertex * projection * scaling * jglTranslationMatrix(origin, vec2(0), jglWindowSize) * rotation * translation;
        }

        float getAspectRatio(vec2 screen) {
            return screen.x / screen.y;
        }

        /*vec2 jglGetTextureCoordinates(uint id) {

            TextureLayer layer = segments[id / 4].layers[id % 4];

            vec2 rawImageSize   = layer.imageSize;
            vec2 setSize        = layer.size;
            vec2 objectSize     = jglObjectSize;
            vec2 scaler         = layer.factor;
            vec2 size           = vec2(1.0);
            vec2 sizeRatio      = rawImageSize / objectSize;

            float rotation  = layer.rotation;
            float oAspect   = getAspectRatio(objectSize);
            vec2 position   = layer.position;

            uint mode               = layer.vertexMode;
            uvec2 sizeController    = layer.controller;

            vec2 transformedVertex;
            vec2 vertex;

            if (mode == JGL_VERTEXMODE_MANUAL) {
                size = objectSize;
            } else if (mode == JGL_VERTEXMODE_RELATIVE) {
                size = objectSize;
            } else if (mode == JGL_VERTEXMODE_ABSOLUTE) {
                size = rawImageSize;
            }

            vertex = textureToWorld(jglTexCoordInput);

            if (sizeController[0] == JGL_SIZE_CONTROLLER) {
                setSize[0] = size[0];
            }

            if (sizeController[1] == JGL_SIZE_CONTROLLER) {
                setSize[1] = size[1];
            }

            vec2 factor             = setSize / size;
            vec2 finalScaler        = pixelsToScreen(size * factor * scaler, objectSize);
            vec2 finalTranslater    = pixelsToScreen(vec2(position.x * -2, position.y * 2), objectSize);

            vec2 angle = vec2(cos(rotation), sin(rotation));

            mat2 rotateV = mat2(
                vec2(angle.x, -angle.y),
                vec2(angle.y, angle.x)
            );

            mat2 scaleV = mat2(
                vec2(1.0f / finalScaler.x, 0),
                vec2(0, 1.0f / finalScaler.y)
            );

            transformedVertex = (vertex + finalTranslater) * scaleV * rotateV;

            return worldToTexture(transformedVertex);

        } | /

        vec4 jglVertexShader() {

            vec4 convertedVertex = vec4(jglVertexCoordInput.xyz, jglVertexCoordInput.w + 1.0);

            jglCoords = jglTransformVertex(
                convertedVertex,
                jglProjectionMatrix(jglGetAspectRatio()),
                jglScalingMatrix(pixelsToScreen(jglObjectSize, jglWindowSize)),
                jglRotationMatrix(jglObjectRotation),
                jglTranslationMatrix(pixelsToScreen(jglObjectPosition, jglWindowSize), pixelsToScreen(jglCameraPosition.xy, jglWindowSize), jglWindowSize)
            );

            /*for (uint i = 0u; i < jglTextureCount; ++i) {
                if (i < 24) jglTexCoords0[i] = jglGetTextureCoordinates(i);
                else jglTexCoords1[i - 24] = jglGetTextureCoordinates(i - 24);
            } | /

            jglVertexAsTexture = worldToTexture(convertedVertex.xy);

            return jglCoords;
        }
    )glsl";

    const char *VERTEX_MAIN = R"glsl(
        #version jgl vertex

        void main() {
            gl_Position = jglVertexShader();
        }
    )glsl";

    const char *FRAGMENT_MAIN = R"glsl(
        #version jgl fragment

        ///resulting color of fragment
        layout(location = 0) out vec4 color;

        void main() {
            color = jglFragmentShader();
        }
    )glsl";*/

    const char *FRAGMENT_SHADER = R"glsl(
        #version 400 core

        #define M_PI 3.1415926535897932384626433832795

        #define JGL_BLENDMODE_STACK     0u
        #define JGL_BLENDMODE_COMBINE   1u
        #define JGL_BLENDMODE_MAP       2u

        #define JGL_CHANNEL_NONE        0u
        #define JGL_CHANNEL_RED         1u
        #define JGL_CHANNEL_GREEN       2u
        #define JGL_CHANNEL_BLUE        3u
        #define JGL_CHANNEL_ALPHA       4u

        in vec2 jglTexCoords0[24];
        in vec2 jglTexCoords1[24];

        in vec2 jglVertexAsTexture;

        #load textures

        #ifdef JGL_BINDLESS_TEXTURES

            #extension GL_ARB_bindless_texture : require

            layout (std140) uniform TexturePool {
                sampler2D textures[JGL_BINDLESS_TEXTURES];
                sampler2D maps[JGL_BINDLESS_TEXTURES / 4];
            };

            vec4 getTextureFragment(uint i, vec2 coords) {
                return texture(textures[i], coords);
            }

        #elif defined JGL_BOUND_TEXTURES

            #define NUM_TEX_ARRAYS JGL_BOUND_TEXTURES / 12
            #define NUM_SEGMENTS NUM_TEX_ARRAYS * 3

            uniform sampler2D minimumTextureSlots[12];

            #if NUM_TEX_ARRAYS > 1
                uniform sampler2D standardTextureSlots[12];
            #endif

            #if NUM_TEX_ARRAYS > 2
                uniform sampler2D extendedTextureSlots[12];
            #endif

            #if NUM_TEX_ARRAYS > 3
                uniform sampler2D maximumTextureSlots[12];
            #endif

            uniform sampler2D maps[NUM_SEGMENTS];

            vec4 getTextureFragment(uint i, vec2 coords) {
                #if NUM_TEX_ARRAYS == 1
                    return texture(minimumTextureSlots[i], coords);
                #elif NUM_TEX_ARRAYS > 1
                    if (i < 24u) return texture(minimumTextureSlots[i], coords);
                    else if (i < 24u) return texture(standardTextureSlots[i - 12u], coords);
                    #if NUM_TEX_ARRAYS > 2
                        else if (i < 36u) return texture(extendedTextureSlots[i - 24u], coords);
                        #if NUM_TEX_ARRAYS > 3
                            else return texture(maximumTextureSlots[i - 36u], coords);
                        #else
                            else return vec4(1.0);
                        #endif
                    #else
                        else return vec4(1.0);
                    #endif
                #endif
            }

        #else

            #error "Unsupported HW!"

        #endif

        struct LightSource {
            vec2 pos;
            float intensity;
            vec3 c;
        };
        struct Material {
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            float shine;
        };
        struct TextureLayer {
            vec2 position;
            vec2 size;
            vec2 imageSize;
            vec2 factor;
            uvec2 controller;
            float rotation;
            uint vertexMode;
            uint mapChannel;
        };
        struct TextureSegment {
            TextureLayer layers[4];
            uint sep;
            uint link;
            uint connect;
        };
        uniform Material material;
        ///MAX 128 lights in scene
        uniform LightSource lights[128];
        layout (std140) uniform JGLVertexDrawData {
            float jglObjectRotation;
            vec2 jglWindowSize;
            vec2 jglObjectPosition;
            vec2 jglObjectSize;
            vec2 jglObjectOrigin;
            vec3 jglCameraPosition;
        };
        layout (std140) uniform JGLFragmentDrawData {
            uint jglTextureCount;
            int jglLightingMode;
            int jglLightCount;
            uint blendingMode;
            vec4 jglObjectColor;
            vec3 jglCameraNormal;
        };
        layout (std140) uniform TextureDrawData {
            TextureSegment segments[0xf];
        };

        vec4 jglFragmentApplyLighting(vec4 initColor, int lightingMode) {
            float rand = gl_FragCoord.x * 353.0 + gl_FragCoord.y * 769.0;
            float noise = fract(rand / 991.0) / 256.0;
            vec3 begColor;
            begColor.rgb = vec3(0.0, 0.0, 0.0);
            float ambience = 0.02;
            vec3 ambientLight = ambience * material.ambient;
            for (int i = 0; i < jglLightCount; ++i) {
                vec3 passColor;
                passColor.rgb = vec3(initColor.r, initColor.g, initColor.b);
                vec3 fc = vec3(gl_FragCoord.x - (jglWindowSize.x / 2.0), gl_FragCoord.y - (jglWindowSize.y / 2.0), gl_FragCoord.z);
                vec3 lightPos = vec3(lights[i].pos, 1.0f);
                lightPos.x += jglCameraPosition.x;
                lightPos.y += jglCameraPosition.y;
                vec3 lc = lights[i].c;
                float mR = ambience + noise;
                lc.r = max(lc.r, mR);
                lc.g = max(lc.g, mR);
                lc.b = max(lc.b, mR);
                float intensity = lights[i].intensity * 100;
                float iA = 1 / sqrt(intensity);
                float iB = 1 / intensity;
                float d = distance(lightPos, fc);
                float attenuation = 1.0 / (1.0 + (iA * d) + (iB * pow(d, 2)));
                vec3 diffuse = lc * (attenuation * initColor.rgb);
                vec3 viewDir = normalize(vec3(lightPos.x, lightPos.y, 1.0) - fc);
                //vec3 lightDir = normalize(lightPos - fc);
                //vec3 reflectDir = reflect(-lightDir, jglCameraNormal);
                float spec = pow(max(dot(viewDir, jglCameraNormal), 0.0),  max(1.0 - material.shine, 0.0));
                vec3 specular = lc * (spec * material.specular);
                vec3 finalColor;
                if (lightingMode == 2) {
                    finalColor = (specular + noise) + (diffuse + noise);
                } else if (lightingMode == 1) {
                    float t = 0.5;
                    float m1 = sqrt(pow(passColor.r, 2) + pow(passColor.g, 2) + pow(passColor.b, 2));
                    float m2 = sqrt(pow(lc.r, 2) + pow(lc.g, 2) + pow(lc.b, 2));
                    float m = sqrt(pow(m1, 2) + pow(m2, 2));
                    vec3 baseColor;
                    baseColor.rgb = vec3(
                        sqrt((1 - t) * pow(passColor.r, 2) + t * pow(lc.r, 2)) * m,
                        sqrt((1 - t) * pow(passColor.g, 2) + t * pow(lc.g, 2)) * m,
                        sqrt((1 - t) * pow(passColor.b, 2) + t * pow(lc.b, 2)) * m
                    );
                    finalColor = (baseColor * diffuse);
                }
                begColor += finalColor;
            }
            return vec4(((ambientLight + noise) + begColor), initColor.a);
        }
        vec2 getTextureVertex(uint i) {
            if (i < 24) {
                return jglTexCoords0[i];
            } else {
                return jglTexCoords1[i - 24];
            }
        }
        vec4 blendTexels(vec4 texelA, vec4 texelB, uint blender) {
            switch (blender) {
            case JGL_BLENDMODE_STACK:
                float alpha = 1.0 - ((1.0 - texelA.a) * (1.0 - texelB.a));
                vec3 rgb = (texelA.rgb * (1.0 - texelB.a)) + (texelB.rgb * texelB.a);
                return vec4(rgb, alpha);
            case JGL_BLENDMODE_COMBINE:
                return (texelA + texelB) / 2.0;
            }
        }
        vec4 blendSegment(vec4 samples[4], uint filled, uint blendingMode) {
            float light = 1.0;
            vec3 lightRGB = samples[0].rgb;
            vec4 additiveFrag = vec4(0.0);
            for (uint i = 0; i < filled; ++i) {
                light *= 1.0 - samples[i].a;
                additiveFrag += samples[i];
                if (i > 0u) {
                    lightRGB *= 1.0 - samples[i].a;
                    lightRGB += samples[i].rgb * samples[i].a;
                }
            }
            if (blendingMode == JGL_BLENDMODE_STACK) return vec4(lightRGB, 1.0 - light);
            else if (blendingMode == JGL_BLENDMODE_COMBINE) return additiveFrag / filled;
        }
        vec4 sampleTextures() {
            //phease 0
            uint tIndex = 0;
            vec4 results[12];
            vec4 finalTexel;
            uint segmentsLoaded = 0;
            uint textured = 0u;
            vec2 texCoords;
            for (uint i = 0; i < 12; ++i) {
                vec4 samples[4];
                uint slotsLoaded = 0;
                for (uint ii = 0; ii < 4; ++ii) {
                    if (tIndex >= jglTextureCount) break;
                    texCoords = getTextureVertex(tIndex);
                    if (segments[i].link == JGL_BLENDMODE_MAP || (texCoords.x > -0.0001 && texCoords.y > -0.0001 && texCoords.x < 1.0001 && texCoords.y < 1.0001)) textured = 1u;
                    samples[ii] = getTextureFragment(tIndex, fract(getTextureVertex(tIndex)));
                    ++slotsLoaded;
                    ++tIndex;
                    if (tIndex == segments[i].sep) break;
                }
                if (slotsLoaded > 0u) {
                    if (segments[i].link < JGL_BLENDMODE_MAP) results[i] = blendSegment(samples, slotsLoaded, segments[i].link);
                    else {
                        vec4 mapSample = texture(maps[segmentsLoaded], jglVertexAsTexture);
                        vec3 backSample = (1.0 - (mapSample.r + mapSample.g + mapSample.b)) * samples[3].rgb;
                        results[i].rgb = backSample + (samples[0].rgb * mapSample.r) + (samples[1].rgb * mapSample.g) + (samples[2].rgb * mapSample.b);
                        results[i].a = mapSample.a;
                    }
                    ++segmentsLoaded;
                } else break;
            }

            //phase 1
            if (textured == 0u) return jglObjectColor;
            if (segmentsLoaded > 0u) {
                finalTexel = results[0];
                for (uint i = 1; i < segmentsLoaded; ++i) {
                    finalTexel = blendTexels(finalTexel, results[i], segments[i].connect);
                }
                return finalTexel;
            } else return jglObjectColor;
        }
        vec4 jglFragmentShader() {
            vec4 fColor;
            if (jglTextureCount > 0u) fColor =  sampleTextures();
            else fColor = jglObjectColor;

            return fColor;
        }
    )glsl";

    const char *VERTEX_SHADER = R"glsl(
        #version 400 core
        #define JGL_VERTEXMODE_MANUAL       0u
        #define JGL_VERTEXMODE_RELATIVE     1u
        #define JGL_VERTEXMODE_ABSOLUTE     2u
        #define JGL_SIZE_CONTROLLER         0u
        #define EXTERNAL_SIZE_CONTROLLER    1u
        layout(location=0) in vec4 jglVertexCoordInput;
        layout(location=1) in vec2 jglTexCoordInput;
        out vec2 jglTexCoords0[24];
        out vec2 jglTexCoords1[24];
        out vec4 jglCoords;
        out vec2 jglVertexAsTexture;
        struct TextureLayer {
            vec2 position;
            vec2 size;
            vec2 imageSize;
            vec2 factor;
            uvec2 controller;
            float rotation;
            uint vertexMode;
            uint mapChannel;
        };
        struct TextureSegment {
            TextureLayer layers[4];
            uint sep;
            uint link;
            uint connect;
        };
        layout (std140) uniform JGLVertexDrawData {
            float jglObjectRotation;
            vec2 jglWindowSize;
            vec2 jglObjectPosition;
            vec2 jglObjectSize;
            vec2 jglObjectOrigin;
            vec3 jglCameraPosition;
        };
        layout (std140) uniform JGLFragmentDrawData {
            uint jglTextureCount;
            int jglLightingMode;
            int jglLightCount;
            uint blendingMode;
            vec4 jglObjectColor;
            vec3 jglCameraNormal;
        };
        layout (std140) uniform TextureDrawData {
            TextureSegment segments[0xf];
        };
        vec4 jglGetVertexInput() {
            return jglVertexCoordInput;
        }
        vec2 worldToTexture(vec2 vA) {
            return vec2((vA.x / 2.0f) + 0.5f, ((vA.y / 2.0f) + 0.5f) + (vA.y * -1.0f));
        }
        vec2 textureToWorld(vec2 vA) {
            return vec2((vA.x * 2.0f) - 1.0f, ((vA.y * 2) - 1) * -1);
        }
        float jglGetAspectRatio() {
            return jglWindowSize.x / jglWindowSize.y;
        }
        vec2 pixelsToScreen(vec2 pixels, vec2 screen) {
            return vec2(pixels.x / screen.x, pixels.y / screen.y);
        }
        vec2 screenToPixels(vec2 screen, vec2 window) {
            return vec2(screen.x * window.x, screen.y * window.y);
        }
        mat4 jglTranslationMatrix(vec2 offset, vec2 cameraTranslation, vec2 screenSize) {
            float yRatio = screenSize.y / (screenSize.x / 2.0);
            return mat4(
                vec4(1, 0, 0, (offset.x + cameraTranslation.x) * 2),
                vec4(0, 1, 0, -(offset.y + cameraTranslation.y) * yRatio),
                vec4(0, 0, 1, 0),
                vec4(0, 0, 0, 1)
            );
        }
        mat4 jglRotationMatrix(float rotation) {
            return mat4(
                vec4(cos(-rotation), -sin(-rotation), 0, 0),
                vec4(sin(-rotation), cos(-rotation), 0, 0),
                vec4(0, 0, 1, 0),
                vec4(0, 0, 0, 1)
            );
        }
        mat4 jglScalingMatrix(vec2 size) {
            return mat4(
                vec4(size.x, 0, 0, 0),
                vec4(0, size.y, 0, 0),
                vec4(0, 0, 1, 0),
                vec4(0, 0, 0, 1)
            );
        }
        mat4 jglProjectionMatrix(float aspect) {
            return mat4(
                vec4(1, 0, 0, 0),
                vec4(0, 2.0 / (aspect + aspect), 0, 0),
                vec4(0, 0, -1, 0),
                vec4(0, 0, 0, 1)
            );
        }
        vec4 jglTransformVertex(vec4 vertex, mat4 projection, mat4 scaling, mat4 rotation, mat4 translation) {
            vec2 origin = -pixelsToScreen(jglObjectOrigin * (jglObjectSize / 2.0), jglWindowSize);
            return vertex * projection * scaling * jglTranslationMatrix(origin, vec2(0), jglWindowSize) * rotation * translation;
        }
        float getAspectRatio(vec2 screen) {
            return screen.x / screen.y;
        }
        vec2 jglGetTextureCoordinates(uint id) {

            TextureLayer layer = segments[id / 4].layers[id % 4];

            vec2 rawImageSize   = layer.imageSize;
            vec2 setSize        = layer.size;
            vec2 objectSize     = jglObjectSize;
            vec2 scaler         = layer.factor;
            vec2 size           = vec2(1.0);
            vec2 sizeRatio      = rawImageSize / objectSize;

            float rotation  = layer.rotation;
            float oAspect   = getAspectRatio(objectSize);
            vec2 position   = layer.position;

            uint mode               = layer.vertexMode;
            uvec2 sizeController    = layer.controller;

            vec2 transformedVertex;
            vec2 vertex;

            if (mode == JGL_VERTEXMODE_MANUAL) {
                size = objectSize;
            } else if (mode == JGL_VERTEXMODE_RELATIVE) {
                size = objectSize;
            } else if (mode == JGL_VERTEXMODE_ABSOLUTE) {
                size = rawImageSize;
            }

            vertex = textureToWorld(jglTexCoordInput);

            if (sizeController[0] == JGL_SIZE_CONTROLLER) {
                setSize[0] = size[0];
            }

            if (sizeController[1] == JGL_SIZE_CONTROLLER) {
                setSize[1] = size[1];
            }

            vec2 factor             = setSize / size;
            vec2 finalScaler        = pixelsToScreen(size * factor * scaler, objectSize);
            vec2 finalTranslater    = pixelsToScreen(vec2(position.x * -2, position.y * 2), objectSize);

            vec2 angle = vec2(cos(rotation), sin(rotation));

            mat2 rotateV = mat2(
                vec2(angle.x, -angle.y),
                vec2(angle.y, angle.x)
            );

            mat2 scaleV = mat2(
                vec2(1.0f / finalScaler.x, 0),
                vec2(0, 1.0f / finalScaler.y)
            );

            transformedVertex = (vertex + finalTranslater) * scaleV * rotateV;

            return worldToTexture(transformedVertex);
        }
        vec4 jglVertexShader() {
            vec4 convertedVertex = vec4(jglVertexCoordInput.xyz, jglVertexCoordInput.w + 1.0);
            jglCoords = jglTransformVertex(
                convertedVertex,
                jglProjectionMatrix(jglGetAspectRatio()),
                jglScalingMatrix(pixelsToScreen(jglObjectSize, jglWindowSize)),
                jglRotationMatrix(jglObjectRotation),
                jglTranslationMatrix(pixelsToScreen(jglObjectPosition, jglWindowSize), pixelsToScreen(jglCameraPosition.xy, jglWindowSize), jglWindowSize)
            );
            uint tLoaded = 0u;
            for (uint i = 0u; i < jglTextureCount; ++i) {
                if (i < 24) jglTexCoords0[i] = jglGetTextureCoordinates(i);
                else jglTexCoords1[i - 24] = jglGetTextureCoordinates(i - 24);
            }
            jglVertexAsTexture = worldToTexture(convertedVertex.xy);
            return jglCoords;
        }
    )glsl";

    const char *VERTEX_MAIN = R"glsl(
        #version jgl vertex
        void main() {
            gl_Position = jglVertexShader();
        }
    )glsl";

    const char *FRAGMENT_MAIN = R"glsl(
        #version jgl fragment
        ///resulting color of fragment
        layout(location = 0) out vec4 color;
        void main() {
            color = jglFragmentShader();
        }
    )glsl";

    jutil::Queue<LightSource> lights;

    class DefaultCore : public jgl::Core {
    public:
        bool update() override {}
    } defaultCore;

    ///variables
    jutil::String name;
    jml::Vector2u dimensions;
    jml::Vector2i position;
    GLFWwindow *win;
    Window *window = NULL;
    bool opens = true;
    ShaderFile *defaultFragmentShader, *defaultVertexShader, *mainFragmentShader, *mainVertexShader;
    Shader *defaultShader;
    Position cameraPos = {0, 0};
    Core *core = &defaultCore;
    long double fTime, fTimeLim = 0;
    float glVersion = 0;
    jutil::Timer timer;

    GLFWwindow *getWindowHandle() {
        return win;
    }

    void pause() {
        jutil::sleep(jml::max(fTimeLim - timer.get(jutil::MILLISECONDS), 0.L));
        fTime = timer.stop(jutil::NANOSECONDS);
        timer.start();
    }

    bool keyPressed(Event::Key k) {
        return (glfwGetKey(win, (int)k) == GLFW_PRESS);
    }

    void keyHandle(GLFWwindow*, int c, int, int a, int m) {
        if (core) {
            Event e(Event::KEY, c, (Event::Action)a, m);
            core->eventHandler(e);
        }
    }

    void mouseHandle(GLFWwindow*, int b, int a, int m) {
        if (core) {
            Event e(Event::MOUSE, b, (Event::Action)a, m);
            core->eventHandler(e);
        }
    }

    void scrollHandle(GLFWwindow*, double x, double y) {
        if (core) {
            Event e(Event::SCROLL, x, y);
            core->eventHandler(e);
        }
    }

    void cursorPositionHandle(GLFWwindow*, double x, double y) {
        if (core) {
            Event e(Event::CURSOR, x, y);
            core->eventHandler(e);
        }
    }

    void closeHandle(GLFWwindow*) {
        if (core) core->eventHandler(Event(Event::CLOSE));
    }

    void sizeHandle(GLFWwindow*, int w, int h) {
        if (core) {
            Event e(Event::RESIZE, w, h);
            core->eventHandler(e);
        }
    }

    void posHandle(GLFWwindow*, int x, int y) {
        if (core) {
            Event e(Event::MOVE, static_cast<double>(x), static_cast<double>(y));
            core->eventHandler(e);
        }
    }

    void focusHandle(GLFWwindow*, int f) {
        if (core) {
            Event::Type id;
            if (f) id = Event::GAIN_FOCUS;
            else id = Event::LOSE_FOCUS;

            Event e(id);
            core->eventHandler(e);
        }

    }

    void cursorFocusHandle(GLFWwindow*, int f) {
        if (core) {
            Event::Type id;
            if (f) id = Event::CURSOR_ENTER;
            else id = Event::CURSOR_LEAVE;

            Event e(id);
            core->eventHandler(e);
        }

    }

    void init(unsigned width, unsigned height, const char *title, int pX, int pY) {

        dimensions[0] = width;
        dimensions[1] = height;

        name = title;
        glewExperimental = GL_TRUE;
        if (!glfwInit()) {
            core->errorHandler(1, "GLFW failed to initialize");
        }

        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(false);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);

        win = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!win) {
            core->errorHandler(2, "Window creation failed");
        }
        glfwMakeContextCurrent(win);

        GLenum lerr = glewInit();
        if (lerr != GLEW_OK) {
            core->errorHandler(3, "GLEW failed to initialize");
        }

        int vers, vers2;

        glGetIntegerv(GL_MAJOR_VERSION, &vers);
        glGetIntegerv(GL_MINOR_VERSION, &vers2);

        unsigned decs = jutil::String(vers2).size();

        glVersion =  (float)vers + ((float)vers2 / (float)(jml::pow(10u, decs)));

        int nModes = 0;
        const GLFWvidmode *modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &nModes);

        if (pX == JGL_POS_MIDDLE) {for (int i = 0; i < nModes; ++i) {if (modes[i].width > position[0]) position[0] = (modes[i].width / 2) - (width / 2);}}
        else position[0] = pX;
        if (pY == JGL_POS_MIDDLE) {for (int i = 0; i < nModes; ++i) {if (modes[i].height > position[1]) position[1] = (modes[i].height / 2) - (height / 2);}}
        else position[1] = pY;

        glfwSetKeyCallback(win, keyHandle);
        glfwSetMouseButtonCallback(win, mouseHandle);
        glfwSetScrollCallback(win, scrollHandle);
        glfwSetCursorPosCallback(win, cursorPositionHandle);
        glfwSetWindowCloseCallback(win, closeHandle);
        glfwSetWindowSizeCallback(win, sizeHandle);
        glfwSetWindowPosCallback(win, posHandle);
        glfwSetWindowFocusCallback(win, focusHandle);
        glfwSetCursorEnterCallback(win, cursorFocusHandle);

        window = new Window(dimensions);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_MULTISAMPLE);

        defaultVertexShader = new ShaderFile(ShaderFileType::VERTEX, jutil::String(VERTEX_SHADER));
        defaultFragmentShader = new ShaderFile(ShaderFileType::FRAGMENT, jutil::String(FRAGMENT_SHADER));

        mainVertexShader = new ShaderFile(ShaderFileType::VERTEX, jutil::String(VERTEX_MAIN));
        mainFragmentShader = new ShaderFile(ShaderFileType::FRAGMENT, jutil::String(FRAGMENT_MAIN));

        initShaders();

        defaultShader = new Shader({mainVertexShader, mainFragmentShader});
        setVsyncEnabled(false);

        glfwSetWindowPos(win, position[0], position[1]);

        glPolygonMode(GL_FRONT, GL_FILL);

        glViewport(0.0f, -(((float)width - (float)height) / 2.0f), (float)width, (float)height * ((float)width / (float)height));
        initImages();
        initializeScreens();
        textInit();
        initTextures();
        timer.start();

    }

    void setVsyncEnabled(bool e) {
        if (e) glfwSwapInterval(-1);
        else glfwSwapInterval(0);
    }

    ShaderFile *getDefaultFragmentShader() {
        return defaultFragmentShader;
    }
    ShaderFile *getDefaultVertexShader() {
        return defaultVertexShader;
    }

    Shader *getDefaultShader() {
        return defaultShader;
    }

    void setMouseVisible(bool v) {
        if (v) {
            glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
    }

    void begin(Core &c) {
        core = &c;
        core->gameLoop();
    }

    float getOpenGLVersion() {
        return glVersion;
    }

    int end(int code) {
        if (opens) {
            textDestroy();
            destroyTextures();
            delete defaultFragmentShader;
            delete defaultVertexShader;
            delete mainFragmentShader;
            delete mainVertexShader;
            delete defaultShader;
            opens = false;
            glUseProgram(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glfwSetWindowShouldClose(win, true);
            glfwTerminate();
        }
        return code;
    }

    void setFrameTimeLimit(long double lim) {
        fTimeLim = lim;
    }

    long double getFrameTime(unsigned t) {
        long double time = fTime;
        for (size_t i = 0; i < t; ++i) {
            time /= 1000.L;
        }
        return time;
    }

    void pollEvents() {
        glfwPollEvents();
    }


    jml::Vector2u getWindowSize() {
        return dimensions;
    }
    bool open() {
        if (glfwWindowShouldClose(win)) return false;
        else return opens;
    }

    void useLightSource(LightSource l) {
        lights.insert(l);
    }

    jutil::Queue<LightSource> getLightsInScene() {
        return lights;
    }

    Position getMouseInWorld() {
        Position r;

        GLint viewport[0x04];
        GLdouble modelview[0x10];
        GLdouble projection[0x10];
        POINT mouse;
        GLfloat winX, winY, winZ;
        GLdouble posX, posY, posZ;

        GLdouble winW = (GLdouble)getWindowSize().x();
        GLdouble winH = (GLdouble)getWindowSize().y();


        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        GetCursorPos(&mouse);
        ScreenToClient(glfwGetWin32Window(win), &mouse);
        winX = (GLfloat)mouse.x;
        winY = (GLfloat)mouse.y;
        winY = (GLfloat)viewport[3] - winY;
        glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
        gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

        GLdouble yRatio = winH / ((winW / GLdouble(2.0f)));

        r[0] = posX * winW / GLdouble(2.0);
        r[1] = (-posY * winH / yRatio) + (winW - winH);

        r[0] -= cameraPos[0];
        r[1] -= cameraPos[1];

        return r;
    }


    void moveCamera(float x, float y) {
        cameraPos.x() += -x;
        cameraPos.y() += y;
    }

    void setCameraPosition(float x, float y) {
        cameraPos = {-x, y};
    }

    Core *getCore() {
        return core;
    }

    Position getCameraPosition() {
        return cameraPos;
    }
}
