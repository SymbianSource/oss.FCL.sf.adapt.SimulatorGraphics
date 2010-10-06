/* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *
 */

#include "common.h"
#include "hgl.h"
#include "context.h"
#include <string.h>

DGLShader* DGLShader_create(GLuint name)
{
	DGLShader* shader = malloc(sizeof(DGLShader));
	if(shader == NULL)
	{
		return NULL;
	}

	shader->obj.name = name;
	shader->obj.next = NULL;

	shader->source = NULL;
	shader->length = 0;

	return shader;
}

void DGLShader_destroy(DGLShader *shader)
{
	DGLES2_ASSERT(shader != NULL);
	if(shader->source != NULL)
	{
		free(shader->source);
		shader->source = NULL;
	}
	free(shader);
}

GL_APICALL_BUILD void GL_APIENTRY glCompileShader(GLuint shader)
{
	DGLES2_ENTER();
	ctx->hgl.CompileShader(shader);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD GLuint GL_APIENTRY glCreateShader(GLenum type)
{
	DGLES2_ENTER_RET(0);
	DGLES2_ERROR_IF_RET(type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER, GL_INVALID_ENUM, 0);
	{
		GLuint name = ctx->hgl.CreateShader(type);
		if(DGLContext_getHostError(ctx) == GL_NO_ERROR && name != 0)
		{
			DGLContext_createShader(ctx, name);
		}

		DGLES2_LEAVE_RET(name);
	}
}

GL_APICALL_BUILD void GL_APIENTRY glDeleteShader(GLuint shader)
{
	DGLES2_ENTER();
	if(shader != 0)
	{
		DGLES2_ERROR_IF(ctx->hgl.IsProgram(shader), GL_INVALID_OPERATION);
		DGLES2_ERROR_IF(!ctx->hgl.IsShader(shader), GL_INVALID_VALUE);
		ctx->hgl.DeleteShader(shader);
		if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
		{
			DGLContext_destroyShader(ctx, shader);
		}
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsProgram(shader), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsShader(shader), GL_INVALID_VALUE);
	if(pname == GL_SHADER_SOURCE_LENGTH)
	{
		DGLShader* shader_obj = DGLContext_findShader(ctx, shader);
		DGLES2_ASSERT(shader_obj != NULL);
		*params = shader_obj->length + 1;
	}
	else
	{
		ctx->hgl.GetShaderiv(shader, pname, params);
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsProgram(shader), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsShader(shader), GL_INVALID_VALUE);
	ctx->hgl.GetShaderInfoLog(shader, bufsize, length, infolog);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(shadertype != GL_VERTEX_SHADER && shadertype != GL_FRAGMENT_SHADER, GL_INVALID_ENUM);
	// Values from the GL ES and GLSL specifications.
	switch(precisiontype)
	{
		case GL_LOW_FLOAT:
		case GL_MEDIUM_FLOAT:
		case GL_HIGH_FLOAT:
			range[0] = 127;
			range[1] = 127;
			*precision = 23;
			break;

		case GL_LOW_INT:
		case GL_MEDIUM_INT:
		case GL_HIGH_INT:
			range[0] = 15;
			range[1] = 14;
			*precision = 0;
			break;
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, char* source)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsProgram(shader), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsShader(shader), GL_INVALID_VALUE);
	DGLES2_ERROR_IF(bufsize < 0, GL_INVALID_VALUE);
	{
		DGLShader* shader_obj = DGLContext_findShader(ctx, shader);
		DGLES2_ASSERT(shader_obj != NULL);
		
		if(length != NULL)
		{
			*length = 0;
		}

		if(source != NULL)
		{
			GLsizei num_chars = shader_obj->length < bufsize - 1 ? shader_obj->length : bufsize - 1;
			if(num_chars > 0)
			{
				strncpy(source, shader_obj->source, num_chars);
				source[num_chars] = 0;
				if(length != NULL)
				{
					*length = num_chars;
				}
			}
		}
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD GLboolean GL_APIENTRY glIsShader(GLuint shader)
{
	DGLES2_ENTER_RET(GL_FALSE);
	DGLES2_LEAVE_RET(ctx->hgl.IsShader(shader));
}

GL_APICALL_BUILD void GL_APIENTRY glReleaseShaderCompiler(void)
{
	DGLES2_ENTER();
	// No-op.
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glShaderBinary(GLsizei n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length)
{
	DGLES2_ENTER();
	// No supported formats.
	DGLES2_ERROR(GL_INVALID_ENUM);
	DGLES2_LEAVE();
}

static const char *opengl_strtok(const char *s, int *n)
{
    static char *buffer = 0;
    static int buffersize = -1;
    static const char *delim = " \t\n\r()[]{},;:";
    static const char *prev = 0;
    int cComment = 0;
    int cppComment = 0;

    if (!s) {
        if (!*prev || !*n) {
            if (buffer) {
                free(buffer);
                buffer = 0;
                buffersize = -1;
            }
            prev = 0;
            return 0;
        }
        s = prev;
    } else {
        if (buffer) {
            free(buffer);
            buffer = 0;
            buffersize = -1;
        }
		prev = s;
    }

    if( *n && *s == '/') {
        if(*(s+1) == '*') cComment = 1;
        if(*(s+1) == '/') cppComment = 1;        
    }
    if( cComment == 1 || cppComment == 1) {
        for(; *n && (cComment == 1 || cppComment == 1); s++, (*n)--) {
            if(cComment == 1 && *s == '*' && *(s+1) == '/' ) {
                cComment = 0;
            }
            if(cppComment == 1 && *s == '\n') {
                cppComment = 0;
            }
        }
    } else {
        for (; *n && strchr(delim, *s); s++, (*n)--);
    }

	if(s - prev > 0) {
		if (buffersize < s - prev) {
			buffersize = s - prev;
			if (buffer) {
				free(buffer);
			}
			buffer = malloc(buffersize + 1);
		}
		memcpy(buffer, prev, s - prev);
		buffer[s - prev] = 0;
		prev = s;
	} else {
		const char *e = s;
		for (; *n && *e && !strchr(delim, *e); e++, (*n)--);
		prev = e;
		if (buffersize < e - s) {
			buffersize = e - s;
			if (buffer) {
				free(buffer);
			}
			buffer = malloc(buffersize + 1);
		}
		memcpy(buffer, s, e - s);
		buffer[e - s] = 0;
	}
    return buffer;
}

static char* do_eglShaderPatch(char *source, int len, int *patched_len)
{
    /* DISCLAIMER: this is not a full-blown shader parser but a simple
     * implementation which tries to remove the OpenGL ES shader
     * "precision" statements and precision qualifiers "lowp", "mediump"
     * and "highp" from the specified shader source, replace built-in
	 * constants that were renamed in GLSL ES ("gl_MaxVertexUniformVectors",
	 * "gl_MaxFragmentUniformVectors" and "gl_MaxVaryingVectors")
	 * and insert a "#version 120" directive in the beginning of the source
	 * or replace an existing "#version 100" directive. */
	DGLES2_ASSERT(source != NULL);
	DGLES2_ASSERT(len >= 0);
	{
#ifndef DGLES2_ALLOW_GLSL_110
		GLboolean version_found = GL_FALSE;
#endif
		int buffer_size;
		char *patched;
		const char *p;
		
		*patched_len = 0;
		buffer_size = len;
		patched = malloc(buffer_size + 1);
		if(patched == NULL)	{
			return NULL;
		}

		p = opengl_strtok(source, &len);
		for (; p; p = opengl_strtok(0, &len)) {
			if (!strcmp(p, "lowp") || !strcmp(p, "mediump") || !strcmp(p, "highp")) {
				continue;
			} else if (!strcmp(p, "precision")) {
				do {
					p = opengl_strtok(0, &len);
				} while(p && !strchr(p, ';'));
			} else {
				int tok_len;
				if (!strcmp(p, "gl_MaxVertexUniformVectors")) {
					p = "(gl_MaxVertexUniformComponents / 4)";
				} else if (!strcmp(p, "gl_MaxFragmentUniformVectors")) {
					p = "(gl_MaxFragmentUniformComponents / 4)";
				} else if (!strcmp(p, "gl_MaxVaryingVectors")) {
					p = "(gl_MaxVaryingFloats / 4)";
				}
#ifndef DGLES2_ALLOW_GLSL_110
				else if (!strcmp(p, "#version")) {
					p = opengl_strtok(0, &len);
					if (!strcmp(p, "100")) {
						p = "#version 120";
						version_found = GL_TRUE;
					}
				} else if (!strcmp(p, "#")) {
					p = opengl_strtok(0, &len);
					if (!strcmp(p, "version")) {
						p = opengl_strtok(0, &len);
						if (!strcmp(p, "100")) {
							p = "#version 120";
							version_found = GL_TRUE;
						}
					}
				} 
#endif // !DGLES2_ALLOW_GLSL_110
				tok_len = strlen(p);
				if(*patched_len + tok_len > buffer_size) {
					buffer_size *= 2;
					patched = realloc(patched, buffer_size + 1);
					if(patched == NULL) {
						return NULL;
					}
				}
				memcpy(patched + *patched_len, p, tok_len);
				*patched_len += tok_len;
			}
		}
		patched[*patched_len] = 0;
#ifndef DGLES2_ALLOW_GLSL_110
		/* add version directive is one was not found */
		if (!version_found) {
			char* new_patched;
			*patched_len += strlen("#version 120\n");
			new_patched = malloc(*patched_len + 1);
			if (new_patched == NULL) {
				return NULL;
			}
			strcpy(new_patched, "#version 120\n");
			strcat(new_patched, patched);
			free(patched);
			patched = new_patched;
		}
#endif // !DGLES2_ALLOW_GLSL_110
		{
			/* check that we don't leave dummy preprocessor lines */
			char *sp;
			for (sp = patched; *sp;) {
				for (; *sp == ' ' || *sp == '\t'; sp++);
				if (!strncmp(sp, "#define", 7)) {
					for (p = sp + 7; *p == ' ' || *p == '\t'; p++);
					if (*p == '\n' || *p == '\r' || *p == '/') {
						memset(sp, 0x20, 7);
					}
				}
				for (; *sp && *sp != '\n' && *sp != '\r'; sp++);
				for (; *sp == '\n' || *sp == '\r'; sp++);
			}
		}

		return patched;
	}
}

GL_APICALL_BUILD void GL_APIENTRY glShaderSource(GLuint shader, GLsizei count, const char** string, const GLint* length)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsProgram(shader), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsShader(shader), GL_INVALID_VALUE);
	DGLES2_ERROR_IF(count < 0, GL_INVALID_VALUE);
/*
#if(CONFIG_DEBUG == 1)
	Dprintf("Editing shader:\n--ORIGINAL-SHADER--\n");
	for(unsigned i = 0; i < count; ++i)
	{
		if(length)
			Dprintf("%*s", length[i], string[i]);
		else
			Dprintf("%s", string[i]);
	}
	Dprintf("---END-ORIGINAL-SHADER---\n");
#endif // !NDEBUG
	char** string_dgl = malloc(sizeof(char*)*count);
	GLint* length_dgl = malloc(sizeof(GLint)*count);

	// Remove the non OpenGL 2.x compilant keywords.
	for(unsigned i = 0; i < count; ++i)
	{
		static const char* removables[] =
		{
			"precision highp float;",
			"precision mediump float;",
			"precision lowp float;",
			"highp",
			"lowp",
			"mediump",
			"precision"
		};

		length_dgl[i] = length ? length[i] : strlen(string[i]);
		string_dgl[i] = malloc(length_dgl[i] + 1);
		memcpy(string_dgl[i], string[i], length_dgl[i]);
		string_dgl[i][length_dgl[i]] = 0;

		for(unsigned j = 0; j < sizeof(removables)/sizeof(removables[0]); ++j)
		{
			char const* p;
			while((p = strstr(string_dgl[i], removables[j])))
			{
				memmove(p, p + strlen(removables[j]), strlen(p + strlen(removables[j])) + 1);
			}
		}
	}
#if(CONFIG_DEBUG == 1)
	Dprintf("Loading shader:\n--DESKTOP-GL-SHADER--\n");
	for(unsigned i = 0; i < count; ++i)
	{
		Dprintf("%*s", length_dgl[i], string_dgl[i]);
	}
	Dprintf("---END-DESKTOP-GL-SHADER---\n");
#endif // !NDEBUG
    
	ctx->hgl.ShaderSource(shader, count, string_dgl, length_dgl);

	for(unsigned i = 0; i < count; ++i)
		free(string_dgl[i]);
	free(string_dgl);
	free(length_dgl);
 */

	if(count > 0 && string != NULL)
	{
		char* source = NULL;
		int total_len = 0;

		if(count > 1)
		{
			int i;

			// Concatenate the passed strings into one source string.
			for(i = 0; i < count; i++)
			{
				int len;

				if(string[i] == NULL)
				{
					continue;
				}

				if(length == NULL || length[i] < 0)
				{
					len = strlen(string[i]);
				}
				else
				{
					len = length[i];
				}

				if(len > 0)
				{
					total_len += len;

					if(source == NULL)
					{
						source = malloc(total_len + 1);
						if(source == NULL)
						{
							DGLES2_ERROR(GL_OUT_OF_MEMORY);
						}
						source[0] = 0;
					}
					else
					{
						source = realloc(source, total_len + 1);
						if(source == NULL)
						{
							DGLES2_ERROR(GL_OUT_OF_MEMORY);
						}
					}

					strncat(source, string[i], len);
				}
			}
		}
		else
		{
			source = (char*)string[0];
			if(length == NULL || length[0] < 0)
			{
				total_len = strlen(source);
			}
			else
			{
				total_len = length[0];
			}
		}

		{
			// FIXME: This will fail with real constant data!
			int patched_len;
			const GLchar* patched = do_eglShaderPatch(source, total_len, &patched_len);
			if(patched == NULL)
			{
				DGLES2_ERROR(GL_OUT_OF_MEMORY);
			}
			ctx->hgl.ShaderSource(shader, 1, &patched, &patched_len);
			free((void*)patched);
		}

		if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
		{
			if(!DGLContext_setShaderSource(ctx, shader, source, total_len))
			{
				DGLES2_ERROR(GL_OUT_OF_MEMORY);
			}
		}

		if(count > 1)
		{
			free(source);
		}
	}
	else
	{
		ctx->hgl.ShaderSource(shader, count, string, length);
	}

	DGLES2_LEAVE();
}
