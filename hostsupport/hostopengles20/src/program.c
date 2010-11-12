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

const char* const DGLES2_INFO_LOG_INVALID_SHADERS = \
	"A program must have exactly one vertex shader and one fragment shader.";

DGLProgram* DGLProgram_create(GLuint name)
{
	DGLProgram* program = (DGLProgram*)malloc(sizeof(DGLProgram));
	if(program == NULL)
	{
		return NULL;
	}

	program->obj.name = name;
	program->obj.next = NULL;

	// Defer everything to the host GL by default.
	program->link_status = GL_TRUE;
	program->validate_status = GL_TRUE;

	return program;
}

void DGLProgram_destroy(DGLProgram *program)
{
	DGLES2_ASSERT(program != NULL);
	free(program);
}

GL_APICALL_BUILD void GL_APIENTRY glAttachShader (GLuint program, GLuint shader)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsShader(program), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsProgram(program), GL_INVALID_VALUE);
	DGLES2_ERROR_IF(ctx->hgl.IsProgram(shader), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsShader(shader), GL_INVALID_VALUE);
    ctx->hgl.AttachShader(program, shader);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glBindAttribLocation(GLuint program, GLuint index, const char* name)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsShader(program), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsProgram(program), GL_INVALID_VALUE);
	DGLES2_ERROR_IF(index >= ctx->max_vertex_attribs, GL_INVALID_VALUE);
	DGLES2_ERROR_IF(strncmp(name, "gl_", 3) == 0, GL_INVALID_OPERATION);
    ctx->hgl.BindAttribLocation(program, index, name);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD GLuint GL_APIENTRY glCreateProgram(void)
{
	DGLES2_ENTER_RET(0);
	{
		GLuint name;
		DGLContext_getHostError(ctx);
		name = ctx->hgl.CreateProgram();
		if(DGLContext_getHostError(ctx) == GL_NO_ERROR && name != 0)
		{
			DGLContext_createProgram(ctx, name);
		}

		DGLES2_LEAVE_RET(name);
	}
}

