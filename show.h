#pragma once

// ------------------------------------------
// ------------ External API ----------------
// ------------------------------------------
enum {
    SHOW_OK = 0,
    SHOW_ERROR_INVALID_WINDOW_DIMENSIONS = (1 << 0),
    SHOW_ERROR_FAILED_TO_OPEN_DISPLAY    = (1 << 1),
    SHOW_ERROR_INVALID_GLX_VERSION       = (1 << 2),
    SHOW_ERROR_FAILED_TO_GET_FBC         = (1 << 3),
    SHOW_ERROR_CREATING_WINDOW           = (1 << 4),
    SHOW_ERROR_CREATING_OPENGL_CONTEXT   = (1 << 5),
    SHOW_ERROR_UNINITIALIZED_SHOW        = (1 << 6),

    SHOW_WARNING_FAILED_CREATING_OPENGL3_CONTEXT  = (1 << 10),
};

// Uses the width and height pointers to create the initial window
// and maintains the pointer updated with the current window dimension.
// Returns a memory block defined by the dimensions assuming a 32 bit
// 4 channels ABGR encoded image. This memory can be used to draw.
void*       show_get_memory(int* width, int* height, unsigned int* error);

// Necessary to be called inside the drawing loop, this function
// draws a frame defined by the memory returned by show_get_memory
double      show_render(unsigned int* error);

// Returns a string version of the first error flag set
const char* show_error_string(unsigned int error);


// ------------------------------------------
// ------------- Internals ------------------
// ------------------------------------------

#if defined(__linux__)

#include "show.h"
#include <X11/X.h> 		// sudo apt-get install libx11-dev
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <GL/gl.h>		// sudo apt-get install mesa-common-dev libgl1-mesa-dev
#include <GL/glx.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <float.h>

typedef struct {
	unsigned int id;
	int width;
	int height;
} Show_Texture;

