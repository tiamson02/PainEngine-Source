/* Wrapper for OpenGL includes...
     Copyright 1998 - Lionel Ulmer
     Copyright 2001 TransGaming Technologies, Inc.

   This wrapper is needed because Mesa uses also the CALLBACK / WINAPI
   constants.  It's also needed to work around certain common problems
   in Linux GL headers where key function pointers are not defined. */


#ifndef __WINE_WINE_GL_H
#define __WINE_WINE_GL_H

#ifndef __WINE_CONFIG_H
# error You must include config.h to use this header
#endif

#if defined(HAVE_OPENGL)

#include "ts_xlib.h"

/* As GLX relies on X, this is needed */
#define ENTER_GL() wine_tsx11_lock()
#define LEAVE_GL() wine_tsx11_unlock()

#undef APIENTRY
#undef CALLBACK
#undef WINAPI

#define XMD_H /* This is to prevent the Xmd.h inclusion bug to happen :-/ */
#include <GL/gl.h>
#include <GL/glu.h>
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>

/* Work around a gl.h vs glext.h snafu - gl.h may activate defines
   that prevent glext.h from giving us function pointer prototypes
   for the extensions below (which we need). If these defines are
   active, we have to define these prototypes ourselves. Of course
   this is further complicated by the fact that Mesa's gl.h header
   (if it's around instead of SGI's gl.h) includes glext.h itself.
   Hence the checks for GLEXT_VERSION for everything but
   GL_ARB_multitexture (prior to OpenGL 1.3, Mesa gl.h defined
   GL_ARB_multitexture itself, rather than leaving it up to glext.h
   - aak!) */
/* The nVidia gl.h also define extensions without giving us
   function pointer prototypes, and does declare OpenGL 1.3
   compatibility, but unlike Mesa, it doesn't include glext.h,
   so the check for GL_ARB_multitexture must check for this as well,
   sigh... */

#if defined(GL_ARB_multitexture) && (!defined(GL_GLEXT_VERSION) || !defined(GL_VERSION_1_3))
#define WINE_DDRAW_need_multitexture_pfns
#endif

#if defined(GL_EXT_secondary_color) && !defined(GL_GLEXT_VERSION)
#define WINE_DDRAW_need_secondary_color_pfns
#endif

#if defined(GL_EXT_fog_coord) && !defined(GL_GLEXT_VERSION)
#define WINE_DDRAW_need_fog_coord_pfns
#endif

#if defined(GL_ARB_texture_compression) && !defined(GL_GLEXT_VERSION)
#define WINE_DDRAW_need_texture_compression_pfns
#endif

#if defined(GL_EXT_vertex_weighting) && !defined(GL_GLEXT_VERSION)
#define WINE_DDRAW_need_vertex_weighting_pfns
#endif

#if defined(GL_NV_vertex_array_range) && !defined(GL_GLEXT_VERSION)
#define WINE_DDRAW_need_NV_VAR_pfns
#endif

#if defined(GL_NV_fence) && !defined(GL_GLEXT_VERSION)
#define WINE_DDRAW_need_NV_fence_pfns
#endif

#if defined(GL_NV_pixel_data_range) && !defined(GL_GLEXT_VERSION)
#define WINE_DDRAW_need_NV_PDR_pfns
#endif

#if defined(GL_NV_vertex_program) && !defined(GL_GLEXT_VERSION)
#define WINE_DDRAW_need_NV_vertex_program_pfns
#endif

#if defined(GL_ARB_vertex_program) && !defined(GL_GLEXT_VERSION)
#define WINE_DDRAW_need_ARB_vertex_program_pfns
#endif

#if defined(GL_ARB_vertex_blend) && !defined(GL_GLEXT_VERSION)
#define WINE_DDRAW_need_ARB_vertex_blend_pfns
#endif

#if defined(GL_ARB_vertex_buffer_object) && !defined(GL_GLEXT_VERSION)
#define WINE_DDRAW_need_ARB_VBO_pfns
#endif

#if defined(GL_ARB_shader_objects) && !defined(GL_GLEXT_VERSION)
#define WINE_DDRAW_need_ARB_shader_objects_pfns
#endif

#if defined(GL_ARB_vertex_shader) && !defined(GL_GLEXT_VERSION)
#define WINE_DDRAW_need_ARB_vertex_shader_pfns
#endif