GL_APICALL_BUILD void GL_APIENTRY glDeleteProgram(GLuint program)
{
	DGLES2_ENTER();
	if(program != 0)
	{
		DGLES2_ERROR_IF(ctx->hgl.IsShader(program), GL_INVALID_OPERATION);
		DGLES2_ERROR_IF(!ctx->hgl.IsProgram(program), GL_INVALID_VALUE);
		ctx->hgl.DeleteProgram(program);
		if(DGLContext_getHostError(ctx) == GL_NO_ERROR)
		{
			DGLContext_destroyProgram(ctx, program);
		}
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glDetachShader(GLuint program, GLuint shader)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsShader(program), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsProgram(program), GL_INVALID_VALUE);
	DGLES2_ERROR_IF(ctx->hgl.IsProgram(shader), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsShader(shader), GL_INVALID_VALUE);
	ctx->hgl.DetachShader(program, shader);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetActiveAttrib (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsShader(program), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsProgram(program), GL_INVALID_VALUE);
	ctx->hgl.GetActiveAttrib(program, index, bufsize, length, size, type, name);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetActiveUniform (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsShader(program), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsProgram(program), GL_INVALID_VALUE);
	ctx->hgl.GetActiveUniform(program, index, bufsize, length, size, type, name);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetAttachedShaders (GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsShader(program), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsProgram(program), GL_INVALID_VALUE);
	ctx->hgl.GetAttachedShaders(program, maxcount, count, shaders);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD int GL_APIENTRY glGetAttribLocation (GLuint program, const char* name)
{
	DGLES2_ENTER_RET(-1);
	DGLES2_ERROR_IF_RET(ctx->hgl.IsShader(program), GL_INVALID_OPERATION, -1);
	DGLES2_ERROR_IF_RET(!ctx->hgl.IsProgram(program), GL_INVALID_VALUE, -1);
	DGLES2_LEAVE_RET(ctx->hgl.GetAttribLocation(program, name););
}

GL_APICALL_BUILD void GL_APIENTRY glGetProgramiv(GLuint program, GLenum pname, GLint* params)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsShader(program), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsProgram(program), GL_INVALID_VALUE);
	switch(pname)
	{
		case GL_LINK_STATUS:
			{
				const DGLProgram* program_obj = DGLContext_findProgram(ctx, program);
				DGLES2_ASSERT(program_obj != NULL);
				if(program_obj->link_status)
				{
					// Our requirement for linking was fulfilled, ask the host.
					ctx->hgl.GetProgramiv(program, GL_LINK_STATUS, params);
				}
				else
				{
					*params = GL_FALSE;
				}
			}
			break;

		case GL_VALIDATE_STATUS:
			{
				const DGLProgram* program_obj = DGLContext_findProgram(ctx, program);
				DGLES2_ASSERT(program_obj != NULL);
				if(program_obj->validate_status)
				{
					// Our requirement for validation was fulfilled, ask the host.
					ctx->hgl.GetProgramiv(program, GL_VALIDATE_STATUS, params);
				}
				else
				{
					*params = GL_FALSE;
				}
			}
			break;

		case GL_INFO_LOG_LENGTH:
			{
				const DGLProgram* program_obj = DGLContext_findProgram(ctx, program);
				DGLES2_ASSERT(program_obj != NULL);
				if(!program_obj->link_status || !program_obj->validate_status)
				{
					// Use our own info log.
					*params = strlen(DGLES2_INFO_LOG_INVALID_SHADERS);
				}
				else
				{
					ctx->hgl.GetProgramiv(program, GL_INFO_LOG_LENGTH, params);
				}
			}
			break;

		default:
			ctx->hgl.GetProgramiv(program, pname, params);
			break;
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsShader(program), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsProgram(program), GL_INVALID_VALUE);
	{
		const DGLProgram* program_obj = DGLContext_findProgram(ctx, program);
		DGLES2_ASSERT(program_obj != NULL);
		if(!program_obj->link_status || !program_obj->validate_status)
		{
			// Use our own info log.

			if(length != NULL)
			{
				*length = 0;
			}

			if(infolog != NULL)
			{
				int log_length = strlen(DGLES2_INFO_LOG_INVALID_SHADERS);
				int num_chars = log_length < bufsize - 1 ? log_length : bufsize - 1;

				strncpy(infolog, DGLES2_INFO_LOG_INVALID_SHADERS, num_chars);
				infolog[num_chars] = 0;

				if(length != NULL)
				{
					*length = num_chars;
				}
			}
		}
		else
		{
			ctx->hgl.GetProgramInfoLog(program, bufsize, length, infolog);
		}
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetUniformfv(GLuint program, GLint location, GLfloat* params)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsShader(program), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsProgram(program), GL_INVALID_VALUE);
	ctx->hgl.GetUniformfv(program, location, params);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glGetUniformiv(GLuint program, GLint location, GLint* params)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsShader(program), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsProgram(program), GL_INVALID_VALUE);
	ctx->hgl.GetUniformiv(program, location, params);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD int GL_APIENTRY glGetUniformLocation(GLuint program, const char* name)
{
	DGLES2_ENTER_RET(-1);
	DGLES2_ERROR_IF_RET(ctx->hgl.IsShader(program), GL_INVALID_OPERATION, -1);
	DGLES2_ERROR_IF_RET(!ctx->hgl.IsProgram(program), GL_INVALID_VALUE, -1);
	DGLES2_LEAVE_RET(ctx->hgl.GetUniformLocation(program, name));
}

GL_APICALL_BUILD GLboolean GL_APIENTRY glIsProgram(GLuint program)
{
	DGLES2_ENTER_RET(GL_FALSE);
	DGLES2_LEAVE_RET(ctx->hgl.IsProgram(program));
}

