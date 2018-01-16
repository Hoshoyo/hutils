#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <GL/gl.h>

#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_DEBUG_BIT_ARB				0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

// windows opengl errors
#define PIXEL_FORMAT_ERROR 1
#define BASIC_GL_CONTEXT_CREATE_ERROR 2
#define BASIC_GL_CONTEXT_SET_ERROR 3
#define OPENGL_MODULE_DLL_NOT_FOUND 4

#elif defined(__linux__)
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>

#ifndef APIENTRY
#define APIENTRY
#endif

#endif

#ifdef __cplusplus
#define extern extern "C"
#else
#define true 1
#define false 0
#endif

typedef struct
{
	int width;
	int height;
#if defined(_WIN32) || defined(_WIN64)
	HWND window_handle;
	WINDOWPLACEMENT window_placement;
	HDC device_context;
	HGLRC rendering_context;
	LRESULT(CALLBACK* window_proc)(HWND, UINT, WPARAM, LPARAM);
	HINSTANCE hinstance;
	s32 cmd_show;
#elif __linux__
	Display* display;
	Window win;
	GLXContext ctx;
	Colormap cmap;
#endif
} Window_Info;

// API external calls

/*
	This function creates an OpenGL context for the current OS
	based on the major and minor version, getting functions pointers
	according to what is available.

	For any OS:
		major		=> the major version of OpenGL context
		minor		=> the minor version of OpenGL context

	For windows:
		Window_Info:
		(in)	- window_handle			=> (HWND)  handle to window to be used by OpenGL
		(out)	- rendering_context		=> (HGLRC) rendering context handle to be used by swap buffers

	For linux:
		Window_Info:
		// @TODO

	Return value:
		0					=> success
		Greater than 0		=> error (see error values for specific OS)
*/
extern int hogl_init_opengl(Window_Info* window_info, int major, int minor);

/*
	This function load all the OpenGL extensions available on the system.
	If any of them are unable to be loaded, then its function pointer will
	have the value 0.
*/
extern int hogl_init_gl_extensions();

#ifndef GL_VERSION_1_0
#define GL_VERSION_1_0 1
typedef void GLvoid;
typedef unsigned int GLenum;
typedef float GLfloat;
typedef int GLint;
typedef int GLsizei;
typedef unsigned int GLbitfield;
typedef double GLdouble;
typedef unsigned int GLuint;
typedef unsigned char GLboolean;
typedef unsigned char GLubyte;

#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_FALSE                          0
#define GL_TRUE                           1
#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006
#define GL_QUADS                          0x0007
#define GL_NEVER                          0x0200
#define GL_LESS                           0x0201
#define GL_EQUAL                          0x0202
#define GL_LEQUAL                         0x0203
#define GL_GREATER                        0x0204
#define GL_NOTEQUAL                       0x0205
#define GL_GEQUAL                         0x0206
#define GL_ALWAYS                         0x0207
#define GL_ZERO                           0
#define GL_ONE                            1
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305
#define GL_DST_COLOR                      0x0306
#define GL_ONE_MINUS_DST_COLOR            0x0307
#define GL_SRC_ALPHA_SATURATE             0x0308
#define GL_NONE                           0
#define GL_FRONT_LEFT                     0x0400
#define GL_FRONT_RIGHT                    0x0401
#define GL_BACK_LEFT                      0x0402
#define GL_BACK_RIGHT                     0x0403
#define GL_FRONT                          0x0404
#define GL_BACK                           0x0405
#define GL_LEFT                           0x0406
#define GL_RIGHT                          0x0407
#define GL_FRONT_AND_BACK                 0x0408
#define GL_NO_ERROR                       0
#define GL_INVALID_ENUM                   0x0500
#define GL_INVALID_VALUE                  0x0501
#define GL_INVALID_OPERATION              0x0502
#define GL_OUT_OF_MEMORY                  0x0505
#define GL_CW                             0x0900
#define GL_CCW                            0x0901
#define GL_POINT_SIZE                     0x0B11
#define GL_POINT_SIZE_RANGE               0x0B12
#define GL_POINT_SIZE_GRANULARITY         0x0B13
#define GL_LINE_SMOOTH                    0x0B20
#define GL_LINE_WIDTH                     0x0B21
#define GL_LINE_WIDTH_RANGE               0x0B22
#define GL_LINE_WIDTH_GRANULARITY         0x0B23
#define GL_POLYGON_MODE                   0x0B40
#define GL_POLYGON_SMOOTH                 0x0B41
#define GL_CULL_FACE                      0x0B44
#define GL_CULL_FACE_MODE                 0x0B45
#define GL_FRONT_FACE                     0x0B46
#define GL_DEPTH_RANGE                    0x0B70
#define GL_DEPTH_TEST                     0x0B71
#define GL_DEPTH_WRITEMASK                0x0B72
#define GL_DEPTH_CLEAR_VALUE              0x0B73
#define GL_DEPTH_FUNC                     0x0B74
#define GL_STENCIL_TEST                   0x0B90
#define GL_STENCIL_CLEAR_VALUE            0x0B91
#define GL_STENCIL_FUNC                   0x0B92
#define GL_STENCIL_VALUE_MASK             0x0B93
#define GL_STENCIL_FAIL                   0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL        0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS        0x0B96
#define GL_STENCIL_REF                    0x0B97
#define GL_STENCIL_WRITEMASK              0x0B98
#define GL_VIEWPORT                       0x0BA2
#define GL_DITHER                         0x0BD0
#define GL_BLEND_DST                      0x0BE0
#define GL_BLEND_SRC                      0x0BE1
#define GL_BLEND                          0x0BE2
#define GL_LOGIC_OP_MODE                  0x0BF0
#define GL_DRAW_BUFFER                    0x0C01
#define GL_READ_BUFFER                    0x0C02
#define GL_SCISSOR_BOX                    0x0C10
#define GL_SCISSOR_TEST                   0x0C11
#define GL_COLOR_CLEAR_VALUE              0x0C22
#define GL_COLOR_WRITEMASK                0x0C23
#define GL_DOUBLEBUFFER                   0x0C32
#define GL_STEREO                         0x0C33
#define GL_LINE_SMOOTH_HINT               0x0C52
#define GL_POLYGON_SMOOTH_HINT            0x0C53
#define GL_UNPACK_SWAP_BYTES              0x0CF0
#define GL_UNPACK_LSB_FIRST               0x0CF1
#define GL_UNPACK_ROW_LENGTH              0x0CF2
#define GL_UNPACK_SKIP_ROWS               0x0CF3
#define GL_UNPACK_SKIP_PIXELS             0x0CF4
#define GL_UNPACK_ALIGNMENT               0x0CF5
#define GL_PACK_SWAP_BYTES                0x0D00
#define GL_PACK_LSB_FIRST                 0x0D01
#define GL_PACK_ROW_LENGTH                0x0D02
#define GL_PACK_SKIP_ROWS                 0x0D03
#define GL_PACK_SKIP_PIXELS               0x0D04
#define GL_PACK_ALIGNMENT                 0x0D05
#define GL_MAX_TEXTURE_SIZE               0x0D33
#define GL_MAX_VIEWPORT_DIMS              0x0D3A
#define GL_SUBPIXEL_BITS                  0x0D50
#define GL_TEXTURE_1D                     0x0DE0
#define GL_TEXTURE_2D                     0x0DE1
#define GL_TEXTURE_WIDTH                  0x1000
#define GL_TEXTURE_HEIGHT                 0x1001
#define GL_TEXTURE_BORDER_COLOR           0x1004
#define GL_DONT_CARE                      0x1100
#define GL_FASTEST                        0x1101
#define GL_NICEST                         0x1102
#define GL_BYTE                           0x1400
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_SHORT                          0x1402
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT                            0x1404
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406
#define GL_STACK_OVERFLOW                 0x0503
#define GL_STACK_UNDERFLOW                0x0504
#define GL_CLEAR                          0x1500
#define GL_AND                            0x1501
#define GL_AND_REVERSE                    0x1502
#define GL_COPY                           0x1503
#define GL_AND_INVERTED                   0x1504
#define GL_NOOP                           0x1505
#define GL_XOR                            0x1506
#define GL_OR                             0x1507
#define GL_NOR                            0x1508
#define GL_EQUIV                          0x1509
#define GL_INVERT                         0x150A
#define GL_OR_REVERSE                     0x150B
#define GL_COPY_INVERTED                  0x150C
#define GL_OR_INVERTED                    0x150D
#define GL_NAND                           0x150E
#define GL_SET                            0x150F
#define GL_TEXTURE                        0x1702
#define GL_COLOR                          0x1800
#define GL_DEPTH                          0x1801
#define GL_STENCIL                        0x1802
#define GL_STENCIL_INDEX                  0x1901
#define GL_DEPTH_COMPONENT                0x1902
#define GL_RED                            0x1903
#define GL_GREEN                          0x1904
#define GL_BLUE                           0x1905
#define GL_ALPHA                          0x1906
#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908
#define GL_POINT                          0x1B00
#define GL_LINE                           0x1B01
#define GL_FILL                           0x1B02
#define GL_KEEP                           0x1E00
#define GL_REPLACE                        0x1E01
#define GL_INCR                           0x1E02
#define GL_DECR                           0x1E03
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_REPEAT                         0x2901
#endif /* GL_VERSION_1_0 */

#ifndef GL_VERSION_1_1
#define GL_VERSION_1_1 1
typedef float GLclampf;
typedef double GLclampd;
#define GL_COLOR_LOGIC_OP                 0x0BF2
#define GL_POLYGON_OFFSET_UNITS           0x2A00
#define GL_POLYGON_OFFSET_POINT           0x2A01
#define GL_POLYGON_OFFSET_LINE            0x2A02
#define GL_POLYGON_OFFSET_FILL            0x8037
#define GL_POLYGON_OFFSET_FACTOR          0x8038
#define GL_TEXTURE_BINDING_1D             0x8068
#define GL_TEXTURE_BINDING_2D             0x8069
#define GL_TEXTURE_INTERNAL_FORMAT        0x1003
#define GL_TEXTURE_RED_SIZE               0x805C
#define GL_TEXTURE_GREEN_SIZE             0x805D
#define GL_TEXTURE_BLUE_SIZE              0x805E
#define GL_TEXTURE_ALPHA_SIZE             0x805F
#define GL_DOUBLE                         0x140A
#define GL_PROXY_TEXTURE_1D               0x8063
#define GL_PROXY_TEXTURE_2D               0x8064
#define GL_R3_G3_B2                       0x2A10
#define GL_RGB4                           0x804F
#define GL_RGB5                           0x8050
#define GL_RGB8                           0x8051
#define GL_RGB10                          0x8052
#define GL_RGB12                          0x8053
#define GL_RGB16                          0x8054
#define GL_RGBA2                          0x8055
#define GL_RGBA4                          0x8056
#define GL_RGB5_A1                        0x8057
#define GL_RGBA8                          0x8058
#define GL_RGB10_A2                       0x8059
#define GL_RGBA12                         0x805A
#define GL_RGBA16                         0x805B
#define GL_VERTEX_ARRAY                   0x8074
#endif /* GL_VERSION_1_1 */

#ifndef GL_VERSION_1_2
#define GL_VERSION_1_2 1
#define GL_UNSIGNED_BYTE_3_3_2            0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4         0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1         0x8034
#define GL_UNSIGNED_INT_8_8_8_8           0x8035
#define GL_UNSIGNED_INT_10_10_10_2        0x8036
#define GL_TEXTURE_BINDING_3D             0x806A
#define GL_PACK_SKIP_IMAGES               0x806B
#define GL_PACK_IMAGE_HEIGHT              0x806C
#define GL_UNPACK_SKIP_IMAGES             0x806D
#define GL_UNPACK_IMAGE_HEIGHT            0x806E
#define GL_TEXTURE_3D                     0x806F
#define GL_PROXY_TEXTURE_3D               0x8070
#define GL_TEXTURE_DEPTH                  0x8071
#define GL_TEXTURE_WRAP_R                 0x8072
#define GL_MAX_3D_TEXTURE_SIZE            0x8073
#define GL_UNSIGNED_BYTE_2_3_3_REV        0x8362
#define GL_UNSIGNED_SHORT_5_6_5           0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV       0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4_REV     0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV     0x8366
#define GL_UNSIGNED_INT_8_8_8_8_REV       0x8367
#define GL_UNSIGNED_INT_2_10_10_10_REV    0x8368
#define GL_BGR                            0x80E0
#define GL_BGRA                           0x80E1
#define GL_MAX_ELEMENTS_VERTICES          0x80E8
#define GL_MAX_ELEMENTS_INDICES           0x80E9
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_TEXTURE_MIN_LOD                0x813A
#define GL_TEXTURE_MAX_LOD                0x813B
#define GL_TEXTURE_BASE_LEVEL             0x813C
#define GL_TEXTURE_MAX_LEVEL              0x813D
#define GL_SMOOTH_POINT_SIZE_RANGE        0x0B12
#define GL_SMOOTH_POINT_SIZE_GRANULARITY  0x0B13
#define GL_SMOOTH_LINE_WIDTH_RANGE        0x0B22
#define GL_SMOOTH_LINE_WIDTH_GRANULARITY  0x0B23
#define GL_ALIASED_LINE_WIDTH_RANGE       0x846E
#endif /* GL_VERSION_1_2 */

#ifndef GL_VERSION_1_3
#define GL_VERSION_1_3 1
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7
#define GL_TEXTURE8                       0x84C8
#define GL_TEXTURE9                       0x84C9
#define GL_TEXTURE10                      0x84CA
#define GL_TEXTURE11                      0x84CB
#define GL_TEXTURE12                      0x84CC
#define GL_TEXTURE13                      0x84CD
#define GL_TEXTURE14                      0x84CE
#define GL_TEXTURE15                      0x84CF
#define GL_TEXTURE16                      0x84D0
#define GL_TEXTURE17                      0x84D1
#define GL_TEXTURE18                      0x84D2
#define GL_TEXTURE19                      0x84D3
#define GL_TEXTURE20                      0x84D4
#define GL_TEXTURE21                      0x84D5
#define GL_TEXTURE22                      0x84D6
#define GL_TEXTURE23                      0x84D7
#define GL_TEXTURE24                      0x84D8
#define GL_TEXTURE25                      0x84D9
#define GL_TEXTURE26                      0x84DA
#define GL_TEXTURE27                      0x84DB
#define GL_TEXTURE28                      0x84DC
#define GL_TEXTURE29                      0x84DD
#define GL_TEXTURE30                      0x84DE
#define GL_TEXTURE31                      0x84DF
#define GL_ACTIVE_TEXTURE                 0x84E0
#define GL_MULTISAMPLE                    0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE       0x809E
#define GL_SAMPLE_ALPHA_TO_ONE            0x809F
#define GL_SAMPLE_COVERAGE                0x80A0
#define GL_SAMPLE_BUFFERS                 0x80A8
#define GL_SAMPLES                        0x80A9
#define GL_SAMPLE_COVERAGE_VALUE          0x80AA
#define GL_SAMPLE_COVERAGE_INVERT         0x80AB
#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP       0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP         0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE      0x851C
#define GL_COMPRESSED_RGB                 0x84ED
#define GL_COMPRESSED_RGBA                0x84EE
#define GL_TEXTURE_COMPRESSION_HINT       0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE  0x86A0
#define GL_TEXTURE_COMPRESSED             0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS     0x86A3
#define GL_CLAMP_TO_BORDER                0x812D
#endif /* GL_VERSION_1_3 */

#ifndef GL_VERSION_1_4
#define GL_VERSION_1_4 1
#define GL_BLEND_DST_RGB                  0x80C8
#define GL_BLEND_SRC_RGB                  0x80C9
#define GL_BLEND_DST_ALPHA                0x80CA
#define GL_BLEND_SRC_ALPHA                0x80CB
#define GL_POINT_FADE_THRESHOLD_SIZE      0x8128
#define GL_DEPTH_COMPONENT16              0x81A5
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_COMPONENT32              0x81A7
#define GL_MIRRORED_REPEAT                0x8370
#define GL_MAX_TEXTURE_LOD_BIAS           0x84FD
#define GL_TEXTURE_LOD_BIAS               0x8501
#define GL_INCR_WRAP                      0x8507
#define GL_DECR_WRAP                      0x8508
#define GL_TEXTURE_DEPTH_SIZE             0x884A
#define GL_TEXTURE_COMPARE_MODE           0x884C
#define GL_TEXTURE_COMPARE_FUNC           0x884D
#define GL_BLEND_COLOR                    0x8005
#define GL_BLEND_EQUATION                 0x8009
#define GL_CONSTANT_COLOR                 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR       0x8002
#define GL_CONSTANT_ALPHA                 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA       0x8004
#define GL_FUNC_ADD                       0x8006
#define GL_FUNC_REVERSE_SUBTRACT          0x800B
#define GL_FUNC_SUBTRACT                  0x800A
#define GL_MIN                            0x8007
#define GL_MAX                            0x8008
#endif /* GL_VERSION_1_4 */

#ifndef GL_VERSION_1_5
#define GL_VERSION_1_5 1
#include <stddef.h>
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
#define GL_BUFFER_SIZE                    0x8764
#define GL_BUFFER_USAGE                   0x8765
#define GL_QUERY_COUNTER_BITS             0x8864
#define GL_CURRENT_QUERY                  0x8865
#define GL_QUERY_RESULT                   0x8866
#define GL_QUERY_RESULT_AVAILABLE         0x8867
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ARRAY_BUFFER_BINDING           0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F
#define GL_READ_ONLY                      0x88B8
#define GL_WRITE_ONLY                     0x88B9
#define GL_READ_WRITE                     0x88BA
#define GL_BUFFER_ACCESS                  0x88BB
#define GL_BUFFER_MAPPED                  0x88BC
#define GL_BUFFER_MAP_POINTER             0x88BD
#define GL_STREAM_DRAW                    0x88E0
#define GL_STREAM_READ                    0x88E1
#define GL_STREAM_COPY                    0x88E2
#define GL_STATIC_DRAW                    0x88E4
#define GL_STATIC_READ                    0x88E5
#define GL_STATIC_COPY                    0x88E6
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA
#define GL_SAMPLES_PASSED                 0x8914
#define GL_SRC1_ALPHA                     0x8589
#endif /* GL_VERSION_1_5 */

#ifndef GL_VERSION_2_0
#define GL_VERSION_2_0 1
typedef char GLchar;
typedef short GLshort;
typedef signed char GLbyte;
typedef unsigned short GLushort;
#define GL_BLEND_EQUATION_RGB             0x8009
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED    0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE       0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE     0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE       0x8625
#define GL_CURRENT_VERTEX_ATTRIB          0x8626
#define GL_VERTEX_PROGRAM_POINT_SIZE      0x8642
#define GL_VERTEX_ATTRIB_ARRAY_POINTER    0x8645
#define GL_STENCIL_BACK_FUNC              0x8800
#define GL_STENCIL_BACK_FAIL              0x8801
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL   0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS   0x8803
#define GL_MAX_DRAW_BUFFERS               0x8824
#define GL_DRAW_BUFFER0                   0x8825
#define GL_DRAW_BUFFER1                   0x8826
#define GL_DRAW_BUFFER2                   0x8827
#define GL_DRAW_BUFFER3                   0x8828
#define GL_DRAW_BUFFER4                   0x8829
#define GL_DRAW_BUFFER5                   0x882A
#define GL_DRAW_BUFFER6                   0x882B
#define GL_DRAW_BUFFER7                   0x882C
#define GL_DRAW_BUFFER8                   0x882D
#define GL_DRAW_BUFFER9                   0x882E
#define GL_DRAW_BUFFER10                  0x882F
#define GL_DRAW_BUFFER11                  0x8830
#define GL_DRAW_BUFFER12                  0x8831
#define GL_DRAW_BUFFER13                  0x8832
#define GL_DRAW_BUFFER14                  0x8833
#define GL_DRAW_BUFFER15                  0x8834
#define GL_BLEND_EQUATION_ALPHA           0x883D
#define GL_MAX_VERTEX_ATTRIBS             0x8869
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
#define GL_MAX_TEXTURE_IMAGE_UNITS        0x8872
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS 0x8B49
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS  0x8B4A
#define GL_MAX_VARYING_FLOATS             0x8B4B
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_SHADER_TYPE                    0x8B4F
#define GL_FLOAT_VEC2                     0x8B50
#define GL_FLOAT_VEC3                     0x8B51
#define GL_FLOAT_VEC4                     0x8B52
#define GL_INT_VEC2                       0x8B53
#define GL_INT_VEC3                       0x8B54
#define GL_INT_VEC4                       0x8B55
#define GL_BOOL                           0x8B56
#define GL_BOOL_VEC2                      0x8B57
#define GL_BOOL_VEC3                      0x8B58
#define GL_BOOL_VEC4                      0x8B59
#define GL_FLOAT_MAT2                     0x8B5A
#define GL_FLOAT_MAT3                     0x8B5B
#define GL_FLOAT_MAT4                     0x8B5C
#define GL_SAMPLER_1D                     0x8B5D
#define GL_SAMPLER_2D                     0x8B5E
#define GL_SAMPLER_3D                     0x8B5F
#define GL_SAMPLER_CUBE                   0x8B60
#define GL_SAMPLER_1D_SHADOW              0x8B61
#define GL_SAMPLER_2D_SHADOW              0x8B62
#define GL_DELETE_STATUS                  0x8B80
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_VALIDATE_STATUS                0x8B83
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_ATTACHED_SHADERS               0x8B85
#define GL_ACTIVE_UNIFORMS                0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH      0x8B87
#define GL_SHADER_SOURCE_LENGTH           0x8B88
#define GL_ACTIVE_ATTRIBUTES              0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH    0x8B8A
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT 0x8B8B
#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_CURRENT_PROGRAM                0x8B8D
#define GL_POINT_SPRITE_COORD_ORIGIN      0x8CA0
#define GL_LOWER_LEFT                     0x8CA1
#define GL_UPPER_LEFT                     0x8CA2
#define GL_STENCIL_BACK_REF               0x8CA3
#define GL_STENCIL_BACK_VALUE_MASK        0x8CA4
#define GL_STENCIL_BACK_WRITEMASK         0x8CA5
#endif /* GL_VERSION_2_0 */


#ifndef GL_VERSION_2_1
#define GL_VERSION_2_1 1
#define GL_PIXEL_PACK_BUFFER              0x88EB
#define GL_PIXEL_UNPACK_BUFFER            0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING      0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING    0x88EF
#define GL_FLOAT_MAT2x3                   0x8B65
#define GL_FLOAT_MAT2x4                   0x8B66
#define GL_FLOAT_MAT3x2                   0x8B67
#define GL_FLOAT_MAT3x4                   0x8B68
#define GL_FLOAT_MAT4x2                   0x8B69
#define GL_FLOAT_MAT4x3                   0x8B6A
#define GL_SRGB                           0x8C40
#define GL_SRGB8                          0x8C41
#define GL_SRGB_ALPHA                     0x8C42
#define GL_SRGB8_ALPHA8                   0x8C43
#define GL_COMPRESSED_SRGB                0x8C48
#define GL_COMPRESSED_SRGB_ALPHA          0x8C49
#endif /* GL_VERSION_2_1 */