#ifdef HAVE_GL_GLEXT_H
#include <GL/glext.h>
#endif

#ifndef APIENTRY
#define APIENTRY
#endif

#ifndef GL_VERSION_1_5
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
#endif

#ifndef GL_ARB_vertex_buffer_object
typedef ptrdiff_t GLintptrARB;
typedef ptrdiff_t GLsizeiptrARB;
#endif

#ifndef GL_ARB_shader_objects
typedef char GLcharARB;
typedef unsigned int GLhandleARB;
#endif

#ifndef GL_NV_half_float
typedef unsigned short GLhalfNV;
#endif

#ifdef WINE_DDRAW_need_multitexture_pfns
typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC)(GLenum texture);
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC)(GLenum texture);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC)(GLenum target, GLfloat u, GLfloat v);
#endif

#ifdef GL_VERSION_1_3
/* 1.3 moves the ARB multitexture extension into the core API, losing the ARB suffix on
 * the multitexture prototypes. However, dlls/opengl32/opengl_norm.c still need prototypes
 * with the ARB suffix, so we'll define them here. */
extern void APIENTRY glActiveTextureARB (GLenum);
extern void APIENTRY glClientActiveTextureARB (GLenum);
extern void APIENTRY glMultiTexCoord1dARB (GLenum, GLdouble);
extern void APIENTRY glMultiTexCoord1dvARB (GLenum, const GLdouble *);
extern void APIENTRY glMultiTexCoord1fARB (GLenum, GLfloat);
extern void APIENTRY glMultiTexCoord1fvARB (GLenum, const GLfloat *);
extern void APIENTRY glMultiTexCoord1iARB (GLenum, GLint);
extern void APIENTRY glMultiTexCoord1ivARB (GLenum, const GLint *);
extern void APIENTRY glMultiTexCoord1sARB (GLenum, GLshort);
extern void APIENTRY glMultiTexCoord1svARB (GLenum, const GLshort *);
extern void APIENTRY glMultiTexCoord2dARB (GLenum, GLdouble, GLdouble);
extern void APIENTRY glMultiTexCoord2dvARB (GLenum, const GLdouble *);
extern void APIENTRY glMultiTexCoord2fARB (GLenum, GLfloat, GLfloat);
extern void APIENTRY glMultiTexCoord2fvARB (GLenum, const GLfloat *);
extern void APIENTRY glMultiTexCoord2iARB (GLenum, GLint, GLint);
extern void APIENTRY glMultiTexCoord2ivARB (GLenum, const GLint *);
extern void APIENTRY glMultiTexCoord2sARB (GLenum, GLshort, GLshort);
extern void APIENTRY glMultiTexCoord2svARB (GLenum, const GLshort *);
extern void APIENTRY glMultiTexCoord3dARB (GLenum, GLdouble, GLdouble, GLdouble);
extern void APIENTRY glMultiTexCoord3dvARB (GLenum, const GLdouble *);
extern void APIENTRY glMultiTexCoord3fARB (GLenum, GLfloat, GLfloat, GLfloat);
extern void APIENTRY glMultiTexCoord3fvARB (GLenum, const GLfloat *);
extern void APIENTRY glMultiTexCoord3iARB (GLenum, GLint, GLint, GLint);
extern void APIENTRY glMultiTexCoord3ivARB (GLenum, const GLint *);
extern void APIENTRY glMultiTexCoord3sARB (GLenum, GLshort, GLshort, GLshort);
extern void APIENTRY glMultiTexCoord3svARB (GLenum, const GLshort *);
extern void APIENTRY glMultiTexCoord4dARB (GLenum, GLdouble, GLdouble, GLdouble, GLdouble);
extern void APIENTRY glMultiTexCoord4dvARB (GLenum, const GLdouble *);
extern void APIENTRY glMultiTexCoord4fARB (GLenum, GLfloat, GLfloat, GLfloat, GLfloat);
extern void APIENTRY glMultiTexCoord4fvARB (GLenum, const GLfloat *);
extern void APIENTRY glMultiTexCoord4iARB (GLenum, GLint, GLint, GLint, GLint);
extern void APIENTRY glMultiTexCoord4ivARB (GLenum, const GLint *);
extern void APIENTRY glMultiTexCoord4sARB (GLenum, GLshort, GLshort, GLshort, GLshort);
extern void APIENTRY glMultiTexCoord4svARB (GLenum, const GLshort *);
#endif

