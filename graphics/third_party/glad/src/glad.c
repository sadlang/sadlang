/*
 * GLAD OpenGL Loader Implementation
 */

#include <glad/glad.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
static HMODULE libgl;
typedef PROC (WINAPI *PFNWGLGETPROCADDRESSPROC)(LPCSTR);
static PFNWGLGETPROCADDRESSPROC wglGetProcAddress_impl;

static void* get_proc(const char *namez) {
    void* result = (void*)wglGetProcAddress_impl(namez);
    if(result != NULL) {
        return result;
    }
    return (void*)GetProcAddress(libgl, namez);
}

static int open_gl(void) {
    libgl = LoadLibraryA("opengl32.dll");
    if(libgl != NULL) {
        wglGetProcAddress_impl = (PFNWGLGETPROCADDRESSPROC)GetProcAddress(libgl, "wglGetProcAddress");
        return wglGetProcAddress_impl != NULL;
    }
    return 0;
}

static void close_gl(void) {
    if(libgl != NULL) {
        FreeLibrary(libgl);
        libgl = NULL;
    }
}
#else
#include <dlfcn.h>
static void* libgl;

static void* get_proc(const char *namez) {
    return dlsym(libgl, namez);
}

static int open_gl(void) {
    libgl = dlopen("libGL.so.1", RTLD_LAZY | RTLD_LOCAL);
    if (!libgl) {
        libgl = dlopen("libGL.so", RTLD_LAZY | RTLD_LOCAL);
    }
    return libgl != NULL;
}

static void close_gl(void) {
    if(libgl != NULL) {
        dlclose(libgl);
        libgl = NULL;
    }
}
#endif

PFNGLCLEARPROC glad_glClear = NULL;
PFNGLCLEARCOLORPROC glad_glClearColor = NULL;
PFNGLVIEWPORTPROC glad_glViewport = NULL;
PFNGLENABLEPROC glad_glEnable = NULL;
PFNGLDISABLEPROC glad_glDisable = NULL;
PFNGLBLENDFUNCPROC glad_glBlendFunc = NULL;
PFNGLDEPTHFUNCPROC glad_glDepthFunc = NULL;
PFNGLGENTEXTURESPROC glad_glGenTextures = NULL;
PFNGLDELETETEXTURESPROC glad_glDeleteTextures = NULL;
PFNGLBINDTEXTUREPROC glad_glBindTexture = NULL;
PFNGLTEXIMAGE2DPROC glad_glTexImage2D = NULL;
PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D = NULL;
PFNGLTEXPARAMETERIPROC glad_glTexParameteri = NULL;
PFNGLTEXPARAMETERFVPROC glad_glTexParameterfv = NULL;
PFNGLACTIVETEXTUREPROC glad_glActiveTexture = NULL;
PFNGLGENERATEMIPMAPPROC glad_glGenerateMipmap = NULL;
PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays = NULL;
PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays = NULL;
PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray = NULL;
PFNGLGENBUFFERSPROC glad_glGenBuffers = NULL;
PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers = NULL;
PFNGLBINDBUFFERPROC glad_glBindBuffer = NULL;
PFNGLBUFFERDATAPROC glad_glBufferData = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer = NULL;
PFNGLDRAWARRAYSPROC glad_glDrawArrays = NULL;
PFNGLDRAWELEMENTSPROC glad_glDrawElements = NULL;
PFNGLCREATESHADERPROC glad_glCreateShader = NULL;
PFNGLDELETESHADERPROC glad_glDeleteShader = NULL;
PFNGLSHADERSOURCEPROC glad_glShaderSource = NULL;
PFNGLCOMPILESHADERPROC glad_glCompileShader = NULL;
PFNGLGETSHADERIVPROC glad_glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog = NULL;
PFNGLCREATEPROGRAMPROC glad_glCreateProgram = NULL;
PFNGLDELETEPROGRAMPROC glad_glDeleteProgram = NULL;
PFNGLATTACHSHADERPROC glad_glAttachShader = NULL;
PFNGLDETACHSHADERPROC glad_glDetachShader = NULL;
PFNGLLINKPROGRAMPROC glad_glLinkProgram = NULL;
PFNGLGETPROGRAMIVPROC glad_glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog = NULL;
PFNGLUSEPROGRAMPROC glad_glUseProgram = NULL;
PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation = NULL;
PFNGLUNIFORM1IPROC glad_glUniform1i = NULL;
PFNGLUNIFORM1FPROC glad_glUniform1f = NULL;
PFNGLUNIFORM2FPROC glad_glUniform2f = NULL;
PFNGLUNIFORM3FPROC glad_glUniform3f = NULL;
PFNGLUNIFORM4FPROC glad_glUniform4f = NULL;
PFNGLUNIFORMMATRIX4FVPROC glad_glUniformMatrix4fv = NULL;
PFNGLGETSTRINGPROC glad_glGetString = NULL;
PFNGLGETSTRINGIPROC glad_glGetStringi = NULL;
PFNGLGETINTEGERVPROC glad_glGetIntegerv = NULL;
PFNGLCLEARDEPTHPROC glad_glClearDepth = NULL;
PFNGLCLEARSTENCILPROC glad_glClearStencil = NULL;
PFNGLCULLFACEPROC glad_glCullFace = NULL;
PFNGLLINEWIDTHPROC glad_glLineWidth = NULL;
PFNGLPOINTSIZEPROC glad_glPointSize = NULL;