#define LOAD_GL_PROC(X) \
hogl_##X = (X##_proctype*)glXGetProcAddress((const GLubyte*)#X); \
if(!(hogl_##X)) hogl_##X = 0 

#define INSTANTIATE_GLCALL(R, X, A) \
typedef R X##_proctype A;	\
X##_proctype* hogl_##X = 0

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

INSTANTIATE_GLCALL(void, glAttachShader, (GLuint program, GLuint shader));
INSTANTIATE_GLCALL(void, glBindAttribLocation, (GLuint program, GLuint index, const GLchar* name));
INSTANTIATE_GLCALL(void, glCompileShader, (GLuint shader));
INSTANTIATE_GLCALL(GLuint, glCreateProgram, ());
INSTANTIATE_GLCALL(GLuint, glCreateShader, (GLenum shaderType));
INSTANTIATE_GLCALL(GLuint, glCreateShaderProgramv, (GLenum type, GLsizei count, const char** strings));
INSTANTIATE_GLCALL(void, glDeleteProgram, (GLuint program));
INSTANTIATE_GLCALL(void, glDeleteShader, (GLuint shader));
INSTANTIATE_GLCALL(void, glDetachShader, (GLuint program, GLuint shader));
INSTANTIATE_GLCALL(void, glShaderSource, (GLuint shader, GLsizei count, const GLchar** string_in, const GLint* length));
INSTANTIATE_GLCALL(void, glGetShaderiv, (GLuint shader, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetShaderInfoLog, (GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog));
INSTANTIATE_GLCALL(void, glLinkProgram, (GLuint program));
INSTANTIATE_GLCALL(void, glGetProgramiv, (GLuint program, GLenum pname, GLint* params));
INSTANTIATE_GLCALL(void, glGetProgramInfoLog, (GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog));
INSTANTIATE_GLCALL(void, glValidateProgram, (GLuint program));
INSTANTIATE_GLCALL(void, glUseProgram, (GLuint program));
INSTANTIATE_GLCALL(GLint, glGetUniformLocation, (GLuint program, const GLchar* name));
INSTANTIATE_GLCALL(void, glGenVertexArrays, (GLsizei n, GLuint* arrays));
INSTANTIATE_GLCALL(void, glBindVertexArray, (GLuint array));
INSTANTIATE_GLCALL(void, glGenBuffers, (GLsizei n, GLuint* buffers));
INSTANTIATE_GLCALL(void, glBindBuffer, (GLenum target, GLuint buffer));
INSTANTIATE_GLCALL(void, glBufferData, (GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage));
INSTANTIATE_GLCALL(void, glEnableVertexAttribArray, (GLuint index));
INSTANTIATE_GLCALL(void, glVertexAttribPointer, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer));
INSTANTIATE_GLCALL(void, glUniform1i, (GLint location, GLint v0));
INSTANTIATE_GLCALL(void, glUniformMatrix4fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value));
INSTANTIATE_GLCALL(void*, glMapBuffer, (GLenum target, GLenum access));
INSTANTIATE_GLCALL(GLboolean, glUnmapBuffer, (GLenum target));

#define glAttachShader hogl_glAttachShader
#define glBindAttribLocation hogl_glBindAttribLocation
#define glCompileShader hogl_glCompileShader
#define glCreateProgram hogl_glCreateProgram
#define glCreateShader hogl_glCreateShader
#define glCreateShaderProgramv hogl_glCreateShaderProgramv
#define glDeleteProgram hogl_glDeleteProgram
#define glDeleteShader hogl_glDeleteShader
#define glDetachShader hogl_glDetachShader
#define glShaderSource hogl_glShaderSource
#define glGetShaderiv hogl_glGetShaderiv
#define glGetShaderInfoLog hogl_glGetShaderInfoLog
#define glLinkProgram hogl_glLinkProgram
#define glGetProgramiv hogl_glGetProgramiv
#define glGetProgramInfoLog hogl_glGetProgramInfoLog
#define glValidateProgram hogl_glValidateProgram
#define glUseProgram hogl_glUseProgram
#define glGetUniformLocation hogl_glGetUniformLocation
#define glGenVertexArrays hogl_glGenVertexArrays
#define glBindVertexArray hogl_glBindVertexArray
#define glGenBuffers hogl_glGenBuffers
#define glBindBuffer hogl_glBindBuffer
#define glBufferData hogl_glBufferData
#define glEnableVertexAttribArray hogl_glEnableVertexAttribArray
#define glVertexAttribPointer hogl_glVertexAttribPointer
#define glUniform1i hogl_glUniform1i
#define glUniformMatrix4fv hogl_glUniformMatrix4fv
#define glMapBuffer hogl_glMapBuffer
#define glUnmapBuffer hogl_glUnmapBuffer

static void
show_init_opengl() {
    LOAD_GL_PROC(glAttachShader);
	LOAD_GL_PROC(glBindAttribLocation);
	LOAD_GL_PROC(glCompileShader);
	LOAD_GL_PROC(glCreateProgram);
	LOAD_GL_PROC(glCreateShader);
	LOAD_GL_PROC(glCreateShaderProgramv);
	LOAD_GL_PROC(glDeleteProgram);
	LOAD_GL_PROC(glDeleteShader);
    LOAD_GL_PROC(glDetachShader);
    LOAD_GL_PROC(glShaderSource);
    LOAD_GL_PROC(glGetShaderiv);
    LOAD_GL_PROC(glGetShaderInfoLog);
    LOAD_GL_PROC(glLinkProgram);
    LOAD_GL_PROC(glGetProgramiv);
    LOAD_GL_PROC(glGetProgramInfoLog);
    LOAD_GL_PROC(glValidateProgram);
    LOAD_GL_PROC(glUseProgram);
    LOAD_GL_PROC(glGetUniformLocation);
    LOAD_GL_PROC(glGenVertexArrays);
    LOAD_GL_PROC(glBindVertexArray);
    LOAD_GL_PROC(glGenBuffers);
    LOAD_GL_PROC(glBindBuffer);
    LOAD_GL_PROC(glBufferData);
    LOAD_GL_PROC(glEnableVertexAttribArray);
    LOAD_GL_PROC(glVertexAttribPointer);
    LOAD_GL_PROC(glUniform1i);
    LOAD_GL_PROC(glUniformMatrix4fv);
    LOAD_GL_PROC(glMapBuffer);
    LOAD_GL_PROC(glUnmapBuffer);
}

typedef struct {
    float x, y, z, w;
} show_vec4;
typedef struct {
    float x, y, z;
} show_vec3;
typedef struct {
    float x, y;
} show_vec2;

typedef struct {
	show_vec3   position;
	float  texture_alpha;
	show_vec2   texture_coordinate;
	show_vec4   color;
	float  mask;
	show_vec4   clipping_box;
} Show_Vertex_3D;

typedef struct {
	Show_Vertex_3D vertices[4];
} Show_Quad_2D;

typedef struct {
	float data[4][4];
} show_mat4;

static show_mat4 
show_gm_mat4_ortho(float left, float right, float bottom, float top)
{
	float f = 1.0f;
	float n = -1.0f;
	show_mat4 result;
	result.data[0][0] = 2.0f / (right - left);	result.data[0][1] = 0;						result.data[0][2] = 0;	result.data[0][3] = -(right + left) / (right - left);
	result.data[1][0] = 0;						result.data[1][1] = 2.0f / (top - bottom);	result.data[1][2] = 0;	result.data[1][3] = -(top + bottom) / (top - bottom);
	result.data[2][0] = 0;						result.data[2][1] = 0;						result.data[2][2] = 1;	result.data[2][3] = 0;
	result.data[3][0] = 0;						result.data[3][1] = 0;						result.data[3][2] = 0;	result.data[3][3] = 1;

	return result;
}

unsigned int 
shader_load_from_buffer(const char* vert_shader, const char* frag_shader, int vert_length, int frag_length)
{
	GLuint vs_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs_id = glCreateShader(GL_FRAGMENT_SHADER);

	GLint compile_status;

	const GLchar* p_v[1] = { vert_shader };
	glShaderSource(vs_id, 1, p_v, &vert_length);

	const GLchar* p_f[1] = { frag_shader };
	glShaderSource(fs_id, 1, p_f, &frag_length);

	glCompileShader(vs_id);
	glGetShaderiv(vs_id, GL_COMPILE_STATUS, &compile_status);
	if (!compile_status) {
		char error_buffer[512] = { 0 };
		glGetShaderInfoLog(vs_id, sizeof(error_buffer), NULL, error_buffer);
		printf("shader_load: Error compiling vertex shader: %s", error_buffer);
		return -1;
	}

	glCompileShader(fs_id);
	glGetShaderiv(fs_id, GL_COMPILE_STATUS, &compile_status);
	if (!compile_status) {
		char error_buffer[512] = { 0 };
		glGetShaderInfoLog(fs_id, sizeof(error_buffer) - 1, NULL, error_buffer);
		printf("shader_load: Error compiling fragment shader: %s", error_buffer);
		return -1;
	}

	GLuint shader_id = glCreateProgram();
	glAttachShader(shader_id, vs_id);
	glAttachShader(shader_id, fs_id);
	glDeleteShader(vs_id);
	glDeleteShader(fs_id);
	glLinkProgram(shader_id);

	glGetProgramiv(shader_id, GL_LINK_STATUS, &compile_status);
	if (compile_status == 0) {
		GLchar error_buffer[512] = { 0 };
		glGetProgramInfoLog(shader_id, sizeof(error_buffer) - 1, NULL, error_buffer);
		printf("shader_load: Error linking program: %s", error_buffer);
		return -1;
	}

	glValidateProgram(shader_id);
	return shader_id;
}

static unsigned int show_shader;
static unsigned int show_shader_projection_matrix_location;
static unsigned int show_shader_translation_matrix_location;
static unsigned int show_shader_text_location;
static unsigned int show_quad_vao;
static unsigned int show_quad_vbo;
static unsigned int show_quad_ebo;

static char show_vert_shader[] = "#version 330 core\n"
"layout(location = 0) in vec3 v_vertex;\n"
"layout(location = 1) in vec2 v_tcoords;\n"
"layout(location = 2) in vec4 v_color;\n"
"layout(location = 3) in float v_texture_alpha;\n"
"layout(location = 4) in float v_is_text;\n"
"layout(location = 5) in vec4 v_clipping;\n"
"\n"
"out vec2 o_tcoords;\n"
"out vec4 o_color;\n"
"out float o_texture_alpha;\n"
"out float o_is_text;\n"
"out vec4 clipping;\n"
"out vec2 position;\n"
"\n"
"uniform mat4 u_projection = mat4(1.0);\n"
"uniform mat4 u_translation = mat4(1.0);\n"
"\n"
"void main(){\n"
"	gl_Position = u_projection * u_translation * vec4(v_vertex.xy, 0.0, 1.0);\n"
"	position = v_vertex.xy;\n"
"	o_tcoords = v_tcoords;\n"
"	o_color = v_color;\n"
"	o_texture_alpha = v_texture_alpha;\n"
"	o_is_text = v_is_text;\n"
"\n"
"	clipping = v_clipping;\n"
"}";
static char show_frag_shader[] = "#version 330 core\n"
"in vec2 o_tcoords;\n"
"in vec4 o_color;\n"
"in float o_texture_alpha;\n"
"in float o_is_text;\n"
"\n"
"in vec2 position;\n"
"in vec4 clipping;\n"
"out vec4 color;\n"
"uniform sampler2D uniform_texture;\n"
"\n"
"void main(){\n"
"   if(position.x < clipping.x || position.x > clipping.x + clipping.z) {\n"
"		discard;\n"
"	}\n"
"	if(position.y < clipping.y || position.y > clipping.y + clipping.w) {\n"
"		discard;\n"
"	}\n"
"   color = texture(uniform_texture, o_tcoords);\n"
"}";

static void 
show_init_renderer()
{
	// Shader setup
	show_shader = shader_load_from_buffer(show_vert_shader, show_frag_shader, sizeof(show_vert_shader), sizeof(show_frag_shader));
	glUseProgram(show_shader);

	show_shader_projection_matrix_location = glGetUniformLocation(show_shader, "u_projection");
	show_shader_translation_matrix_location = glGetUniformLocation(show_shader, "u_translation");

	// Buffers setup for rendering
	glGenVertexArrays(1, &show_quad_vao);
	glBindVertexArray(show_quad_vao);
	glGenBuffers(1, &show_quad_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, show_quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Show_Quad_2D), 0, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Show_Vertex_3D), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Show_Vertex_3D), &((Show_Vertex_3D*)0)->texture_coordinate);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Show_Vertex_3D), &((Show_Vertex_3D*)0)->color);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Show_Vertex_3D), &((Show_Vertex_3D*)0)->texture_alpha);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Show_Vertex_3D), &((Show_Vertex_3D*)0)->mask);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Show_Vertex_3D), &((Show_Vertex_3D*)0)->clipping_box);

    unsigned int indices[] = {
        0, 1, 2, 2, 1, 3
    };

	glGenBuffers(1, &show_quad_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, show_quad_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}