GL_APICALL_BUILD void GL_APIENTRY glLinkProgram(GLuint program)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsShader(program), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsProgram(program), GL_INVALID_VALUE);
	{
		GLint num_shaders;

		DGLProgram* program_obj = DGLContext_findProgram(ctx, program);
		DGLES2_ASSERT(program_obj != NULL);

		ctx->hgl.GetProgramiv(program, GL_ATTACHED_SHADERS, &num_shaders);

		// Linked programs must have both a vertex and a fragment shader in GL ES.
		// Multiple shaders of the same type may not be attached to a single program object.
		if(num_shaders != 2)
		{
			program_obj->link_status = GL_FALSE;
		}
		else
		{
			// Check that the shaders are of different type.

			GLuint shaders[2];
			GLint types[2];
			ctx->hgl.GetAttachedShaders(program, 2, NULL, shaders);
			ctx->hgl.GetShaderiv(shaders[0], GL_SHADER_TYPE, &types[0]);
			ctx->hgl.GetShaderiv(shaders[1], GL_SHADER_TYPE, &types[1]);
			
			if(types[0] == types[1])
			{
				program_obj->link_status = GL_FALSE;
			}
			else
			{
				program_obj->link_status = GL_TRUE;
				ctx->hgl.LinkProgram(program);
			}
		}
	}
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform1f (GLint location, GLfloat x)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform1f(location, x);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform1fv (GLint location, GLsizei count, const GLfloat* v)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform1fv(location, count, v);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform1i (GLint location, GLint x)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform1i(location, x);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform1iv (GLint location, GLsizei count, const GLint* v)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform1iv(location, count, v);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform2f (GLint location, GLfloat x, GLfloat y)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform2f(location, x, y);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform2fv (GLint location, GLsizei count, const GLfloat* v)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform2fv(location, count, v);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform2i (GLint location, GLint x, GLint y)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform2i(location, x, y);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform2iv (GLint location, GLsizei count, const GLint* v)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform2iv(location, count, v);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform3f (GLint location, GLfloat x, GLfloat y, GLfloat z)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform3f(location, x, y, z);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform3fv (GLint location, GLsizei count, const GLfloat* v)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform3fv(location, count, v);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform3i (GLint location, GLint x, GLint y, GLint z)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform3i(location, x, y, z);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform3iv (GLint location, GLsizei count, const GLint* v)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform3iv(location, count, v);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform4f (GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform4f(location, x, y, z, w);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform4fv (GLint location, GLsizei count, const GLfloat* v)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform4fv(location, count, v);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform4i (GLint location, GLint x, GLint y, GLint z, GLint w)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform4i(location, x, y, z, w);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniform4iv (GLint location, GLsizei count, const GLint* v)
{
	DGLES2_ENTER();
	ctx->hgl.Uniform4iv(location, count, v);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	DGLES2_ENTER();
	ctx->hgl.UniformMatrix2fv(location, count, transpose, value);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	DGLES2_ENTER();
	ctx->hgl.UniformMatrix3fv(location, count, transpose, value);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	DGLES2_ENTER();
	ctx->hgl.UniformMatrix4fv(location, count, transpose, value);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glUseProgram (GLuint program)
{
	DGLES2_ENTER();
	ctx->hgl.UseProgram(program);
	DGLES2_LEAVE();
}

GL_APICALL_BUILD void GL_APIENTRY glValidateProgram (GLuint program)
{
	DGLES2_ENTER();
	DGLES2_ERROR_IF(ctx->hgl.IsShader(program), GL_INVALID_OPERATION);
	DGLES2_ERROR_IF(!ctx->hgl.IsProgram(program), GL_INVALID_VALUE);
	{
		GLint num_shaders;

		DGLProgram* program_obj = DGLContext_findProgram(ctx, program);
		DGLES2_ASSERT(program_obj != NULL);

		// The program must no be empty.
		ctx->hgl.GetProgramiv(program, GL_ATTACHED_SHADERS, &num_shaders);
		if(num_shaders == 0)
		{
			program_obj->validate_status = GL_FALSE;
		}
		else
		{
			program_obj->validate_status = GL_TRUE;
			ctx->hgl.ValidateProgram(program);
		}
	}
	DGLES2_LEAVE();
}