#ifdef WINE_DDRAW_need_secondary_color_pfns
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UBEXTPROC) (GLubyte red, GLubyte green, GLubyte blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLORPOINTEREXTPROC) (GLint size, GLenum type,
              GLsizei stride, GLvoid *pointer);
#endif

#ifdef WINE_DDRAW_need_fog_coord_pfns
typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat coord);
typedef void (APIENTRY * PFNGLFOGCOORDPOINTEREXTPROC) (GLenum type, GLsizei stride,
              const GLvoid *pointer);
#endif

#ifdef WINE_DDRAW_need_texture_compression_pfns
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DARBPROC) (GLenum target, GLint level,
              GLenum internalformat, GLsizei width, GLsizei height, GLint border,
              GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC) (GLenum target, GLint level,
              GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
              GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLGETCOMPRESSEDTEXIMAGEARBPROC) (GLenum target, GLint level,
              void *img);
#endif


#if defined(WINE_DDRAW_need_vertex_weighting_pfns) || !defined(GL_EXT_vertex_weighting)
typedef void (APIENTRY * PFNGLVERTEXWEIGHTFEXTPROC) (GLfloat weight);
typedef void (APIENTRY * PFNGLVERTEXWEIGHTFVEXTPROC) (const GLfloat *weight);
typedef void (APIENTRY * PFNGLVERTEXWEIGHTPOINTEREXTPROC) (GLsizei size, GLenum type, GLsizei stride, const GLvoid *pointer);
#endif

#if defined(WINE_DDRAW_need_NV_VAR_pfns) || !defined(GL_NV_vertex_array_range)
typedef void (APIENTRY * PFNGLFLUSHVERTEXARRAYRANGENVPROC)(void);
typedef void (APIENTRY * PFNGLVERTEXARRAYRANGENVPROC)(GLsizei size, const GLvoid *pointer);
#endif

#if !defined(GLX_NV_vertex_array_range)
typedef void * (APIENTRY * PFNGLXALLOCATEMEMORYNVPROC)(GLsizei size, GLfloat readfreq, GLfloat writefreq, GLfloat priority);
typedef void (APIENTRY * PFNGLXFREEMEMORYNVPROC)(GLvoid *pointer);
#endif

#if defined(WINE_DDRAW_need_NV_fence_pfns) || !defined(GL_NV_fence)
typedef void (APIENTRY * PFNGLGENFENCESNVPROC)(GLsizei n, GLuint *fences);
typedef void (APIENTRY * PFNGLDELETEFENCESNVPROC)(GLsizei n, const GLuint *fences);
typedef void (APIENTRY * PFNGLSETFENCENVPROC)(GLuint fence, GLenum condition);
typedef GLboolean (APIENTRY * PFNGLTESTFENCENVPROC)(GLuint fence);
typedef void (APIENTRY * PFNGLFINISHFENCENVPROC)(GLuint fence);
typedef GLboolean (APIENTRY * PFNGLISFENCENVPROC)(GLuint fence);
typedef void (APIENTRY * PFNGLGETFENCEIVNVPROC)(GLuint fence, GLenum pname, GLint *params);
#endif

#if defined(WINE_DDRAW_need_NV_PDR_pfns) || !defined(GL_NV_pixel_data_range)
typedef void (APIENTRY * PFNGLPIXELDATARANGENVPROC)(GLenum target, GLsizei length, GLvoid *pointer);
typedef void (APIENTRY * PFNGLFLUSHPIXELDATARANGENVPROC)(GLenum target);
#endif