#ifndef GL_VERSION_3_0
#define GL_VERSION_3_0 1
typedef unsigned short GLhalf;
#define GL_COMPARE_REF_TO_TEXTURE         0x884E
#define GL_CLIP_DISTANCE0                 0x3000
#define GL_CLIP_DISTANCE1                 0x3001
#define GL_CLIP_DISTANCE2                 0x3002
#define GL_CLIP_DISTANCE3                 0x3003
#define GL_CLIP_DISTANCE4                 0x3004
#define GL_CLIP_DISTANCE5                 0x3005
#define GL_CLIP_DISTANCE6                 0x3006
#define GL_CLIP_DISTANCE7                 0x3007
#define GL_MAX_CLIP_DISTANCES             0x0D32
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_NUM_EXTENSIONS                 0x821D
#define GL_CONTEXT_FLAGS                  0x821E
#define GL_COMPRESSED_RED                 0x8225
#define GL_COMPRESSED_RG                  0x8226
#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT 0x00000001
#define GL_RGBA32F                        0x8814
#define GL_RGB32F                         0x8815
#define GL_RGBA16F                        0x881A
#define GL_RGB16F                         0x881B
#define GL_VERTEX_ATTRIB_ARRAY_INTEGER    0x88FD
#define GL_MAX_ARRAY_TEXTURE_LAYERS       0x88FF
#define GL_MIN_PROGRAM_TEXEL_OFFSET       0x8904
#define GL_MAX_PROGRAM_TEXEL_OFFSET       0x8905
#define GL_CLAMP_READ_COLOR               0x891C
#define GL_FIXED_ONLY                     0x891D
#define GL_MAX_VARYING_COMPONENTS         0x8B4B
#define GL_TEXTURE_1D_ARRAY               0x8C18
#define GL_PROXY_TEXTURE_1D_ARRAY         0x8C19
#define GL_TEXTURE_2D_ARRAY               0x8C1A
#define GL_PROXY_TEXTURE_2D_ARRAY         0x8C1B
#define GL_TEXTURE_BINDING_1D_ARRAY       0x8C1C
#define GL_TEXTURE_BINDING_2D_ARRAY       0x8C1D
#define GL_R11F_G11F_B10F                 0x8C3A
#define GL_UNSIGNED_INT_10F_11F_11F_REV   0x8C3B
#define GL_RGB9_E5                        0x8C3D
#define GL_UNSIGNED_INT_5_9_9_9_REV       0x8C3E
#define GL_TEXTURE_SHARED_SIZE            0x8C3F
#define GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH 0x8C76
#define GL_TRANSFORM_FEEDBACK_BUFFER_MODE 0x8C7F
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS 0x8C80
#define GL_TRANSFORM_FEEDBACK_VARYINGS    0x8C83
#define GL_TRANSFORM_FEEDBACK_BUFFER_START 0x8C84
#define GL_TRANSFORM_FEEDBACK_BUFFER_SIZE 0x8C85
#define GL_PRIMITIVES_GENERATED           0x8C87
#define GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN 0x8C88
#define GL_RASTERIZER_DISCARD             0x8C89
#define GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS 0x8C8A
#define GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS 0x8C8B
#define GL_INTERLEAVED_ATTRIBS            0x8C8C
#define GL_SEPARATE_ATTRIBS               0x8C8D
#define GL_TRANSFORM_FEEDBACK_BUFFER      0x8C8E
#define GL_TRANSFORM_FEEDBACK_BUFFER_BINDING 0x8C8F
#define GL_RGBA32UI                       0x8D70
#define GL_RGB32UI                        0x8D71
#define GL_RGBA16UI                       0x8D76
#define GL_RGB16UI                        0x8D77
#define GL_RGBA8UI                        0x8D7C
#define GL_RGB8UI                         0x8D7D
#define GL_RGBA32I                        0x8D82
#define GL_RGB32I                         0x8D83
#define GL_RGBA16I                        0x8D88
#define GL_RGB16I                         0x8D89
#define GL_RGBA8I                         0x8D8E
#define GL_RGB8I                          0x8D8F
#define GL_RED_INTEGER                    0x8D94
#define GL_GREEN_INTEGER                  0x8D95
#define GL_BLUE_INTEGER                   0x8D96
#define GL_RGB_INTEGER                    0x8D98
#define GL_RGBA_INTEGER                   0x8D99
#define GL_BGR_INTEGER                    0x8D9A
#define GL_BGRA_INTEGER                   0x8D9B
#define GL_SAMPLER_1D_ARRAY               0x8DC0
#define GL_SAMPLER_2D_ARRAY               0x8DC1
#define GL_SAMPLER_1D_ARRAY_SHADOW        0x8DC3
#define GL_SAMPLER_2D_ARRAY_SHADOW        0x8DC4
#define GL_SAMPLER_CUBE_SHADOW            0x8DC5
#define GL_UNSIGNED_INT_VEC2              0x8DC6
#define GL_UNSIGNED_INT_VEC3              0x8DC7
#define GL_UNSIGNED_INT_VEC4              0x8DC8
#define GL_INT_SAMPLER_1D                 0x8DC9
#define GL_INT_SAMPLER_2D                 0x8DCA
#define GL_INT_SAMPLER_3D                 0x8DCB
#define GL_INT_SAMPLER_CUBE               0x8DCC
#define GL_INT_SAMPLER_1D_ARRAY           0x8DCE
#define GL_INT_SAMPLER_2D_ARRAY           0x8DCF
#define GL_UNSIGNED_INT_SAMPLER_1D        0x8DD1
#define GL_UNSIGNED_INT_SAMPLER_2D        0x8DD2
#define GL_UNSIGNED_INT_SAMPLER_3D        0x8DD3
#define GL_UNSIGNED_INT_SAMPLER_CUBE      0x8DD4
#define GL_UNSIGNED_INT_SAMPLER_1D_ARRAY  0x8DD6
#define GL_UNSIGNED_INT_SAMPLER_2D_ARRAY  0x8DD7
#define GL_QUERY_WAIT                     0x8E13
#define GL_QUERY_NO_WAIT                  0x8E14
#define GL_QUERY_BY_REGION_WAIT           0x8E15
#define GL_QUERY_BY_REGION_NO_WAIT        0x8E16
#define GL_BUFFER_ACCESS_FLAGS            0x911F
#define GL_BUFFER_MAP_LENGTH              0x9120
#define GL_BUFFER_MAP_OFFSET              0x9121
#define GL_DEPTH_COMPONENT32F             0x8CAC
#define GL_DEPTH32F_STENCIL8              0x8CAD
#define GL_FLOAT_32_UNSIGNED_INT_24_8_REV 0x8DAD
#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING 0x8210
#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE 0x8211
#define GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE 0x8212
#define GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE 0x8213
#define GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE 0x8214
#define GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE 0x8215
#define GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE 0x8216
#define GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE 0x8217
#define GL_FRAMEBUFFER_DEFAULT            0x8218
#define GL_FRAMEBUFFER_UNDEFINED          0x8219
#define GL_DEPTH_STENCIL_ATTACHMENT       0x821A
#define GL_MAX_RENDERBUFFER_SIZE          0x84E8
#define GL_DEPTH_STENCIL                  0x84F9
#define GL_UNSIGNED_INT_24_8              0x84FA
#define GL_DEPTH24_STENCIL8               0x88F0
#define GL_TEXTURE_STENCIL_SIZE           0x88F1
#define GL_TEXTURE_RED_TYPE               0x8C10
#define GL_TEXTURE_GREEN_TYPE             0x8C11
#define GL_TEXTURE_BLUE_TYPE              0x8C12
#define GL_TEXTURE_ALPHA_TYPE             0x8C13
#define GL_TEXTURE_DEPTH_TYPE             0x8C16
#define GL_UNSIGNED_NORMALIZED            0x8C17
#define GL_FRAMEBUFFER_BINDING            0x8CA6
#define GL_DRAW_FRAMEBUFFER_BINDING       0x8CA6
#define GL_RENDERBUFFER_BINDING           0x8CA7
#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#define GL_READ_FRAMEBUFFER_BINDING       0x8CAA
#define GL_RENDERBUFFER_SAMPLES           0x8CAB
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE 0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME 0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL 0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE 0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER 0x8CD4
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED        0x8CDD
#define GL_MAX_COLOR_ATTACHMENTS          0x8CDF
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1              0x8CE1
#define GL_COLOR_ATTACHMENT2              0x8CE2
#define GL_COLOR_ATTACHMENT3              0x8CE3
#define GL_COLOR_ATTACHMENT4              0x8CE4
#define GL_COLOR_ATTACHMENT5              0x8CE5
#define GL_COLOR_ATTACHMENT6              0x8CE6
#define GL_COLOR_ATTACHMENT7              0x8CE7
#define GL_COLOR_ATTACHMENT8              0x8CE8
#define GL_COLOR_ATTACHMENT9              0x8CE9
#define GL_COLOR_ATTACHMENT10             0x8CEA
#define GL_COLOR_ATTACHMENT11             0x8CEB
#define GL_COLOR_ATTACHMENT12             0x8CEC
#define GL_COLOR_ATTACHMENT13             0x8CED
#define GL_COLOR_ATTACHMENT14             0x8CEE
#define GL_COLOR_ATTACHMENT15             0x8CEF
#define GL_COLOR_ATTACHMENT16             0x8CF0
#define GL_COLOR_ATTACHMENT17             0x8CF1
#define GL_COLOR_ATTACHMENT18             0x8CF2
#define GL_COLOR_ATTACHMENT19             0x8CF3
#define GL_COLOR_ATTACHMENT20             0x8CF4
#define GL_COLOR_ATTACHMENT21             0x8CF5
#define GL_COLOR_ATTACHMENT22             0x8CF6
#define GL_COLOR_ATTACHMENT23             0x8CF7
#define GL_COLOR_ATTACHMENT24             0x8CF8
#define GL_COLOR_ATTACHMENT25             0x8CF9
#define GL_COLOR_ATTACHMENT26             0x8CFA
#define GL_COLOR_ATTACHMENT27             0x8CFB
#define GL_COLOR_ATTACHMENT28             0x8CFC
#define GL_COLOR_ATTACHMENT29             0x8CFD
#define GL_COLOR_ATTACHMENT30             0x8CFE
#define GL_COLOR_ATTACHMENT31             0x8CFF
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_STENCIL_ATTACHMENT             0x8D20
#define GL_FRAMEBUFFER                    0x8D40
#define GL_RENDERBUFFER                   0x8D41
#define GL_RENDERBUFFER_WIDTH             0x8D42
#define GL_RENDERBUFFER_HEIGHT            0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT   0x8D44
#define GL_STENCIL_INDEX1                 0x8D46
#define GL_STENCIL_INDEX4                 0x8D47
#define GL_STENCIL_INDEX8                 0x8D48
#define GL_STENCIL_INDEX16                0x8D49
#define GL_RENDERBUFFER_RED_SIZE          0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE        0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE         0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE        0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE        0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE      0x8D55
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE 0x8D56
#define GL_MAX_SAMPLES                    0x8D57
#define GL_FRAMEBUFFER_SRGB               0x8DB9
#define GL_HALF_FLOAT                     0x140B
#define GL_MAP_READ_BIT                   0x0001
#define GL_MAP_WRITE_BIT                  0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT       0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT      0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT         0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT         0x0020
#define GL_COMPRESSED_RED_RGTC1           0x8DBB
#define GL_COMPRESSED_SIGNED_RED_RGTC1    0x8DBC
#define GL_COMPRESSED_RG_RGTC2            0x8DBD
#define GL_COMPRESSED_SIGNED_RG_RGTC2     0x8DBE
#define GL_RG                             0x8227
#define GL_RG_INTEGER                     0x8228
#define GL_R8                             0x8229
#define GL_R16                            0x822A
#define GL_RG8                            0x822B
#define GL_RG16                           0x822C
#define GL_R16F                           0x822D
#define GL_R32F                           0x822E
#define GL_RG16F                          0x822F
#define GL_RG32F                          0x8230
#define GL_R8I                            0x8231
#define GL_R8UI                           0x8232
#define GL_R16I                           0x8233
#define GL_R16UI                          0x8234
#define GL_R32I                           0x8235
#define GL_R32UI                          0x8236
#define GL_RG8I                           0x8237
#define GL_RG8UI                          0x8238
#define GL_RG16I                          0x8239
#define GL_RG16UI                         0x823A
#define GL_RG32I                          0x823B
#define GL_RG32UI                         0x823C
#define GL_VERTEX_ARRAY_BINDING           0x85B5
#endif /* GL_VERSION_3_0 */


#ifndef GL_VERSION_3_1
#define GL_VERSION_3_1 1
#define GL_SAMPLER_2D_RECT                0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW         0x8B64
#define GL_SAMPLER_BUFFER                 0x8DC2
#define GL_INT_SAMPLER_2D_RECT            0x8DCD
#define GL_INT_SAMPLER_BUFFER             0x8DD0
#define GL_UNSIGNED_INT_SAMPLER_2D_RECT   0x8DD5
#define GL_UNSIGNED_INT_SAMPLER_BUFFER    0x8DD8
#define GL_TEXTURE_BUFFER                 0x8C2A
#define GL_MAX_TEXTURE_BUFFER_SIZE        0x8C2B
#define GL_TEXTURE_BINDING_BUFFER         0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING 0x8C2D
#define GL_TEXTURE_RECTANGLE              0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE      0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE        0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE     0x84F8
#define GL_R8_SNORM                       0x8F94
#define GL_RG8_SNORM                      0x8F95
#define GL_RGB8_SNORM                     0x8F96
#define GL_RGBA8_SNORM                    0x8F97
#define GL_R16_SNORM                      0x8F98
#define GL_RG16_SNORM                     0x8F99
#define GL_RGB16_SNORM                    0x8F9A
#define GL_RGBA16_SNORM                   0x8F9B
#define GL_SIGNED_NORMALIZED              0x8F9C
#define GL_PRIMITIVE_RESTART              0x8F9D
#define GL_PRIMITIVE_RESTART_INDEX        0x8F9E
#define GL_COPY_READ_BUFFER               0x8F36
#define GL_COPY_WRITE_BUFFER              0x8F37
#define GL_UNIFORM_BUFFER                 0x8A11
#define GL_UNIFORM_BUFFER_BINDING         0x8A28
#define GL_UNIFORM_BUFFER_START           0x8A29
#define GL_UNIFORM_BUFFER_SIZE            0x8A2A
#define GL_MAX_VERTEX_UNIFORM_BLOCKS      0x8A2B
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS    0x8A2C
#define GL_MAX_FRAGMENT_UNIFORM_BLOCKS    0x8A2D
#define GL_MAX_COMBINED_UNIFORM_BLOCKS    0x8A2E
#define GL_MAX_UNIFORM_BUFFER_BINDINGS    0x8A2F
#define GL_MAX_UNIFORM_BLOCK_SIZE         0x8A30
#define GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS 0x8A31
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS 0x8A32
#define GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS 0x8A33
#define GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT 0x8A34
#define GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH 0x8A35
#define GL_ACTIVE_UNIFORM_BLOCKS          0x8A36
#define GL_UNIFORM_TYPE                   0x8A37
#define GL_UNIFORM_SIZE                   0x8A38
#define GL_UNIFORM_NAME_LENGTH            0x8A39
#define GL_UNIFORM_BLOCK_INDEX            0x8A3A
#define GL_UNIFORM_OFFSET                 0x8A3B
#define GL_UNIFORM_ARRAY_STRIDE           0x8A3C
#define GL_UNIFORM_MATRIX_STRIDE          0x8A3D
#define GL_UNIFORM_IS_ROW_MAJOR           0x8A3E
#define GL_UNIFORM_BLOCK_BINDING          0x8A3F
#define GL_UNIFORM_BLOCK_DATA_SIZE        0x8A40
#define GL_UNIFORM_BLOCK_NAME_LENGTH      0x8A41
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS  0x8A42
#define GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES 0x8A43
#define GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER 0x8A44
#define GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER 0x8A45
#define GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER 0x8A46
#define GL_INVALID_INDEX                  0xFFFFFFFFu
#endif /* GL_VERSION_3_1 */

#ifndef GL_VERSION_3_2
#define GL_VERSION_3_2 1
#include <stdint.h>
typedef struct __GLsync *GLsync;
typedef uint64_t GLuint64;
typedef int64_t GLint64;
#define GL_CONTEXT_CORE_PROFILE_BIT       0x00000001
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_LINES_ADJACENCY                0x000A
#define GL_LINE_STRIP_ADJACENCY           0x000B
#define GL_TRIANGLES_ADJACENCY            0x000C
#define GL_TRIANGLE_STRIP_ADJACENCY       0x000D
#define GL_PROGRAM_POINT_SIZE             0x8642
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS 0x8C29
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED 0x8DA7
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
#define GL_GEOMETRY_SHADER                0x8DD9
#define GL_GEOMETRY_VERTICES_OUT          0x8916
#define GL_GEOMETRY_INPUT_TYPE            0x8917
#define GL_GEOMETRY_OUTPUT_TYPE           0x8918
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS 0x8DDF
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES   0x8DE0
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS 0x8DE1
#define GL_MAX_VERTEX_OUTPUT_COMPONENTS   0x9122
#define GL_MAX_GEOMETRY_INPUT_COMPONENTS  0x9123
#define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS 0x9124
#define GL_MAX_FRAGMENT_INPUT_COMPONENTS  0x9125
#define GL_CONTEXT_PROFILE_MASK           0x9126
#define GL_DEPTH_CLAMP                    0x864F
#define GL_QUADS_FOLLOW_PROVOKING_VERTEX_CONVENTION 0x8E4C
#define GL_FIRST_VERTEX_CONVENTION        0x8E4D
#define GL_LAST_VERTEX_CONVENTION         0x8E4E
#define GL_PROVOKING_VERTEX               0x8E4F
#define GL_TEXTURE_CUBE_MAP_SEAMLESS      0x884F
#define GL_MAX_SERVER_WAIT_TIMEOUT        0x9111
#define GL_OBJECT_TYPE                    0x9112
#define GL_SYNC_CONDITION                 0x9113
#define GL_SYNC_STATUS                    0x9114
#define GL_SYNC_FLAGS                     0x9115
#define GL_SYNC_FENCE                     0x9116
#define GL_SYNC_GPU_COMMANDS_COMPLETE     0x9117
#define GL_UNSIGNALED                     0x9118
#define GL_SIGNALED                       0x9119
#define GL_ALREADY_SIGNALED               0x911A
#define GL_TIMEOUT_EXPIRED                0x911B
#define GL_CONDITION_SATISFIED            0x911C
#define GL_WAIT_FAILED                    0x911D
#define GL_TIMEOUT_IGNORED                0xFFFFFFFFFFFFFFFFull
#define GL_SYNC_FLUSH_COMMANDS_BIT        0x00000001
#define GL_SAMPLE_POSITION                0x8E50
#define GL_SAMPLE_MASK                    0x8E51
#define GL_SAMPLE_MASK_VALUE              0x8E52
#define GL_MAX_SAMPLE_MASK_WORDS          0x8E59
#define GL_TEXTURE_2D_MULTISAMPLE         0x9100
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE   0x9101
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY   0x9102
#define GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9103
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE 0x9104
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY 0x9105
#define GL_TEXTURE_SAMPLES                0x9106
#define GL_TEXTURE_FIXED_SAMPLE_LOCATIONS 0x9107
#define GL_SAMPLER_2D_MULTISAMPLE         0x9108
#define GL_INT_SAMPLER_2D_MULTISAMPLE     0x9109
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE 0x910A
#define GL_SAMPLER_2D_MULTISAMPLE_ARRAY   0x910B
#define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910C
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 0x910D
#define GL_MAX_COLOR_TEXTURE_SAMPLES      0x910E
#define GL_MAX_DEPTH_TEXTURE_SAMPLES      0x910F
#define GL_MAX_INTEGER_SAMPLES            0x9110
#endif /* GL_VERSION_3_2 */


#ifndef GL_VERSION_3_3
#define GL_VERSION_3_3 1
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR    0x88FE
#define GL_SRC1_COLOR                     0x88F9
#define GL_ONE_MINUS_SRC1_COLOR           0x88FA
#define GL_ONE_MINUS_SRC1_ALPHA           0x88FB
#define GL_MAX_DUAL_SOURCE_DRAW_BUFFERS   0x88FC
#define GL_ANY_SAMPLES_PASSED             0x8C2F
#define GL_SAMPLER_BINDING                0x8919
#define GL_RGB10_A2UI                     0x906F
#define GL_TEXTURE_SWIZZLE_R              0x8E42
#define GL_TEXTURE_SWIZZLE_G              0x8E43
#define GL_TEXTURE_SWIZZLE_B              0x8E44
#define GL_TEXTURE_SWIZZLE_A              0x8E45
#define GL_TEXTURE_SWIZZLE_RGBA           0x8E46
#define GL_TIME_ELAPSED                   0x88BF
#define GL_TIMESTAMP                      0x8E28
#define GL_INT_2_10_10_10_REV             0x8D9F
#endif /* GL_VERSION_3_3 */


#ifndef GL_VERSION_4_0
#define GL_VERSION_4_0 1
#define GL_SAMPLE_SHADING                 0x8C36
#define GL_MIN_SAMPLE_SHADING_VALUE       0x8C37
#define GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5E
#define GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5F
#define GL_TEXTURE_CUBE_MAP_ARRAY         0x9009
#define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY 0x900A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARRAY   0x900B
#define GL_SAMPLER_CUBE_MAP_ARRAY         0x900C
#define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW  0x900D
#define GL_INT_SAMPLER_CUBE_MAP_ARRAY     0x900E
#define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY 0x900F
#define GL_DRAW_INDIRECT_BUFFER           0x8F3F
#define GL_DRAW_INDIRECT_BUFFER_BINDING   0x8F43
#define GL_GEOMETRY_SHADER_INVOCATIONS    0x887F
#define GL_MAX_GEOMETRY_SHADER_INVOCATIONS 0x8E5A
#define GL_MIN_FRAGMENT_INTERPOLATION_OFFSET 0x8E5B
#define GL_MAX_FRAGMENT_INTERPOLATION_OFFSET 0x8E5C
#define GL_FRAGMENT_INTERPOLATION_OFFSET_BITS 0x8E5D
#define GL_MAX_VERTEX_STREAMS             0x8E71
#define GL_DOUBLE_VEC2                    0x8FFC
#define GL_DOUBLE_VEC3                    0x8FFD
#define GL_DOUBLE_VEC4                    0x8FFE
#define GL_DOUBLE_MAT2                    0x8F46
#define GL_DOUBLE_MAT3                    0x8F47
#define GL_DOUBLE_MAT4                    0x8F48
#define GL_DOUBLE_MAT2x3                  0x8F49
#define GL_DOUBLE_MAT2x4                  0x8F4A
#define GL_DOUBLE_MAT3x2                  0x8F4B
#define GL_DOUBLE_MAT3x4                  0x8F4C
#define GL_DOUBLE_MAT4x2                  0x8F4D
#define GL_DOUBLE_MAT4x3                  0x8F4E
#define GL_ACTIVE_SUBROUTINES             0x8DE5
#define GL_ACTIVE_SUBROUTINE_UNIFORMS     0x8DE6
#define GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS 0x8E47
#define GL_ACTIVE_SUBROUTINE_MAX_LENGTH   0x8E48
#define GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH 0x8E49
#define GL_MAX_SUBROUTINES                0x8DE7
#define GL_MAX_SUBROUTINE_UNIFORM_LOCATIONS 0x8DE8
#define GL_NUM_COMPATIBLE_SUBROUTINES     0x8E4A
#define GL_COMPATIBLE_SUBROUTINES         0x8E4B
#define GL_PATCHES                        0x000E
#define GL_PATCH_VERTICES                 0x8E72
#define GL_PATCH_DEFAULT_INNER_LEVEL      0x8E73
#define GL_PATCH_DEFAULT_OUTER_LEVEL      0x8E74
#define GL_TESS_CONTROL_OUTPUT_VERTICES   0x8E75
#define GL_TESS_GEN_MODE                  0x8E76
#define GL_TESS_GEN_SPACING               0x8E77
#define GL_TESS_GEN_VERTEX_ORDER          0x8E78
#define GL_TESS_GEN_POINT_MODE            0x8E79
#define GL_ISOLINES                       0x8E7A
#define GL_FRACTIONAL_ODD                 0x8E7B
#define GL_FRACTIONAL_EVEN                0x8E7C
#define GL_MAX_PATCH_VERTICES             0x8E7D
#define GL_MAX_TESS_GEN_LEVEL             0x8E7E
#define GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS 0x8E7F
#define GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS 0x8E80
#define GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS 0x8E81
#define GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS 0x8E82
#define GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS 0x8E83
#define GL_MAX_TESS_PATCH_COMPONENTS      0x8E84
#define GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS 0x8E85
#define GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS 0x8E86
#define GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS 0x8E89
#define GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS 0x8E8A
#define GL_MAX_TESS_CONTROL_INPUT_COMPONENTS 0x886C
#define GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS 0x886D
#define GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS 0x8E1E
#define GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS 0x8E1F
#define GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER 0x84F0
#define GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER 0x84F1
#define GL_TESS_EVALUATION_SHADER         0x8E87
#define GL_TESS_CONTROL_SHADER            0x8E88
#define GL_TRANSFORM_FEEDBACK             0x8E22
#define GL_TRANSFORM_FEEDBACK_BUFFER_PAUSED 0x8E23
#define GL_TRANSFORM_FEEDBACK_BUFFER_ACTIVE 0x8E24
#define GL_TRANSFORM_FEEDBACK_BINDING     0x8E25
#define GL_MAX_TRANSFORM_FEEDBACK_BUFFERS 0x8E70
#endif /* GL_VERSION_4_0 */

#ifndef GL_VERSION_4_1
#define GL_VERSION_4_1 1
#define GL_FIXED                          0x140C
#define GL_IMPLEMENTATION_COLOR_READ_TYPE 0x8B9A
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#define GL_LOW_FLOAT                      0x8DF0
#define GL_MEDIUM_FLOAT                   0x8DF1
#define GL_HIGH_FLOAT                     0x8DF2
#define GL_LOW_INT                        0x8DF3
#define GL_MEDIUM_INT                     0x8DF4
#define GL_HIGH_INT                       0x8DF5
#define GL_SHADER_COMPILER                0x8DFA
#define GL_SHADER_BINARY_FORMATS          0x8DF8
#define GL_NUM_SHADER_BINARY_FORMATS      0x8DF9
#define GL_MAX_VERTEX_UNIFORM_VECTORS     0x8DFB
#define GL_MAX_VARYING_VECTORS            0x8DFC
#define GL_MAX_FRAGMENT_UNIFORM_VECTORS   0x8DFD
#define GL_RGB565                         0x8D62
#define GL_PROGRAM_BINARY_RETRIEVABLE_HINT 0x8257
#define GL_PROGRAM_BINARY_LENGTH          0x8741
#define GL_NUM_PROGRAM_BINARY_FORMATS     0x87FE
#define GL_PROGRAM_BINARY_FORMATS         0x87FF
#define GL_VERTEX_SHADER_BIT              0x00000001
#define GL_FRAGMENT_SHADER_BIT            0x00000002
#define GL_GEOMETRY_SHADER_BIT            0x00000004
#define GL_TESS_CONTROL_SHADER_BIT        0x00000008
#define GL_TESS_EVALUATION_SHADER_BIT     0x00000010
#define GL_ALL_SHADER_BITS                0xFFFFFFFF
#define GL_PROGRAM_SEPARABLE              0x8258
#define GL_ACTIVE_PROGRAM                 0x8259
#define GL_PROGRAM_PIPELINE_BINDING       0x825A
#define GL_MAX_VIEWPORTS                  0x825B
#define GL_VIEWPORT_SUBPIXEL_BITS         0x825C
#define GL_VIEWPORT_BOUNDS_RANGE          0x825D
#define GL_LAYER_PROVOKING_VERTEX         0x825E
#define GL_VIEWPORT_INDEX_PROVOKING_VERTEX 0x825F
#define GL_UNDEFINED_VERTEX               0x8260
#endif /* GL_VERSION_4_1 */

#ifndef GL_VERSION_4_2
#define GL_VERSION_4_2 1
#define GL_COPY_READ_BUFFER_BINDING       0x8F36
#define GL_COPY_WRITE_BUFFER_BINDING      0x8F37
#define GL_TRANSFORM_FEEDBACK_ACTIVE      0x8E24
#define GL_TRANSFORM_FEEDBACK_PAUSED      0x8E23
#define GL_UNPACK_COMPRESSED_BLOCK_WIDTH  0x9127
#define GL_UNPACK_COMPRESSED_BLOCK_HEIGHT 0x9128
#define GL_UNPACK_COMPRESSED_BLOCK_DEPTH  0x9129
#define GL_UNPACK_COMPRESSED_BLOCK_SIZE   0x912A
#define GL_PACK_COMPRESSED_BLOCK_WIDTH    0x912B
#define GL_PACK_COMPRESSED_BLOCK_HEIGHT   0x912C
#define GL_PACK_COMPRESSED_BLOCK_DEPTH    0x912D
#define GL_PACK_COMPRESSED_BLOCK_SIZE     0x912E
#define GL_NUM_SAMPLE_COUNTS              0x9380
#define GL_MIN_MAP_BUFFER_ALIGNMENT       0x90BC
#define GL_ATOMIC_COUNTER_BUFFER          0x92C0
#define GL_ATOMIC_COUNTER_BUFFER_BINDING  0x92C1
#define GL_ATOMIC_COUNTER_BUFFER_START    0x92C2
#define GL_ATOMIC_COUNTER_BUFFER_SIZE     0x92C3
#define GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE 0x92C4
#define GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS 0x92C5
#define GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES 0x92C6
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER 0x92C7
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER 0x92C8
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER 0x92C9
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER 0x92CA
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER 0x92CB
#define GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS 0x92CC
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS 0x92CD
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS 0x92CE
#define GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS 0x92CF
#define GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS 0x92D0
#define GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS 0x92D1
#define GL_MAX_VERTEX_ATOMIC_COUNTERS     0x92D2
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS 0x92D3
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS 0x92D4
#define GL_MAX_GEOMETRY_ATOMIC_COUNTERS   0x92D5
#define GL_MAX_FRAGMENT_ATOMIC_COUNTERS   0x92D6
#define GL_MAX_COMBINED_ATOMIC_COUNTERS   0x92D7
#define GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE 0x92D8
#define GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS 0x92DC
#define GL_ACTIVE_ATOMIC_COUNTER_BUFFERS  0x92D9
#define GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX 0x92DA
#define GL_UNSIGNED_INT_ATOMIC_COUNTER    0x92DB
#define GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT 0x00000001
#define GL_ELEMENT_ARRAY_BARRIER_BIT      0x00000002
#define GL_UNIFORM_BARRIER_BIT            0x00000004
#define GL_TEXTURE_FETCH_BARRIER_BIT      0x00000008
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020
#define GL_COMMAND_BARRIER_BIT            0x00000040
#define GL_PIXEL_BUFFER_BARRIER_BIT       0x00000080
#define GL_TEXTURE_UPDATE_BARRIER_BIT     0x00000100
#define GL_BUFFER_UPDATE_BARRIER_BIT      0x00000200
#define GL_FRAMEBUFFER_BARRIER_BIT        0x00000400
#define GL_TRANSFORM_FEEDBACK_BARRIER_BIT 0x00000800
#define GL_ATOMIC_COUNTER_BARRIER_BIT     0x00001000
#define GL_ALL_BARRIER_BITS               0xFFFFFFFF
#define GL_MAX_IMAGE_UNITS                0x8F38
#define GL_MAX_COMBINED_IMAGE_UNITS_AND_FRAGMENT_OUTPUTS 0x8F39
#define GL_IMAGE_BINDING_NAME             0x8F3A
#define GL_IMAGE_BINDING_LEVEL            0x8F3B
#define GL_IMAGE_BINDING_LAYERED          0x8F3C
#define GL_IMAGE_BINDING_LAYER            0x8F3D
#define GL_IMAGE_BINDING_ACCESS           0x8F3E
#define GL_IMAGE_1D                       0x904C
#define GL_IMAGE_2D                       0x904D
#define GL_IMAGE_3D                       0x904E
#define GL_IMAGE_2D_RECT                  0x904F
#define GL_IMAGE_CUBE                     0x9050
#define GL_IMAGE_BUFFER                   0x9051
#define GL_IMAGE_1D_ARRAY                 0x9052
#define GL_IMAGE_2D_ARRAY                 0x9053
#define GL_IMAGE_CUBE_MAP_ARRAY           0x9054
#define GL_IMAGE_2D_MULTISAMPLE           0x9055
#define GL_IMAGE_2D_MULTISAMPLE_ARRAY     0x9056
#define GL_INT_IMAGE_1D                   0x9057
#define GL_INT_IMAGE_2D                   0x9058
#define GL_INT_IMAGE_3D                   0x9059
#define GL_INT_IMAGE_2D_RECT              0x905A
#define GL_INT_IMAGE_CUBE                 0x905B
#define GL_INT_IMAGE_BUFFER               0x905C
#define GL_INT_IMAGE_1D_ARRAY             0x905D
#define GL_INT_IMAGE_2D_ARRAY             0x905E
#define GL_INT_IMAGE_CUBE_MAP_ARRAY       0x905F
#define GL_INT_IMAGE_2D_MULTISAMPLE       0x9060
#define GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY 0x9061
#define GL_UNSIGNED_INT_IMAGE_1D          0x9062
#define GL_UNSIGNED_INT_IMAGE_2D          0x9063
#define GL_UNSIGNED_INT_IMAGE_3D          0x9064
#define GL_UNSIGNED_INT_IMAGE_2D_RECT     0x9065
#define GL_UNSIGNED_INT_IMAGE_CUBE        0x9066
#define GL_UNSIGNED_INT_IMAGE_BUFFER      0x9067
#define GL_UNSIGNED_INT_IMAGE_1D_ARRAY    0x9068
#define GL_UNSIGNED_INT_IMAGE_2D_ARRAY    0x9069
#define GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY 0x906A
#define GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE 0x906B
#define GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY 0x906C
#define GL_MAX_IMAGE_SAMPLES              0x906D
#define GL_IMAGE_BINDING_FORMAT           0x906E
#define GL_IMAGE_FORMAT_COMPATIBILITY_TYPE 0x90C7
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE 0x90C8
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS 0x90C9
#define GL_MAX_VERTEX_IMAGE_UNIFORMS      0x90CA
#define GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS 0x90CB
#define GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS 0x90CC
#define GL_MAX_GEOMETRY_IMAGE_UNIFORMS    0x90CD
#define GL_MAX_FRAGMENT_IMAGE_UNIFORMS    0x90CE
#define GL_MAX_COMBINED_IMAGE_UNIFORMS    0x90CF
#define GL_COMPRESSED_RGBA_BPTC_UNORM     0x8E8C
#define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM 0x8E8D
#define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT 0x8E8E
#define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT 0x8E8F
#define GL_TEXTURE_IMMUTABLE_FORMAT       0x912F
#endif /* GL_VERSION_4_2 */