typedef show_vec4 Show_Clipping_Rect;
static Show_Quad_2D
show_quad_new_clipped(show_vec2 position, float width, float height, show_vec4 color, Show_Clipping_Rect rect) {
	Show_Quad_2D q =
	{
		(Show_Vertex_3D) { (show_vec3) { position.x, position.y, 0 }, 1.0f, (show_vec2) { 0.0f, 0.0f }, color, 1.0f, rect },
		(Show_Vertex_3D) { (show_vec3) { position.x + width, position.y, 0}, 1.0f, (show_vec2) { 1.0f, 0.0f }, color, 1.0f, rect },
		(Show_Vertex_3D) { (show_vec3) { position.x, position.y + height, 0 }, 1.0f, (show_vec2) { 0.0f, 1.0f }, color, 1.0f, rect },
		(Show_Vertex_3D) { (show_vec3) { position.x + width, position.y + height, 0 }, 1.0f, (show_vec2) { 1.0f, 1.0f }, color, 1.0f, rect }
	};
	return q;
}

static void 
show_flush_quad_render(Show_Quad_2D* quad, int width, int height, unsigned int texture_id) {
	glUseProgram(show_shader);
    glBindVertexArray(show_quad_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, show_quad_ebo);
    glBindBuffer(GL_ARRAY_BUFFER, show_quad_vbo);

    glBindTexture(GL_TEXTURE_2D, texture_id);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(show_shader, "uniform_textture"), 0);

    {
        Show_Quad_2D* quad_mem = (Show_Quad_2D*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(quad_mem, quad, sizeof(Show_Quad_2D));
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
	{
		show_mat4 projection = show_gm_mat4_ortho(0, (float)width, 0, (float)height);
		glUniformMatrix4fv(show_shader_projection_matrix_location, 1, GL_TRUE, (GLfloat*)projection.data);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}

static void show_texture_update(Show_Texture* texture, int width, int height, char* data);

static Show_Texture
show_texture_create(int width, int height, char* data)
{
    Show_Texture result = {0};
	unsigned int texture_id = 0;

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

    result.width = width;
    result.height = height;
    result.id = texture_id;

    return result;
}

static void
show_texture_delete(unsigned int id) {
    glDeleteTextures(1, &id);
}

static void
show_texture_update(Show_Texture* texture, int width, int height, char* data) {
    glBindTexture(GL_TEXTURE_2D, texture->id);
    
    if(texture->width != width || texture->height != height) {
        if(width > 0 && height > 0) {
            // Allocate a new texture
            show_texture_delete(texture->id);
            Show_Texture newtexture = show_texture_create(width, height, data);
            texture->id = newtexture.id;
            texture->width = width;
            texture->height = height;
        }
    } else if(width > 0 && height > 0) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        texture->width = width;
        texture->height = height;
    }

}

static void show_render_memory();

static double 
show_os_time_us()
{
	clockid_t clockid;
	struct timespec t_spec;
	int start = clock_gettime(CLOCK_MONOTONIC_RAW, &t_spec);
	double res = t_spec.tv_nsec + 1000000000 * t_spec.tv_sec;
	return (double)res / 1000.0;
}

#define OPENGL_MAJOR_VERSION 3
#define OPENGL_MINOR_VERSION 3
#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

static long show_x11_allmasks = KeyPressMask
		| KeyReleaseMask
		| ButtonPressMask
		| ButtonReleaseMask
		| EnterWindowMask
		| LeaveWindowMask
		| PointerMotionMask
		| PointerMotionHintMask
		| Button1MotionMask
		| Button2MotionMask
		| Button3MotionMask
		| Button4MotionMask
		| Button5MotionMask
		| ButtonMotionMask
		| KeymapStateMask
		| ExposureMask
		| VisibilityChangeMask
		| StructureNotifyMask
		//| ResizeRedirectMask
		| SubstructureNotifyMask
		| SubstructureRedirectMask
		| FocusChangeMask
		| PropertyChangeMask
		| ColormapChangeMask
		| OwnerGrabButtonMask;
;

static void
show_print_event(int ev) {
	switch(ev) {
		case KeyPress:				printf("KeyPress"); break;
		case KeyRelease:			printf("KeyRelease"); break;
		case ButtonPress:			printf("ButtonPress"); break;
		case ButtonRelease:			printf("ButtonRelease"); break;
		case MotionNotify:			printf("MotionNotify"); break;
		case EnterNotify:			printf("EnterNotify"); break;
		case LeaveNotify:			printf("LeaveNotify"); break;
		case FocusIn:				printf("FocusIn"); break;
		case FocusOut:				printf("FocusOut"); break;
		case KeymapNotify:			printf("KeymapNotify"); break;
		case Expose:				printf("Expose"); break;
		case GraphicsExpose:		printf("GraphicsExpose"); break;
		case NoExpose:				printf("NoExpose"); break;
		case VisibilityNotify:		printf("VisibilityNotify"); break;
		case CreateNotify:			printf("CreateNotify"); break;
		case DestroyNotify:			printf("DestroyNotify"); break;
		case UnmapNotify:			printf("UnmapNotify"); break;
		case MapNotify:				printf("MapNotify"); break;
		case MapRequest:			printf("MapRequest"); break;
		case ReparentNotify:		printf("ReparentNotify"); break;
		case ConfigureNotify:		printf("ConfigureNotify"); break;
		case ConfigureRequest:		printf("ConfigureRequest"); break;
		case GravityNotify:			printf("GravityNotify"); break;
		case ResizeRequest:			printf("ResizeRequest"); break;
		case CirculateNotify:		printf("CirculateNotify"); break;
		case CirculateRequest:		printf("CirculateRequest"); break;
		case PropertyNotify:		printf("PropertyNotify"); break;
		case SelectionClear:		printf("SelectionClear"); break;
		case SelectionRequest:		printf("SelectionRequest"); break;
		case SelectionNotify:		printf("SelectionNotify"); break;
		case ColormapNotify:		printf("ColormapNotify"); break;
		case ClientMessage:			printf("ClientMessage"); break;
		case MappingNotify:			printf("MappingNotify"); break;
		case GenericEvent:			printf("GenericEvent"); break;
		case LASTEvent:				printf("LASTEvent"); break;
	}
	printf("\n");
}

static int 
isExtensionSupported(const char *extList, const char *extension)
{
	const char *start;
	const char *where, *terminator;

	/* Extension names should not have spaces. */
	where = strchr(extension, ' ');
	if (where || *extension == '\0')
		return 0;

	/* It takes a bit of care to be fool-proof about parsing the
	   OpenGL extensions string. Don't be fooled by sub-strings,
	   etc. */
	for (start=extList;;) {
		where = strstr(start, extension);

		if (!where)
			break;

		terminator = where + strlen(extension);

		if ( where == start || *(where - 1) == ' ' )
			if ( *terminator == ' ' || *terminator == '\0' )
				return 1;

		start = terminator;
	}

	return 0;
}

static int ctxErrorOccurred;
static int ctxErrorHandler(Display *dpy, XErrorEvent *ev)
{
	ctxErrorOccurred = 1;
	return 0;
}

typedef struct {
	// Window
    Display*   display;
    GLXContext context;
    int        window;

	// Global info
	int screen_width;
	int screen_height;

	// 
	void* memory;
	int*  width;
	int*  height;
	Show_Texture texture;

	// Misc
	int    fps;
	double frame_time_us;
} Show_Window_Info;

static Show_Window_Info info;

static void
show_update_window_info() {
	int x, y;
	Window root_window;
	unsigned int w, h, bw, dr;
	XGetGeometry(info.display, info.window, &root_window, &x, &y, &w, &h, &bw, &dr);
	*info.width = (int)w;
	*info.height = (int)h;
}

static int
show_create_window(int width, int height, Show_Window_Info* out_info, unsigned int* error) {
	char name[] = "sHOw";
	Display* display = XOpenDisplay(NULL);
	if (!display) {
		if(error) *error |= SHOW_ERROR_FAILED_TO_OPEN_DISPLAY;
		return -1;
	}

	// Get a matching FB config
	static int visual_attribs[] =
	{
		GLX_X_RENDERABLE    , True,
		GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
		GLX_RENDER_TYPE     , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		GLX_RED_SIZE        , 8,
		GLX_GREEN_SIZE      , 8,
		GLX_BLUE_SIZE       , 8,
		GLX_ALPHA_SIZE      , 8,
		GLX_DEPTH_SIZE      , 24,
		GLX_STENCIL_SIZE    , 8,
		GLX_DOUBLEBUFFER    , True,
		None
	};

	int glx_major, glx_minor;

	// FBConfigs were added in GLX version 1.3.
	if (!glXQueryVersion(display, &glx_major, &glx_minor) || ( (glx_major == 1) && (glx_minor < 3)) || (glx_major < 1)) {
		if(error) *error |= SHOW_ERROR_INVALID_GLX_VERSION;
		return -1;
	}

	int fbcount;
	GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
	if (!fbc) {
		if(error) *error |= SHOW_ERROR_FAILED_TO_GET_FBC;
		return -1;
	}
	// Pick the FB config/visual with the most samples per pixel
	int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

	int i;
	for (i=0; i < fbcount; ++i)
	{
		XVisualInfo* vi = glXGetVisualFromFBConfig(display, fbc[i]);
		if (vi)
		{
			int samp_buf, samples;
			glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
			glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES       , &samples);

			if (best_fbc < 0 || samp_buf && samples > best_num_samp)
				best_fbc = i, best_num_samp = samples;
			if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
				worst_fbc = i, worst_num_samp = samples;
		}
		XFree(vi);
	}

	GLXFBConfig bestFbc = fbc[best_fbc];

	// Be sure to free the FBConfig list allocated by glXChooseFBConfig()
	XFree(fbc);

	// Get a visual
	XVisualInfo *vi = glXGetVisualFromFBConfig(display, bestFbc);

	XSetWindowAttributes swa;
	swa.colormap = XCreateColormap(display,
			RootWindow(display, vi->screen), 
			vi->visual, AllocNone);
	swa.background_pixmap = None;
	swa.border_pixel      = 0;
	swa.event_mask        = StructureNotifyMask;

	int win = XCreateWindow(display, RootWindow(display, vi->screen), 0, 0, width, height, 0, vi->depth, InputOutput, vi->visual, 
			CWBorderPixel|CWColormap|CWEventMask, &swa);
	
	// This will signal the window to grab all events and ignore none.
	XSelectInput(display, win, show_x11_allmasks);
	if (!win) {
		if(error) *error |= SHOW_ERROR_CREATING_WINDOW;
		return -1;
	}

	XClassHint* class_hint = XAllocClassHint();
	char class_name[] = "show_class";
	char class_class[] = "show_class_class";
	class_hint->res_name = (char*)class_name;
	class_hint->res_class = (char*)class_class;
	XSetClassHint(display, win, class_hint);

	// Done with the visual info data
	XFree(vi);

	XStoreName(display, win, name);
	XMapWindow(display, win);

	// Get the default screen's GLX extension list
	const char *glxExts = glXQueryExtensionsString(display, DefaultScreen(display));
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte *) "glXCreateContextAttribsARB");

    GLXContext ctx = 0;
	// Check for the GLX_ARB_create_context extension string and the function.
	// If either is not present, use GLX 1.3 context creation method.
	if (!isExtensionSupported(glxExts, "GLX_ARB_create_context" ) || !glXCreateContextAttribsARB) {
		ctx = glXCreateNewContext(display, bestFbc, GLX_RGBA_TYPE, 0, True);
	}
	// If it does, try to get a GL 3.0 context!
	else
	{
		int context_attribs[] = 
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, OPENGL_MAJOR_VERSION,
			GLX_CONTEXT_MINOR_VERSION_ARB, OPENGL_MINOR_VERSION,
			GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			None
		};
		ctx = glXCreateContextAttribsARB(display, bestFbc, 0, True, context_attribs);

		// Sync to ensure any errors generated are processed.
		XSync(display, False);
		if (!ctxErrorOccurred && ctx) {
            // Created gl 3.3 context
        } else {
			context_attribs[1] = 1;
			context_attribs[3] = 0;
			ctxErrorOccurred = 0;

			if(error) *error |= SHOW_WARNING_FAILED_CREATING_OPENGL3_CONTEXT;
			ctx = glXCreateContextAttribsARB(display, bestFbc, 0, True, context_attribs);
		}
	}

	// Sync to ensure any errors generated are processed.
	XSync(display, False);

	if (ctxErrorOccurred || !ctx) {
		if(error) *error |= SHOW_ERROR_CREATING_OPENGL_CONTEXT;
		return -1;
	}

	glXMakeCurrent(display, win, ctx);

	Screen*  s = DefaultScreenOfDisplay(display);
	XAllowEvents(display, AsyncBoth, CurrentTime);

    out_info->display = display;
    out_info->context = ctx;
    out_info->window = win;
	out_info->screen_width = WidthOfScreen(s);
	out_info->screen_height = HeightOfScreen(s);	
}