#if defined(WINE_DDRAW_need_NV_vertex_program_pfns) || !defined(GL_NV_vertex_program)
typedef void (APIENTRY * PFNGLBINDPROGRAMNVPROC) (GLenum target, GLuint id);
typedef void (APIENTRY * PFNGLDELETEPROGRAMSNVPROC) (GLsizei n, const GLuint *programs);
typedef void (APIENTRY * PFNGLGENPROGRAMSNVPROC) (GLsizei n, GLuint *programs);
typedef void (APIENTRY * PFNGLLOADPROGRAMNVPROC) (GLenum target, GLuint id, GLsizei len, const GLubyte *program);
typedef void (APIENTRY * PFNGLPROGRAMPARAMETER4DNVPROC) (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRY * PFNGLPROGRAMPARAMETER4DVNVPROC) (GLenum target, GLuint index, const GLdouble *v);
typedef void (APIENTRY * PFNGLPROGRAMPARAMETER4FNVPROC) (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRY * PFNGLPROGRAMPARAMETER4FVNVPROC) (GLenum target, GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLPROGRAMPARAMETERS4DVNVPROC) (GLenum target, GLuint index, GLuint count, const GLdouble *v);
typedef void (APIENTRY * PFNGLPROGRAMPARAMETERS4FVNVPROC) (GLenum target, GLuint index, GLuint count, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBPOINTERNVPROC) (GLuint index, GLint fsize, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLGETPROGRAMPARAMETERFVNVPROC) (GLenum target, GLuint index, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETPROGRAMPARAMETERDVNVPROC) (GLenum target, GLuint index, GLenum pname, GLdouble *params);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBPOINTERVNVPROC) (GLuint index, GLenum pname, GLvoid **pointer);
#endif

#if defined(WINE_DDRAW_need_ARB_vertex_program_pfns) || !defined(GL_ARB_vertex_program)
typedef void (APIENTRY * PFNGLVERTEXATTRIBPOINTERARBPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLENABLEVERTEXATTRIBARRAYARBPROC) (GLuint index);
typedef void (APIENTRY * PFNGLDISABLEVERTEXATTRIBARRAYARBPROC) (GLuint index);
typedef void (APIENTRY * PFNGLPROGRAMSTRINGARBPROC) (GLenum target, GLenum format, GLsizei len, const GLvoid *string);
typedef void (APIENTRY * PFNGLBINDPROGRAMARBPROC) (GLenum target, GLuint program);
typedef void (APIENTRY * PFNGLDELETEPROGRAMSARBPROC) (GLsizei n, const GLuint *programs);
typedef void (APIENTRY * PFNGLGENPROGRAMSARBPROC) (GLsizei n, GLuint *programs);
typedef void (APIENTRY * PFNGLPROGRAMENVPARAMETER4FARBPROC) (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRY * PFNGLPROGRAMENVPARAMETER4FVARBPROC) (GLenum target, GLuint index, const GLfloat *params);
typedef void (APIENTRY * PFNGLPROGRAMLOCALPARAMETER4FARBPROC) (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRY * PFNGLPROGRAMLOCALPARAMETER4FVARBPROC) (GLenum target, GLuint index, const GLfloat *params);
typedef void (APIENTRY * PFNGLGETPROGRAMENVPARAMETERFVARBPROC) (GLenum target, GLuint index, GLfloat *params);
typedef void (APIENTRY * PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC) (GLenum target, GLuint index, GLfloat *params);
typedef void (APIENTRY * PFNGLGETPROGRAMIVARBPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBPOINTERVARBPROC) (GLuint index, GLenum pname, GLvoid* *pointer);
#endif

#if defined(WINE_DDRAW_need_ARB_vertex_blend_pfns) || !defined(GL_ARB_vertex_blend)
typedef void (APIENTRY * PFNGLWEIGHTPOINTERARBPROC) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLVERTEXBLENDARBPROC) (GLint count);
#endif

#if defined(WINE_DDRAW_need_ARB_VBO_pfns) || !defined(GL_ARB_vertex_buffer_object)
typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
typedef GLboolean (APIENTRY * PFNGLISBUFFERARBPROC) (GLuint buffer);
typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage);
typedef void (APIENTRY * PFNGLBUFFERSUBDATAARBPROC) (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid *data);
typedef void (APIENTRY * PFNGLGETBUFFERSUBDATAARBPROC) (GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid *data);
typedef GLvoid* (APIENTRY * PFNGLMAPBUFFERARBPROC) (GLenum target, GLenum access);
typedef GLboolean (APIENTRY * PFNGLUNMAPBUFFERARBPROC) (GLenum target);
typedef void (APIENTRY * PFNGLGETBUFFERPARAMETERIVARBPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETBUFFERPOINTERVARBPROC) (GLenum target, GLenum pname, GLvoid* *params);
#endif