/* Additional OpenGL 3.x functions */
PFNGLGENFRAMEBUFFERSPROC glad_glGenFramebuffers = NULL;
PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers = NULL;
PFNGLBINDFRAMEBUFFERPROC glad_glBindFramebuffer = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus = NULL;
PFNGLDRAWBUFFERSPROC glad_glDrawBuffers = NULL;
PFNGLREADBUFFERPROC glad_glReadBuffer = NULL;
PFNGLREADPIXELSPROC glad_glReadPixels = NULL;
PFNGLBLITFRAMEBUFFERPROC glad_glBlitFramebuffer = NULL;
PFNGLBUFFERSUBDATAPROC glad_glBufferSubData = NULL;
PFNGLGENRENDERBUFFERSPROC glad_glGenRenderbuffers = NULL;
PFNGLDELETERENDERBUFFERSPROC glad_glDeleteRenderbuffers = NULL;
PFNGLBINDRENDERBUFFERPROC glad_glBindRenderbuffer = NULL;
PFNGLRENDERBUFFERSTORAGEPROC glad_glRenderbufferStorage = NULL;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glad_glFramebufferRenderbuffer = NULL;

int gladLoadGL(void) {
    if (!open_gl()) {
        return 0;
    }

    glad_glClear = (PFNGLCLEARPROC)get_proc("glClear");
    glad_glClearColor = (PFNGLCLEARCOLORPROC)get_proc("glClearColor");
    glad_glViewport = (PFNGLVIEWPORTPROC)get_proc("glViewport");
    glad_glEnable = (PFNGLENABLEPROC)get_proc("glEnable");
    glad_glDisable = (PFNGLDISABLEPROC)get_proc("glDisable");
    glad_glBlendFunc = (PFNGLBLENDFUNCPROC)get_proc("glBlendFunc");
    glad_glDepthFunc = (PFNGLDEPTHFUNCPROC)get_proc("glDepthFunc");
    glad_glGenTextures = (PFNGLGENTEXTURESPROC)get_proc("glGenTextures");
    glad_glDeleteTextures = (PFNGLDELETETEXTURESPROC)get_proc("glDeleteTextures");
    glad_glBindTexture = (PFNGLBINDTEXTUREPROC)get_proc("glBindTexture");
    glad_glTexImage2D = (PFNGLTEXIMAGE2DPROC)get_proc("glTexImage2D");
    glad_glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC)get_proc("glTexSubImage2D");
    glad_glTexParameteri = (PFNGLTEXPARAMETERIPROC)get_proc("glTexParameteri");
    glad_glTexParameterfv = (PFNGLTEXPARAMETERFVPROC)get_proc("glTexParameterfv");
    glad_glActiveTexture = (PFNGLACTIVETEXTUREPROC)get_proc("glActiveTexture");
    glad_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)get_proc("glGenerateMipmap");
    glad_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)get_proc("glGenVertexArrays");
    glad_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)get_proc("glDeleteVertexArrays");
    glad_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)get_proc("glBindVertexArray");
    glad_glGenBuffers = (PFNGLGENBUFFERSPROC)get_proc("glGenBuffers");
    glad_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)get_proc("glDeleteBuffers");
    glad_glBindBuffer = (PFNGLBINDBUFFERPROC)get_proc("glBindBuffer");
    glad_glBufferData = (PFNGLBUFFERDATAPROC)get_proc("glBufferData");
    glad_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)get_proc("glEnableVertexAttribArray");
    glad_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)get_proc("glVertexAttribPointer");
    glad_glDrawArrays = (PFNGLDRAWARRAYSPROC)get_proc("glDrawArrays");
    glad_glDrawElements = (PFNGLDRAWELEMENTSPROC)get_proc("glDrawElements");
    glad_glCreateShader = (PFNGLCREATESHADERPROC)get_proc("glCreateShader");
    glad_glDeleteShader = (PFNGLDELETESHADERPROC)get_proc("glDeleteShader");
    glad_glShaderSource = (PFNGLSHADERSOURCEPROC)get_proc("glShaderSource");
    glad_glCompileShader = (PFNGLCOMPILESHADERPROC)get_proc("glCompileShader");
    glad_glGetShaderiv = (PFNGLGETSHADERIVPROC)get_proc("glGetShaderiv");
    glad_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)get_proc("glGetShaderInfoLog");
    glad_glCreateProgram = (PFNGLCREATEPROGRAMPROC)get_proc("glCreateProgram");
    glad_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)get_proc("glDeleteProgram");
    glad_glAttachShader = (PFNGLATTACHSHADERPROC)get_proc("glAttachShader");
    glad_glDetachShader = (PFNGLDETACHSHADERPROC)get_proc("glDetachShader");
    glad_glLinkProgram = (PFNGLLINKPROGRAMPROC)get_proc("glLinkProgram");
    glad_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)get_proc("glGetProgramiv");
    glad_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)get_proc("glGetProgramInfoLog");
    glad_glUseProgram = (PFNGLUSEPROGRAMPROC)get_proc("glUseProgram");
    glad_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)get_proc("glGetUniformLocation");
    glad_glUniform1i = (PFNGLUNIFORM1IPROC)get_proc("glUniform1i");
    glad_glUniform1f = (PFNGLUNIFORM1FPROC)get_proc("glUniform1f");
    glad_glUniform2f = (PFNGLUNIFORM2FPROC)get_proc("glUniform2f");
    glad_glUniform3f = (PFNGLUNIFORM3FPROC)get_proc("glUniform3f");
    glad_glUniform4f = (PFNGLUNIFORM4FPROC)get_proc("glUniform4f");
    glad_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)get_proc("glUniformMatrix4fv");
    glad_glGetString = (PFNGLGETSTRINGPROC)get_proc("glGetString");
    glad_glGetStringi = (PFNGLGETSTRINGIPROC)get_proc("glGetStringi");
    glad_glGetIntegerv = (PFNGLGETINTEGERVPROC)get_proc("glGetIntegerv");
    glad_glClearDepth = (PFNGLCLEARDEPTHPROC)get_proc("glClearDepth");
    glad_glClearStencil = (PFNGLCLEARSTENCILPROC)get_proc("glClearStencil");
    glad_glCullFace = (PFNGLCULLFACEPROC)get_proc("glCullFace");
    glad_glLineWidth = (PFNGLLINEWIDTHPROC)get_proc("glLineWidth");
    glad_glPointSize = (PFNGLPOINTSIZEPROC)get_proc("glPointSize");

    /* Additional OpenGL 3.x functions */
    glad_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)get_proc("glGenFramebuffers");
    glad_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)get_proc("glDeleteFramebuffers");
    glad_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)get_proc("glBindFramebuffer");
    glad_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)get_proc("glFramebufferTexture2D");
    glad_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)get_proc("glCheckFramebufferStatus");
    glad_glDrawBuffers = (PFNGLDRAWBUFFERSPROC)get_proc("glDrawBuffers");
    glad_glReadBuffer = (PFNGLREADBUFFERPROC)get_proc("glReadBuffer");
    glad_glReadPixels = (PFNGLREADPIXELSPROC)get_proc("glReadPixels");
    glad_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)get_proc("glBlitFramebuffer");
    glad_glBufferSubData = (PFNGLBUFFERSUBDATAPROC)get_proc("glBufferSubData");
    glad_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)get_proc("glGenRenderbuffers");
    glad_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)get_proc("glDeleteRenderbuffers");
    glad_glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)get_proc("glBindRenderbuffer");
    glad_glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)get_proc("glRenderbufferStorage");
    glad_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)get_proc("glFramebufferRenderbuffer");

    return glad_glClear != NULL && glad_glClearColor != NULL && 
           glad_glViewport != NULL && glad_glCreateShader != NULL;
}