static double show_total_time = 0.0;
static int    show_frames = 0;

static int show_key_states[1024];
static int show_key_event[1024];

double
show_render(unsigned int* error) {
    if(!info.memory) {
        if(error) *error |= SHOW_ERROR_UNINITIALIZED_SHOW;
        return 0.0;
    }
	XEvent xev;

    double start_time = show_os_time_us();

	// On this website it lists all the event masks
	// https://tronche.com/gui/x/xlib/events/mask.html
	// https://tronche.com/gui/x/xlib/events/processing-overview.html
	memset(show_key_event, 0, sizeof(show_key_event));

	while(XCheckWindowEvent(info.display, info.window, show_x11_allmasks, &xev))
	{
		// /usr/include/X11/X.h line 181 has #define of all types of events
		if(xev.type == Expose) {
			XExposeEvent* e = (XExposeEvent*)&xev;
			*info.width = e->width;
			*info.height = e->height;
		}
		if(xev.type == KeyPress) {
			XKeyPressedEvent* e = (XKeyPressedEvent*)&xev;
			int key = XLookupKeysym(e, 0);
			if(key < 1024) show_key_states[key] = 1;
		}
		if(xev.type == KeyRelease) {
			XKeyReleasedEvent* e = (XKeyReleasedEvent*)&xev;
			int key = XLookupKeysym(e, 0);

			if(key < 1024) show_key_states[key] = 0;
			if(key < 1024) show_key_event[key] = 1;
		}
		//show_print_event(xev.type);
	}
	show_frames++;
	show_render_memory();
	
	double dt = 1.0/60.0;
	double elapsed_us = show_os_time_us() - start_time;
	double sleep_time = (dt * 1000000.0) - elapsed_us;

	if (sleep_time > 0.0) {
		usleep((double)sleep_time);
	}

	info.fps = show_frames;
	info.frame_time_us = show_os_time_us() - start_time;
	show_total_time += info.frame_time_us;
	if (show_total_time > 1000000.0) {
		show_total_time = 0;
		show_frames = 0;
	}

	start_time = show_os_time_us();

	glXSwapBuffers(info.display, info.window);
	return info.frame_time_us;
}