#if defined(WINE_DDRAW_need_ARB_shader_objects_pfns) || !defined(GL_ARB_shader_objects)
typedef void (APIENTRY * PFNGLDELETEOBJECTARBPROC) (GLhandleARB obj);
typedef GLhandleARB (APIENTRY * PFNGLGETHANDLEARBPROC) (GLenum pname);
typedef void (APIENTRY * PFNGLDETACHOBJECTARBPROC) (GLhandleARB containerObj, GLhandleARB attachedObj);
typedef GLhandleARB (APIENTRY * PFNGLCREATESHADEROBJECTARBPROC) (GLenum shaderType);
typedef void (APIENTRY * PFNGLSHADERSOURCEARBPROC) (GLhandleARB shaderObj, GLsizei count, const GLcharARB* *string, const GLint *length);
typedef void (APIENTRY * PFNGLCOMPILESHADERARBPROC) (GLhandleARB shaderObj);
typedef GLhandleARB (APIENTRY * PFNGLCREATEPROGRAMOBJECTARBPROC) (void);
typedef void (APIENTRY * PFNGLATTACHOBJECTARBPROC) (GLhandleARB containerObj, GLhandleARB obj);
typedef void (APIENTRY * PFNGLLINKPROGRAMARBPROC) (GLhandleARB programObj);
typedef void (APIENTRY * PFNGLUSEPROGRAMOBJECTARBPROC) (GLhandleARB programObj);
typedef void (APIENTRY * PFNGLVALIDATEPROGRAMARBPROC) (GLhandleARB programObj);
typedef void (APIENTRY * PFNGLUNIFORM1FARBPROC) (GLint location, GLfloat v0);
typedef void (APIENTRY * PFNGLUNIFORM2FARBPROC) (GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRY * PFNGLUNIFORM3FARBPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRY * PFNGLUNIFORM4FARBPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRY * PFNGLUNIFORM1IARBPROC) (GLint location, GLint v0);
typedef void (APIENTRY * PFNGLUNIFORM2IARBPROC) (GLint location, GLint v0, GLint v1);
typedef void (APIENTRY * PFNGLUNIFORM3IARBPROC) (GLint location, GLint v0, GLint v1, GLint v2);
typedef void (APIENTRY * PFNGLUNIFORM4IARBPROC) (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (APIENTRY * PFNGLUNIFORM1FVARBPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORM2FVARBPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORM3FVARBPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORM4FVARBPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORM1IVARBPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRY * PFNGLUNIFORM2IVARBPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRY * PFNGLUNIFORM3IVARBPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRY * PFNGLUNIFORM4IVARBPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRY * PFNGLUNIFORMMATRIX2FVARBPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORMMATRIX3FVARBPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORMMATRIX4FVARBPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY * PFNGLGETOBJECTPARAMETERFVARBPROC) (GLhandleARB obj, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETOBJECTPARAMETERIVARBPROC) (GLhandleARB obj, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETINFOLOGARBPROC) (GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog);
typedef void (APIENTRY * PFNGLGETATTACHEDOBJECTSARBPROC) (GLhandleARB containerObj, GLsizei maxCount, GLsizei *count, GLhandleARB *obj);
typedef GLint (APIENTRY * PFNGLGETUNIFORMLOCATIONARBPROC) (GLhandleARB programObj, const GLcharARB *name);
typedef void (APIENTRY * PFNGLGETACTIVEUNIFORMARBPROC) (GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
typedef void (APIENTRY * PFNGLGETUNIFORMFVARBPROC) (GLhandleARB programObj, GLint location, GLfloat *params);
typedef void (APIENTRY * PFNGLGETUNIFORMIVARBPROC) (GLhandleARB programObj, GLint location, GLint *params);
typedef void (APIENTRY * PFNGLGETSHADERSOURCEARBPROC) (GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *source);
#endif

#if defined(WINE_DDRAW_need_ARB_vertex_shader_pfns) || !defined(GL_ARB_vertex_shader)
typedef void (APIENTRY * PFNGLBINDATTRIBLOCATIONARBPROC) (GLhandleARB programObj, GLuint index, const GLcharARB *name);
typedef void (APIENTRY * PFNGLGETACTIVEATTRIBARBPROC) (GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
typedef GLint (APIENTRY * PFNGLGETATTRIBLOCATIONARBPROC) (GLhandleARB programObj, const GLcharARB *name);
#endif

#undef  XMD_H

#undef APIENTRY
#undef CALLBACK
#undef WINAPI

/* Redefines the constants */
#define CALLBACK    __stdcall
#define WINAPI      __stdcall
#define APIENTRY    WINAPI


/* Now the extensions we don't want to have to duplicate all over.. */

/* Rectangle textures (non power of two support) */
#ifndef GL_ARB_texture_rectangle
#define GL_TEXTURE_RECTANGLE_ARB          0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE_ARB  0x84F6
#define PROXY_TEXTURE_RECTANGLE_ARB       0x84F7
#define MAX_RECTANGLE_TEXTURE_SIZE_ARB    0x84F8
#endif

/* ARB_vertex_program defines */
#ifndef GL_ARB_vertex_program
#define GL_ARB_vertex_program 1
#define GL_VERTEX_PROGRAM_ARB             0x8620
#define GL_VERTEX_PROGRAM_POINT_SIZE_ARB  0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE_ARB    0x8643
#define GL_COLOR_SUM_ARB                  0x8458
#define GL_PROGRAM_FORMAT_ASCII_ARB       0x8875
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB 0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB   0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB 0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB   0x8625
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB 0x886A
#define GL_CURRENT_VERTEX_ATTRIB_ARB      0x8626
#define GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB 0x8645
#define GL_PROGRAM_LENGTH_ARB             0x8627
#define GL_PROGRAM_FORMAT_ARB             0x8876
#define GL_PROGRAM_BINDING_ARB            0x8677
#define GL_PROGRAM_INSTRUCTIONS_ARB       0x88A0
#define GL_MAX_PROGRAM_INSTRUCTIONS_ARB   0x88A1
#define GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB 0x88A2
#define GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB 0x88A3
#define GL_PROGRAM_TEMPORARIES_ARB        0x88A4
#define GL_MAX_PROGRAM_TEMPORARIES_ARB    0x88A5
#define GL_PROGRAM_NATIVE_TEMPORARIES_ARB 0x88A6
#define GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB 0x88A7
#define GL_PROGRAM_PARAMETERS_ARB         0x88A8
#define GL_MAX_PROGRAM_PARAMETERS_ARB     0x88A9
#define GL_PROGRAM_NATIVE_PARAMETERS_ARB  0x88AA
#define GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB 0x88AB
#define GL_PROGRAM_ATTRIBS_ARB            0x88AC
#define GL_MAX_PROGRAM_ATTRIBS_ARB        0x88AD
#define GL_PROGRAM_NATIVE_ATTRIBS_ARB     0x88AE
#define GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB 0x88AF
#define GL_PROGRAM_ADDRESS_REGISTERS_ARB  0x88B0
#define GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB 0x88B1
#define GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB 0x88B2
#define GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB 0x88B3
#define GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB 0x88B4
#define GL_MAX_PROGRAM_ENV_PARAMETERS_ARB 0x88B5
#define GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB 0x88B6
#define GL_PROGRAM_STRING_ARB             0x8628
#define GL_PROGRAM_ERROR_POSITION_ARB     0x864B
#define GL_CURRENT_MATRIX_ARB             0x8641
#define GL_TRANSPOSE_CURRENT_MATRIX_ARB   0x88B7
#define GL_CURRENT_MATRIX_STACK_DEPTH_ARB 0x8640
#define GL_MAX_VERTEX_ATTRIBS_ARB         0x8869
#define GL_MAX_PROGRAM_MATRICES_ARB       0x862F
#define GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB 0x862E
#define GL_PROGRAM_ERROR_STRING_ARB       0x8874
#define GL_MATRIX0_ARB                    0x88C0
#define GL_MATRIX1_ARB                    0x88C1
#define GL_MATRIX2_ARB                    0x88C2
#define GL_MATRIX3_ARB                    0x88C3
#define GL_MATRIX4_ARB                    0x88C4
#define GL_MATRIX5_ARB                    0x88C5
#define GL_MATRIX6_ARB                    0x88C6
#define GL_MATRIX7_ARB                    0x88C7
#define GL_MATRIX8_ARB                    0x88C8
#define GL_MATRIX9_ARB                    0x88C9
#define GL_MATRIX10_ARB                   0x88CA
#define GL_MATRIX11_ARB                   0x88CB
#define GL_MATRIX12_ARB                   0x88CC
#define GL_MATRIX13_ARB                   0x88CD
#define GL_MATRIX14_ARB                   0x88CE
#define GL_MATRIX15_ARB                   0x88CF
#define GL_MATRIX16_ARB                   0x88D0
#define GL_MATRIX17_ARB                   0x88D1
#define GL_MATRIX18_ARB                   0x88D2
#define GL_MATRIX19_ARB                   0x88D3
#define GL_MATRIX20_ARB                   0x88D4
#define GL_MATRIX21_ARB                   0x88D5
#define GL_MATRIX22_ARB                   0x88D6
#define GL_MATRIX23_ARB                   0x88D7
#define GL_MATRIX24_ARB                   0x88D8
#define GL_MATRIX25_ARB                   0x88D9
#define GL_MATRIX26_ARB                   0x88DA
#define GL_MATRIX27_ARB                   0x88DB
#define GL_MATRIX28_ARB                   0x88DC
#define GL_MATRIX29_ARB                   0x88DD
#define GL_MATRIX30_ARB                   0x88DE
#define GL_MATRIX31_ARB                   0x88DF
#endif /* GL_ARB_vertex_program */

#ifndef GL_ARB_shader_objects
#define GL_PROGRAM_OBJECT_ARB             0x8B40
#define GL_SHADER_OBJECT_ARB              0x8B48
#define GL_OBJECT_TYPE_ARB                0x8B4E
#define GL_OBJECT_SUBTYPE_ARB             0x8B4F
#define GL_FLOAT_VEC2_ARB                 0x8B50
#define GL_FLOAT_VEC3_ARB                 0x8B51
#define GL_FLOAT_VEC4_ARB                 0x8B52
#define GL_INT_VEC2_ARB                   0x8B53
#define GL_INT_VEC3_ARB                   0x8B54
#define GL_INT_VEC4_ARB                   0x8B55
#define GL_BOOL_ARB                       0x8B56
#define GL_BOOL_VEC2_ARB                  0x8B57
#define GL_BOOL_VEC3_ARB                  0x8B58
#define GL_BOOL_VEC4_ARB                  0x8B59
#define GL_FLOAT_MAT2_ARB                 0x8B5A
#define GL_FLOAT_MAT3_ARB                 0x8B5B
#define GL_FLOAT_MAT4_ARB                 0x8B5C
#define GL_SAMPLER_1D_ARB                 0x8B5D
#define GL_SAMPLER_2D_ARB                 0x8B5E
#define GL_SAMPLER_3D_ARB                 0x8B5F
#define GL_SAMPLER_CUBE_ARB               0x8B60
#define GL_SAMPLER_1D_SHADOW_ARB          0x8B61
#define GL_SAMPLER_2D_SHADOW_ARB          0x8B62
#define GL_SAMPLER_2D_RECT_ARB            0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW_ARB     0x8B64
#define GL_OBJECT_DELETE_STATUS_ARB       0x8B80
#define GL_OBJECT_COMPILE_STATUS_ARB      0x8B81
#define GL_OBJECT_LINK_STATUS_ARB         0x8B82
#define GL_OBJECT_VALIDATE_STATUS_ARB     0x8B83
#define GL_OBJECT_INFO_LOG_LENGTH_ARB     0x8B84
#define GL_OBJECT_ATTACHED_OBJECTS_ARB    0x8B85
#define GL_OBJECT_ACTIVE_UNIFORMS_ARB     0x8B86
#define GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB 0x8B87
#define GL_OBJECT_SHADER_SOURCE_LENGTH_ARB 0x8B88
#endif

#ifndef GL_ARB_vertex_shader
#define GL_VERTEX_SHADER_ARB              0x8B31
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB 0x8B4A
#define GL_MAX_VARYING_FLOATS_ARB         0x8B4B
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB 0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB 0x8B4D
#define GL_OBJECT_ACTIVE_ATTRIBUTES_ARB   0x8B89
#define GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB 0x8B8A
#endif

#ifndef GL_ARB_fragment_shader
#define GL_FRAGMENT_SHADER_ARB            0x8B30
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB 0x8B49
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT_ARB 0x8B8B
#endif

#ifndef GL_ARB_shading_language_100
#define GL_SHADING_LANGUAGE_VERSION_ARB   0x8B8C
#endif


#endif /* HAVE_OPENGL */

#endif /* __WINE_WINE_GL_H */
