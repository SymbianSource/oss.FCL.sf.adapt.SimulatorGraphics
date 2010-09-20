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

#include "object.h"

void DGLObject_insert(DGLObject** root, DGLObject* object)
{
	// Insert the object into the linked list while maintaining the ordering by name.

	DGLES2_ASSERT(root != NULL);
	DGLES2_ASSERT(object != NULL);

	if(*root == NULL)
	{
		// This is the first object.
		*root = object;
	}
	else if((*root)->name > object->name)
	{
		// Insert in the front.
		object->next = *root;
		*root = object;
	}
	else
	{
		DGLObject* prev = *root;
		while(prev->next != NULL && prev->next->name < object->name)
		{
			prev = prev->next;
		}
		
		object->next = prev->next;
		prev->next = object;
	}
}

DGLObject* DGLObject_remove(DGLObject** root, GLuint name)
{
	DGLES2_ASSERT(root != NULL);
	{
		DGLObject* object = *root;

		if(object == NULL)
		{
			// There are no buffers.
			return NULL;
		}

		if(object->name == name)
		{
			// The first buffer is to be destroyed.
			*root = object->next;
			return object;
		}
		else
		{
			DGLObject* removed = NULL;

			while(object->next != NULL && object->next->name != name)
			{
				object = object->next;
			}

			if(object->next != NULL)
			{
				// The buffer to be destroyed was found.
				DGLObject* newNext = object->next->next;
				removed = object->next;
				object->next = newNext;
			}

			return removed;
		}
	}
}

DGLObject* DGLObject_find(DGLObject* root, GLuint name)
{
	DGLObject* object = root;
	while(object != NULL && object->name != name)
	{
		object = object->next;
	}
	return object;
}