static void
show_render_memory() {
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	show_update_window_info();
	glViewport(0, 0, *info.width, *info.height);

	show_texture_update(&info.texture, *info.width, *info.height, info.memory);

	show_vec4 clipping = (show_vec4) { 0, 0, FLT_MAX, FLT_MAX };
	Show_Quad_2D q = show_quad_new_clipped((show_vec2){0.0f, 0.0f}, (float)*info.width, (float)*info.height, (show_vec4){1.0f, 1.0f, 1.0f, 1.0f}, clipping);
	show_flush_quad_render(&q, *info.width, *info.height, info.texture.id);
}

void* 
show_get_memory(int* width, int* height, unsigned int* error) {
	if(!width || !height || *width == 0 || *height == 0) {
        if(error) *error |= SHOW_ERROR_INVALID_WINDOW_DIMENSIONS;
		return 0;
	}
	info.width = width;
	info.height = height;
    show_create_window(*width, *height, &info, error);
	
	show_init_opengl();
	show_update_window_info();
	show_init_renderer();
	int d;
	XkbSetDetectableAutoRepeat(info.display, 1, &d);

	info.memory = calloc(1, info.screen_width * info.screen_height * 4);
	info.texture = show_texture_create(*width, *height, info.memory);

	return info.memory;
}

const char*
show_error_string(unsigned int error) {
    if(error == SHOW_ERROR_INVALID_WINDOW_DIMENSIONS) {
        return "Fatal error: invalid window dimensions";
    }
    if(error == SHOW_ERROR_FAILED_TO_OPEN_DISPLAY) {
        return "Fatal error: failed to open x11 display";
    }
    if(error == SHOW_ERROR_INVALID_GLX_VERSION) {
        return "Fatal error: invalid glx version";
    }
    if(error == SHOW_ERROR_FAILED_TO_GET_FBC) {
        return "Fatal error: failed to get frame buffer config";
    }
    if(error == SHOW_ERROR_CREATING_WINDOW) {
        return "Fatal error: failed to create x11 window";
    }
    if(error == SHOW_ERROR_CREATING_OPENGL_CONTEXT) {
        return "Fatal error: failed to create an OpenGL context";
    }
    if(error == SHOW_ERROR_UNINITIALIZED_SHOW) {
        return "Fatal error: sHOw memory was not initialized";
    }
    if(error == SHOW_WARNING_FAILED_CREATING_OPENGL3_CONTEXT) {
        return "Warning: failed to create OpenGL 3.3 context, using 1.0 instead";
    }
}

#endif