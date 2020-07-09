/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// OpenGL ES 2.0 code

#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NANOVG_GLES2_IMPLEMENTATION   // Use GL2 es implementation
#include "nanovg/nanovg.h"
#include "nanovg/nanovg_gl.h"

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

struct NVGcontext* vg;

float rotation,             // final value
    currentRotation;    // current animated value
float r,g,b,a;      // bg
int w, h;
float x, y;

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

//auto gVertexShader =
//    "attribute vec4 vPosition;\n"
//    "void main() {\n"
//    "  gl_Position = vPosition;\n"
//    "}\n";
//
//auto gFragmentShader =
//    "precision mediump float;\n"
//    "void main() {\n"
//    "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
//    "}\n";
//
//GLuint loadShader(GLenum shaderType, const char* pSource) {
//    GLuint shader = glCreateShader(shaderType);
//    if (shader) {
//        glShaderSource(shader, 1, &pSource, NULL);
//        glCompileShader(shader);
//        GLint compiled = 0;
//        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
//        if (!compiled) {
//            GLint infoLen = 0;
//            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
//            if (infoLen) {
//                char* buf = (char*) malloc(infoLen);
//                if (buf) {
//                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
//                    LOGE("Could not compile shader %d:\n%s\n",
//                            shaderType, buf);
//                    free(buf);
//                }
//                glDeleteShader(shader);
//                shader = 0;
//            }
//        }
//    }
//    return shader;
//}
//
//GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
//    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
//    if (!vertexShader) {
//        return 0;
//    }
//
//    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
//    if (!pixelShader) {
//        return 0;
//    }
//
//    GLuint program = glCreateProgram();
//    if (program) {
//        glAttachShader(program, vertexShader);
//        checkGlError("glAttachShader");
//        glAttachShader(program, pixelShader);
//        checkGlError("glAttachShader");
//        glLinkProgram(program);
//        GLint linkStatus = GL_FALSE;
//        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
//        if (linkStatus != GL_TRUE) {
//            GLint bufLength = 0;
//            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
//            if (bufLength) {
//                char* buf = (char*) malloc(bufLength);
//                if (buf) {
//                    glGetProgramInfoLog(program, bufLength, NULL, buf);
//                    LOGE("Could not link program:\n%s\n", buf);
//                    free(buf);
//                }
//            }
//            glDeleteProgram(program);
//            program = 0;
//        }
//    }
//    return program;
//}
//
//GLuint gProgram;
//GLuint gvPositionHandle;

float len()
{
    return sqrt( 0.25f * w * w + 0.25f * h * h) * 0.5f; // 0.75 of 0.5w * 0.5w + 0.5h * 0.5h
}

float verticeX(float angle)
{
    return x + w / 2.0f + cos(nvgDegToRad(angle + currentRotation) ) * len();
}

float verticeY(float angle)
{
    return y + h / 2.0f - sin(nvgDegToRad(angle + currentRotation) ) * len();
}

//bool setupGraphics(int w, int h) {
//    printGLString("Version", GL_VERSION);
//    printGLString("Vendor", GL_VENDOR);
//    printGLString("Renderer", GL_RENDERER);
//    printGLString("Extensions", GL_EXTENSIONS);
//
//    LOGI("setupGraphics(%d, %d)", w, h);
//    gProgram = createProgram(gVertexShader, gFragmentShader);
//    if (!gProgram) {
//        LOGE("Could not create program.");
//        return false;
//    }
//    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
//    checkGlError("glGetAttribLocation");
//    LOGI("glGetAttribLocation(\"vPosition\") = %d\n",
//         gvPositionHandle);
//
//    glViewport(0, 0, w, h);
//    checkGlError("glViewport");
//    return true;
//}

bool setupGraphics(int _w, int _h) {
    w = _w; h = _h;

    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    LOGI("setupGraphics(%d, %d)", w, h);
    vg = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

    glViewport(0, 0, w, h);
    checkGlError("glViewport");

    glClearColor(r,g,b,a);
    checkGlError("glClearColor");
    return true;
}

//const GLfloat gTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f,
//        0.5f, -0.5f };

void renderFrame() {
//    static float grey;
//    grey += 0.01f;
//    if (grey > 1.0f) {
//        grey = 0.0f;
//    }
//    glClearColor(grey, grey, grey, 1.0f);
//    checkGlError("glClearColor");
//    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//    checkGlError("glClear");
//
//    glUseProgram(gProgram);
//    checkGlError("glUseProgram");
//
//    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
//    checkGlError("glVertexAttribPointer");
//    glEnableVertexAttribArray(gvPositionHandle);
//    checkGlError("glEnableVertexAttribArray");
//    glDrawArrays(GL_TRIANGLES, 0, 3);
//    checkGlError("glDrawArrays");

    currentRotation = currentRotation * 0.9f + rotation * 0.1f;

    glStencilMask(0xff);
    checkGlError("glStencilMask");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    checkGlError("glClear");

    nvgBeginFrame(vg, w, h, w/h);
    nvgBeginPath(vg);
    nvgMoveTo(vg, verticeX(90.f), verticeY(90.f));
    nvgLineTo(vg, verticeX(90.f + 120.0f), verticeY(90.0f + 120.0f));
    nvgLineTo(vg, verticeX(90.f + 240.f), verticeY(90.0f + 240.0f) );
    nvgClosePath(vg);
    nvgFillColor(vg, nvgRGBA(255,192,0,255));
    nvgFill(vg);
    nvgFillColor(vg, nvgRGBA(255,192,0,255));
    nvgEndFrame(vg);
}

extern "C" {
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jclass cls,  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jclass cls);

    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_setRotation(JNIEnv *env, jclass cls, jfloat _rotation);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_setBackgroundColor(JNIEnv *env, jclass cls, jfloat R, jfloat G, jfloat B, jfloat A);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_setTranslation(JNIEnv *env, jclass cls, jfloat _x, jfloat _y);
};

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jclass cls,  jint width, jint height)
{
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jclass cls)
{
    renderFrame();
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_setRotation(JNIEnv *env, jclass cls, jfloat _rotation)
{
    rotation = _rotation;
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_setBackgroundColor(JNIEnv *env, jclass cls, jfloat R, jfloat G, jfloat B, jfloat A)
{

    r = R; g = G; b = B; a = A;     // if method was called before initialization, save params and defer...
    glClearColor(r,g,b,a);      // if method is called after GL initialization
    checkGlError("glClearColor");
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_setTranslation(JNIEnv *env, jclass cls, jfloat _x, jfloat _y)
{
    x = _x; y=_y;
}