#ifndef GL_VERSION_4_3
#define GL_VERSION_4_3 1
typedef void (APIENTRY  *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#define GL_NUM_SHADING_LANGUAGE_VERSIONS  0x82E9
#define GL_VERTEX_ATTRIB_ARRAY_LONG       0x874E
#define GL_COMPRESSED_RGB8_ETC2           0x9274
#define GL_COMPRESSED_SRGB8_ETC2          0x9275
#define GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9276
#define GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9277
#define GL_COMPRESSED_RGBA8_ETC2_EAC      0x9278
#define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279
#define GL_COMPRESSED_R11_EAC             0x9270
#define GL_COMPRESSED_SIGNED_R11_EAC      0x9271
#define GL_COMPRESSED_RG11_EAC            0x9272
#define GL_COMPRESSED_SIGNED_RG11_EAC     0x9273
#define GL_PRIMITIVE_RESTART_FIXED_INDEX  0x8D69
#define GL_ANY_SAMPLES_PASSED_CONSERVATIVE 0x8D6A
#define GL_MAX_ELEMENT_INDEX              0x8D6B
#define GL_COMPUTE_SHADER                 0x91B9
#define GL_MAX_COMPUTE_UNIFORM_BLOCKS     0x91BB
#define GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS 0x91BC
#define GL_MAX_COMPUTE_IMAGE_UNIFORMS     0x91BD
#define GL_MAX_COMPUTE_SHARED_MEMORY_SIZE 0x8262
#define GL_MAX_COMPUTE_UNIFORM_COMPONENTS 0x8263
#define GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS 0x8264
#define GL_MAX_COMPUTE_ATOMIC_COUNTERS    0x8265
#define GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS 0x8266
#define GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS 0x90EB
#define GL_MAX_COMPUTE_WORK_GROUP_COUNT   0x91BE
#define GL_MAX_COMPUTE_WORK_GROUP_SIZE    0x91BF
#define GL_COMPUTE_WORK_GROUP_SIZE        0x8267
#define GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER 0x90EC
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER 0x90ED
#define GL_DISPATCH_INDIRECT_BUFFER       0x90EE
#define GL_DISPATCH_INDIRECT_BUFFER_BINDING 0x90EF
#define GL_COMPUTE_SHADER_BIT             0x00000020
#define GL_DEBUG_OUTPUT_SYNCHRONOUS       0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION        0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM      0x8245
#define GL_DEBUG_SOURCE_API               0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM     0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER   0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY       0x8249
#define GL_DEBUG_SOURCE_APPLICATION       0x824A
#define GL_DEBUG_SOURCE_OTHER             0x824B
#define GL_DEBUG_TYPE_ERROR               0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR  0x824E
#define GL_DEBUG_TYPE_PORTABILITY         0x824F
#define GL_DEBUG_TYPE_PERFORMANCE         0x8250
#define GL_DEBUG_TYPE_OTHER               0x8251
#define GL_MAX_DEBUG_MESSAGE_LENGTH       0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES      0x9144
#define GL_DEBUG_LOGGED_MESSAGES          0x9145
#define GL_DEBUG_SEVERITY_HIGH            0x9146
#define GL_DEBUG_SEVERITY_MEDIUM          0x9147
#define GL_DEBUG_SEVERITY_LOW             0x9148
#define GL_DEBUG_TYPE_MARKER              0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP          0x8269
#define GL_DEBUG_TYPE_POP_GROUP           0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION    0x826B
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH    0x826C
#define GL_DEBUG_GROUP_STACK_DEPTH        0x826D
#define GL_BUFFER                         0x82E0
#define GL_SHADER                         0x82E1
#define GL_PROGRAM                        0x82E2
#define GL_QUERY                          0x82E3
#define GL_PROGRAM_PIPELINE               0x82E4
#define GL_SAMPLER                        0x82E6
#define GL_MAX_LABEL_LENGTH               0x82E8
#define GL_DEBUG_OUTPUT                   0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT         0x00000002
#define GL_MAX_UNIFORM_LOCATIONS          0x826E
#define GL_FRAMEBUFFER_DEFAULT_WIDTH      0x9310
#define GL_FRAMEBUFFER_DEFAULT_HEIGHT     0x9311
#define GL_FRAMEBUFFER_DEFAULT_LAYERS     0x9312
#define GL_FRAMEBUFFER_DEFAULT_SAMPLES    0x9313
#define GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS 0x9314
#define GL_MAX_FRAMEBUFFER_WIDTH          0x9315
#define GL_MAX_FRAMEBUFFER_HEIGHT         0x9316
#define GL_MAX_FRAMEBUFFER_LAYERS         0x9317
#define GL_MAX_FRAMEBUFFER_SAMPLES        0x9318
#define GL_INTERNALFORMAT_SUPPORTED       0x826F
#define GL_INTERNALFORMAT_PREFERRED       0x8270
#define GL_INTERNALFORMAT_RED_SIZE        0x8271
#define GL_INTERNALFORMAT_GREEN_SIZE      0x8272
#define GL_INTERNALFORMAT_BLUE_SIZE       0x8273
#define GL_INTERNALFORMAT_ALPHA_SIZE      0x8274
#define GL_INTERNALFORMAT_DEPTH_SIZE      0x8275
#define GL_INTERNALFORMAT_STENCIL_SIZE    0x8276
#define GL_INTERNALFORMAT_SHARED_SIZE     0x8277
#define GL_INTERNALFORMAT_RED_TYPE        0x8278
#define GL_INTERNALFORMAT_GREEN_TYPE      0x8279
#define GL_INTERNALFORMAT_BLUE_TYPE       0x827A
#define GL_INTERNALFORMAT_ALPHA_TYPE      0x827B
#define GL_INTERNALFORMAT_DEPTH_TYPE      0x827C
#define GL_INTERNALFORMAT_STENCIL_TYPE    0x827D
#define GL_MAX_WIDTH                      0x827E
#define GL_MAX_HEIGHT                     0x827F
#define GL_MAX_DEPTH                      0x8280
#define GL_MAX_LAYERS                     0x8281
#define GL_MAX_COMBINED_DIMENSIONS        0x8282
#define GL_COLOR_COMPONENTS               0x8283
#define GL_DEPTH_COMPONENTS               0x8284
#define GL_STENCIL_COMPONENTS             0x8285
#define GL_COLOR_RENDERABLE               0x8286
#define GL_DEPTH_RENDERABLE               0x8287
#define GL_STENCIL_RENDERABLE             0x8288
#define GL_FRAMEBUFFER_RENDERABLE         0x8289
#define GL_FRAMEBUFFER_RENDERABLE_LAYERED 0x828A
#define GL_FRAMEBUFFER_BLEND              0x828B
#define GL_READ_PIXELS                    0x828C
#define GL_READ_PIXELS_FORMAT             0x828D
#define GL_READ_PIXELS_TYPE               0x828E
#define GL_TEXTURE_IMAGE_FORMAT           0x828F
#define GL_TEXTURE_IMAGE_TYPE             0x8290
#define GL_GET_TEXTURE_IMAGE_FORMAT       0x8291
#define GL_GET_TEXTURE_IMAGE_TYPE         0x8292
#define GL_MIPMAP                         0x8293
#define GL_MANUAL_GENERATE_MIPMAP         0x8294
#define GL_AUTO_GENERATE_MIPMAP           0x8295
#define GL_COLOR_ENCODING                 0x8296
#define GL_SRGB_READ                      0x8297
#define GL_SRGB_WRITE                     0x8298
#define GL_FILTER                         0x829A
#define GL_VERTEX_TEXTURE                 0x829B
#define GL_TESS_CONTROL_TEXTURE           0x829C
#define GL_TESS_EVALUATION_TEXTURE        0x829D
#define GL_GEOMETRY_TEXTURE               0x829E
#define GL_FRAGMENT_TEXTURE               0x829F
#define GL_COMPUTE_TEXTURE                0x82A0
#define GL_TEXTURE_SHADOW                 0x82A1
#define GL_TEXTURE_GATHER                 0x82A2
#define GL_TEXTURE_GATHER_SHADOW          0x82A3
#define GL_SHADER_IMAGE_LOAD              0x82A4
#define GL_SHADER_IMAGE_STORE             0x82A5
#define GL_SHADER_IMAGE_ATOMIC            0x82A6
#define GL_IMAGE_TEXEL_SIZE               0x82A7
#define GL_IMAGE_COMPATIBILITY_CLASS      0x82A8
#define GL_IMAGE_PIXEL_FORMAT             0x82A9
#define GL_IMAGE_PIXEL_TYPE               0x82AA
#define GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST 0x82AC
#define GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST 0x82AD
#define GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE 0x82AE
#define GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE 0x82AF
#define GL_TEXTURE_COMPRESSED_BLOCK_WIDTH 0x82B1
#define GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT 0x82B2
#define GL_TEXTURE_COMPRESSED_BLOCK_SIZE  0x82B3
#define GL_CLEAR_BUFFER                   0x82B4
#define GL_TEXTURE_VIEW                   0x82B5
#define GL_VIEW_COMPATIBILITY_CLASS       0x82B6
#define GL_FULL_SUPPORT                   0x82B7
#define GL_CAVEAT_SUPPORT                 0x82B8
#define GL_IMAGE_CLASS_4_X_32             0x82B9
#define GL_IMAGE_CLASS_2_X_32             0x82BA
#define GL_IMAGE_CLASS_1_X_32             0x82BB
#define GL_IMAGE_CLASS_4_X_16             0x82BC
#define GL_IMAGE_CLASS_2_X_16             0x82BD
#define GL_IMAGE_CLASS_1_X_16             0x82BE
#define GL_IMAGE_CLASS_4_X_8              0x82BF
#define GL_IMAGE_CLASS_2_X_8              0x82C0
#define GL_IMAGE_CLASS_1_X_8              0x82C1
#define GL_IMAGE_CLASS_11_11_10           0x82C2
#define GL_IMAGE_CLASS_10_10_10_2         0x82C3
#define GL_VIEW_CLASS_128_BITS            0x82C4
#define GL_VIEW_CLASS_96_BITS             0x82C5
#define GL_VIEW_CLASS_64_BITS             0x82C6
#define GL_VIEW_CLASS_48_BITS             0x82C7
#define GL_VIEW_CLASS_32_BITS             0x82C8
#define GL_VIEW_CLASS_24_BITS             0x82C9
#define GL_VIEW_CLASS_16_BITS             0x82CA
#define GL_VIEW_CLASS_8_BITS              0x82CB
#define GL_VIEW_CLASS_S3TC_DXT1_RGB       0x82CC
#define GL_VIEW_CLASS_S3TC_DXT1_RGBA      0x82CD
#define GL_VIEW_CLASS_S3TC_DXT3_RGBA      0x82CE
#define GL_VIEW_CLASS_S3TC_DXT5_RGBA      0x82CF
#define GL_VIEW_CLASS_RGTC1_RED           0x82D0
#define GL_VIEW_CLASS_RGTC2_RG            0x82D1
#define GL_VIEW_CLASS_BPTC_UNORM          0x82D2
#define GL_VIEW_CLASS_BPTC_FLOAT          0x82D3
#define GL_UNIFORM                        0x92E1
#define GL_UNIFORM_BLOCK                  0x92E2
#define GL_PROGRAM_INPUT                  0x92E3
#define GL_PROGRAM_OUTPUT                 0x92E4
#define GL_BUFFER_VARIABLE                0x92E5
#define GL_SHADER_STORAGE_BLOCK           0x92E6
#define GL_VERTEX_SUBROUTINE              0x92E8
#define GL_TESS_CONTROL_SUBROUTINE        0x92E9
#define GL_TESS_EVALUATION_SUBROUTINE     0x92EA
#define GL_GEOMETRY_SUBROUTINE            0x92EB
#define GL_FRAGMENT_SUBROUTINE            0x92EC
#define GL_COMPUTE_SUBROUTINE             0x92ED
#define GL_VERTEX_SUBROUTINE_UNIFORM      0x92EE
#define GL_TESS_CONTROL_SUBROUTINE_UNIFORM 0x92EF
#define GL_TESS_EVALUATION_SUBROUTINE_UNIFORM 0x92F0
#define GL_GEOMETRY_SUBROUTINE_UNIFORM    0x92F1
#define GL_FRAGMENT_SUBROUTINE_UNIFORM    0x92F2
#define GL_COMPUTE_SUBROUTINE_UNIFORM     0x92F3
#define GL_TRANSFORM_FEEDBACK_VARYING     0x92F4
#define GL_ACTIVE_RESOURCES               0x92F5
#define GL_MAX_NAME_LENGTH                0x92F6
#define GL_MAX_NUM_ACTIVE_VARIABLES       0x92F7
#define GL_MAX_NUM_COMPATIBLE_SUBROUTINES 0x92F8
#define GL_NAME_LENGTH                    0x92F9
#define GL_TYPE                           0x92FA
#define GL_ARRAY_SIZE                     0x92FB
#define GL_OFFSET                         0x92FC
#define GL_BLOCK_INDEX                    0x92FD
#define GL_ARRAY_STRIDE                   0x92FE
#define GL_MATRIX_STRIDE                  0x92FF
#define GL_IS_ROW_MAJOR                   0x9300
#define GL_ATOMIC_COUNTER_BUFFER_INDEX    0x9301
#define GL_BUFFER_BINDING                 0x9302
#define GL_BUFFER_DATA_SIZE               0x9303
#define GL_NUM_ACTIVE_VARIABLES           0x9304
#define GL_ACTIVE_VARIABLES               0x9305
#define GL_REFERENCED_BY_VERTEX_SHADER    0x9306
#define GL_REFERENCED_BY_TESS_CONTROL_SHADER 0x9307
#define GL_REFERENCED_BY_TESS_EVALUATION_SHADER 0x9308
#define GL_REFERENCED_BY_GEOMETRY_SHADER  0x9309
#define GL_REFERENCED_BY_FRAGMENT_SHADER  0x930A
#define GL_REFERENCED_BY_COMPUTE_SHADER   0x930B
#define GL_TOP_LEVEL_ARRAY_SIZE           0x930C
#define GL_TOP_LEVEL_ARRAY_STRIDE         0x930D
#define GL_LOCATION                       0x930E
#define GL_LOCATION_INDEX                 0x930F
#define GL_IS_PER_PATCH                   0x92E7
#define GL_SHADER_STORAGE_BUFFER          0x90D2
#define GL_SHADER_STORAGE_BUFFER_BINDING  0x90D3
#define GL_SHADER_STORAGE_BUFFER_START    0x90D4
#define GL_SHADER_STORAGE_BUFFER_SIZE     0x90D5
#define GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS 0x90D6
#define GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS 0x90D7
#define GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS 0x90D8
#define GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS 0x90D9
#define GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS 0x90DA
#define GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS 0x90DB
#define GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS 0x90DC
#define GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS 0x90DD
#define GL_MAX_SHADER_STORAGE_BLOCK_SIZE  0x90DE
#define GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT 0x90DF
#define GL_SHADER_STORAGE_BARRIER_BIT     0x00002000
#define GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES 0x8F39
#define GL_DEPTH_STENCIL_TEXTURE_MODE     0x90EA
#define GL_TEXTURE_BUFFER_OFFSET          0x919D
#define GL_TEXTURE_BUFFER_SIZE            0x919E
#define GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT 0x919F
#define GL_TEXTURE_VIEW_MIN_LEVEL         0x82DB
#define GL_TEXTURE_VIEW_NUM_LEVELS        0x82DC
#define GL_TEXTURE_VIEW_MIN_LAYER         0x82DD
#define GL_TEXTURE_VIEW_NUM_LAYERS        0x82DE
#define GL_TEXTURE_IMMUTABLE_LEVELS       0x82DF
#define GL_VERTEX_ATTRIB_BINDING          0x82D4
#define GL_VERTEX_ATTRIB_RELATIVE_OFFSET  0x82D5
#define GL_VERTEX_BINDING_DIVISOR         0x82D6
#define GL_VERTEX_BINDING_OFFSET          0x82D7
#define GL_VERTEX_BINDING_STRIDE          0x82D8
#define GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET 0x82D9
#define GL_MAX_VERTEX_ATTRIB_BINDINGS     0x82DA
#define GL_VERTEX_BINDING_BUFFER          0x8F4F
#endif /* GL_VERSION_4_3 */

#ifndef GL_VERSION_4_4
#define GL_VERSION_4_4 1
#define GL_MAX_VERTEX_ATTRIB_STRIDE       0x82E5
#define GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED 0x8221
#define GL_TEXTURE_BUFFER_BINDING         0x8C2A
#define GL_MAP_PERSISTENT_BIT             0x0040
#define GL_MAP_COHERENT_BIT               0x0080
#define GL_DYNAMIC_STORAGE_BIT            0x0100
#define GL_CLIENT_STORAGE_BIT             0x0200
#define GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT 0x00004000
#define GL_BUFFER_IMMUTABLE_STORAGE       0x821F
#define GL_BUFFER_STORAGE_FLAGS           0x8220
#define GL_CLEAR_TEXTURE                  0x9365
#define GL_LOCATION_COMPONENT             0x934A
#define GL_TRANSFORM_FEEDBACK_BUFFER_INDEX 0x934B
#define GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE 0x934C
#define GL_QUERY_BUFFER                   0x9192
#define GL_QUERY_BUFFER_BARRIER_BIT       0x00008000
#define GL_QUERY_BUFFER_BINDING           0x9193
#define GL_QUERY_RESULT_NO_WAIT           0x9194
#define GL_MIRROR_CLAMP_TO_EDGE           0x8743
#endif /* GL_VERSION_4_4 */

#ifndef GL_VERSION_4_5
#define GL_VERSION_4_5 1
#define GL_CONTEXT_LOST                   0x0507
#define GL_NEGATIVE_ONE_TO_ONE            0x935E
#define GL_ZERO_TO_ONE                    0x935F
#define GL_CLIP_ORIGIN                    0x935C
#define GL_CLIP_DEPTH_MODE                0x935D
#define GL_QUERY_WAIT_INVERTED            0x8E17
#define GL_QUERY_NO_WAIT_INVERTED         0x8E18
#define GL_QUERY_BY_REGION_WAIT_INVERTED  0x8E19
#define GL_QUERY_BY_REGION_NO_WAIT_INVERTED 0x8E1A
#define GL_MAX_CULL_DISTANCES             0x82F9
#define GL_MAX_COMBINED_CLIP_AND_CULL_DISTANCES 0x82FA
#define GL_TEXTURE_TARGET                 0x1006
#define GL_QUERY_TARGET                   0x82EA
#define GL_GUILTY_CONTEXT_RESET           0x8253
#define GL_INNOCENT_CONTEXT_RESET         0x8254
#define GL_UNKNOWN_CONTEXT_RESET          0x8255
#define GL_RESET_NOTIFICATION_STRATEGY    0x8256
#define GL_LOSE_CONTEXT_ON_RESET          0x8252
#define GL_NO_RESET_NOTIFICATION          0x8261
#define GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT 0x00000004
#define GL_CONTEXT_RELEASE_BEHAVIOR       0x82FB
#define GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH 0x82FC
#endif /* GL_VERSION_4_5 */

#ifndef GL_VERSION_4_6
#define GL_VERSION_4_6 1
#define GL_SHADER_BINARY_FORMAT_SPIR_V    0x9551
#define GL_SPIR_V_BINARY                  0x9552
#define GL_PARAMETER_BUFFER               0x80EE
#define GL_PARAMETER_BUFFER_BINDING       0x80EF
#define GL_CONTEXT_FLAG_NO_ERROR_BIT      0x00000008
#define GL_VERTICES_SUBMITTED             0x82EE
#define GL_PRIMITIVES_SUBMITTED           0x82EF
#define GL_VERTEX_SHADER_INVOCATIONS      0x82F0
#define GL_TESS_CONTROL_SHADER_PATCHES    0x82F1
#define GL_TESS_EVALUATION_SHADER_INVOCATIONS 0x82F2
#define GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED 0x82F3
#define GL_FRAGMENT_SHADER_INVOCATIONS    0x82F4
#define GL_COMPUTE_SHADER_INVOCATIONS     0x82F5
#define GL_CLIPPING_INPUT_PRIMITIVES      0x82F6
#define GL_CLIPPING_OUTPUT_PRIMITIVES     0x82F7
#define GL_POLYGON_OFFSET_CLAMP           0x8E1B
#define GL_SPIR_V_EXTENSIONS              0x9553
#define GL_NUM_SPIR_V_EXTENSIONS          0x9554
#define GL_TEXTURE_MAX_ANISOTROPY         0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY     0x84FF
#define GL_TRANSFORM_FEEDBACK_OVERFLOW    0x82EC
#define GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW 0x82ED
#endif /* GL_VERSION_4_6 */

#define HOGL_IMPLEMENT
#ifdef HOGL_IMPLEMENT

#if defined(_WIN32) || defined(_WIN64)

#define LOAD_GL_PROC(X) \
hogl_##X = (X##_proctype*)wglGetProcAddress(#X); \
if(!(hogl_##X)) hogl_##X = (X##_proctype*)GetProcAddress(glcode, #X)

#define GL_CALL __stdcall

#elif defined(__linux__)
#define LOAD_GL_PROC(X) \
hogl_##X = (X##_proctype*)glXGetProcAddress((const GLubyte*)#X); \
if(!(hogl_##X)) hogl_##X = 0 

#define GL_CALL
#endif

#define INSTANTIATE_GLCALL(R, X, A) \
typedef R GL_CALL X##_proctype A;	\
X##_proctype* hogl_##X = 0

INSTANTIATE_GLCALL(void, glClear, (GLbitfield mask));
INSTANTIATE_GLCALL(void, glClearBufferiv, (GLenum buffer, GLint drawbuffer, const GLint* value));
INSTANTIATE_GLCALL(void, glClearBufferuiv, (GLenum buffer, GLint drawbuffer, const GLuint* value));
INSTANTIATE_GLCALL(void, glClearBufferfv, (GLenum buffer, GLint drawbuffer, const GLfloat* value));
INSTANTIATE_GLCALL(void, glClearBufferfi, (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil));
INSTANTIATE_GLCALL(void, glClearNamedFramebufferiv, (GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value));
INSTANTIATE_GLCALL(void, glClearNamedFramebufferuiv, (GLuint framebuffer, GLuint buffer, GLint drawbuffer, const GLuint* value));
INSTANTIATE_GLCALL(void, glClearNamedFramebufferfv, (GLuint framebuffer, GLuint buffer, GLint drawbuffer, const GLfloat* value));
INSTANTIATE_GLCALL(void, glClearNamedFramebufferfi, (GLuint framebuffer, GLuint buffer, GLint drawbuffer, const GLfloat depth, GLint stencil));
INSTANTIATE_GLCALL(void, glNamedFramebufferReadBuffer, (GLuint framebuffer, GLenum mode));
INSTANTIATE_GLCALL(void, glReadnPixels, (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void* data));
INSTANTIATE_GLCALL(void, glClearColor, (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha));
INSTANTIATE_GLCALL(void, glClearDepth, (GLdouble depth));
INSTANTIATE_GLCALL(void, glClearStencil, (GLint s));
INSTANTIATE_GLCALL(void, glDrawBuffer, (GLenum buf));
INSTANTIATE_GLCALL(void, glFinish, ());
INSTANTIATE_GLCALL(void, glFlush, ());
INSTANTIATE_GLCALL(void, glReadBuffer, (GLenum mode));
INSTANTIATE_GLCALL(void, glReadPixels, (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* data));

INSTANTIATE_GLCALL(void, glActiveTexture, (GLenum texture));
INSTANTIATE_GLCALL(void, glBindImageTexture, (GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format));
INSTANTIATE_GLCALL(void, glBindImageTextures, (GLuint first, GLsizei count, const GLuint *textures));
INSTANTIATE_GLCALL(void, glBindTexture, (GLenum target, GLuint texture));
INSTANTIATE_GLCALL(void, glBindTextureUnit, (GLuint unit, GLuint texture));
INSTANTIATE_GLCALL(void, glBindTextures, (GLuint first, GLsizei count, const GLuint* textures));
INSTANTIATE_GLCALL(void, glClearTexImage, (GLuint texture, GLint level, GLenum format, GLenum type, const void* data));
INSTANTIATE_GLCALL(void, glClearTexSubImage, (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* data));
INSTANTIATE_GLCALL(void, glCompressedTexImage1D, (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid* data));
INSTANTIATE_GLCALL(void, glCompressedTexImage2D, (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data));
INSTANTIATE_GLCALL(void, glCompressedTexImage3D, (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid* data));
INSTANTIATE_GLCALL(void, glCompressedTexSubImage1D, (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid* data));
INSTANTIATE_GLCALL(void, glCompressedTextureSubImage1D, (GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data));
INSTANTIATE_GLCALL(void, glCompressedTexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data));
INSTANTIATE_GLCALL(void, glCompressedTextureSubImage2D, (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data));
INSTANTIATE_GLCALL(void, glCompressedTexSubImage3D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid* data));
INSTANTIATE_GLCALL(void, glCompressedTextureSubImage3D, (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data));
INSTANTIATE_GLCALL(void, glCopyTextureSubImage1D, (GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width));
INSTANTIATE_GLCALL(void, glCopyTextureSubImage2D, (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height));
INSTANTIATE_GLCALL(void, glCopyImageSubData, (GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth));
INSTANTIATE_GLCALL(void, glCopyTexImage1D, (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border));
INSTANTIATE_GLCALL(void, glCopyTexImage2D, (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border));
INSTANTIATE_GLCALL(void, glCopyTexSubImage1D, (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width));
INSTANTIATE_GLCALL(void, glCopyTexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height));
INSTANTIATE_GLCALL(void, glCopyTexSubImage3D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height));
INSTANTIATE_GLCALL(void, glCopyTextureSubImage3D, (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height));
INSTANTIATE_GLCALL(void, glCreateTextures, (GLenum target, GLsizei n, GLuint *textures));
INSTANTIATE_GLCALL(void, glDeleteTextures, (GLsizei n, const GLuint* textures));
INSTANTIATE_GLCALL(void, glGenTextures, (GLsizei n, GLuint* textures));
INSTANTIATE_GLCALL(void, glGetCompressedTexImage, (GLenum target, GLint level, GLvoid * pixels));
INSTANTIATE_GLCALL(void, glGetnCompressedTexImage, (GLenum target, GLint level, GLsizei bufSize, void* pixels));
INSTANTIATE_GLCALL(void, glGetCompressedTextureImage, (GLuint texture, GLint level, GLsizei bufSize, void* pixels));
INSTANTIATE_GLCALL(void, glGetCompressedTextureSubImage, (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei bufSize, void* pixels));
INSTANTIATE_GLCALL(void, glGetTexImage, (GLenum target, GLint level, GLenum format, GLenum type, GLvoid* pixels));
INSTANTIATE_GLCALL(void, glGetnTexImage, (GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels));
INSTANTIATE_GLCALL(void, glGetTextureImage, (GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels));
INSTANTIATE_GLCALL(void, glGetTexLevelParameterfv, (GLenum target, GLint level, GLenum pname, GLfloat* params));
INSTANTIATE_GLCALL(void, glGetTexLevelParameteriv, (GLenum target, GLint level, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetTextureLevelParameterfv, (GLuint texture, GLint level, GLenum pname, GLfloat* params));
INSTANTIATE_GLCALL(void, glGetTextureLevelParameteriv, (GLuint texture, GLint level, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetTexParameterfv, (GLenum target, GLint level, GLenum pname, GLfloat* params));
INSTANTIATE_GLCALL(void, glGetTexParameteriv, (GLenum target, GLint level, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetTexParameterIiv, (GLenum target, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetTexParameterIuiv, (GLenum target, GLenum pname, GLuint* params));
INSTANTIATE_GLCALL(void, glGetTextureParameterfv, (GLuint texture, GLenum pname, GLfloat* params));
INSTANTIATE_GLCALL(void, glGetTextureParameteriv, (GLuint texture, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetTextureParameterIiv, (GLuint texture, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetTextureParameterIuiv, (GLuint texture, GLenum pname, GLuint* params));
INSTANTIATE_GLCALL(void, glGetTextureSubImage, (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void* pixels));
INSTANTIATE_GLCALL(void, glInvalidateTexImage, (GLuint texture, GLint level));
INSTANTIATE_GLCALL(void, glInvalidateTexSubImage, (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth));
INSTANTIATE_GLCALL(GLboolean, glIsTexture, (GLuint texture));
INSTANTIATE_GLCALL(void, glTexBuffer, (GLenum target, GLenum internalFormat, GLuint buffer));
INSTANTIATE_GLCALL(void, glTextureBuffer, (GLuint texture, GLenum internalformat, GLuint buffer));
INSTANTIATE_GLCALL(void, glTexBufferRange, (GLenum target, GLenum internalFormat, GLuint buffer, GLintptr offset, GLsizeiptr size));
INSTANTIATE_GLCALL(void, glTextureBufferRange, (GLenum target, GLenum internalFormat, GLuint buffer, GLintptr offset, GLsizei size));
INSTANTIATE_GLCALL(void, glTexImage1D, (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* data));
INSTANTIATE_GLCALL(void, glTexImage2D, (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data));
INSTANTIATE_GLCALL(void, glTexImage2DMultisample, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations));
INSTANTIATE_GLCALL(void, glTexImage3D, (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* data));
INSTANTIATE_GLCALL(void, glTexImage3DMultisample, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations));
INSTANTIATE_GLCALL(void, glTexParameterf, (GLenum target, GLenum pname, GLfloat param));
INSTANTIATE_GLCALL(void, glTexParameteri, (GLenum target, GLenum pname, GLint param));
INSTANTIATE_GLCALL(void, glTextureParameterf, (GLenum target, GLenum pname, GLfloat param));
INSTANTIATE_GLCALL(void, glTextureParameteri, (GLenum target, GLenum pname, GLint param));
INSTANTIATE_GLCALL(void, glTexParameterfv, (GLenum target, GLenum pname, const GLfloat* params));
INSTANTIATE_GLCALL(void, glTexParameteriv, (GLenum target, GLenum pname, const GLint* params));
INSTANTIATE_GLCALL(void, glTexParameterIiv, (GLenum target, GLenum pname, const GLint* params));
INSTANTIATE_GLCALL(void, glTexParameterIuiv, (GLenum target, GLenum pname, const GLuint* params));
INSTANTIATE_GLCALL(void, glTextureParameterfv, (GLuint texture, GLenum pname, const GLfloat* paramtexture));
INSTANTIATE_GLCALL(void, glTextureParameteriv, (GLuint texture, GLenum pname, const GLint* param));
INSTANTIATE_GLCALL(void, glTextureParameterIiv, (GLuint texture, GLenum pname, const GLint* params));
INSTANTIATE_GLCALL(void, glTextureParameterIuiv, (GLuint texture, GLenum pname, const GLuint* params));
INSTANTIATE_GLCALL(void, glTexStorage1D, (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width));
INSTANTIATE_GLCALL(void, glTextureStorage1D, (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width));
INSTANTIATE_GLCALL(void, glTexStorage2D, (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height));
INSTANTIATE_GLCALL(void, glTextureStorage2D, (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height));
INSTANTIATE_GLCALL(void, glTexStorage2DMultisample, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations));
INSTANTIATE_GLCALL(void, glTextureStorage2DMultisample, (GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations));
INSTANTIATE_GLCALL(void, glTexStorage3D, (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth));
INSTANTIATE_GLCALL(void, glTextureStorage3D, (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth));
INSTANTIATE_GLCALL(void, glTexStorage3DMultisample, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations));
INSTANTIATE_GLCALL(void, glTextureStorage3DMultisample, (GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations));
INSTANTIATE_GLCALL(void, glTexSubImage1D, (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid* pixels));
INSTANTIATE_GLCALL(void, glTextureSubImage1D, (GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels));
INSTANTIATE_GLCALL(void, glTexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels));
INSTANTIATE_GLCALL(void, glTextureSubImage2D, (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels));
INSTANTIATE_GLCALL(void, glTexSubImage3D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid* pixels));
INSTANTIATE_GLCALL(void, glTextureSubImage3D, (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels));
INSTANTIATE_GLCALL(void, glTextureView, (GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers));

INSTANTIATE_GLCALL(void, glBindFramebuffer, (GLenum target, GLuint framebuffer));
INSTANTIATE_GLCALL(void, glBindRenderbuffer, (GLenum target, GLuint renderbuffer));
INSTANTIATE_GLCALL(void, glBlitFramebuffer, (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter));
INSTANTIATE_GLCALL(void, glBlitNamedFramebuffer, (GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter));
INSTANTIATE_GLCALL(GLenum, glCheckFramebufferStatus, (GLenum target));
INSTANTIATE_GLCALL(GLenum, glCheckNamedFramebufferStatus, (GLuint framebuffer, GLenum target));
INSTANTIATE_GLCALL(void, glCreateFramebuffers, (GLsizei n, GLuint* ids));
INSTANTIATE_GLCALL(void, glCreateRenderbuffers, (GLsizei n, GLuint* renderbuffers));
INSTANTIATE_GLCALL(void, glDeleteFramebuffers, (GLsizei n, GLuint* framebuffers));
INSTANTIATE_GLCALL(void, glDeleteRenderbuffers, (GLsizei n, GLuint* renderbuffers));
INSTANTIATE_GLCALL(void, glDrawBuffers, (GLsizei n, const GLenum* bufs));
INSTANTIATE_GLCALL(void, glNamedFramebufferDrawBuffers, (GLuint framebuffer, GLsizei n, const GLenum* bufs));
INSTANTIATE_GLCALL(void, glFramebufferParameteri, (GLenum target, GLenum pname, GLint param));
INSTANTIATE_GLCALL(void, glNamedFramebufferParameteri, (GLuint framebuffer, GLenum pname, GLint param));
INSTANTIATE_GLCALL(void, glFramebufferRenderbuffer, (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer));
INSTANTIATE_GLCALL(void, glNamedFramebufferRenderbuffer, (GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer));
INSTANTIATE_GLCALL(void, glFramebufferTexture, (GLenum target, GLenum attachment, GLuint texture, GLint level));
INSTANTIATE_GLCALL(void, glFramebufferTexture1D, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level));
INSTANTIATE_GLCALL(void, glFramebufferTexture2D, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level));
INSTANTIATE_GLCALL(void, glFramebufferTexture3D, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint layer));
INSTANTIATE_GLCALL(void, glNamedFramebufferTexture, (GLuint framebuffer, GLenum attachment, GLuint texture, GLint level));
INSTANTIATE_GLCALL(void, glFramebufferTextureLayer, (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer));
INSTANTIATE_GLCALL(void, glNamedFramebufferTextureLayer, (GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer));
INSTANTIATE_GLCALL(void, glGenFramebuffers, (GLsizei n, GLuint* ids));
INSTANTIATE_GLCALL(void, glGenRenderbuffers, (GLsizei n, GLuint* renderbuffers));
INSTANTIATE_GLCALL(void, glGenerateMipmap, (GLenum target));
INSTANTIATE_GLCALL(void, glGenerateTextureMipmap, (GLuint texture));
INSTANTIATE_GLCALL(void, glGetFramebufferAttachmentParameteriv, (GLenum target, GLenum attachment, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetNamedFramebufferAttachmentParameteriv, (GLuint framebuffer, GLenum attachment, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetFramebufferParameteriv, (GLenum target, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetNamedFramebufferParameteriv, (GLuint framebuffer, GLenum pname, GLint* param));
INSTANTIATE_GLCALL(void, glGetRenderbufferParameteriv, (GLenum target, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetNamedRenderbufferParameteriv, (GLuint renderbuffer, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glInvalidateFramebuffer, (GLenum target, GLsizei numAttachments, const GLenum* attachments));
INSTANTIATE_GLCALL(void, glInvalidateNamedFramebufferData, (GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments));
INSTANTIATE_GLCALL(void, glInvalidateSubFramebuffer, (GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLint width, GLint height));
INSTANTIATE_GLCALL(void, glInvalidateNamedFramebufferSubData, (GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height));
INSTANTIATE_GLCALL(GLboolean, glIsFramebuffer, (GLuint framebuffer));
INSTANTIATE_GLCALL(GLboolean, glIsRenderbuffer, (GLuint renderbuffer));
INSTANTIATE_GLCALL(void, glRenderbufferStorage, (GLenum target, GLenum internalformat, GLsizei width, GLsizei height));
INSTANTIATE_GLCALL(void, glNamedRenderbufferStorage, (GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height));
INSTANTIATE_GLCALL(void, glRenderbufferStorageMultisample, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height));
INSTANTIATE_GLCALL(void, glNamedRenderbufferStorageMultisample, (GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height));
INSTANTIATE_GLCALL(void, glSampleMaski, (GLuint maskNumber, GLbitfield mask));

INSTANTIATE_GLCALL(void, glAttachShader, (GLuint program, GLuint shader));
INSTANTIATE_GLCALL(void, glBindAttribLocation, (GLuint program, GLuint index, const GLchar* name));
INSTANTIATE_GLCALL(void, glBindFragDataLocation, (GLuint program, GLuint colorNumber, const char* name));
INSTANTIATE_GLCALL(void, glBindFragDataLocationIndexed, (GLuint program, GLuint colorNumber, GLuint index, const char* name));
INSTANTIATE_GLCALL(void, glCompileShader, (GLuint shader));
INSTANTIATE_GLCALL(GLuint, glCreateProgram, ());
INSTANTIATE_GLCALL(GLuint, glCreateShader, (GLenum shaderType));
INSTANTIATE_GLCALL(GLuint, glCreateShaderProgramv, (GLenum type, GLsizei count, const char** strings));
INSTANTIATE_GLCALL(void, glDeleteProgram, (GLuint program));
INSTANTIATE_GLCALL(void, glDeleteShader, (GLuint shader));
INSTANTIATE_GLCALL(void, glDetachShader, (GLuint program, GLuint shader));
INSTANTIATE_GLCALL(void, glGetActiveAtomicCounterBufferiv, (GLuint program, GLuint bufferIndex, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetActiveAttrib, (GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name));
INSTANTIATE_GLCALL(void, glGetActiveSubroutineName, (GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei* length, GLchar* name));
INSTANTIATE_GLCALL(void, glGetActiveSubroutineUniformiv, (GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint* values));
INSTANTIATE_GLCALL(void, glGetActiveSubroutineUniformName, (GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei* length, GLchar* name));
INSTANTIATE_GLCALL(void, glGetActiveUniform, (GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name));
INSTANTIATE_GLCALL(void, glGetActiveUniformBlockiv, (GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetActiveUniformBlockName, (GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName));
INSTANTIATE_GLCALL(void, glGetActiveUniformName, (GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformName));
INSTANTIATE_GLCALL(void, glGetActiveUniformsiv, (GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetAttachedShaders, (GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders));
INSTANTIATE_GLCALL(GLint, glGetAttribLocation, (GLuint program, const GLchar* name));
INSTANTIATE_GLCALL(GLint, glGetFragDataIndex, (GLuint program, const char* name));
INSTANTIATE_GLCALL(GLint, glGetFragDataLocation, (GLuint program, const char* name));
INSTANTIATE_GLCALL(void, glGetProgramiv, (GLuint program, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetProgramBinary, (GLuint program, GLsizei bufsize, GLsizei* length, GLenum* binaryFormat, void* binary));
INSTANTIATE_GLCALL(void, glGetProgramInfoLog, (GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog));
INSTANTIATE_GLCALL(void, glGetProgramResourceiv, (GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum* props, GLsizei bufSize, GLsizei* length, GLint* params));
INSTANTIATE_GLCALL(GLuint, glGetProgramResourceIndex, (GLuint program, GLenum programInterface, const char* name));
INSTANTIATE_GLCALL(GLint, glGetProgramResourceLocation, (GLuint program, GLenum programInterface, const char* name));
INSTANTIATE_GLCALL(GLint, glGetProgramResourceLocationIndex, (GLuint program, GLenum programInterface, const char* name));
INSTANTIATE_GLCALL(void, glGetProgramResourceName, (GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei* length, char* name));
INSTANTIATE_GLCALL(void, glGetProgramStageiv, (GLuint program, GLenum shadertype, GLenum pname, GLint* values));
INSTANTIATE_GLCALL(void, glGetShaderiv, (GLuint shader, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetShaderInfoLog, (GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog));
INSTANTIATE_GLCALL(void, glGetShaderPrecisionFormat, (GLenum shaderType, GLenum precisionType, GLint* range, GLint* precision));
INSTANTIATE_GLCALL(void, glGetShaderSource, (GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source));
INSTANTIATE_GLCALL(GLuint, glGetSubroutineIndex, (GLuint program, GLenum shadertype, const GLchar* name));
INSTANTIATE_GLCALL(GLint, glGetSubroutineUniformLocation, (GLuint program, GLenum shadertype, const GLchar* name));
INSTANTIATE_GLCALL(void, glGetUniformfv, (GLuint program, GLint location, GLfloat* params));
INSTANTIATE_GLCALL(void, glGetUniformiv, (GLuint program, GLint location, GLint* params));
INSTANTIATE_GLCALL(void, glGetUniformuiv, (GLuint program, GLint location, GLuint* params));
INSTANTIATE_GLCALL(void, glGetUniformdv, (GLuint program, GLint location, GLdouble* params));
INSTANTIATE_GLCALL(void, glGetnUniformfv, (GLuint program, GLint location, GLsizei bufSize, GLfloat* params));
INSTANTIATE_GLCALL(void, glGetnUniformiv, (GLuint program, GLint location, GLsizei bufSize, GLint* params));
INSTANTIATE_GLCALL(void, glGetnUniformuiv, (GLuint program, GLint location, GLsizei bufSize, GLuint* params));
INSTANTIATE_GLCALL(void, glGetnUniformdv, (GLuint program, GLint location, GLsizei bufSize, GLdouble *params));
INSTANTIATE_GLCALL(GLuint, glGetUniformBlockIndex, (GLuint program, const GLchar* uniformBlockName));
INSTANTIATE_GLCALL(void, glGetUniformIndices, (GLuint program, GLsizei uniformCount, const GLchar** uniformNames, GLuint* uniformIndices));
INSTANTIATE_GLCALL(GLint, glGetUniformLocation, (GLuint program, const GLchar* name));
INSTANTIATE_GLCALL(void, glGetUniformSubroutineuiv, (GLenum shadertype, GLint location, GLuint* values));
INSTANTIATE_GLCALL(GLboolean, glIsProgram, (GLuint program));
INSTANTIATE_GLCALL(GLboolean, glIsShader, (GLuint shader));
INSTANTIATE_GLCALL(void, glLinkProgram, (GLuint program));
INSTANTIATE_GLCALL(void, glMinSampleShading, (GLfloat value));
INSTANTIATE_GLCALL(void, glProgramBinary, (GLuint program, GLenum binaryFormat, const void* binary, GLsizei length));
INSTANTIATE_GLCALL(void, glProgramParameteri, (GLuint program, GLenum pname, GLint value));
INSTANTIATE_GLCALL(void, glProgramUniform1f, (GLuint program, GLint location, GLfloat v0));
INSTANTIATE_GLCALL(void, glProgramUniform2f, (GLuint program, GLint location, GLfloat v0, GLfloat v1));
INSTANTIATE_GLCALL(void, glProgramUniform3f, (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2));
INSTANTIATE_GLCALL(void, glProgramUniform4f, (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3));
INSTANTIATE_GLCALL(void, glProgramUniform1i, (GLuint program, GLint location, GLint v0));
INSTANTIATE_GLCALL(void, glProgramUniform2i, (GLuint program, GLint location, GLint v0, GLint v1));
INSTANTIATE_GLCALL(void, glProgramUniform3i, (GLuint program, GLint location, GLint v0, GLint v1, GLint v2));
INSTANTIATE_GLCALL(void, glProgramUniform4i, (GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3));
INSTANTIATE_GLCALL(void, glProgramUniform1ui, (GLuint program, GLint location, GLuint v0));
INSTANTIATE_GLCALL(void, glProgramUniform2ui, (GLuint program, GLint location, GLint v0, GLuint v1));
INSTANTIATE_GLCALL(void, glProgramUniform3ui, (GLuint program, GLint location, GLint v0, GLint v1, GLuint v2));
INSTANTIATE_GLCALL(void, glProgramUniform4ui, (GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLuint v3));
INSTANTIATE_GLCALL(void, glProgramUniform1fv, (GLuint program, GLint location, GLsizei count, const GLfloat* value));
INSTANTIATE_GLCALL(void, glProgramUniform2fv, (GLuint program, GLint location, GLsizei count, const GLfloat* value));
INSTANTIATE_GLCALL(void, glProgramUniform3fv, (GLuint program, GLint location, GLsizei count, const GLfloat* value));
INSTANTIATE_GLCALL(void, glProgramUniform4fv, (GLuint program, GLint location, GLsizei count, const GLfloat* value));
INSTANTIATE_GLCALL(void, glProgramUniform1iv, (GLuint program, GLint location, GLsizei count, const GLint* value));
INSTANTIATE_GLCALL(void, glProgramUniform2iv, (GLuint program, GLint location, GLsizei count, const GLint* value));
INSTANTIATE_GLCALL(void, glProgramUniform3iv, (GLuint program, GLint location, GLsizei count, const GLint* value));
INSTANTIATE_GLCALL(void, glProgramUniform4iv, (GLuint program, GLint location, GLsizei count, const GLint* value));
INSTANTIATE_GLCALL(void, glProgramUniform1uiv, (GLuint program, GLint location, GLsizei count, const GLuint* value));
INSTANTIATE_GLCALL(void, glProgramUniform2uiv, (GLuint program, GLint location, GLsizei count, const GLuint* value));
INSTANTIATE_GLCALL(void, glProgramUniform3uiv, (GLuint program, GLint location, GLsizei count, const GLuint* value));
INSTANTIATE_GLCALL(void, glProgramUniform4uiv, (GLuint program, GLint location, GLsizei count, const GLuint* value));
INSTANTIATE_GLCALL(void, glProgramUniformMatrix2fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glProgramUniformMatrix3fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glProgramUniformMatrix4fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glProgramUniformMatrix2x3fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glProgramUniformMatrix3x2fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glProgramUniformMatrix2x4fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glProgramUniformMatrix4x2fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glProgramUniformMatrix3x4fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glProgramUniformMatrix4x3fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glReleaseShaderCompiler, ());
INSTANTIATE_GLCALL(void, glShaderBinary, (GLsizei count, const GLuint* shaders, GLenum binaryFormat, const void* binary, GLsizei length));
INSTANTIATE_GLCALL(void, glShaderSource, (GLuint shader, GLsizei count, const GLchar** string_in, const GLint* length));
INSTANTIATE_GLCALL(void, glShaderStorageBlockBinding, (GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding));
INSTANTIATE_GLCALL(void, glUniform1f, (GLint location, GLfloat v0));
INSTANTIATE_GLCALL(void, glUniform2f, (GLint location, GLfloat v0, GLfloat v1));
INSTANTIATE_GLCALL(void, glUniform3f, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2));
INSTANTIATE_GLCALL(void, glUniform4f, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3));
INSTANTIATE_GLCALL(void, glUniform1i, (GLint location, GLint v0));
INSTANTIATE_GLCALL(void, glUniform2i, (GLint location, GLint v0, GLint v1));
INSTANTIATE_GLCALL(void, glUniform3i, (GLint location, GLint v0, GLint v1, GLint v2));
INSTANTIATE_GLCALL(void, glUniform4i, (GLint location, GLint v0, GLint v1, GLint v2, GLint v3));
INSTANTIATE_GLCALL(void, glUniform1ui, (GLint location, GLuint v0));
INSTANTIATE_GLCALL(void, glUniform2ui, (GLint location, GLuint v0, GLuint v1));
INSTANTIATE_GLCALL(void, glUniform3ui, (GLint location, GLuint v0, GLuint v1, GLuint v2));
INSTANTIATE_GLCALL(void, glUniform4ui, (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3));
INSTANTIATE_GLCALL(void, glUniform1fv, (GLint location, GLsizei count, const GLfloat* value));
INSTANTIATE_GLCALL(void, glUniform2fv, (GLint location, GLsizei count, const GLfloat* value));
INSTANTIATE_GLCALL(void, glUniform3fv, (GLint location, GLsizei count, const GLfloat* value));
INSTANTIATE_GLCALL(void, glUniform4fv, (GLint location, GLsizei count, const GLfloat* value));
INSTANTIATE_GLCALL(void, glUniform1iv, (GLint location, GLsizei count, const GLint* value));
INSTANTIATE_GLCALL(void, glUniform2iv, (GLint location, GLsizei count, const GLint* value));
INSTANTIATE_GLCALL(void, glUniform3iv, (GLint location, GLsizei count, const GLint* value));
INSTANTIATE_GLCALL(void, glUniform4iv, (GLint location, GLsizei count, const GLint* value));
INSTANTIATE_GLCALL(void, glUniform1uiv, (GLint location, GLsizei count, const GLuint* value));
INSTANTIATE_GLCALL(void, glUniform2uiv, (GLint location, GLsizei count, const GLuint* value));
INSTANTIATE_GLCALL(void, glUniform3uiv, (GLint location, GLsizei count, const GLuint* value));
INSTANTIATE_GLCALL(void, glUniform4uiv, (GLint location, GLsizei count, const GLuint* value));
INSTANTIATE_GLCALL(void, glUniformMatrix2fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glUniformMatrix3fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glUniformMatrix4fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glUniformMatrix2x3fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glUniformMatrix3x2fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glUniformMatrix2x4fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glUniformMatrix4x2fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glUniformMatrix3x4fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glUniformMatrix4x3fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void, glUniformBlockBinding, (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding));
INSTANTIATE_GLCALL(void, glUniformSubroutinesuiv, (GLenum shadertype, GLsizei count, const GLuint* indices));
INSTANTIATE_GLCALL(void, glUseProgram, (GLuint program));
INSTANTIATE_GLCALL(void, glUseProgramStages, (GLuint pipeline, GLbitfield stages, GLuint program));
INSTANTIATE_GLCALL(void, glValidateProgram, (GLuint program));

INSTANTIATE_GLCALL(void, glBlendColor, (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha));
INSTANTIATE_GLCALL(void, glBlendEquation, (GLenum mode));
INSTANTIATE_GLCALL(void, glBlendEquationi, (GLuint buf, GLenum mode));
INSTANTIATE_GLCALL(void, glBlendEquationSeparate, (GLenum modeRGB, GLenum modeAlpha));
INSTANTIATE_GLCALL(void, glBlendEquationSeparatei, (GLuint buf, GLenum modeRGB, GLenum modeAlpha));
INSTANTIATE_GLCALL(void, glBlendFunc, (GLenum sfactor, GLenum dfactor));
INSTANTIATE_GLCALL(void, glBlendFunci, (GLuint buf, GLenum sfactor, GLenum dfactor));
INSTANTIATE_GLCALL(void, glBlendFuncSeparate, (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha));
INSTANTIATE_GLCALL(void, glBlendFuncSeparatei, (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha));
INSTANTIATE_GLCALL(void, glClampColor, (GLenum target, GLenum clamp));
INSTANTIATE_GLCALL(void, glClipControl, (GLenum origin, GLenum depth));
INSTANTIATE_GLCALL(void, glColorMask, (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha));
INSTANTIATE_GLCALL(void, glColorMaski, (GLuint buf, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha));
INSTANTIATE_GLCALL(void, glCullFace, (GLenum mode));
INSTANTIATE_GLCALL(void, glDepthFunc, (GLenum func));
INSTANTIATE_GLCALL(void, glDepthMask, (GLboolean flag));
INSTANTIATE_GLCALL(void, glDepthRange, (GLdouble nearVal, GLdouble farVal));
INSTANTIATE_GLCALL(void, glDepthRangef, (GLfloat nearVal, GLfloat farVal));
INSTANTIATE_GLCALL(void, glDepthRangeArrayv, (GLuint first, GLsizei count, const GLdouble* v));
INSTANTIATE_GLCALL(void, glDepthRangeIndexed, (GLuint index, GLdouble nearVal, GLdouble farVa));
INSTANTIATE_GLCALL(void, glDisable, (GLenum cap));
INSTANTIATE_GLCALL(void, glEnablei, (GLenum cap, GLuint index));
INSTANTIATE_GLCALL(void, glDisablei, (GLenum cap, GLuint index));
INSTANTIATE_GLCALL(void, glEnable, (GLenum cap));
INSTANTIATE_GLCALL(void, glFrontFace, (GLenum mode));
INSTANTIATE_GLCALL(void, glGetBooleanv, (GLenum pname, GLboolean* data));
INSTANTIATE_GLCALL(void, glGetDoublev, (GLenum pname, GLdouble* data));
INSTANTIATE_GLCALL(void, glGetFloatv, (GLenum pname, GLfloat* data));
INSTANTIATE_GLCALL(void, glGetIntegerv, (GLenum pname, GLint* data));
INSTANTIATE_GLCALL(void, glGetInteger64v, (GLenum pname, GLint64* data));
INSTANTIATE_GLCALL(void, glGetBooleani_v, (GLenum target, GLuint index, GLboolean* data));
INSTANTIATE_GLCALL(void, glGetFloati_v, (GLenum target, GLuint index, GLfloat* data));
INSTANTIATE_GLCALL(void, glGetDoublei_v, (GLenum target, GLuint index, GLdouble* data));
INSTANTIATE_GLCALL(void, glGetInteger64i_v, (GLenum target, GLuint index, GLint64* data));
INSTANTIATE_GLCALL(GLenum, glGetError, ());
INSTANTIATE_GLCALL(void, glHint, (GLenum target, GLenum mode));
INSTANTIATE_GLCALL(GLboolean, glIsEnabled, (GLenum cap));
INSTANTIATE_GLCALL(GLboolean, glIsEnabledi, (GLenum cap, GLuint index));
INSTANTIATE_GLCALL(void, glLineWidth, (GLfloat width));
INSTANTIATE_GLCALL(void, glLogicOp, (GLenum opcode));
INSTANTIATE_GLCALL(void, glPixelStoref, (GLenum pname, GLfloat param));
INSTANTIATE_GLCALL(void, glPixelStorei, (GLenum pname, GLint param));
INSTANTIATE_GLCALL(void, glPointParameterf, (GLenum pname, GLfloat param));
INSTANTIATE_GLCALL(void, glPointParameteri, (GLenum pname, GLint param));
INSTANTIATE_GLCALL(void, glPointParameterfv, (GLenum pname, const GLfloat* params));
INSTANTIATE_GLCALL(void, glPointParameteriv, (GLenum pname, const GLint* params));
INSTANTIATE_GLCALL(void, glPointSize, (GLfloat size));
INSTANTIATE_GLCALL(void, glPolygonMode, (GLenum face, GLenum mode));
INSTANTIATE_GLCALL(void, glPolygonOffset, (GLfloat factor, GLfloat units));
INSTANTIATE_GLCALL(void, glScissor, (GLint x, GLint y, GLsizei width, GLsizei height));
INSTANTIATE_GLCALL(void, glSampleCoverage, (GLfloat value, GLboolean invert));
INSTANTIATE_GLCALL(void, glScissorArrayv, (GLuint first, GLsizei count, const GLint* v));
INSTANTIATE_GLCALL(void, glScissorIndexed, (GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height));
INSTANTIATE_GLCALL(void, glScissorIndexedv, (GLuint index, const GLint* v));
INSTANTIATE_GLCALL(void, glStencilFunc, (GLenum func, GLint ref, GLuint mask));
INSTANTIATE_GLCALL(void, glStencilFuncSeparate, (GLenum face, GLenum func, GLint ref, GLuint mask));
INSTANTIATE_GLCALL(void, glStencilMask, (GLuint mask));
INSTANTIATE_GLCALL(void, glStencilMaskSeparate, (GLenum face, GLuint mask));
INSTANTIATE_GLCALL(void, glStencilOp, (GLenum sfail, GLenum dpfail, GLenum dppass));
INSTANTIATE_GLCALL(void, glStencilOpSeparate, (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass));
INSTANTIATE_GLCALL(void, glViewport, (GLint x, GLint y, GLsizei width, GLsizei height));
INSTANTIATE_GLCALL(void, glViewportArrayv, (GLuint first, GLsizei count, const GLfloat* v));
INSTANTIATE_GLCALL(void, glViewportIndexedf, (GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h));
INSTANTIATE_GLCALL(void, glViewportIndexedfv, (GLuint index, const GLfloat* v));

INSTANTIATE_GLCALL(void, glBeginTransformFeedback, (GLenum primitiveMode));
INSTANTIATE_GLCALL(void, glBindTransformFeedback, (GLenum target, GLuint id));
INSTANTIATE_GLCALL(void, glCreateTransformFeedbacks, (GLsizei n, GLuint* ids));
INSTANTIATE_GLCALL(void, glDeleteTransformFeedbacks, (GLsizei n, const GLuint* ids));
INSTANTIATE_GLCALL(void, glDrawTransformFeedback, (GLenum mode, GLuint id));
INSTANTIATE_GLCALL(void, glDrawTransformFeedbackInstanced, (GLenum mode, GLuint id, GLsizei primcount));
INSTANTIATE_GLCALL(void, glDrawTransformFeedbackStream, (GLenum mode, GLuint id, GLuint stream));
INSTANTIATE_GLCALL(void, glDrawTransformFeedbackStreamInstanced, (GLenum mode, GLuint id, GLuint stream, GLsizei primcount));
INSTANTIATE_GLCALL(void, glEndTransformFeedback, ());
INSTANTIATE_GLCALL(void, glGenTransformFeedbacks, (GLsizei n, GLuint* ids));
INSTANTIATE_GLCALL(void, glGetTransformFeedbackiv, (GLuint xfb, GLenum pname, GLint* param));
INSTANTIATE_GLCALL(void, glGetTransformFeedbacki_v, (GLuint xfb, GLenum pname, GLuint index, GLint* param));
INSTANTIATE_GLCALL(void, glGetTransformFeedbacki64_v, (GLuint xfb, GLenum pname, GLuint index, GLint64* param));
INSTANTIATE_GLCALL(void, glGetTransformFeedbackVarying, (GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLsizei* size, GLenum* type, char* name));
INSTANTIATE_GLCALL(GLboolean, glIsTransformFeedback, (GLuint id));
INSTANTIATE_GLCALL(void, glPauseTransformFeedback, ());
INSTANTIATE_GLCALL(void, glResumeTransformFeedback, ());
INSTANTIATE_GLCALL(void, glTransformFeedbackBufferBase, (GLuint xfb, GLuint index, GLuint buffer));
INSTANTIATE_GLCALL(void, glTransformFeedbackBufferRange, (GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizei size));
INSTANTIATE_GLCALL(void, glTransformFeedbackVaryings, (GLuint program, GLsizei count, const char** varyings, GLenum bufferMode));

INSTANTIATE_GLCALL(void, glDispatchCompute, (GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z));
INSTANTIATE_GLCALL(void, glDispatchComputeIndirect, (GLintptr indirect));
INSTANTIATE_GLCALL(GLenum, glGetGraphicsResetStatus, ());
INSTANTIATE_GLCALL(void, glGetInternalformativ, (GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params));
INSTANTIATE_GLCALL(void, glGetInternalformati64v, (GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params));
INSTANTIATE_GLCALL(void, glGetMultisamplefv, (GLenum pname, GLuint index, GLfloat* val));
INSTANTIATE_GLCALL(const GLubyte*, glGetString, (GLenum name));
INSTANTIATE_GLCALL(const GLubyte*, glGetStringi, (GLenum name, GLuint index));
INSTANTIATE_GLCALL(void, glMemoryBarrier, (GLbitfield barriers));
INSTANTIATE_GLCALL(void, glMemoryBarrierByRegion, (GLbitfield barriers));

INSTANTIATE_GLCALL(void, glBeginConditionalRender, (GLuint id, GLenum mode));
INSTANTIATE_GLCALL(void, glBeginQuery, (GLenum target, GLuint id));
INSTANTIATE_GLCALL(void, glBeginQueryIndexed, (GLenum target, GLuint index, GLuint id));
INSTANTIATE_GLCALL(void, glCreateQueries, (GLenum target, GLsizei n, GLuint* ids));
INSTANTIATE_GLCALL(void, glDeleteQueries, (GLsizei n, const GLuint* ids));
INSTANTIATE_GLCALL(void, glEndConditionalRender, ());
INSTANTIATE_GLCALL(void, glEndQuery, (GLenum target));
INSTANTIATE_GLCALL(void, glEndQueryIndexed, (GLenum target, GLuint index));
INSTANTIATE_GLCALL(void, glGenQueries, (GLsizei n, GLuint* ids));
INSTANTIATE_GLCALL(void, glGetQueryIndexediv, (GLenum target, GLuint index, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetQueryObjectiv, (GLuint id, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetQueryObjectuiv, (GLuint id, GLenum pname, GLuint* params));
INSTANTIATE_GLCALL(void, glGetQueryObjecti64v, (GLuint id, GLenum pname, GLint64* params));
INSTANTIATE_GLCALL(void, glGetQueryObjectui64v, (GLuint id, GLenum pname, GLuint64* params));
INSTANTIATE_GLCALL(void, glGetQueryiv, (GLenum target, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(GLboolean, glIsQuery, (GLuint id));
INSTANTIATE_GLCALL(void, glQueryCounter, (GLuint id, GLenum target));

INSTANTIATE_GLCALL(GLenum, glClientWaitSync, (GLsync sync, GLbitfield flags, GLuint64 timeout));
INSTANTIATE_GLCALL(void, glDeleteSync, (GLsync sync));
INSTANTIATE_GLCALL(GLsync, glFenceSync, (GLenum condition, GLbitfield flags));
INSTANTIATE_GLCALL(void, glGetSynciv, (GLsync sync, GLenum pname, GLsizei bufSize, GLsizei* length, GLint* values));
INSTANTIATE_GLCALL(GLboolean, glIsSync, (GLsync sync));
INSTANTIATE_GLCALL(void, glTextureBarrier, ());
INSTANTIATE_GLCALL(void, glWaitSync, (GLsync sync, GLbitfield flags, GLuint64 timeout));

INSTANTIATE_GLCALL(void, glBindVertexArray, (GLuint array));
INSTANTIATE_GLCALL(void, glDeleteVertexArrays, (GLsizei n, const GLuint* arrays));
INSTANTIATE_GLCALL(void, glGenVertexArrays, (GLsizei n, GLuint* arrays));
INSTANTIATE_GLCALL(GLboolean, glIsVertexArray, (GLuint array));

INSTANTIATE_GLCALL(void, glBindSampler, (GLuint unit, GLuint sampler));
INSTANTIATE_GLCALL(void, glBindSamplers, (GLuint first, GLsizei count, const GLuint* samplers));
INSTANTIATE_GLCALL(void, glCreateSamplers, (GLsizei n, GLuint* samplers));
INSTANTIATE_GLCALL(void, glDeleteSamplers, (GLsizei n, const GLuint* samplers));
INSTANTIATE_GLCALL(void, glGenSamplers, (GLsizei n, GLuint* samplers));
INSTANTIATE_GLCALL(void, glGetSamplerParameterfv, (GLuint sampler, GLenum pname, GLfloat* params));
INSTANTIATE_GLCALL(void, glGetSamplerParameteriv, (GLuint sampler, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetSamplerParameterIiv, (GLuint sampler, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetSamplerParameterIuiv, (GLuint sampler, GLenum pname, GLuint* params));
INSTANTIATE_GLCALL(GLboolean, glIsSampler, (GLuint id));
INSTANTIATE_GLCALL(void, glSamplerParameterf, (GLuint sampler, GLenum pname, GLfloat param));
INSTANTIATE_GLCALL(void, glSamplerParameteri, (GLuint sampler, GLenum pname, GLint param));
INSTANTIATE_GLCALL(void, glSamplerParameterfv, (GLuint sampler, GLenum pname, const GLfloat* params));
INSTANTIATE_GLCALL(void, glSamplerParameteriv, (GLuint sampler, GLenum pname, const GLint* params));
INSTANTIATE_GLCALL(void, glSamplerParameterIiv, (GLuint sampler, GLenum pname, const GLint* params));
INSTANTIATE_GLCALL(void, glSamplerParameterIuiv, (GLuint sampler, GLenum pname, const GLuint* params));

INSTANTIATE_GLCALL(void, glActiveShaderProgram, (GLuint pipeline, GLuint program));
INSTANTIATE_GLCALL(void, glBindProgramPipeline, (GLuint pipeline));
INSTANTIATE_GLCALL(void, glCreateProgramPipelines, (GLsizei n, GLuint* pipelines));
INSTANTIATE_GLCALL(void, glDeleteProgramPipelines, (GLsizei n, const GLuint* pipelines));
INSTANTIATE_GLCALL(void, glGenProgramPipelines, (GLsizei n, GLuint* pipelines));
INSTANTIATE_GLCALL(void, glGetProgramPipelineiv, (GLuint pipeline, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetProgramPipelineInfoLog, (GLuint pipeline, GLsizei bufSize, GLsizei* length, GLchar* infoLog));
INSTANTIATE_GLCALL(GLboolean, glIsProgramPipeline, (GLuint pipeline));
INSTANTIATE_GLCALL(void, glValidateProgramPipeline, (GLuint pipeline));

INSTANTIATE_GLCALL(void, glDebugMessageCallback, (GLDEBUGPROC callback, void* userParam));
INSTANTIATE_GLCALL(void, glDebugMessageControl, (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled));
INSTANTIATE_GLCALL(void, glDebugMessageInsert, (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message));
INSTANTIATE_GLCALL(GLuint, glGetDebugMessageLog, (GLuint count, GLsizei bufSize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths, GLchar* messageLog));
INSTANTIATE_GLCALL(void, glGetObjectLabel, (GLenum identifier, GLuint name, GLsizei bifSize, GLsizei* length, char* label));
INSTANTIATE_GLCALL(void, glGetObjectPtrLabel, (void* ptr, GLsizei bifSize, GLsizei* length, char* label));
INSTANTIATE_GLCALL(void, glGetPointerv, (GLenum pname, GLvoid** params));
INSTANTIATE_GLCALL(void, glGetProgramInterfaceiv, (GLuint program, GLenum programInterface, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glObjectLabel, (GLenum identifier, GLuint name, GLsizei length, const char* label));
INSTANTIATE_GLCALL(void, glObjectPtrLabel, (void* ptr, GLsizei length, const char* label));
INSTANTIATE_GLCALL(void, glPopDebugGroup, ());
INSTANTIATE_GLCALL(void, glPushDebugGroup, (GLenum source, GLuint id, GLsizei length, const char* message));

INSTANTIATE_GLCALL(void, glBindBuffer, (GLenum target, GLuint buffer));
INSTANTIATE_GLCALL(void, glBindBufferBase, (GLenum target, GLuint index, GLuint buffer));
INSTANTIATE_GLCALL(void, glBindBufferRange, (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size));
INSTANTIATE_GLCALL(void, glBindBuffersBase, (GLenum target, GLuint first, GLsizei count, const GLuint* buffers));
INSTANTIATE_GLCALL(void, glBindBuffersRange, (GLenum target, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLintptr* sizes));
INSTANTIATE_GLCALL(void, glBindVertexBuffer, (GLuint bindingindex, GLuint buffer, GLintptr offset, GLintptr stride));
INSTANTIATE_GLCALL(void, glVertexArrayVertexBuffer, (GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride));
INSTANTIATE_GLCALL(void, glBindVertexBuffers, (GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides));
INSTANTIATE_GLCALL(void, glVertexArrayVertexBuffers, (GLuint vaobj, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides));
INSTANTIATE_GLCALL(void, glBufferData, (GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage));
INSTANTIATE_GLCALL(void, glNamedBufferData, (GLuint buffer, GLsizei size, const void* data, GLenum usage));
INSTANTIATE_GLCALL(void, glBufferStorage, (GLenum target, GLsizeiptr size, const GLvoid* data, GLbitfield flags));
INSTANTIATE_GLCALL(void, glNamedBufferStorage, (GLuint buffer, GLsizei size, const void* data, GLbitfield flags));
INSTANTIATE_GLCALL(void, glBufferSubData, (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data));
INSTANTIATE_GLCALL(void, glNamedBufferSubData, (GLuint buffer, GLintptr offset, GLsizei size, const void* data));
INSTANTIATE_GLCALL(void, glClearBufferData, (GLenum target, GLenum internalformat, GLenum format, GLenum type, const void* data));
INSTANTIATE_GLCALL(void, glClearNamedBufferData, (GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void* data));
INSTANTIATE_GLCALL(void, glClearBufferSubData, (GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data));
INSTANTIATE_GLCALL(void, glClearNamedBufferSubData, (GLuint buffer, GLenum internalformat, GLintptr offset, GLsizei size, GLenum format, GLenum type, const void* data));
INSTANTIATE_GLCALL(void, glCopyBufferSubData, (GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size));
INSTANTIATE_GLCALL(void, glCopyNamedBufferSubData, (GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizei size));
INSTANTIATE_GLCALL(void, glCreateBuffers, (GLsizei n, GLuint* buffers));
INSTANTIATE_GLCALL(void, glCreateVertexArrays, (GLsizei n, GLuint* arrays));
INSTANTIATE_GLCALL(void, glDeleteBuffers, (GLsizei n, const GLuint* buffers));
INSTANTIATE_GLCALL(void, glDisableVertexAttribArray, (GLuint index));
INSTANTIATE_GLCALL(void, glDisableVertexArrayAttrib, (GLuint vaobj, GLuint index));
INSTANTIATE_GLCALL(void, glDrawArrays, (GLenum mode, GLint first, GLsizei count));
INSTANTIATE_GLCALL(void, glDrawArraysIndirect, (GLenum mode, const void* indirect));
INSTANTIATE_GLCALL(void, glDrawArraysInstanced, (GLenum mode, GLint first, GLsizei count, GLsizei primcount));
INSTANTIATE_GLCALL(void, glDrawArraysInstancedBaseInstance, (GLenum mode, GLint first, GLsizei count, GLsizei primcount, GLuint baseinstance));
INSTANTIATE_GLCALL(void, glDrawElements, (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices));
INSTANTIATE_GLCALL(void, glDrawElementsBaseVertex, (GLenum mode, GLsizei count, GLenum type, GLvoid* indices, GLint basevertex));
INSTANTIATE_GLCALL(void, glDrawElementsIndirect, (GLenum mode, GLenum type, const void* indirect));
INSTANTIATE_GLCALL(void, glDrawElementsInstanced, (GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primcount));
INSTANTIATE_GLCALL(void, glDrawElementsInstancedBaseInstance, (GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primcount, GLuint baseinstance));
INSTANTIATE_GLCALL(void, glDrawElementsInstancedBaseVertex, (GLenum mode, GLsizei count, GLenum type, GLvoid* indices, GLsizei primcount, GLint basevertex));
INSTANTIATE_GLCALL(void, glDrawElementsInstancedBaseVertexBaseInstance, (GLenum mode, GLsizei count, GLenum type, GLvoid* indices, GLsizei primcount, GLint basevertex, GLuint baseinstance));
INSTANTIATE_GLCALL(void, glDrawRangeElements, (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid* indices));
INSTANTIATE_GLCALL(void, glDrawRangeElementsBaseVertex, (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLvoid* indices, GLint basevertex));
INSTANTIATE_GLCALL(void, glEnableVertexAttribArray, (GLuint index));
INSTANTIATE_GLCALL(void, glEnableVertexArrayAttrib, (GLuint vaobj, GLuint index));
INSTANTIATE_GLCALL(void, glFlushMappedBufferRange, (GLenum target, GLintptr offset, GLsizeiptr length));
INSTANTIATE_GLCALL(void, glFlushMappedNamedBufferRange, (GLuint buffer, GLintptr offset, GLsizei length));
INSTANTIATE_GLCALL(void, glGenBuffers, (GLsizei n, GLuint* buffers));
INSTANTIATE_GLCALL(void, glGetBufferParameteriv, (GLenum target, GLenum value, GLint* data));
INSTANTIATE_GLCALL(void, glGetBufferParameteri64v, (GLenum target, GLenum value, GLint64* data));
INSTANTIATE_GLCALL(void, glGetNamedBufferParameteriv, (GLuint buffer, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetNamedBufferParameteri64v, (GLuint buffer, GLenum pname, GLint64* params));
INSTANTIATE_GLCALL(void, glGetBufferPointerv, (GLenum target, GLenum pname, GLvoid** params));
INSTANTIATE_GLCALL(void, glGetNamedBufferPointerv, (GLuint buffer, GLenum pname, void** params));
INSTANTIATE_GLCALL(void, glGetBufferSubData, (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid* data));
INSTANTIATE_GLCALL(void, glGetNamedBufferSubData, (GLuint buffer, GLintptr offset, GLsizei size, void* data));
INSTANTIATE_GLCALL(void, glGetVertexArrayIndexediv, (GLuint vaobj, GLuint index, GLenum pname, GLint* param));
INSTANTIATE_GLCALL(void, glGetVertexArrayIndexed64iv, (GLuint vaobj, GLuint index, GLenum pname, GLint64* param));
INSTANTIATE_GLCALL(void, glGetVertexArrayiv, (GLuint vaobj, GLenum pname, GLint* param));
INSTANTIATE_GLCALL(void, glGetVertexAttribdv, (GLuint index, GLenum pname, GLdouble* params));
INSTANTIATE_GLCALL(void, glGetVertexAttribfv, (GLuint index, GLenum pname, GLfloat* params));
INSTANTIATE_GLCALL(void, glGetVertexAttribiv, (GLuint index, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetVertexAttribIiv, (GLuint index, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetVertexAttribIuiv, (GLuint index, GLenum pname, GLuint* params));
INSTANTIATE_GLCALL(void, glGetVertexAttribLdv, (GLuint index, GLenum pname, GLdouble* params));
INSTANTIATE_GLCALL(void, glGetVertexAttribPointerv, (GLuint index, GLenum pname, GLvoid** pointer));
INSTANTIATE_GLCALL(void, glInvalidateBufferData, (GLuint buffer));
INSTANTIATE_GLCALL(void, glInvalidateBufferSubData, (GLuint buffer, GLintptr offset, GLsizeiptr length));
INSTANTIATE_GLCALL(GLboolean, glIsBuffer, (GLuint buffer));
INSTANTIATE_GLCALL(void*, glMapBuffer, (GLenum target, GLenum access));
INSTANTIATE_GLCALL(void*, glMapNamedBuffer, (GLuint buffer, GLenum access));
INSTANTIATE_GLCALL(void*, glMapBufferRange, (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access));
INSTANTIATE_GLCALL(void*, glMapNamedBufferRange, (GLuint buffer, GLintptr offset, GLsizei length, GLbitfield access));
INSTANTIATE_GLCALL(void, glMultiDrawArrays, (GLenum mode, const GLint* first, const GLsizei* count, GLsizei drawcount));
INSTANTIATE_GLCALL(void, glMultiDrawArraysIndirect, (GLenum mode, const void* indirect, GLsizei drawcount, GLsizei stride));
INSTANTIATE_GLCALL(void, glMultiDrawElements, (GLenum mode, const GLsizei* count, GLenum type, const GLvoid* const * indices, GLsizei drawcount));
INSTANTIATE_GLCALL(void, glMultiDrawElementsBaseVertex, (GLenum mode, const GLsizei* count, GLenum type, const GLvoid* const *indices, GLsizei drawcount, const GLint* basevertex));
INSTANTIATE_GLCALL(void, glMultiDrawElementsIndirect, (GLenum mode, GLenum type, const void* indirect, GLsizei drawcount, GLsizei stride));
INSTANTIATE_GLCALL(void, glPatchParameteri, (GLenum pname, GLint value));
INSTANTIATE_GLCALL(void, glPatchParameterfv, (GLenum pname, const GLfloat* values));
INSTANTIATE_GLCALL(void, glPrimitiveRestartIndex, (GLuint index));
INSTANTIATE_GLCALL(void, glProvokingVertex, (GLenum provokeMode));
INSTANTIATE_GLCALL(GLboolean, glUnmapBuffer, (GLenum target));
INSTANTIATE_GLCALL(GLboolean, glUnmapNamedBuffer, (GLuint buffer));
INSTANTIATE_GLCALL(void, glVertexArrayElementBuffer, (GLuint vaobj, GLuint buffer));
INSTANTIATE_GLCALL(void, glVertexAttrib1f, (GLuint index, GLfloat v0));
INSTANTIATE_GLCALL(void, glVertexAttrib1s, (GLuint index, GLshort v0));
INSTANTIATE_GLCALL(void, glVertexAttrib1d, (GLuint index, GLdouble v0));
INSTANTIATE_GLCALL(void, glVertexAttribI1i, (GLuint index, GLint v0));
INSTANTIATE_GLCALL(void, glVertexAttribI1ui, (GLuint index, GLuint v0));
INSTANTIATE_GLCALL(void, glVertexAttrib2f, (GLuint index, GLfloat v0, GLfloat v1));
INSTANTIATE_GLCALL(void, glVertexAttrib2s, (GLuint index, GLshort v0, GLshort v1));
INSTANTIATE_GLCALL(void, glVertexAttrib2d, (GLuint index, GLdouble v0, GLdouble v1));
INSTANTIATE_GLCALL(void, glVertexAttribI2i, (GLuint index, GLint v0, GLint v1));
INSTANTIATE_GLCALL(void, glVertexAttribI2ui, (GLuint index, GLint v0, GLint v1));
INSTANTIATE_GLCALL(void, glVertexAttrib3f, (GLuint index, GLfloat v0, GLfloat v1, GLfloat v2));
INSTANTIATE_GLCALL(void, glVertexAttrib3s, (GLuint index, GLshort v0, GLshort v1, GLshort v2));
INSTANTIATE_GLCALL(void, glVertexAttrib3d, (GLuint index, GLdouble v0, GLdouble v1, GLdouble v2));
INSTANTIATE_GLCALL(void, glVertexAttribI3i, (GLuint index, GLint v0, GLint v1, GLint v2));
INSTANTIATE_GLCALL(void, glVertexAttribI3ui, (GLuint index, GLuint v0, GLuint v1, GLuint v2));
INSTANTIATE_GLCALL(void, glVertexAttrib4f, (GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3));
INSTANTIATE_GLCALL(void, glVertexAttrib4s, (GLuint index, GLshort v0, GLshort v1, GLshort v2, GLshort v3));
INSTANTIATE_GLCALL(void, glVertexAttrib4d, (GLuint index, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3));
INSTANTIATE_GLCALL(void, glVertexAttrib4Nub, (GLuint index, GLubyte v0, GLubyte v1, GLubyte v2, GLubyte v3));
INSTANTIATE_GLCALL(void, glVertexAttribI4i, (GLuint index, GLint v0, GLint v1, GLint v2, GLint v3));
INSTANTIATE_GLCALL(void, glVertexAttribI4ui, (GLuint index, GLuint v0, GLuint v1, GLuint v2, GLuint v3));
INSTANTIATE_GLCALL(void, glVertexAttribL1d, (GLuint index, GLdouble v0));
INSTANTIATE_GLCALL(void, glVertexAttribL2d, (GLuint index, GLdouble v0, GLdouble v1));
INSTANTIATE_GLCALL(void, glVertexAttribL3d, (GLuint index, GLdouble v0, GLdouble v1, GLdouble v2));
INSTANTIATE_GLCALL(void, glVertexAttribL4d, (GLuint index, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3));
INSTANTIATE_GLCALL(void, glVertexAttrib1fv, (GLuint index, const GLfloat* v));
INSTANTIATE_GLCALL(void, glVertexAttrib1sv, (GLuint index, const GLshort* v));
INSTANTIATE_GLCALL(void, glVertexAttrib1dv, (GLuint index, const GLdouble* v));
INSTANTIATE_GLCALL(void, glVertexAttribI1iv, (GLuint index, const GLint* v));
INSTANTIATE_GLCALL(void, glVertexAttribI1uiv, (GLuint index, const GLuint* v));
INSTANTIATE_GLCALL(void, glVertexAttrib2fv, (GLuint index, const GLfloat* v));
INSTANTIATE_GLCALL(void, glVertexAttrib2sv, (GLuint index, const GLshort* v));
INSTANTIATE_GLCALL(void, glVertexAttrib2dv, (GLuint index, const GLdouble* v));
INSTANTIATE_GLCALL(void, glVertexAttribI2iv, (GLuint index, const GLint* v));
INSTANTIATE_GLCALL(void, glVertexAttribI2uiv, (GLuint index, const GLuint* v));
INSTANTIATE_GLCALL(void, glVertexAttrib3fv, (GLuint index, const GLfloat* v));
INSTANTIATE_GLCALL(void, glVertexAttrib3sv, (GLuint index, const GLshort* v));
INSTANTIATE_GLCALL(void, glVertexAttrib3dv, (GLuint index, const GLdouble* v));
INSTANTIATE_GLCALL(void, glVertexAttribI3iv, (GLuint index, const GLint *v));
INSTANTIATE_GLCALL(void, glVertexAttribI3uiv, (GLuint index, const GLuint* v));
INSTANTIATE_GLCALL(void, glVertexAttrib4fv, (GLuint index, const GLfloat* v));
INSTANTIATE_GLCALL(void, glVertexAttrib4sv, (GLuint index, const GLshort* v));
INSTANTIATE_GLCALL(void, glVertexAttrib4dv, (GLuint index, const GLdouble* v));
INSTANTIATE_GLCALL(void, glVertexAttrib4iv, (GLuint index, const GLint* v));
INSTANTIATE_GLCALL(void, glVertexAttrib4bv, (GLuint index, const GLbyte* v));
INSTANTIATE_GLCALL(void, glVertexAttrib4ubv, (GLuint index, const GLubyte* v));
INSTANTIATE_GLCALL(void, glVertexAttrib4usv, (GLuint index, const GLushort* v));
INSTANTIATE_GLCALL(void, glVertexAttrib4uiv, (GLuint index, const GLuint* v));
INSTANTIATE_GLCALL(void, glVertexAttrib4Nbv, (GLuint index, const GLbyte* v));
INSTANTIATE_GLCALL(void, glVertexAttrib4Nsv, (GLuint index, const GLshort* v));
INSTANTIATE_GLCALL(void, glVertexAttrib4Niv, (GLuint index, const GLint* v));
INSTANTIATE_GLCALL(void, glVertexAttrib4Nubv, (GLuint index, const GLubyte* v));
INSTANTIATE_GLCALL(void, glVertexAttrib4Nusv, (GLuint index, const GLushort* v));
INSTANTIATE_GLCALL(void, glVertexAttrib4Nuiv, (GLuint index, const GLuint* v));
INSTANTIATE_GLCALL(void, glVertexAttribI4bv, (GLuint index, const GLbyte* v));
INSTANTIATE_GLCALL(void, glVertexAttribI4ubv, (GLuint index, const GLubyte* v));
INSTANTIATE_GLCALL(void, glVertexAttribI4sv, (GLuint index, const GLshort* v));
INSTANTIATE_GLCALL(void, glVertexAttribI4usv, (GLuint index, const GLushort* v));
INSTANTIATE_GLCALL(void, glVertexAttribI4iv, (GLuint index, const GLint* v));
INSTANTIATE_GLCALL(void, glVertexAttribI4uiv, (GLuint index, const GLuint* v));
INSTANTIATE_GLCALL(void, glVertexAttribL1dv, (GLuint index, const GLdouble* v));
INSTANTIATE_GLCALL(void, glVertexAttribL2dv, (GLuint index, const GLdouble* v));
INSTANTIATE_GLCALL(void, glVertexAttribL3dv, (GLuint index, const GLdouble* v));
INSTANTIATE_GLCALL(void, glVertexAttribL4dv, (GLuint index, const GLdouble* v));
INSTANTIATE_GLCALL(void, glVertexAttribP1ui, (GLuint index, GLenum type, GLboolean normalized, GLuint value));
INSTANTIATE_GLCALL(void, glVertexAttribP2ui, (GLuint index, GLenum type, GLboolean normalized, GLuint value));
INSTANTIATE_GLCALL(void, glVertexAttribP3ui, (GLuint index, GLenum type, GLboolean normalized, GLuint value));
INSTANTIATE_GLCALL(void, glVertexAttribP4ui, (GLuint index, GLenum type, GLboolean normalized, GLuint value));
INSTANTIATE_GLCALL(void, glVertexAttribBinding, (GLuint attribindex, GLuint bindingindex));
INSTANTIATE_GLCALL(void, glVertexArrayAttribBinding, (GLuint vaobj, GLuint attribindex, GLuint bindingindex));
INSTANTIATE_GLCALL(void, glVertexAttribDivisor, (GLuint index, GLuint divisor));
INSTANTIATE_GLCALL(void, glVertexAttribFormat, (GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset));
INSTANTIATE_GLCALL(void, glVertexAttribIFormat, (GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset));
INSTANTIATE_GLCALL(void, glVertexAttribLFormat, (GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset));
INSTANTIATE_GLCALL(void, glVertexArrayAttribFormat, (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset));
INSTANTIATE_GLCALL(void, glVertexArrayAttribIFormat, (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset));
INSTANTIATE_GLCALL(void, glVertexArrayAttribLFormat, (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset));
INSTANTIATE_GLCALL(void, glVertexAttribPointer, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer));
INSTANTIATE_GLCALL(void, glVertexAttribIPointer, (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer));
INSTANTIATE_GLCALL(void, glVertexAttribLPointer, (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer));
INSTANTIATE_GLCALL(void, glVertexBindingDivisor, (GLuint bindingindex, GLuint divisor));
INSTANTIATE_GLCALL(void, glVertexArrayBindingDivisor, (GLuint vaobj, GLuint bindingindex, GLuint divisor));

extern int hogl_init_gl_extensions() {
	#if defined (_WIN32) || defined(_WIN64)
	HMODULE glcode = LoadLibraryA("opengl32.dll");
	if (!glcode) {
		return -1;
	}
	#elif defined(__linux__)
	
	#endif

	// Rendering
	LOAD_GL_PROC(glClear);					// opengl32.dll
	LOAD_GL_PROC(glClearBufferiv);
	LOAD_GL_PROC(glClearBufferuiv);
	LOAD_GL_PROC(glClearBufferfv);
	LOAD_GL_PROC(glClearBufferfi);
	LOAD_GL_PROC(glClearNamedFramebufferiv);
	LOAD_GL_PROC(glClearNamedFramebufferuiv);
	LOAD_GL_PROC(glClearNamedFramebufferfv);
	LOAD_GL_PROC(glClearNamedFramebufferfi);
	LOAD_GL_PROC(glNamedFramebufferReadBuffer);
	LOAD_GL_PROC(glReadnPixels);
	LOAD_GL_PROC(glClearColor);				// opengl32.dll
	LOAD_GL_PROC(glClearDepth);				// opengl32.dll
	LOAD_GL_PROC(glClearStencil);			// opengl32.dll
	LOAD_GL_PROC(glDrawBuffer);				// opengl32.dll
	LOAD_GL_PROC(glFinish);					// opengl32.dll
	LOAD_GL_PROC(glFlush);					// opengl32.dll
	LOAD_GL_PROC(glReadBuffer);				// opengl32.dll
	LOAD_GL_PROC(glReadPixels);				// opengl32.dll

	// Textures
	LOAD_GL_PROC(glActiveTexture);
	LOAD_GL_PROC(glBindImageTexture);
	LOAD_GL_PROC(glBindImageTextures);
	LOAD_GL_PROC(glBindTexture);			// opengl32.dll
	LOAD_GL_PROC(glBindTextureUnit);
	LOAD_GL_PROC(glBindTextures);
	LOAD_GL_PROC(glClearTexImage);
	LOAD_GL_PROC(glClearTexSubImage);
	LOAD_GL_PROC(glCompressedTexImage1D);
	LOAD_GL_PROC(glCompressedTexImage2D);
	LOAD_GL_PROC(glCompressedTexImage3D);
	LOAD_GL_PROC(glCompressedTexSubImage1D);
	LOAD_GL_PROC(glCompressedTextureSubImage1D);
	LOAD_GL_PROC(glCompressedTexSubImage2D);
	LOAD_GL_PROC(glCompressedTextureSubImage2D);
	LOAD_GL_PROC(glCompressedTexSubImage3D);
	LOAD_GL_PROC(glCompressedTextureSubImage3D);
	LOAD_GL_PROC(glCopyImageSubData);
	LOAD_GL_PROC(glCopyTexImage1D);			// opengl32.dll
	LOAD_GL_PROC(glCopyTexImage2D);			// opengl32.dll
	LOAD_GL_PROC(glCopyTexSubImage1D);		// opengl32.dll
	LOAD_GL_PROC(glCopyTextureSubImage1D);
	LOAD_GL_PROC(glCopyTexSubImage2D);		// opengl32.dll
	LOAD_GL_PROC(glCopyTextureSubImage2D);
	LOAD_GL_PROC(glCopyTexSubImage3D);
	LOAD_GL_PROC(glCopyTextureSubImage3D);
	LOAD_GL_PROC(glCreateTextures);
	LOAD_GL_PROC(glDeleteTextures);			// opengl32.dll
	LOAD_GL_PROC(glGenTextures);			// opengl32.dll
	LOAD_GL_PROC(glGetCompressedTexImage);
	LOAD_GL_PROC(glGetnCompressedTexImage);
	LOAD_GL_PROC(glGetCompressedTextureImage);
	LOAD_GL_PROC(glGetCompressedTextureSubImage);
	LOAD_GL_PROC(glGetTexImage);			// opengl32.dll ???
	LOAD_GL_PROC(glGetnTexImage);
	LOAD_GL_PROC(glGetTextureImage);
	LOAD_GL_PROC(glGetTexLevelParameterfv);	// opengl32.dll
	LOAD_GL_PROC(glGetTexLevelParameteriv);	// opengl32.dll
	LOAD_GL_PROC(glGetTextureLevelParameterfv);
	LOAD_GL_PROC(glGetTextureLevelParameteriv);
	LOAD_GL_PROC(glGetTexParameterfv);		// opengl32.dll
	LOAD_GL_PROC(glGetTexParameteriv);		// opengl32.dll
	LOAD_GL_PROC(glGetTexParameterIiv);
	LOAD_GL_PROC(glGetTexParameterIuiv);
	LOAD_GL_PROC(glGetTextureParameterfv);
	LOAD_GL_PROC(glGetTextureParameteriv);
	LOAD_GL_PROC(glGetTextureParameterIiv);
	LOAD_GL_PROC(glGetTextureParameterIuiv);
	LOAD_GL_PROC(glGetTextureSubImage);
	LOAD_GL_PROC(glInvalidateTexImage);
	LOAD_GL_PROC(glInvalidateTexSubImage);
	LOAD_GL_PROC(glIsTexture);				// opengl32.dll
	LOAD_GL_PROC(glTexBuffer);
	LOAD_GL_PROC(glTextureBuffer);
	LOAD_GL_PROC(glTexBufferRange);
	LOAD_GL_PROC(glTextureBufferRange);
	LOAD_GL_PROC(glTexImage1D);				// opengl32.dll
	LOAD_GL_PROC(glTexImage2D);				// opengl32.dll
	LOAD_GL_PROC(glTexImage2DMultisample);
	LOAD_GL_PROC(glTexImage3D);
	LOAD_GL_PROC(glTexImage3DMultisample);
	LOAD_GL_PROC(glTexParameterf);			// opengl32.dll
	LOAD_GL_PROC(glTexParameteri);			// opengl32.dll
	LOAD_GL_PROC(glTextureParameterf);
	LOAD_GL_PROC(glTextureParameteri);
	LOAD_GL_PROC(glTexParameterfv);			// opengl32.dll
	LOAD_GL_PROC(glTexParameteriv);			// opengl32.dll
	LOAD_GL_PROC(glTexParameterIiv);
	LOAD_GL_PROC(glTexParameterIuiv);
	LOAD_GL_PROC(glTextureParameterfv);
	LOAD_GL_PROC(glTextureParameteriv);
	LOAD_GL_PROC(glTextureParameterIiv);
	LOAD_GL_PROC(glTextureParameterIuiv);
	LOAD_GL_PROC(glTexStorage1D);
	LOAD_GL_PROC(glTextureStorage1D);
	LOAD_GL_PROC(glTexStorage2D);
	LOAD_GL_PROC(glTextureStorage2D);
	LOAD_GL_PROC(glTexStorage2DMultisample);
	LOAD_GL_PROC(glTextureStorage2DMultisample);
	LOAD_GL_PROC(glTexStorage3D);
	LOAD_GL_PROC(glTextureStorage3D);
	LOAD_GL_PROC(glTexStorage3DMultisample);
	LOAD_GL_PROC(glTextureStorage3DMultisample);

	LOAD_GL_PROC(glTexSubImage1D);			// opengl32.dll
	LOAD_GL_PROC(glTextureSubImage1D);
	LOAD_GL_PROC(glTexSubImage2D);			// opengl32.dll
	LOAD_GL_PROC(glTextureSubImage2D);

	LOAD_GL_PROC(glTexSubImage3D);
	LOAD_GL_PROC(glTextureSubImage3D);
	LOAD_GL_PROC(glTextureView);

	// Frame Buffers
	LOAD_GL_PROC(glBindFramebuffer);
	LOAD_GL_PROC(glBindRenderbuffer);
	LOAD_GL_PROC(glBlitFramebuffer);
	LOAD_GL_PROC(glBlitNamedFramebuffer);
	LOAD_GL_PROC(glCheckFramebufferStatus);
	LOAD_GL_PROC(glCheckNamedFramebufferStatus);
	LOAD_GL_PROC(glCreateFramebuffers);
	LOAD_GL_PROC(glCreateRenderbuffers);
	LOAD_GL_PROC(glDeleteFramebuffers);
	LOAD_GL_PROC(glDeleteRenderbuffers);
	LOAD_GL_PROC(glDrawBuffers);
	LOAD_GL_PROC(glNamedFramebufferDrawBuffers);
	LOAD_GL_PROC(glFramebufferParameteri);
	LOAD_GL_PROC(glNamedFramebufferParameteri);
	LOAD_GL_PROC(glFramebufferRenderbuffer);
	LOAD_GL_PROC(glNamedFramebufferRenderbuffer);
	LOAD_GL_PROC(glFramebufferTexture);
	LOAD_GL_PROC(glFramebufferTexture1D);
	LOAD_GL_PROC(glFramebufferTexture2D);
	LOAD_GL_PROC(glFramebufferTexture3D);
	LOAD_GL_PROC(glNamedFramebufferTexture);
	LOAD_GL_PROC(glFramebufferTextureLayer);
	LOAD_GL_PROC(glNamedFramebufferTextureLayer);
	LOAD_GL_PROC(glGenFramebuffers);
	LOAD_GL_PROC(glGenRenderbuffers);
	LOAD_GL_PROC(glGenerateMipmap);
	LOAD_GL_PROC(glGenerateTextureMipmap);
	LOAD_GL_PROC(glGetFramebufferAttachmentParameteriv);
	LOAD_GL_PROC(glGetNamedFramebufferAttachmentParameteriv);
	LOAD_GL_PROC(glGetFramebufferParameteriv);
	LOAD_GL_PROC(glGetNamedFramebufferParameteriv);
	LOAD_GL_PROC(glGetRenderbufferParameteriv);
	LOAD_GL_PROC(glGetNamedRenderbufferParameteriv);
	LOAD_GL_PROC(glInvalidateFramebuffer);
	LOAD_GL_PROC(glInvalidateNamedFramebufferData);
	LOAD_GL_PROC(glInvalidateSubFramebuffer);
	LOAD_GL_PROC(glInvalidateNamedFramebufferSubData);
	LOAD_GL_PROC(glIsFramebuffer);
	LOAD_GL_PROC(glIsRenderbuffer);
	LOAD_GL_PROC(glRenderbufferStorage);
	LOAD_GL_PROC(glNamedRenderbufferStorage);
	LOAD_GL_PROC(glRenderbufferStorageMultisample);
	LOAD_GL_PROC(glNamedRenderbufferStorageMultisample);
	LOAD_GL_PROC(glSampleMaski);

	// Shaders
	LOAD_GL_PROC(glAttachShader);
	LOAD_GL_PROC(glBindAttribLocation);
	LOAD_GL_PROC(glBindFragDataLocation);
	LOAD_GL_PROC(glBindFragDataLocationIndexed);
	LOAD_GL_PROC(glCompileShader);
	LOAD_GL_PROC(glCreateProgram);
	LOAD_GL_PROC(glCreateShader);
	LOAD_GL_PROC(glCreateShaderProgramv);
	LOAD_GL_PROC(glDeleteProgram);
	LOAD_GL_PROC(glDeleteShader);
	LOAD_GL_PROC(glDetachShader);
	LOAD_GL_PROC(glGetActiveAtomicCounterBufferiv);
	LOAD_GL_PROC(glGetActiveAttrib);
	LOAD_GL_PROC(glGetActiveSubroutineName);
	LOAD_GL_PROC(glGetActiveSubroutineUniformiv);
	LOAD_GL_PROC(glGetActiveSubroutineUniformName);
	LOAD_GL_PROC(glGetActiveUniform);
	LOAD_GL_PROC(glGetActiveUniformBlockiv);
	LOAD_GL_PROC(glGetActiveUniformBlockName);
	LOAD_GL_PROC(glGetActiveUniformName);
	LOAD_GL_PROC(glGetActiveUniformsiv);
	LOAD_GL_PROC(glGetAttachedShaders);
	LOAD_GL_PROC(glGetAttribLocation);
	LOAD_GL_PROC(glGetFragDataIndex);
	LOAD_GL_PROC(glGetFragDataLocation);
	LOAD_GL_PROC(glGetProgramiv);
	LOAD_GL_PROC(glGetProgramBinary);
	LOAD_GL_PROC(glGetProgramInfoLog);
	LOAD_GL_PROC(glGetProgramResourceiv);
	LOAD_GL_PROC(glGetProgramResourceIndex);
	LOAD_GL_PROC(glGetProgramResourceLocation);
	LOAD_GL_PROC(glGetProgramResourceLocationIndex);
	LOAD_GL_PROC(glGetProgramResourceName);
	LOAD_GL_PROC(glGetProgramStageiv);
	LOAD_GL_PROC(glGetShaderiv);
	LOAD_GL_PROC(glGetShaderInfoLog);
	LOAD_GL_PROC(glGetShaderPrecisionFormat);
	LOAD_GL_PROC(glGetShaderSource);
	LOAD_GL_PROC(glGetSubroutineIndex);
	LOAD_GL_PROC(glGetSubroutineUniformLocation);
	LOAD_GL_PROC(glGetUniformfv);
	LOAD_GL_PROC(glGetUniformiv);
	LOAD_GL_PROC(glGetUniformuiv);
	LOAD_GL_PROC(glGetUniformdv);
	LOAD_GL_PROC(glGetnUniformfv);
	LOAD_GL_PROC(glGetnUniformiv);
	LOAD_GL_PROC(glGetnUniformuiv);
	LOAD_GL_PROC(glGetnUniformdv);
	LOAD_GL_PROC(glGetUniformBlockIndex);
	LOAD_GL_PROC(glGetUniformIndices);
	LOAD_GL_PROC(glGetUniformLocation);
	LOAD_GL_PROC(glGetUniformSubroutineuiv);
	LOAD_GL_PROC(glIsProgram);
	LOAD_GL_PROC(glIsShader);
	LOAD_GL_PROC(glLinkProgram);
	LOAD_GL_PROC(glMinSampleShading);
	LOAD_GL_PROC(glProgramBinary);
	LOAD_GL_PROC(glProgramParameteri);
	LOAD_GL_PROC(glProgramUniform1f);
	LOAD_GL_PROC(glProgramUniform2f);
	LOAD_GL_PROC(glProgramUniform3f);
	LOAD_GL_PROC(glProgramUniform4f);
	LOAD_GL_PROC(glProgramUniform1i);
	LOAD_GL_PROC(glProgramUniform2i);
	LOAD_GL_PROC(glProgramUniform3i);
	LOAD_GL_PROC(glProgramUniform4i);
	LOAD_GL_PROC(glProgramUniform1ui);
	LOAD_GL_PROC(glProgramUniform2ui);
	LOAD_GL_PROC(glProgramUniform3ui);
	LOAD_GL_PROC(glProgramUniform4ui);
	LOAD_GL_PROC(glProgramUniform1fv);
	LOAD_GL_PROC(glProgramUniform2fv);
	LOAD_GL_PROC(glProgramUniform3fv);
	LOAD_GL_PROC(glProgramUniform4fv);
	LOAD_GL_PROC(glProgramUniform1iv);
	LOAD_GL_PROC(glProgramUniform2iv);
	LOAD_GL_PROC(glProgramUniform3iv);
	LOAD_GL_PROC(glProgramUniform4iv);
	LOAD_GL_PROC(glProgramUniform1uiv);
	LOAD_GL_PROC(glProgramUniform2uiv);
	LOAD_GL_PROC(glProgramUniform3uiv);
	LOAD_GL_PROC(glProgramUniform4uiv);
	LOAD_GL_PROC(glProgramUniformMatrix2fv);
	LOAD_GL_PROC(glProgramUniformMatrix3fv);
	LOAD_GL_PROC(glProgramUniformMatrix4fv);
	LOAD_GL_PROC(glProgramUniformMatrix2x3fv);
	LOAD_GL_PROC(glProgramUniformMatrix3x2fv);
	LOAD_GL_PROC(glProgramUniformMatrix2x4fv);
	LOAD_GL_PROC(glProgramUniformMatrix4x2fv);
	LOAD_GL_PROC(glProgramUniformMatrix3x4fv);
	LOAD_GL_PROC(glProgramUniformMatrix4x3fv);
	LOAD_GL_PROC(glReleaseShaderCompiler);
	LOAD_GL_PROC(glShaderBinary);
	LOAD_GL_PROC(glShaderSource);
	LOAD_GL_PROC(glShaderStorageBlockBinding);
	LOAD_GL_PROC(glUniform1f);
	LOAD_GL_PROC(glUniform2f);
	LOAD_GL_PROC(glUniform3f);
	LOAD_GL_PROC(glUniform4f);
	LOAD_GL_PROC(glUniform1i);
	LOAD_GL_PROC(glUniform2i);
	LOAD_GL_PROC(glUniform3i);
	LOAD_GL_PROC(glUniform4i);
	LOAD_GL_PROC(glUniform1ui);
	LOAD_GL_PROC(glUniform2ui);
	LOAD_GL_PROC(glUniform3ui);
	LOAD_GL_PROC(glUniform4ui);
	LOAD_GL_PROC(glUniform1fv);
	LOAD_GL_PROC(glUniform2fv);
	LOAD_GL_PROC(glUniform3fv);
	LOAD_GL_PROC(glUniform4fv);
	LOAD_GL_PROC(glUniform1iv);
	LOAD_GL_PROC(glUniform2iv);
	LOAD_GL_PROC(glUniform3iv);
	LOAD_GL_PROC(glUniform4iv);
	LOAD_GL_PROC(glUniform1uiv);
	LOAD_GL_PROC(glUniform2uiv);
	LOAD_GL_PROC(glUniform3uiv);
	LOAD_GL_PROC(glUniform4uiv);
	LOAD_GL_PROC(glUniformMatrix2fv);
	LOAD_GL_PROC(glUniformMatrix3fv);
	LOAD_GL_PROC(glUniformMatrix4fv);
	LOAD_GL_PROC(glUniformMatrix2x3fv);
	LOAD_GL_PROC(glUniformMatrix3x2fv);
	LOAD_GL_PROC(glUniformMatrix2x4fv);
	LOAD_GL_PROC(glUniformMatrix4x2fv);
	LOAD_GL_PROC(glUniformMatrix3x4fv);
	LOAD_GL_PROC(glUniformMatrix4x3fv);

	LOAD_GL_PROC(glUniformBlockBinding);
	LOAD_GL_PROC(glUniformSubroutinesuiv);
	LOAD_GL_PROC(glUseProgram);
	LOAD_GL_PROC(glUseProgramStages);
	LOAD_GL_PROC(glValidateProgram);

	// State Management
	LOAD_GL_PROC(glBlendColor);
	LOAD_GL_PROC(glBlendEquation);
	LOAD_GL_PROC(glBlendEquationi);
	LOAD_GL_PROC(glBlendEquationSeparate);
	LOAD_GL_PROC(glBlendEquationSeparatei);
	LOAD_GL_PROC(glBlendFunc);			// opengl32.dll
	LOAD_GL_PROC(glBlendFunci);
	LOAD_GL_PROC(glBlendFuncSeparate);
	LOAD_GL_PROC(glBlendFuncSeparatei);
	LOAD_GL_PROC(glClampColor);
	LOAD_GL_PROC(glClipControl);
	LOAD_GL_PROC(glColorMask);			// opengl32.dll
	LOAD_GL_PROC(glColorMaski);
	LOAD_GL_PROC(glCullFace);			// opengl32.dll
	LOAD_GL_PROC(glDepthFunc);			// opengl32.dll
	LOAD_GL_PROC(glDepthMask);			// opengl32.dll
	LOAD_GL_PROC(glDepthRange);			// opengl32.dll
	LOAD_GL_PROC(glDepthRangef);
	LOAD_GL_PROC(glDepthRangeArrayv);
	LOAD_GL_PROC(glDepthRangeIndexed);
	LOAD_GL_PROC(glDisable);			// opengl32.dll
	LOAD_GL_PROC(glDisablei);
	LOAD_GL_PROC(glEnable);				// opengl32.dll
	LOAD_GL_PROC(glEnablei);
	LOAD_GL_PROC(glFrontFace);			// opengl32.dll
	LOAD_GL_PROC(glGetBooleanv);		// opengl32.dll
	LOAD_GL_PROC(glGetDoublev);			// opengl32.dll
	LOAD_GL_PROC(glGetFloatv);			// opengl32.dll
	LOAD_GL_PROC(glGetIntegerv);		// opengl32.dll
	LOAD_GL_PROC(glGetInteger64v);
	LOAD_GL_PROC(glGetBooleani_v);
	LOAD_GL_PROC(glGetFloati_v);
	LOAD_GL_PROC(glGetDoublei_v);
	LOAD_GL_PROC(glGetInteger64i_v);
	LOAD_GL_PROC(glGetError);			// opengl32.dll
	LOAD_GL_PROC(glHint);				// opengl32.dll
	LOAD_GL_PROC(glIsEnabled);			// opengl32.dll
	LOAD_GL_PROC(glIsEnabledi);				
	LOAD_GL_PROC(glLineWidth);			// opengl32.dll
	LOAD_GL_PROC(glLogicOp);			// opengl32.dll
	LOAD_GL_PROC(glPixelStoref);		// opengl32.dll
	LOAD_GL_PROC(glPixelStorei);		// opengl32.dll
	LOAD_GL_PROC(glPointParameterf);
	LOAD_GL_PROC(glPointParameteri);
	LOAD_GL_PROC(glPointParameterfv);
	LOAD_GL_PROC(glPointParameteriv);
	LOAD_GL_PROC(glPointSize);			// opengl32.dll
	LOAD_GL_PROC(glPolygonMode);		// opengl32.dll
	LOAD_GL_PROC(glPolygonOffset);		// opengl32.dll
	LOAD_GL_PROC(glSampleCoverage);
	LOAD_GL_PROC(glScissor);			// opengl32.dll
	LOAD_GL_PROC(glScissorArrayv);
	LOAD_GL_PROC(glScissorIndexed);
	LOAD_GL_PROC(glScissorIndexedv);
	LOAD_GL_PROC(glStencilFunc);		// opengl32.dll
	LOAD_GL_PROC(glStencilFuncSeparate);
	LOAD_GL_PROC(glStencilMask);		// opengl32.dll
	LOAD_GL_PROC(glStencilMaskSeparate);
	LOAD_GL_PROC(glStencilOp);			// opengl32.dll
	LOAD_GL_PROC(glStencilOpSeparate);
	LOAD_GL_PROC(glViewport);			// opengl32.dll
	LOAD_GL_PROC(glViewportArrayv);
	LOAD_GL_PROC(glViewportIndexedf);
	LOAD_GL_PROC(glViewportIndexedfv);

	// Transform Feedback
	LOAD_GL_PROC(glBeginTransformFeedback);
	LOAD_GL_PROC(glBindTransformFeedback);
	LOAD_GL_PROC(glCreateTransformFeedbacks);
	LOAD_GL_PROC(glDeleteTransformFeedbacks);
	LOAD_GL_PROC(glDrawTransformFeedback);
	LOAD_GL_PROC(glDrawTransformFeedbackInstanced);
	LOAD_GL_PROC(glDrawTransformFeedbackStream);
	LOAD_GL_PROC(glDrawTransformFeedbackStreamInstanced);
	LOAD_GL_PROC(glEndTransformFeedback);
	LOAD_GL_PROC(glGenTransformFeedbacks);
	LOAD_GL_PROC(glGetTransformFeedbackiv);
	LOAD_GL_PROC(glGetTransformFeedbacki_v);
	LOAD_GL_PROC(glGetTransformFeedbacki64_v);
	LOAD_GL_PROC(glGetTransformFeedbackVarying);
	LOAD_GL_PROC(glIsTransformFeedback);
	LOAD_GL_PROC(glPauseTransformFeedback);
	LOAD_GL_PROC(glResumeTransformFeedback);
	LOAD_GL_PROC(glTransformFeedbackBufferBase);
	LOAD_GL_PROC(glTransformFeedbackBufferRange);
	LOAD_GL_PROC(glTransformFeedbackVaryings);

	// Utility
	LOAD_GL_PROC(glDispatchCompute);
	LOAD_GL_PROC(glDispatchComputeIndirect);
	LOAD_GL_PROC(glGetGraphicsResetStatus);
	LOAD_GL_PROC(glGetInternalformativ);
	LOAD_GL_PROC(glGetInternalformati64v);
	LOAD_GL_PROC(glGetMultisamplefv);
	LOAD_GL_PROC(glGetString);			// opengl32.dll
	LOAD_GL_PROC(glGetStringi);
	LOAD_GL_PROC(glMemoryBarrier);
	LOAD_GL_PROC(glMemoryBarrierByRegion);

	// Queries
	LOAD_GL_PROC(glBeginConditionalRender);
	LOAD_GL_PROC(glBeginQuery);
	LOAD_GL_PROC(glBeginQueryIndexed);
	LOAD_GL_PROC(glCreateQueries);
	LOAD_GL_PROC(glDeleteQueries);
	LOAD_GL_PROC(glEndConditionalRender);
	LOAD_GL_PROC(glEndQuery);
	LOAD_GL_PROC(glEndQueryIndexed);
	LOAD_GL_PROC(glGenQueries);
	LOAD_GL_PROC(glGetQueryIndexediv);
	LOAD_GL_PROC(glGetQueryObjectiv);
	LOAD_GL_PROC(glGetQueryObjectuiv);
	LOAD_GL_PROC(glGetQueryObjecti64v);
	LOAD_GL_PROC(glGetQueryObjectui64v);
	LOAD_GL_PROC(glGetQueryiv);
	LOAD_GL_PROC(glIsQuery);
	LOAD_GL_PROC(glQueryCounter);

	// Syncing
	LOAD_GL_PROC(glClientWaitSync);
	LOAD_GL_PROC(glDeleteSync);
	LOAD_GL_PROC(glFenceSync);
	LOAD_GL_PROC(glGetSynciv);
	LOAD_GL_PROC(glIsSync);
	LOAD_GL_PROC(glTextureBarrier);
	LOAD_GL_PROC(glWaitSync);

	// Vertex Array Objects
	LOAD_GL_PROC(glBindVertexArray);
	LOAD_GL_PROC(glDeleteVertexArrays);
	LOAD_GL_PROC(glGenVertexArrays);
	LOAD_GL_PROC(glIsVertexArray);

	// Samplers
	LOAD_GL_PROC(glBindSampler);
	LOAD_GL_PROC(glBindSamplers);
	LOAD_GL_PROC(glCreateSamplers);
	LOAD_GL_PROC(glDeleteSamplers);
	LOAD_GL_PROC(glGenSamplers);
	LOAD_GL_PROC(glGetSamplerParameterfv);
	LOAD_GL_PROC(glGetSamplerParameteriv);
	LOAD_GL_PROC(glGetSamplerParameterIiv);
	LOAD_GL_PROC(glGetSamplerParameterIuiv);
	LOAD_GL_PROC(glIsSampler);
	LOAD_GL_PROC(glSamplerParameterf);
	LOAD_GL_PROC(glSamplerParameteri);
	LOAD_GL_PROC(glSamplerParameterfv);
	LOAD_GL_PROC(glSamplerParameteriv);
	LOAD_GL_PROC(glSamplerParameterIiv);
	LOAD_GL_PROC(glSamplerParameterIuiv);

	// Program Pipelines
	LOAD_GL_PROC(glActiveShaderProgram);
	LOAD_GL_PROC(glBindProgramPipeline);
	LOAD_GL_PROC(glCreateProgramPipelines);
	LOAD_GL_PROC(glDeleteProgramPipelines);
	LOAD_GL_PROC(glGenProgramPipelines);
	LOAD_GL_PROC(glGetProgramPipelineiv);
	LOAD_GL_PROC(glGetProgramPipelineInfoLog);
	LOAD_GL_PROC(glIsProgramPipeline);
	LOAD_GL_PROC(glValidateProgramPipeline);

	// Debug
	LOAD_GL_PROC(glDebugMessageCallback);
	LOAD_GL_PROC(glDebugMessageControl);
	LOAD_GL_PROC(glDebugMessageInsert);
	LOAD_GL_PROC(glGetDebugMessageLog);
	LOAD_GL_PROC(glGetObjectLabel);
	LOAD_GL_PROC(glGetObjectPtrLabel);
	LOAD_GL_PROC(glGetPointerv);			// @TODO investigate, both in opengl32.dll and amd driver
	LOAD_GL_PROC(glGetProgramInterfaceiv);
	LOAD_GL_PROC(glObjectLabel);
	LOAD_GL_PROC(glObjectPtrLabel);
	LOAD_GL_PROC(glPopDebugGroup);
	LOAD_GL_PROC(glPushDebugGroup);

	// Buffer Objects
	LOAD_GL_PROC(glBindBuffer);
	LOAD_GL_PROC(glBindBufferBase);
	LOAD_GL_PROC(glBindBufferRange);
	LOAD_GL_PROC(glBindBuffersBase);
	LOAD_GL_PROC(glBindBuffersRange);
	LOAD_GL_PROC(glBindVertexBuffer);
	LOAD_GL_PROC(glVertexArrayVertexBuffer);
	LOAD_GL_PROC(glBindVertexBuffers);
	LOAD_GL_PROC(glVertexArrayVertexBuffers);
	LOAD_GL_PROC(glBufferData);
	LOAD_GL_PROC(glNamedBufferData);
	LOAD_GL_PROC(glBufferStorage);
	LOAD_GL_PROC(glNamedBufferStorage);
	LOAD_GL_PROC(glBufferSubData);
	LOAD_GL_PROC(glNamedBufferSubData);
	LOAD_GL_PROC(glClearBufferData);
	LOAD_GL_PROC(glClearNamedBufferData);
	LOAD_GL_PROC(glClearBufferSubData);
	LOAD_GL_PROC(glClearNamedBufferSubData);
	LOAD_GL_PROC(glCopyBufferSubData);
	LOAD_GL_PROC(glCopyNamedBufferSubData);
	LOAD_GL_PROC(glCreateBuffers);
	LOAD_GL_PROC(glCreateVertexArrays);
	LOAD_GL_PROC(glDeleteBuffers);
	LOAD_GL_PROC(glDisableVertexAttribArray);
	LOAD_GL_PROC(glDisableVertexArrayAttrib);
	LOAD_GL_PROC(glDrawArrays);			// opengl32.dll
	LOAD_GL_PROC(glDrawArraysIndirect);
	LOAD_GL_PROC(glDrawArraysInstanced);
	LOAD_GL_PROC(glDrawArraysInstancedBaseInstance);
	LOAD_GL_PROC(glDrawElements);		// opengl32.dll
	LOAD_GL_PROC(glDrawElementsBaseVertex);
	LOAD_GL_PROC(glDrawElementsIndirect);
	LOAD_GL_PROC(glDrawElementsInstanced);
	LOAD_GL_PROC(glDrawElementsInstancedBaseInstance);
	LOAD_GL_PROC(glDrawElementsInstancedBaseVertex);
	LOAD_GL_PROC(glDrawElementsInstancedBaseVertexBaseInstance);
	LOAD_GL_PROC(glDrawRangeElements);
	LOAD_GL_PROC(glDrawRangeElementsBaseVertex);
	LOAD_GL_PROC(glEnableVertexAttribArray);
	LOAD_GL_PROC(glEnableVertexArrayAttrib);
	LOAD_GL_PROC(glFlushMappedBufferRange);
	LOAD_GL_PROC(glFlushMappedNamedBufferRange);
	LOAD_GL_PROC(glGenBuffers);
	LOAD_GL_PROC(glGetBufferParameteriv);
	LOAD_GL_PROC(glGetBufferParameteri64v);
	LOAD_GL_PROC(glGetNamedBufferParameteriv);
	LOAD_GL_PROC(glGetNamedBufferParameteri64v);
	LOAD_GL_PROC(glGetBufferPointerv);
	LOAD_GL_PROC(glGetNamedBufferPointerv);
	LOAD_GL_PROC(glGetBufferSubData);
	LOAD_GL_PROC(glGetVertexArrayIndexediv);
	LOAD_GL_PROC(glGetVertexArrayIndexed64iv);
	LOAD_GL_PROC(glGetVertexArrayiv);
	LOAD_GL_PROC(glGetVertexAttribdv);
	LOAD_GL_PROC(glGetVertexAttribfv);
	LOAD_GL_PROC(glGetVertexAttribiv);
	LOAD_GL_PROC(glGetVertexAttribIiv);
	LOAD_GL_PROC(glGetVertexAttribIuiv);
	LOAD_GL_PROC(glGetVertexAttribLdv);
	LOAD_GL_PROC(glGetVertexAttribPointerv);
	LOAD_GL_PROC(glInvalidateBufferData);
	LOAD_GL_PROC(glInvalidateBufferSubData);
	LOAD_GL_PROC(glIsBuffer);
	LOAD_GL_PROC(glMapBuffer);
	LOAD_GL_PROC(glMapNamedBuffer);
	LOAD_GL_PROC(glMapBufferRange);
	LOAD_GL_PROC(glMapNamedBufferRange);
	LOAD_GL_PROC(glMultiDrawArrays);
	LOAD_GL_PROC(glMultiDrawArraysIndirect);
	LOAD_GL_PROC(glMultiDrawElements);
	LOAD_GL_PROC(glMultiDrawElementsBaseVertex);
	LOAD_GL_PROC(glMultiDrawElementsIndirect);
	LOAD_GL_PROC(glPatchParameteri);
	LOAD_GL_PROC(glPatchParameterfv);
	LOAD_GL_PROC(glPrimitiveRestartIndex);
	LOAD_GL_PROC(glProvokingVertex);
	LOAD_GL_PROC(glUnmapBuffer);
	LOAD_GL_PROC(glUnmapNamedBuffer);
	LOAD_GL_PROC(glVertexArrayElementBuffer);
	LOAD_GL_PROC(glVertexAttrib1f);
	LOAD_GL_PROC(glVertexAttrib1s);
	LOAD_GL_PROC(glVertexAttrib1d);
	LOAD_GL_PROC(glVertexAttribI1i);
	LOAD_GL_PROC(glVertexAttribI1ui);
	LOAD_GL_PROC(glVertexAttrib2f);
	LOAD_GL_PROC(glVertexAttrib2s);
	LOAD_GL_PROC(glVertexAttrib2d);
	LOAD_GL_PROC(glVertexAttribI2i);
	LOAD_GL_PROC(glVertexAttribI2ui);
	LOAD_GL_PROC(glVertexAttrib3f);
	LOAD_GL_PROC(glVertexAttrib3s);
	LOAD_GL_PROC(glVertexAttrib3d);
	LOAD_GL_PROC(glVertexAttribI3i);
	LOAD_GL_PROC(glVertexAttribI3ui);
	LOAD_GL_PROC(glVertexAttrib4f);
	LOAD_GL_PROC(glVertexAttrib4s);
	LOAD_GL_PROC(glVertexAttrib4d);
	LOAD_GL_PROC(glVertexAttrib4Nub);
	LOAD_GL_PROC(glVertexAttribI4i);
	LOAD_GL_PROC(glVertexAttribI4ui);
	LOAD_GL_PROC(glVertexAttribL1d);
	LOAD_GL_PROC(glVertexAttribL2d);
	LOAD_GL_PROC(glVertexAttribL3d);
	LOAD_GL_PROC(glVertexAttribL4d);
	LOAD_GL_PROC(glVertexAttrib1fv);
	LOAD_GL_PROC(glVertexAttrib1sv);
	LOAD_GL_PROC(glVertexAttrib1dv);
	LOAD_GL_PROC(glVertexAttribI1iv);
	LOAD_GL_PROC(glVertexAttribI1uiv);
	LOAD_GL_PROC(glVertexAttrib2fv);
	LOAD_GL_PROC(glVertexAttrib2sv);
	LOAD_GL_PROC(glVertexAttrib2dv);
	LOAD_GL_PROC(glVertexAttribI2iv);
	LOAD_GL_PROC(glVertexAttribI2uiv);
	LOAD_GL_PROC(glVertexAttrib3fv);
	LOAD_GL_PROC(glVertexAttrib3sv);
	LOAD_GL_PROC(glVertexAttrib3dv);
	LOAD_GL_PROC(glVertexAttribI3iv);
	LOAD_GL_PROC(glVertexAttribI3uiv);
	LOAD_GL_PROC(glVertexAttrib4fv);
	LOAD_GL_PROC(glVertexAttrib4sv);
	LOAD_GL_PROC(glVertexAttrib4dv);
	LOAD_GL_PROC(glVertexAttrib4iv);
	LOAD_GL_PROC(glVertexAttrib4bv);
	LOAD_GL_PROC(glVertexAttrib4ubv);
	LOAD_GL_PROC(glVertexAttrib4usv);
	LOAD_GL_PROC(glVertexAttrib4uiv);
	LOAD_GL_PROC(glVertexAttrib4Nbv);
	LOAD_GL_PROC(glVertexAttrib4Nsv);
	LOAD_GL_PROC(glVertexAttrib4Niv);
	LOAD_GL_PROC(glVertexAttrib4Nubv);
	LOAD_GL_PROC(glVertexAttrib4Nusv);
	LOAD_GL_PROC(glVertexAttrib4Nuiv);
	LOAD_GL_PROC(glVertexAttribI4bv);
	LOAD_GL_PROC(glVertexAttribI4ubv);
	LOAD_GL_PROC(glVertexAttribI4sv);
	LOAD_GL_PROC(glVertexAttribI4usv);
	LOAD_GL_PROC(glVertexAttribI4iv);
	LOAD_GL_PROC(glVertexAttribI4uiv);
	LOAD_GL_PROC(glVertexAttribL1dv);
	LOAD_GL_PROC(glVertexAttribL2dv);
	LOAD_GL_PROC(glVertexAttribL3dv);
	LOAD_GL_PROC(glVertexAttribL4dv);
	LOAD_GL_PROC(glVertexAttribP1ui);
	LOAD_GL_PROC(glVertexAttribP2ui);
	LOAD_GL_PROC(glVertexAttribP3ui);
	LOAD_GL_PROC(glVertexAttribP4ui);
	LOAD_GL_PROC(glVertexAttribBinding);
	LOAD_GL_PROC(glVertexArrayAttribBinding);
	LOAD_GL_PROC(glVertexAttribDivisor);
	LOAD_GL_PROC(glVertexAttribFormat);
	LOAD_GL_PROC(glVertexAttribIFormat);
	LOAD_GL_PROC(glVertexAttribLFormat);
	LOAD_GL_PROC(glVertexArrayAttribFormat);
	LOAD_GL_PROC(glVertexArrayAttribIFormat);
	LOAD_GL_PROC(glVertexArrayAttribLFormat);
	LOAD_GL_PROC(glVertexAttribPointer);
	LOAD_GL_PROC(glVertexAttribIPointer);
	LOAD_GL_PROC(glVertexAttribLPointer);
	LOAD_GL_PROC(glVertexBindingDivisor);
	LOAD_GL_PROC(glVertexArrayBindingDivisor);

	#if defined (_WIN32) || defined(_WIN64)
	FreeLibrary(glcode);
	#elif defined(__linux__)
	#endif
	return 0;
}

#define glClear hogl_glClear
#define glClearBufferiv hogl_glClearBufferiv
#define glClearBufferuiv hogl_glClearBufferuiv
#define glClearBufferfv hogl_glClearBufferfv
#define glClearBufferfi hogl_glClearBufferfi
#define glClearNamedFramebufferiv hogl_glClearNamedFramebufferiv
#define glClearNamedFramebufferuiv hogl_glClearNamedFramebufferuiv
#define glClearNamedFramebufferfv hogl_glClearNamedFramebufferfv
#define glClearNamedFramebufferfi hogl_glClearNamedFramebufferfi
#define glNamedFramebufferReadBuffer hogl_glNamedFramebufferReadBuffer
#define glReadnPixels hogl_glReadnPixels
#define glClearColor hogl_glClearColor
#define glClearDepth hogl_glClearDepth
#define glClearStencil hogl_glClearStencil
#define glDrawBuffer hogl_glDrawBuffer
#define glFinish hogl_glFinish
#define glFlush hogl_glFlush
#define glReadBuffer hogl_glReadBuffer
#define glReadPixels hogl_glReadPixels

#define glActiveTexture hogl_glActiveTexture
#define glBindImageTexture hogl_glBindImageTexture
#define glBindImageTextures hogl_glBindImageTextures
#define glBindTextureUnit hogl_glBindTextureUnit
#define glBindTextures hogl_glBindTextures
#define glBindTexture hogl_glBindTexture
#define glClearTexImage hogl_glClearTexImage
#define glClearTexSubImage hogl_glClearTexSubImage
#define glCompressedTexImage1D hogl_glCompressedTexImage1D
#define glCompressedTexImage2D hogl_glCompressedTexImage2D
#define glCompressedTexImage3D hogl_glCompressedTexImage3D
#define glCompressedTexSubImage1D hogl_glCompressedTexSubImage1D
#define glCompressedTextureSubImage1D hogl_glCompressedTextureSubImage1D
#define glCompressedTexSubImage2D hogl_glCompressedTexSubImage2D
#define glCompressedTextureSubImage2D hogl_glCompressedTextureSubImage2D
#define glCompressedTexSubImage3D hogl_glCompressedTexSubImage3D
#define glCompressedTextureSubImage3D hogl_glCompressedTextureSubImage3D
#define glCopyTextureSubImage1D hogl_glCopyTextureSubImage1D
#define glCopyTextureSubImage2D hogl_glCopyTextureSubImage2D
#define glCopyImageSubData hogl_glCopyImageSubData
#define glCopyTexImage1D hogl_glCopyTexImage1D
#define glCopyTexImage2D hogl_glCopyTexImage2D
#define glCopyTexSubImage1D hogl_glCopyTexSubImage1D
#define glCopyTexSubImage2D hogl_glCopyTexSubImage2D
#define glCopyTexSubImage3D hogl_glCopyTexSubImage3D
#define glCopyTextureSubImage3D hogl_glCopyTextureSubImage3D
#define glCreateTextures hogl_glCreateTextures
#define glDeleteTextures hogl_glDeleteTextures
#define glGenTextures hogl_glGenTextures
#define glGetCompressedTexImage hogl_glGetCompressedTexImage
#define glGetnCompressedTexImage hogl_glGetnCompressedTexImage
#define glGetCompressedTextureImage hogl_glGetCompressedTextureImage
#define glGetCompressedTextureSubImage hogl_glGetCompressedTextureSubImage
#define glGetTexImage hogl_glGetTexImage
#define glGetnTexImage hogl_glGetnTexImage
#define glGetTextureImage hogl_glGetTextureImage
#define glGetTexLevelParameterfv hogl_glGetTexLevelParameterfv
#define glGetTexLevelParameteriv hogl_glGetTexLevelParameteriv
#define glGetTextureLevelParameterfv hogl_glGetTextureLevelParameterfv
#define glGetTextureLevelParameteriv hogl_glGetTextureLevelParameteriv
#define glGetTexParameterfv hogl_glGetTexParameterfv
#define glGetTexParameteriv hogl_glGetTexParameteriv
#define glGetTexParameterIiv hogl_glGetTexParameterIiv
#define glGetTexParameterIuiv hogl_glGetTexParameterIuiv
#define glGetTextureParameterfv hogl_glGetTextureParameterfv
#define glGetTextureParameteriv hogl_glGetTextureParameteriv
#define glGetTextureParameterIiv hogl_glGetTextureParameterIiv
#define glGetTextureParameterIuiv hogl_glGetTextureParameterIuiv
#define glGetTextureSubImage hogl_glGetTextureSubImage
#define glInvalidateTexImage hogl_glInvalidateTexImage
#define glInvalidateTexSubImage hogl_glInvalidateTexSubImage
#define glIsTexture hogl_glIsTexture
#define glTexBuffer hogl_glTexBuffer
#define glTextureBuffer hogl_glTextureBuffer
#define glTexBufferRange hogl_glTexBufferRange
#define glTextureBufferRange hogl_glTextureBufferRange
#define glTexImage1D hogl_glTexImage1D
#define glTexImage2D hogl_glTexImage2D
#define glTexImage2DMultisample hogl_glTexImage2DMultisample
#define glTexImage3D hogl_glTexImage3D
#define glTexImage3DMultisample hogl_glTexImage3DMultisample
#define glTexParameterf hogl_glTexParameterf
#define glTexParameteri hogl_glTexParameteri
#define glTextureParameterf hogl_glTextureParameterf
#define glTextureParameteri hogl_glTextureParameteri
#define glTexParameterfv hogl_glTexParameterfv
#define glTexParameteriv hogl_glTexParameteriv
#define glTexParameterIiv hogl_glTexParameterIiv
#define glTexParameterIuiv hogl_glTexParameterIuiv
#define glTextureParameterfv hogl_glTextureParameterfv
#define glTextureParameteriv hogl_glTextureParameteriv
#define glTextureParameterIiv hogl_glTextureParameterIiv
#define glTextureParameterIuiv hogl_glTextureParameterIuiv
#define glTexStorage1D hogl_glTexStorage1D
#define glTextureStorage1D hogl_glTextureStorage1D
#define glTexStorage2D hogl_glTexStorage2D
#define glTextureStorage2D hogl_glTextureStorage2D
#define glTexStorage2DMultisample hogl_glTexStorage2DMultisample
#define glTextureStorage2DMultisample hogl_glTextureStorage2DMultisample
#define glTexStorage3D hogl_glTexStorage3D
#define glTextureStorage3D hogl_glTextureStorage3D
#define glTexStorage3DMultisample hogl_glTexStorage3DMultisample
#define glTextureStorage3DMultisample hogl_glTextureStorage3DMultisample
#define glTexSubImage1D hogl_glTexSubImage1D
#define glTextureSubImage1D hogl_glTextureSubImage1D
#define glTexSubImage2D hogl_glTexSubImage2D
#define glTextureSubImage2D hogl_glTextureSubImage2D
#define glTexSubImage3D hogl_glTexSubImage3D
#define glTextureSubImage3D hogl_glTextureSubImage3D
#define glTextureView hogl_glTextureView

#define glBindFramebuffer hogl_glBindFramebuffer
#define glBindRenderbuffer hogl_glBindRenderbuffer
#define glBlitFramebuffer hogl_glBlitFramebuffer
#define glBlitNamedFramebuffer hogl_glBlitNamedFramebuffer
#define glCheckFramebufferStatus hogl_glCheckFramebufferStatus
#define glCheckNamedFramebufferStatus hogl_glCheckNamedFramebufferStatus
#define glCreateFramebuffers hogl_glCreateFramebuffers
#define glCreateRenderbuffers hogl_glCreateRenderbuffers
#define glDeleteFramebuffers hogl_glDeleteFramebuffers
#define glDeleteRenderbuffers hogl_glDeleteRenderbuffers
#define glDrawBuffers hogl_glDrawBuffers
#define glNamedFramebufferDrawBuffers hogl_glNamedFramebufferDrawBuffers
#define glFramebufferParameteri hogl_glFramebufferParameteri
#define glNamedFramebufferParameteri hogl_glNamedFramebufferParameteri
#define glFramebufferRenderbuffer hogl_glFramebufferRenderbuffer
#define glNamedFramebufferRenderbuffer hogl_glNamedFramebufferRenderbuffer
#define glFramebufferTexture hogl_glFramebufferTexture
#define glFramebufferTexture1D hogl_glFramebufferTexture1D
#define glFramebufferTexture2D hogl_glFramebufferTexture2D
#define glFramebufferTexture3D hogl_glFramebufferTexture3D
#define glNamedFramebufferTexture hogl_glNamedFramebufferTexture
#define glFramebufferTextureLayer hogl_glFramebufferTextureLayer
#define glNamedFramebufferTextureLayer hogl_glNamedFramebufferTextureLayer
#define glGenFramebuffers hogl_glGenFramebuffers
#define glGenRenderbuffers hogl_glGenRenderbuffers
#define glGenerateMipmap hogl_glGenerateMipmap
#define glGenerateTextureMipmap hogl_glGenerateTextureMipmap
#define glGetFramebufferAttachmentParameteriv hogl_glGetFramebufferAttachmentParameteriv
#define glGetNamedFramebufferAttachmentParameteriv hogl_glGetNamedFramebufferAttachmentParameteriv
#define glGetFramebufferParameteriv hogl_glGetFramebufferParameteriv
#define glGetNamedFramebufferParameteriv hogl_glGetNamedFramebufferParameteriv
#define glGetRenderbufferParameteriv hogl_glGetRenderbufferParameteriv
#define glGetNamedRenderbufferParameteriv hogl_glGetNamedRenderbufferParameteriv
#define glInvalidateFramebuffer hogl_glInvalidateFramebuffer
#define glInvalidateNamedFramebufferData hogl_glInvalidateNamedFramebufferData
#define glInvalidateSubFramebuffer hogl_glInvalidateSubFramebuffer
#define glInvalidateNamedFramebufferSubData hogl_glInvalidateNamedFramebufferSubData
#define glIsFramebuffer hogl_glIsFramebuffer
#define glIsRenderbuffer hogl_glIsRenderbuffer
#define glRenderbufferStorage hogl_glRenderbufferStorage
#define glNamedRenderbufferStorage hogl_glNamedRenderbufferStorage
#define glRenderbufferStorageMultisample hogl_glRenderbufferStorageMultisample
#define glNamedRenderbufferStorageMultisample hogl_glNamedRenderbufferStorageMultisample
#define glSampleMaski hogl_glSampleMaski

#define glAttachShader hogl_glAttachShader
#define glBindAttribLocation hogl_glBindAttribLocation
#define glBindFragDataLocation hogl_glBindFragDataLocation
#define glBindFragDataLocationIndexed hogl_glBindFragDataLocationIndexed
#define glCompileShader hogl_glCompileShader
#define glCreateProgram hogl_glCreateProgram
#define glCreateShader hogl_glCreateShader
#define glCreateShaderProgramv hogl_glCreateShaderProgramv
#define glDeleteProgram hogl_glDeleteProgram
#define glDeleteShader hogl_glDeleteShader
#define glDetachShader hogl_glDetachShader
#define glGetActiveAtomicCounterBufferiv hogl_glGetActiveAtomicCounterBufferiv
#define glGetActiveAttrib hogl_glGetActiveAttrib
#define glGetActiveSubroutineName hogl_glGetActiveSubroutineName
#define glGetActiveSubroutineUniformiv hogl_glGetActiveSubroutineUniformiv
#define glGetActiveSubroutineUniformName hogl_glGetActiveSubroutineUniformName
#define glGetActiveUniform hogl_glGetActiveUniform
#define glGetActiveUniformBlockiv hogl_glGetActiveUniformBlockiv
#define glGetActiveUniformBlockName hogl_glGetActiveUniformBlockName
#define glGetActiveUniformName hogl_glGetActiveUniformName
#define glGetActiveUniformsiv hogl_glGetActiveUniformsiv
#define glGetAttachedShaders hogl_glGetAttachedShaders
#define glGetAttribLocation hogl_glGetAttribLocation
#define glGetFragDataIndex hogl_glGetFragDataIndex
#define glGetFragDataLocation hogl_glGetFragDataLocation
#define glGetProgramiv hogl_glGetProgramiv
#define glGetProgramBinary hogl_glGetProgramBinary
#define glGetProgramInfoLog hogl_glGetProgramInfoLog
#define glGetProgramResourceiv hogl_glGetProgramResourceiv
#define glGetProgramResourceIndex hogl_glGetProgramResourceIndex
#define glGetProgramResourceLocation hogl_glGetProgramResourceLocation
#define glGetProgramResourceLocationIndex hogl_glGetProgramResourceLocationIndex
#define glGetProgramResourceName hogl_glGetProgramResourceName
#define glGetProgramStageiv hogl_glGetProgramStageiv
#define glGetShaderiv hogl_glGetShaderiv
#define glGetShaderInfoLog hogl_glGetShaderInfoLog
#define glGetShaderPrecisionFormat hogl_glGetShaderPrecisionFormat
#define glGetShaderSource hogl_glGetShaderSource
#define glGetSubroutineIndex hogl_glGetSubroutineIndex
#define glGetSubroutineUniformLocation hogl_glGetSubroutineUniformLocation
#define glGetUniformfv hogl_glGetUniformfv
#define glGetUniformiv hogl_glGetUniformiv
#define glGetUniformuiv hogl_glGetUniformuiv
#define glGetUniformdv hogl_glGetUniformdv
#define glGetnUniformfv hogl_glGetnUniformfv
#define glGetnUniformiv hogl_glGetnUniformiv
#define glGetnUniformuiv hogl_glGetnUniformuiv
#define glGetnUniformdv hogl_glGetnUniformdv
#define glGetUniformBlockIndex hogl_glGetUniformBlockIndex
#define glGetUniformIndices hogl_glGetUniformIndices
#define glGetUniformLocation hogl_glGetUniformLocation
#define glGetUniformSubroutineuiv hogl_glGetUniformSubroutineuiv
#define glIsProgram hogl_glIsProgram
#define glIsShader hogl_glIsShader
#define glLinkProgram hogl_glLinkProgram
#define glMinSampleShading hogl_glMinSampleShading
#define glProgramBinary hogl_glProgramBinary
#define glProgramParameteri hogl_glProgramParameteri
#define glProgramUniform1f hogl_glProgramUniform1f
#define glProgramUniform2f hogl_glProgramUniform2f
#define glProgramUniform3f hogl_glProgramUniform3f
#define glProgramUniform4f hogl_glProgramUniform4f
#define glProgramUniform1i hogl_glProgramUniform1i
#define glProgramUniform2i hogl_glProgramUniform2i
#define glProgramUniform3i hogl_glProgramUniform3i
#define glProgramUniform4i hogl_glProgramUniform4i
#define glProgramUniform1ui hogl_glProgramUniform1ui
#define glProgramUniform2ui hogl_glProgramUniform2ui
#define glProgramUniform3ui hogl_glProgramUniform3ui
#define glProgramUniform4ui hogl_glProgramUniform4ui
#define glProgramUniform1fv hogl_glProgramUniform1fv
#define glProgramUniform2fv hogl_glProgramUniform2fv
#define glProgramUniform3fv hogl_glProgramUniform3fv
#define glProgramUniform4fv hogl_glProgramUniform4fv
#define glProgramUniform1iv hogl_glProgramUniform1iv
#define glProgramUniform2iv hogl_glProgramUniform2iv
#define glProgramUniform3iv hogl_glProgramUniform3iv
#define glProgramUniform4iv hogl_glProgramUniform4iv
#define glProgramUniform1uiv hogl_glProgramUniform1uiv
#define glProgramUniform2uiv hogl_glProgramUniform2uiv
#define glProgramUniform3uiv hogl_glProgramUniform3uiv
#define glProgramUniform4uiv hogl_glProgramUniform4uiv
#define glProgramUniformMatrix2fv hogl_glProgramUniformMatrix2fv
#define glProgramUniformMatrix3fv hogl_glProgramUniformMatrix3fv
#define glProgramUniformMatrix4fv hogl_glProgramUniformMatrix4fv
#define glProgramUniformMatrix2x3fv hogl_glProgramUniformMatrix2x3fv
#define glProgramUniformMatrix3x2fv hogl_glProgramUniformMatrix3x2fv
#define glProgramUniformMatrix2x4fv hogl_glProgramUniformMatrix2x4fv
#define glProgramUniformMatrix4x2fv hogl_glProgramUniformMatrix4x2fv
#define glProgramUniformMatrix3x4fv hogl_glProgramUniformMatrix3x4fv
#define glProgramUniformMatrix4x3fv hogl_glProgramUniformMatrix4x3fv
#define glReleaseShaderCompiler hogl_glReleaseShaderCompiler
#define glShaderBinary hogl_glShaderBinary
#define glShaderSource hogl_glShaderSource
#define glShaderStorageBlockBinding hogl_glShaderStorageBlockBinding
#define glUniform1f hogl_glUniform1f
#define glUniform2f hogl_glUniform2f
#define glUniform3f hogl_glUniform3f
#define glUniform4f hogl_glUniform4f
#define glUniform1i hogl_glUniform1i
#define glUniform2i hogl_glUniform2i
#define glUniform3i hogl_glUniform3i
#define glUniform4i hogl_glUniform4i
#define glUniform1ui hogl_glUniform1ui
#define glUniform2ui hogl_glUniform2ui
#define glUniform3ui hogl_glUniform3ui
#define glUniform4ui hogl_glUniform4ui
#define glUniform1fv hogl_glUniform1fv
#define glUniform2fv hogl_glUniform2fv
#define glUniform3fv hogl_glUniform3fv
#define glUniform4fv hogl_glUniform4fv
#define glUniform1iv hogl_glUniform1iv
#define glUniform2iv hogl_glUniform2iv
#define glUniform3iv hogl_glUniform3iv
#define glUniform4iv hogl_glUniform4iv
#define glUniform1uiv hogl_glUniform1uiv
#define glUniform2uiv hogl_glUniform2uiv
#define glUniform3uiv hogl_glUniform3uiv
#define glUniform4uiv hogl_glUniform4uiv
#define glUniformMatrix2fv hogl_glUniformMatrix2fv
#define glUniformMatrix3fv hogl_glUniformMatrix3fv
#define glUniformMatrix4fv hogl_glUniformMatrix4fv
#define glUniformMatrix2x3fv hogl_glUniformMatrix2x3fv
#define glUniformMatrix3x2fv hogl_glUniformMatrix3x2fv
#define glUniformMatrix2x4fv hogl_glUniformMatrix2x4fv
#define glUniformMatrix4x2fv hogl_glUniformMatrix4x2fv
#define glUniformMatrix3x4fv hogl_glUniformMatrix3x4fv
#define glUniformMatrix4x3fv hogl_glUniformMatrix4x3fv
#define glUniformBlockBinding hogl_glUniformBlockBinding
#define glUniformSubroutinesuiv hogl_glUniformSubroutinesuiv
#define glUseProgram hogl_glUseProgram
#define glUseProgramStages hogl_glUseProgramStages
#define glValidateProgram hogl_glValidateProgram

#define glBlendColor hogl_glBlendColor
#define glBlendEquation hogl_glBlendEquation
#define glBlendEquationi hogl_glBlendEquationi
#define glBlendEquationSeparate hogl_glBlendEquationSeparate
#define glBlendEquationSeparatei hogl_glBlendEquationSeparatei
#define glBlendFunc hogl_glBlendFunc
#define glBlendFunci hogl_glBlendFunci
#define glBlendFuncSeparate hogl_glBlendFuncSeparate
#define glBlendFuncSeparatei hogl_glBlendFuncSeparatei
#define glClampColor hogl_glClampColor
#define glClipControl hogl_glClipControl
#define glColorMask hogl_glColorMask
#define glColorMaski hogl_glColorMaski
#define glCullFace hogl_glCullFace
#define glDepthFunc hogl_glDepthFunc
#define glDepthMask hogl_glDepthMask
#define glDepthRange hogl_glDepthRange
#define glDepthRangef hogl_glDepthRangef
#define glDepthRangeArrayv hogl_glDepthRangeArrayv
#define glDepthRangeIndexed hogl_glDepthRangeIndexed
#define glDisable hogl_glDisable
#define glEnablei hogl_glEnablei
#define glDisablei hogl_glDisablei
#define glEnable hogl_glEnable
#define glFrontFace hogl_glFrontFace
#define glGetBooleanv hogl_glGetBooleanv
#define glGetDoublev hogl_glGetDoublev
#define glGetFloatv hogl_glGetFloatv
#define glGetIntegerv hogl_glGetIntegerv
#define glGetInteger64v hogl_glGetInteger64v
#define glGetBooleani_v hogl_glGetBooleani_v
#define glGetFloati_v hogl_glGetFloati_v
#define glGetDoublei_v hogl_glGetDoublei_v
#define glGetInteger64i_v hogl_glGetInteger64i_v
#define glGetError hogl_glGetError
#define glHint hogl_glHint
#define glIsEnabled hogl_glIsEnabled
#define glIsEnabledi hogl_glIsEnabledi
#define glLineWidth hogl_glLineWidth
#define glLogicOp hogl_glLogicOp
#define glPixelStoref hogl_glPixelStoref
#define glPixelStorei hogl_glPixelStorei
#define glPointParameterf hogl_glPointParameterf
#define glPointParameteri hogl_glPointParameteri
#define glPointParameterfv hogl_glPointParameterfv
#define glPointParameteriv hogl_glPointParameteriv
#define glPointSize hogl_glPointSize
#define glPolygonMode hogl_glPolygonMode
#define glPolygonOffset hogl_glPolygonOffset
#define glScissor hogl_glScissor
#define glSampleCoverage hogl_glSampleCoverage
#define glScissorArrayv hogl_glScissorArrayv
#define glScissorIndexed hogl_glScissorIndexed
#define glScissorIndexedv hogl_glScissorIndexedv
#define glStencilFunc hogl_glStencilFunc
#define glStencilFuncSeparate hogl_glStencilFuncSeparate
#define glStencilMask hogl_glStencilMask
#define glStencilMaskSeparate hogl_glStencilMaskSeparate
#define glStencilOp hogl_glStencilOp
#define glStencilOpSeparate hogl_glStencilOpSeparate
#define glViewport hogl_glViewport
#define glViewportArrayv hogl_glViewportArrayv
#define glViewportIndexedf hogl_glViewportIndexedf
#define glViewportIndexedfv hogl_glViewportIndexedfv

#define glBeginTransformFeedback hogl_glBeginTransformFeedback
#define glBindTransformFeedback hogl_glBindTransformFeedback
#define glCreateTransformFeedbacks hogl_glCreateTransformFeedbacks
#define glDeleteTransformFeedbacks hogl_glDeleteTransformFeedbacks
#define glDrawTransformFeedback hogl_glDrawTransformFeedback
#define glDrawTransformFeedbackInstanced hogl_glDrawTransformFeedbackInstanced
#define glDrawTransformFeedbackStream hogl_glDrawTransformFeedbackStream
#define glDrawTransformFeedbackStreamInstanced hogl_glDrawTransformFeedbackStreamInstanced
#define glEndTransformFeedback hogl_glEndTransformFeedback
#define glGenTransformFeedbacks hogl_glGenTransformFeedbacks
#define glGetTransformFeedbackiv hogl_glGetTransformFeedbackiv
#define glGetTransformFeedbacki_v hogl_glGetTransformFeedbacki_v
#define glGetTransformFeedbacki64_v hogl_glGetTransformFeedbacki64_v
#define glGetTransformFeedbackVarying hogl_glGetTransformFeedbackVarying
#define glIsTransformFeedback hogl_glIsTransformFeedback
#define glPauseTransformFeedback hogl_glPauseTransformFeedback
#define glResumeTransformFeedback hogl_glResumeTransformFeedback
#define glTransformFeedbackBufferBase hogl_glTransformFeedbackBufferBase
#define glTransformFeedbackBufferRange hogl_glTransformFeedbackBufferRange
#define glTransformFeedbackVaryings hogl_glTransformFeedbackVaryings

#define glDispatchCompute hogl_glDispatchCompute
#define glDispatchComputeIndirect hogl_glDispatchComputeIndirect
#define glGetGraphicsResetStatus hogl_glGetGraphicsResetStatus
#define glGetInternalformativ hogl_glGetInternalformativ
#define glGetInternalformati64v hogl_glGetInternalformati64v
#define glGetMultisamplefv hogl_glGetMultisamplefv
#define glGetString hogl_glGetString
#define glGetStringi hogl_glGetStringi
#define glMemoryBarrier hogl_glMemoryBarrier
#define glMemoryBarrierByRegion hogl_glMemoryBarrierByRegion

#define glBeginConditionalRender hogl_glBeginConditionalRender
#define glBeginQuery hogl_glBeginQuery
#define glBeginQueryIndexed hogl_glBeginQueryIndexed
#define glCreateQueries hogl_glCreateQueries
#define glDeleteQueries hogl_glDeleteQueries
#define glEndConditionalRender hogl_glEndConditionalRender
#define glEndQuery hogl_glEndQuery
#define glEndQueryIndexed hogl_glEndQueryIndexed
#define glGenQueries hogl_glGenQueries
#define glGetQueryIndexediv hogl_glGetQueryIndexediv
#define glGetQueryObjectiv hogl_glGetQueryObjectiv
#define glGetQueryObjectuiv hogl_glGetQueryObjectuiv
#define glGetQueryObjecti64v hogl_glGetQueryObjecti64v
#define glGetQueryObjectui64v hogl_glGetQueryObjectui64v
#define glGetQueryiv hogl_glGetQueryiv
#define glIsQuery hogl_glIsQuery
#define glQueryCounter hogl_glQueryCounter

#define glClientWaitSync hogl_glClientWaitSync
#define glDeleteSync hogl_glDeleteSync
#define glFenceSync hogl_glFenceSync
#define glGetSynciv hogl_glGetSynciv
#define glIsSync hogl_glIsSync
#define glTextureBarrier hogl_glTextureBarrier
#define glWaitSync hogl_glWaitSync

#define glBindVertexArray hogl_glBindVertexArray
#define glDeleteVertexArrays hogl_glDeleteVertexArrays
#define glGenVertexArrays hogl_glGenVertexArrays
#define glIsVertexArray hogl_glIsVertexArray

#define glBindSampler hogl_glBindSampler
#define glBindSamplers hogl_glBindSamplers
#define glCreateSamplers hogl_glCreateSamplers
#define glDeleteSamplers hogl_glDeleteSamplers
#define glGenSamplers hogl_glGenSamplers
#define glGetSamplerParameterfv hogl_glGetSamplerParameterfv
#define glGetSamplerParameteriv hogl_glGetSamplerParameteriv
#define glGetSamplerParameterIiv hogl_glGetSamplerParameterIiv
#define glGetSamplerParameterIuiv hogl_glGetSamplerParameterIuiv
#define glIsSampler hogl_glIsSampler
#define glSamplerParameterf hogl_glSamplerParameterf
#define glSamplerParameteri hogl_glSamplerParameteri
#define glSamplerParameterfv hogl_glSamplerParameterfv
#define glSamplerParameteriv hogl_glSamplerParameteriv
#define glSamplerParameterIiv hogl_glSamplerParameterIiv
#define glSamplerParameterIuiv hogl_glSamplerParameterIuiv

#define glActiveShaderProgram hogl_glActiveShaderProgram
#define glBindProgramPipeline hogl_glBindProgramPipeline
#define glCreateProgramPipelines hogl_glCreateProgramPipelines
#define glDeleteProgramPipelines hogl_glDeleteProgramPipelines
#define glGenProgramPipelines hogl_glGenProgramPipelines
#define glGetProgramPipelineiv hogl_glGetProgramPipelineiv
#define glGetProgramPipelineInfoLog hogl_glGetProgramPipelineInfoLog
#define glIsProgramPipeline hogl_glIsProgramPipeline
#define glValidateProgramPipeline hogl_glValidateProgramPipeline

#define glDebugMessageCallback hogl_glDebugMessageCallback
#define glDebugMessageControl hogl_glDebugMessageControl
#define glDebugMessageInsert hogl_glDebugMessageInsert
#define glGetDebugMessageLog hogl_glGetDebugMessageLog
#define glGetObjectLabel hogl_glGetObjectLabel
#define glGetObjectPtrLabel hogl_glGetObjectPtrLabel
#define glGetPointerv hogl_glGetPointerv
#define glGetProgramInterfaceiv hogl_glGetProgramInterfaceiv
#define glObjectLabel hogl_glObjectLabel
#define glObjectPtrLabel hogl_glObjectPtrLabel
#define glPopDebugGroup hogl_glPopDebugGroup
#define glPushDebugGroup hogl_glPushDebugGroup

#define glBindBuffer hogl_glBindBuffer
#define glBindBufferBase hogl_glBindBufferBase
#define glBindBufferRange hogl_glBindBufferRange
#define glBindBuffersBase hogl_glBindBuffersBase
#define glBindBuffersRange hogl_glBindBuffersRange
#define glBindVertexBuffer hogl_glBindVertexBuffer
#define glVertexArrayVertexBuffer hogl_glVertexArrayVertexBuffer
#define glBindVertexBuffers hogl_glBindVertexBuffers
#define glVertexArrayVertexBuffers hogl_glVertexArrayVertexBuffers
#define glBufferData hogl_glBufferData
#define glNamedBufferData hogl_glNamedBufferData
#define glBufferStorage hogl_glBufferStorage
#define glNamedBufferStorage hogl_glNamedBufferStorage
#define glBufferSubData hogl_glBufferSubData
#define glNamedBufferSubData hogl_glNamedBufferSubData
#define glClearBufferData hogl_glClearBufferData
#define glClearNamedBufferData hogl_glClearNamedBufferData
#define glClearBufferSubData hogl_glClearBufferSubData
#define glClearNamedBufferSubData hogl_glClearNamedBufferSubData
#define glCopyBufferSubData hogl_glCopyBufferSubData
#define glCopyNamedBufferSubData hogl_glCopyNamedBufferSubData
#define glCreateBuffers hogl_glCreateBuffers
#define glCreateVertexArrays hogl_glCreateVertexArrays
#define glDeleteBuffers hogl_glDeleteBuffers
#define glDisableVertexAttribArray hogl_glDisableVertexAttribArray
#define glDisableVertexArrayAttrib hogl_glDisableVertexArrayAttrib
#define glDrawArrays hogl_glDrawArrays
#define glDrawArraysIndirect hogl_glDrawArraysIndirect
#define glDrawArraysInstanced hogl_glDrawArraysInstanced
#define glDrawArraysInstancedBaseInstance hogl_glDrawArraysInstancedBaseInstance
#define glDrawElements hogl_glDrawElements
#define glDrawElementsBaseVertex hogl_glDrawElementsBaseVertex
#define glDrawElementsIndirect hogl_glDrawElementsIndirect
#define glDrawElementsInstanced hogl_glDrawElementsInstanced
#define glDrawElementsInstancedBaseInstance hogl_glDrawElementsInstancedBaseInstance
#define glDrawElementsInstancedBaseVertex hogl_glDrawElementsInstancedBaseVertex
#define glDrawElementsInstancedBaseVertexBaseInstance hogl_glDrawElementsInstancedBaseVertexBaseInstance
#define glDrawRangeElements hogl_glDrawRangeElements
#define glDrawRangeElementsBaseVertex hogl_glDrawRangeElementsBaseVertex
#define glEnableVertexAttribArray hogl_glEnableVertexAttribArray
#define glEnableVertexArrayAttrib hogl_glEnableVertexArrayAttrib
#define glFlushMappedBufferRange hogl_glFlushMappedBufferRange
#define glFlushMappedNamedBufferRange hogl_glFlushMappedNamedBufferRange
#define glGenBuffers hogl_glGenBuffers
#define glGetBufferParameteriv hogl_glGetBufferParameteriv
#define glGetBufferParameteri64v hogl_glGetBufferParameteri64v
#define glGetNamedBufferParameteriv hogl_glGetNamedBufferParameteriv
#define glGetNamedBufferParameteri64v hogl_glGetNamedBufferParameteri64v
#define glGetBufferPointerv hogl_glGetBufferPointerv
#define glGetNamedBufferPointerv hogl_glGetNamedBufferPointerv
#define glGetBufferSubData hogl_glGetBufferSubData
#define glGetNamedBufferSubData hogl_glGetNamedBufferSubData
#define glGetVertexArrayIndexediv hogl_glGetVertexArrayIndexediv
#define glGetVertexArrayIndexed64iv hogl_glGetVertexArrayIndexed64iv
#define glGetVertexArrayiv hogl_glGetVertexArrayiv
#define glGetVertexAttribdv hogl_glGetVertexAttribdv
#define glGetVertexAttribfv hogl_glGetVertexAttribfv
#define glGetVertexAttribiv hogl_glGetVertexAttribiv
#define glGetVertexAttribIiv hogl_glGetVertexAttribIiv
#define glGetVertexAttribIuiv hogl_glGetVertexAttribIuiv
#define glGetVertexAttribLdv hogl_glGetVertexAttribLdv
#define glGetVertexAttribPointerv hogl_glGetVertexAttribPointerv
#define glInvalidateBufferData hogl_glInvalidateBufferData
#define glInvalidateBufferSubData hogl_glInvalidateBufferSubData
#define glIsBuffer hogl_glIsBuffer
#define glMapBuffer hogl_glMapBuffer
#define glMapNamedBuffer hogl_glMapNamedBuffer
#define glMapBufferRange hogl_glMapBufferRange
#define glMapNamedBufferRange hogl_glMapNamedBufferRange
#define glMultiDrawArrays hogl_glMultiDrawArrays
#define glMultiDrawArraysIndirect hogl_glMultiDrawArraysIndirect
#define glMultiDrawElements hogl_glMultiDrawElements
#define glMultiDrawElementsBaseVertex hogl_glMultiDrawElementsBaseVertex
#define glMultiDrawElementsIndirect hogl_glMultiDrawElementsIndirect
#define glPatchParameteri hogl_glPatchParameteri
#define glPatchParameterfv hogl_glPatchParameterfv
#define glPrimitiveRestartIndex hogl_glPrimitiveRestartIndex
#define glProvokingVertex hogl_glProvokingVertex
#define glUnmapBuffer hogl_glUnmapBuffer
#define glUnmapNamedBuffer hogl_glUnmapNamedBuffer
#define glVertexArrayElementBuffer hogl_glVertexArrayElementBuffer
#define glVertexAttrib1f hogl_glVertexAttrib1f
#define glVertexAttrib1s hogl_glVertexAttrib1s
#define glVertexAttrib1d hogl_glVertexAttrib1d
#define glVertexAttribI1i hogl_glVertexAttribI1i
#define glVertexAttribI1ui hogl_glVertexAttribI1ui
#define glVertexAttrib2f hogl_glVertexAttrib2f
#define glVertexAttrib2s hogl_glVertexAttrib2s
#define glVertexAttrib2d hogl_glVertexAttrib2d
#define glVertexAttribI2i hogl_glVertexAttribI2i
#define glVertexAttribI2ui hogl_glVertexAttribI2ui
#define glVertexAttrib3f hogl_glVertexAttrib3f
#define glVertexAttrib3s hogl_glVertexAttrib3s
#define glVertexAttrib3d hogl_glVertexAttrib3d
#define glVertexAttribI3i hogl_glVertexAttribI3i
#define glVertexAttribI3ui hogl_glVertexAttribI3ui
#define glVertexAttrib4f hogl_glVertexAttrib4f
#define glVertexAttrib4s hogl_glVertexAttrib4s
#define glVertexAttrib4d hogl_glVertexAttrib4d
#define glVertexAttrib4Nub hogl_glVertexAttrib4Nub
#define glVertexAttribI4i hogl_glVertexAttribI4i
#define glVertexAttribI4ui hogl_glVertexAttribI4ui
#define glVertexAttribL1d hogl_glVertexAttribL1d
#define glVertexAttribL2d hogl_glVertexAttribL2d
#define glVertexAttribL3d hogl_glVertexAttribL3d
#define glVertexAttribL4d hogl_glVertexAttribL4d
#define glVertexAttrib1fv hogl_glVertexAttrib1fv
#define glVertexAttrib1sv hogl_glVertexAttrib1sv
#define glVertexAttrib1dv hogl_glVertexAttrib1dv
#define glVertexAttribI1iv hogl_glVertexAttribI1iv
#define glVertexAttribI1uiv hogl_glVertexAttribI1uiv
#define glVertexAttrib2fv hogl_glVertexAttrib2fv
#define glVertexAttrib2sv hogl_glVertexAttrib2sv
#define glVertexAttrib2dv hogl_glVertexAttrib2dv
#define glVertexAttribI2iv hogl_glVertexAttribI2iv
#define glVertexAttribI2uiv hogl_glVertexAttribI2uiv
#define glVertexAttrib3fv hogl_glVertexAttrib3fv
#define glVertexAttrib3sv hogl_glVertexAttrib3sv
#define glVertexAttrib3dv hogl_glVertexAttrib3dv
#define glVertexAttribI3iv hogl_glVertexAttribI3iv
#define glVertexAttribI3uiv hogl_glVertexAttribI3uiv
#define glVertexAttrib4fv hogl_glVertexAttrib4fv
#define glVertexAttrib4sv hogl_glVertexAttrib4sv
#define glVertexAttrib4dv hogl_glVertexAttrib4dv
#define glVertexAttrib4iv hogl_glVertexAttrib4iv
#define glVertexAttrib4bv hogl_glVertexAttrib4bv
#define glVertexAttrib4ubv hogl_glVertexAttrib4ubv
#define glVertexAttrib4usv hogl_glVertexAttrib4usv
#define glVertexAttrib4uiv hogl_glVertexAttrib4uiv
#define glVertexAttrib4Nbv hogl_glVertexAttrib4Nbv
#define glVertexAttrib4Nsv hogl_glVertexAttrib4Nsv
#define glVertexAttrib4Niv hogl_glVertexAttrib4Niv
#define glVertexAttrib4Nubv hogl_glVertexAttrib4Nubv
#define glVertexAttrib4Nusv hogl_glVertexAttrib4Nusv
#define glVertexAttrib4Nuiv hogl_glVertexAttrib4Nuiv
#define glVertexAttribI4bv hogl_glVertexAttribI4bv
#define glVertexAttribI4ubv hogl_glVertexAttribI4ubv
#define glVertexAttribI4sv hogl_glVertexAttribI4sv
#define glVertexAttribI4usv hogl_glVertexAttribI4usv
#define glVertexAttribI4iv hogl_glVertexAttribI4iv
#define glVertexAttribI4uiv hogl_glVertexAttribI4uiv
#define glVertexAttribL1dv hogl_glVertexAttribL1dv
#define glVertexAttribL2dv hogl_glVertexAttribL2dv
#define glVertexAttribL3dv hogl_glVertexAttribL3dv
#define glVertexAttribL4dv hogl_glVertexAttribL4dv
#define glVertexAttribP1ui hogl_glVertexAttribP1ui
#define glVertexAttribP2ui hogl_glVertexAttribP2ui
#define glVertexAttribP3ui hogl_glVertexAttribP3ui
#define glVertexAttribP4ui hogl_glVertexAttribP4ui
#define glVertexAttribBinding hogl_glVertexAttribBinding
#define glVertexArrayAttribBinding hogl_glVertexArrayAttribBinding
#define glVertexAttribDivisor hogl_glVertexAttribDivisor
#define glVertexAttribFormat hogl_glVertexAttribFormat
#define glVertexAttribIFormat hogl_glVertexAttribIFormat
#define glVertexAttribLFormat hogl_glVertexAttribLFormat
#define glVertexArrayAttribFormat hogl_glVertexArrayAttribFormat
#define glVertexArrayAttribIFormat hogl_glVertexArrayAttribIFormat
#define glVertexArrayAttribLFormat hogl_glVertexArrayAttribLFormat
#define glVertexAttribPointer hogl_glVertexAttribPointer
#define glVertexAttribIPointer hogl_glVertexAttribIPointer
#define glVertexAttribLPointer hogl_glVertexAttribLPointer
#define glVertexBindingDivisor hogl_glVertexBindingDivisor
#define glVertexArrayBindingDivisor hogl_glVertexArrayBindingDivisor

#if defined(_WIN32) || defined(_WIN64)

extern int hogl_init_opengl(Window_Info* window_info, int major, int minor) {
	int PixelFormat;
	window_info->device_context = GetDC(window_info->window_handle);

	PIXELFORMATDESCRIPTOR pfd;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pfd.cDepthBits = 32;
	pfd.cColorBits = 24;
	pfd.iPixelType = PFD_TYPE_RGBA;

	PixelFormat = ChoosePixelFormat(window_info->device_context, &pfd);
	if (!SetPixelFormat(window_info->device_context, PixelFormat, &pfd)) {
		return PIXEL_FORMAT_ERROR;
	}

	HGLRC temp_context = wglCreateContext(window_info->device_context);
	if (!temp_context) {
		return BASIC_GL_CONTEXT_CREATE_ERROR;
	}
	if (!wglMakeCurrent(window_info->device_context, temp_context)) {
		return BASIC_GL_CONTEXT_SET_ERROR;
	}
	if (major <= 3) {
		if ((major == 3 && minor <= 1) || major < 3)
			return 0;
	}

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB, 0,
	};

	HGLRC(WINAPI* wglCreateContextAttribsARB)(HDC hDC, HGLRC hShareContext, int *attribList);
	wglCreateContextAttribsARB = (HGLRC(WINAPI*)(HDC, HGLRC, int *))wglGetProcAddress("wglCreateContextAttribsARB");
	if (!wglCreateContextAttribsARB)
		return -1;
	window_info->rendering_context = wglCreateContextAttribsARB(window_info->device_context, 0, attribs);
	if (!window_info->rendering_context) {
		return 0;
	}
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(temp_context);
	wglMakeCurrent(window_info->device_context, window_info->rendering_context);

	return 0;
}
#elif defined(__linux__)
#endif

#endif	// HOGL_IMPLEMENT

#ifdef __cplusplus
#undef extern
#else
#undef true
#undef false
#endif
