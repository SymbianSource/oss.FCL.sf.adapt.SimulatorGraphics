/* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and /or associated documentation files
 * (the "Materials "), to deal in the Materials without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Materials,
 * and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR
 * THE USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *
 */

#include "EGLConfig.h"
#include "ColorDescriptor.h"
#include "TransparentColor.h"

const int KPbufferHeight = 16384;       
const int KPbufferWidth = 16384;
const int KPbufferPixels = KPbufferHeight * KPbufferWidth;

CEGLConfig::CEGLConfig(void) :
    m_nativeVisualId( NULL )
    {
    }

CEGLConfig::CEGLConfig( CColorDescriptor&   colorDescriptor,        
                        EGLBoolean          bindToTexRGB,           /* EGL_BIND_TO_TEXTURE_RGB*/
                        EGLBoolean          bindToTexRGBA,          /* EGL_BIND_TO_TEXTURE_RGBA */
                        EGLenum             configCaveat,           /* EGL_CONFIG_CAVEAT */
                        EGLint              configId,               /* EGL_CONFIG_ID */
                        EGLint              conformant,             /* EGL_CONFORMANT */
                        EGLint              depthSize,              /* EGL_DEPTH_SIZE */
                        EGLint              level,                  /* EGL_LEVEL */
                        EGLint              maxSwapInterval,        /* EGL_MAX_SWAP_INTERVAL */
                        EGLint              minSwapInterval,        /* EGL_MIN_SWAP_INTERVAL */
                        EGLBoolean          nativeRenderable,       /* EGL_NATIVE_RENDERABLE */
                        EGLint              nativeVisualType,       /* EGL_NATIVE_VIDUAL_TYPE */
                        EGLint              renderableType,         /* EGL_RENDERABLE_TYPE */
                        EGLint              sampleBuffers,          /* EGL_SAMPLE_BUFFERS */
                        EGLint              samples,                /* EGL_SAMPLES */
                        EGLint              stencilSize,            /* EGL_STENCIL_SIZE */
                        EGLint              surfaceType,
                        CTransparentColor*  transparentColor) :
    m_bindToTexRGB( bindToTexRGB ),
    m_bindToTexRGBA( bindToTexRGBA ),
    m_configCaveat( configCaveat ),
    m_configId( configId ),
    m_conformant( conformant ),
    m_depthSize( depthSize ),
    m_level( level ),
    m_maxPbufferWidth( KPbufferWidth ),
    m_maxPbufferHeight( KPbufferHeight ),
    m_maxPbufferPixels( KPbufferPixels ),
    m_maxSwapInterval( maxSwapInterval ),
    m_minSwapInterval( minSwapInterval ),
    m_nativeRenderable( nativeRenderable ),
    m_nativeVisualId( NULL ),
    m_nativeVisualType( nativeVisualType ),
    m_renderableType( renderableType ),
    m_sampleBuffers( samples > 1 ? 1 : 0 ),
    m_samples( samples ),
    m_stencilSize( stencilSize ),
    m_surfaceType( surfaceType )
    {
    m_colorDescriptor = colorDescriptor;
    if( m_colorDescriptor.isLuminance() )
        {
        m_bufferSize = m_colorDescriptor.LuminanceSize() + m_colorDescriptor.AlphaSize();
        }
    else
        {
        m_bufferSize = m_colorDescriptor.RedSize() +
            m_colorDescriptor.GreenSize() +
            m_colorDescriptor.BlueSize() +
            m_colorDescriptor.AlphaSize();
        }
    if( transparentColor )
        {
        m_transparentColor = *transparentColor;
        }
    }
    
CEGLConfig::~CEGLConfig(void)
    {
    }

EGLint CEGLConfig::SetUpConfig( EGLint red, EGLint green, EGLint blue, 
                                EGLint luminance, EGLint alpha, 
                                EGLint alphaMask, EGLint samples, EGLint bpp, EGLint id )
    {
    int ret = EGL_SUCCESS;
    int rgb = red + green + blue;
    if( luminance && rgb )
        {
        return EGL_BAD_PARAMETER;
        }
    m_colorDescriptor.Set( red, green, blue, luminance, false, alpha, true, alphaMask, bpp );
    //m_colorDescriptor = EGLI_NEW CColorDescriptor( red, green, blue, luminance, false, alpha, true, alphaMask, bpp );

    m_configId = id;
    
    if( m_colorDescriptor.isLuminance() )
        {
        m_bufferSize = luminance + alpha;
        }
    else
        {
        m_bufferSize = red + green + blue + alpha;  
        }

    if( rgb )
        {
        m_bindToTexRGB = EGL_TRUE;
        m_bindToTexRGBA = EGL_TRUE;
        }
    else
        {
        m_bindToTexRGB = EGL_FALSE;
        m_bindToTexRGBA = EGL_FALSE;
        }
    
    /*
    The EGL_CONFIG_CAVEAT attribute may be set to one of the following values:
    EGL_NONE, EGL_SLOW_CONFIG or EGL_NON_CONFORMANT_CONFIG. If the
    attribute is set to EGL_NONE then the configuration has no caveats; if it is set to
    EGL_SLOW_CONFIG then rendering to a surface with this configuration may run at
    reduced performance (for example, the hardware may not support the color buffer
    depths described by the configuration); if it is set to EGL_NON_CONFORMANT_-
    CONFIG then rendering to a surface with this configuration will not pass the required
    OpenGL ES conformance tests (note that EGL_NON_CONFORMANT_CONFIG
    is obsolete, and the same information can be obtained from the EGL_CONFORMANT
    attribute on a per-client-API basis, not just for OpenGL ES ).
    */
    m_configCaveat = EGL_NONE;

    /*
    Same as EGL_RENDERABLE_TYPE:
    EGL_OPENGL_ES_BIT,
    EGL_OPENVG_BIT,
    EGL_OPENGL_ES2_BIT,
    EGL_OPENGL_BIT
    */
    m_conformant = EGL_OPENGL_ES2_BIT;//0;
    m_renderableType = EGL_OPENVG_BIT | EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT;
    
    /*
    The Depth Buffer
    The depth buffer is shared by OpenGL and OpenGL ES . It contains fragment
    depth (Z) information generated during rasterization. EGL_DEPTH_SIZE indicates
    the depth of this buffer in bits.
    */
    m_depthSize = 0;
    
    /*
     EGL_LEVEL is the framebuffer overlay or underlay level in which an
     EGLSurface created with eglCreateWindowSurface will be placed. Level zero
     indicates the default layer. The behavior of windows placed in overlay and under-
     lay levels depends on the underlying native window system.
    */
    m_level = 0;
    
    /*
    EGL_MAX_PBUFFER_WIDTH and EGL_MAX_PBUFFER_HEIGHT indicate the
    maximum width and height that can be passed into eglCreatePbufferSurface,
    and EGL_MAX_PBUFFER_PIXELS indicates the maximum number of pixels (width
    times height) for a pbuffer surface. Note that an implementation may return a value
    for EGL_MAX_PBUFFER_PIXELS that is less than the maximum width times the
    maximum height. The value for EGL_MAX_PBUFFER_PIXELS is static and assumes
    that no other pbuffers or native resources are contending for the framebuffer
    memory. Thus it may not be possible to allocate a pbuffer of the size given by
    EGL_MAX_PBUFFER_PIXELS.
    */
    m_maxPbufferHeight = KPbufferHeight;       
    m_maxPbufferWidth = KPbufferWidth;
    m_maxPbufferPixels = KPbufferPixels;   
    
    /*
    EGL_MAX_SWAP_INTERVAL is the maximum value that can be passed to
    eglSwapInterval, and indicates the number of swap intervals that will elapse before
    a buffer swap takes place after calling eglSwapBuffers. Larger values will be
    silently clamped to this value.
    */
    m_maxSwapInterval = 1;
    
    /*
    EGL_MIN_SWAP_INTERVAL is the minimum value that can be passed to
    eglSwapInterval, and indicates the number of swap intervals that will elapse before
    a buffer swap takes place after calling eglSwapBuffers. Smaller values will
    be silently clamped to this value.
    */
    m_minSwapInterval = 1;
    
    /*
    EGL_NATIVE_RENDERABLE is an EGLBoolean indicating whether the native
    window system can be used to render into a surface created with the EGLConfig.
    */
    m_nativeRenderable = EGL_FALSE;
    
    /*
    If an EGLConfig supports windows then it may have an associated native visual.
    EGL_NATIVE_VISUAL_ID specifies an identifier for this visual, and EGL_-
    NATIVE_VISUAL_TYPE specifies its type. If an EGLConfig does not support
    windows, or if there is no associated native visual type, then querying EGL_-
    NATIVE_VISUAL_ID will return 0 and querying EGL_NATIVE_VISUAL_TYPE
    will return EGL_NONE.
    The interpretation of the native visual identifier and type is platform-dependent.
    For example, if the native window system is X, then the identifier will be the XID
    of an X Visual.
    */
    m_nativeVisualId = 0;
    
    m_nativeVisualType = EGL_NONE;    
    
    /*
    EGL_SAMPLE_BUFFERS indicates the number of multisample buffers, which
    must be zero or one. EGL_SAMPLES gives the number of samples per pixel; if
    EGL_SAMPLE_BUFFERS is zero, then EGL_SAMPLES will also be zero. If EGL_-
    SAMPLE_BUFFERS is one, then the number of color, depth, and stencil bits for each
    sample in the multisample buffer are as specified by the EGL_*_SIZE attributes.
    There are no single-sample depth or stencil buffers for a multisample
    EGLConfig; the only depth and stencil buffers are those in the multisample
    buffer. If the color samples in the multisample buffer store fewer bits than are
    stored in the color buffers, this fact will not be reported accurately. Presumably a
    compression scheme is being employed, and is expected to maintain an aggregate
    resolution equal to that of the color buffers.
    */
    m_samples = samples; 
    m_sampleBuffers = samples > 1 ? 1 : 0;   
    
    /*
    The stencil buffer is shared by OpenGL and OpenGL ES . It contains fragment
    stencil information generated during rasterization. EGL_STENCIL_SIZE indicates
    the depth of this buffer in bits.
    */
    m_stencilSize = 0;
    
    /*
    EGL_SURFACE_TYPE is a mask indicating capabilities of surfaces that can be
    created with the corresponding EGLConfig (the config is said to support these
    surface types).

    EGL_SURFACE_TYPE is a mask indicating capabilities of surfaces that can be
    created with the corresponding EGLConfig (the config is said to support these
    surface types). The valid bit settings are shown in Table 3.2.
    For example, an EGLConfig for which the value of the EGL_SURFACE_TYPE
    attribute is
    EGL_WINDOW_BIT j EGL_PIXMAP_BIT j EGL_PBUFFER_BIT
    can be used to create any type of EGL surface, while an EGLConfig for which this
    attribute value is EGL_WINDOW_BIT cannot be used to create a pbuffer or pixmap.
    If EGL_MULTISAMPLE_RESOLVE_BOX_BIT is set in EGL_SURFACE_TYPE,
    then the EGL_MULTISAMPLE_RESOLVE attribute of a surface can be specified as a
    box filter with eglSurfaceAttrib (see section 3.5.6).
    If EGL_SWAP_BEHAVIOR_PRESERVED_BIT is set in EGL_SURFACE_TYPE,
    then the EGL_SWAP_BEHAVIOR attribute of a surface can be specified to preserve
    color buffer contents using eglSurfaceAttrib (see section 3.5.6).
    If EGL_VG_COLORSPACE_LINEAR_BIT is set in EGL_SURFACE_TYPE, then
    the EGL_VG_COLORSPACE attribute may be set to EGL_VG_COLORSPACE_-
    LINEAR when creating a window, pixmap, or pbuffer surface (see section 3.5).
    If EGL_VG_ALPHA_FORMAT_PRE_BIT is set in EGL_SURFACE_TYPE, then the
    EGL_VG_ALPHA_FORMAT attribute may be set to EGL_VG_ALPHA_FORMAT_PRE
    when creating a window, pixmap, or pbuffer surface (see section 3.5).
    
    */
    m_surfaceType = EGL_WINDOW_BIT |
                    EGL_PBUFFER_BIT | 
                    //EGL_VG_COLORSPACE_LINEAR_BIT |
                    EGL_VG_ALPHA_FORMAT_PRE_BIT |
                    EGL_SWAP_BEHAVIOR_PRESERVED_BIT;
    m_colorDescriptor.SetSurfaceType( m_surfaceType );
    m_surfaceType |= EGL_VG_COLORSPACE_LINEAR_BIT;
    
    /*
    If EGL_TRANSPARENT_TYPE is EGL_NONE, then the values for EGL_-
    TRANSPARENT_RED_VALUE, EGL_TRANSPARENT_GREEN_VALUE, and EGL_-
    TRANSPARENT_BLUE_VALUE are undefined. Otherwise, they are interpreted as
    integer framebuffer values between 0 and the maximum framebuffer value for the
    component. For example, EGL_TRANSPARENT_RED_VALUE will range between 0
    and 2EGL_RED_SIZE 1.
    */

    return ret;
    }

EGLBoolean CEGLConfig::SetAttribute( EGLint attribute, EGLint value )
    {
    EGLBoolean ret = EGL_FALSE;
    switch( attribute )
        {
        case EGL_BUFFER_SIZE:
            {
            if( value == EGL_DONT_CARE ||
                value >= 0 )
                {
                m_bufferSize = value;
                ret = EGL_TRUE;
                }
            break;
            }

        case EGL_BLUE_SIZE:
        case EGL_GREEN_SIZE:
        case EGL_RED_SIZE:
        case EGL_LUMINANCE_SIZE:
        case EGL_ALPHA_SIZE:
        case EGL_ALPHA_MASK_SIZE:
            {
            if( (value == EGL_DONT_CARE ||
                 value >= 0) )
                {
                m_colorDescriptor.SetAttribute( attribute, value );
                ret = EGL_TRUE;
                }
            break;
            }

        case EGL_COLOR_BUFFER_TYPE:
            {
            if( (value == EGL_DONT_CARE ||
                 value == EGL_RGB_BUFFER ||
                 value == EGL_LUMINANCE_BUFFER) )
                {
                m_colorDescriptor.SetAttribute( attribute, value );
                ret = EGL_TRUE;
                }
            break;
            }

        case EGL_DEPTH_SIZE:
            {
            if( value == EGL_DONT_CARE ||
                value >= 0 )
                {
                m_depthSize = value;
                ret = EGL_TRUE;
                }
            break;
            }
        case EGL_STENCIL_SIZE:
            {
            if( value == EGL_DONT_CARE ||
                value >= 0 )
                {
                m_stencilSize = value;
                ret = EGL_TRUE;
                }
            break;
            }
        case EGL_CONFIG_CAVEAT:
            {
            if( value == EGL_DONT_CARE ||
                value == EGL_NONE ||
                value == EGL_SLOW_CONFIG ||
                value == EGL_NON_CONFORMANT_CONFIG )
                {
                m_configCaveat = value;
                ret = EGL_TRUE;
                }
            break;
            }
        case EGL_CONFIG_ID:
            {
            if( value == EGL_DONT_CARE ||
                value > 0 )
                {
                m_configId = value;
                ret = EGL_TRUE;
                }
            break;
            }
        case EGL_LEVEL:
            {
            // \todo validate value
            m_level = value;
            ret = EGL_TRUE;
            break;
            }

        case EGL_NATIVE_VISUAL_ID:
        case EGL_MAX_PBUFFER_HEIGHT:
        case EGL_MAX_PBUFFER_PIXELS:
        case EGL_MAX_PBUFFER_WIDTH:
            {
            // \todo?
            break;
            }

        case EGL_NATIVE_RENDERABLE:
            {
            if( value == EGL_DONT_CARE ||
                value == EGL_TRUE ||
                value == EGL_FALSE )
                {
                m_nativeRenderable = value;
                ret = EGL_TRUE;
                }
            break;
            }

        case EGL_NATIVE_VISUAL_TYPE:
            {
            if( value == EGL_DONT_CARE ||
                value >= 0 )
                {
                m_nativeVisualType = value;
                ret = EGL_TRUE;
                }
            break;
            }
        case EGL_SAMPLES:
            {
            if( value == EGL_DONT_CARE ||
                value >= 0 )
                {
                m_samples = value;
                ret = EGL_TRUE;
                }
            break;
            }
        case EGL_SAMPLE_BUFFERS:
            {
            if( value == EGL_DONT_CARE ||
                value >= 0 )
                {
                m_sampleBuffers = value;
                ret = EGL_TRUE;
                }
            break;
            }
        case EGL_SURFACE_TYPE:
            {
            if( (value == EGL_DONT_CARE ||
                !(value & ~(EGL_WINDOW_BIT |
                            EGL_PIXMAP_BIT |
                            EGL_PBUFFER_BIT |
                            EGL_MULTISAMPLE_RESOLVE_BOX_BIT |
                            EGL_SWAP_BEHAVIOR_PRESERVED_BIT |
                            EGL_VG_COLORSPACE_LINEAR_BIT |
                            EGL_VG_ALPHA_FORMAT_PRE_BIT))) )
                {
                m_colorDescriptor.SetSurfaceType(value);
                m_surfaceType = value;
                ret = EGL_TRUE;
                }
            break;
            }
        case EGL_TRANSPARENT_TYPE:
            {
            if( (value == EGL_DONT_CARE ||
                 value == EGL_NONE ||
                 value == EGL_TRANSPARENT_RGB) )
                {
                m_transparentColor.SetAttribute( attribute, value );
                ret = EGL_TRUE;
                }
            break;
            }
        case EGL_TRANSPARENT_BLUE_VALUE:
        case EGL_TRANSPARENT_GREEN_VALUE:
        case EGL_TRANSPARENT_RED_VALUE:
            {
            if( value == EGL_DONT_CARE ||
                value >= 0 )
                {
                // \todo If EGL_TRANSPARENT_TYPE is EGL_NONE, then the values for EGL_-
                //       TRANSPARENT_RED_VALUE, EGL_TRANSPARENT_GREEN_VALUE, and EGL_-
                //       TRANSPARENT_BLUE_VALUE are undefined. Otherwise, they are interpreted as
                //       integer framebuffer values between 0 and the maximum framebuffer value for the
                //       component. For example, EGL_TRANSPARENT_RED_VALUE will range between 0
                //       and 2^EGL_RED_SIZE - 1.
                m_transparentColor.SetAttribute( attribute, value );
                ret = EGL_TRUE;
                }
            break;
            }
        case EGL_BIND_TO_TEXTURE_RGB:
            {
            if( value == EGL_DONT_CARE ||
                value == EGL_TRUE ||
                value == EGL_FALSE )
                {
                m_bindToTexRGB = value;
                ret = EGL_TRUE;
                }
            break;
            }
        case EGL_BIND_TO_TEXTURE_RGBA:
            {
            if( value == EGL_DONT_CARE ||
                value == EGL_TRUE ||
                value == EGL_FALSE )
                {
                m_bindToTexRGBA = value;
                ret = EGL_TRUE;
                }
            break;
            }
        case EGL_MIN_SWAP_INTERVAL:
            {
            if( value == EGL_DONT_CARE ||
                value >= 0 )
                {
                m_minSwapInterval = value;
                ret = EGL_TRUE;
                }
            break;
            }
        case EGL_MAX_SWAP_INTERVAL:
            {
            if( value == EGL_DONT_CARE ||
                value >= 0 )
                {
                m_maxSwapInterval = value;
                ret = EGL_TRUE;
                }
            break;
            }
        case EGL_RENDERABLE_TYPE:
            {
            if( value == EGL_DONT_CARE ||
                !(value & ~(EGL_OPENGL_BIT |
                            EGL_OPENGL_ES_BIT |
                            EGL_OPENGL_ES2_BIT |
                            EGL_OPENVG_BIT)) )
                {
                m_renderableType = value;
                ret = EGL_TRUE;
                }
            break;
            }
        case EGL_CONFORMANT:
            {
            if( value == EGL_DONT_CARE ||
                !(value & ~(EGL_OPENGL_BIT |
                            EGL_OPENGL_ES_BIT |
                            EGL_OPENGL_ES2_BIT |
                            EGL_OPENVG_BIT)) )
                {
                m_conformant = value;
                ret = EGL_TRUE;
                }
            break;
            }

        default:
            {
            // Unknown attribute
            EGLI_ASSERT( false );
            }
        }
    return ret;
    }
	
EGLint CEGLConfig::GetAttribute( EGLint attribute ) const
    {
    EGLint ret = 0;
    switch( attribute )
        {
        case EGL_BUFFER_SIZE:
            {
            ret = m_bufferSize;
            break;
            }
        case EGL_BLUE_SIZE:
        case EGL_GREEN_SIZE:
        case EGL_RED_SIZE:
        case EGL_LUMINANCE_SIZE:
        case EGL_ALPHA_SIZE:
        case EGL_ALPHA_MASK_SIZE:
        case EGL_COLOR_BUFFER_TYPE:
            {
            ret = m_colorDescriptor.GetAttribute( attribute );
            break;
            }

        case EGL_DEPTH_SIZE:
            {
            ret = m_depthSize;
            break;
            }
        case EGL_STENCIL_SIZE:
            {
            ret = m_stencilSize;
            break;
            }
        case EGL_CONFIG_CAVEAT:
            {
            ret = m_configCaveat;
            break;
            }
        case EGL_CONFIG_ID:
            {
            ret = m_configId;
            break;
            }
        case EGL_LEVEL:
            {
            ret = m_level;
            break;
            }

        case EGL_NATIVE_VISUAL_ID:
            {
            ret = m_nativeVisualId;
            break;
            }
        case EGL_MAX_PBUFFER_HEIGHT:
            {
            ret = m_maxPbufferHeight;
            break;
            }
        case EGL_MAX_PBUFFER_PIXELS:
            {
            ret = m_maxPbufferPixels;
            break;
            }
        case EGL_MAX_PBUFFER_WIDTH:
            {
            ret = m_maxPbufferWidth;
            break;
            }
        case EGL_NATIVE_RENDERABLE:
            {
            ret = m_nativeRenderable;
            break;
            }
        case EGL_NATIVE_VISUAL_TYPE:
            {
            ret = m_nativeVisualType;
            break;
            }
        case EGL_SAMPLES:
            {
            ret = m_samples;
            break;
            }
        case EGL_SAMPLE_BUFFERS:
            {
            ret = m_sampleBuffers;
            break;
            }
        case EGL_SURFACE_TYPE:
            {
            ret = m_surfaceType;
            break;
            }
        case EGL_TRANSPARENT_TYPE:
            {
            ret = m_transparentColor.Type();
            break;
            }
        case EGL_TRANSPARENT_BLUE_VALUE:
            {
            ret = m_transparentColor.BlueValue();
            break;
            }
        case EGL_TRANSPARENT_GREEN_VALUE:
            {
            ret = m_transparentColor.GreenValue();
            break;
            }
        case EGL_TRANSPARENT_RED_VALUE:
            {
            ret = m_transparentColor.RedValue();
            break;
            }
        case EGL_BIND_TO_TEXTURE_RGB:
            {
            ret = m_bindToTexRGB;
            break;
            }
        case EGL_BIND_TO_TEXTURE_RGBA:
            {
            ret = m_bindToTexRGBA;
            break;
            }
        case EGL_MIN_SWAP_INTERVAL:
            {
            ret = m_minSwapInterval;
            break;
            }
        case EGL_MAX_SWAP_INTERVAL:
            {
            ret = m_maxSwapInterval;
            break;
            }
        case EGL_RENDERABLE_TYPE:
            {
            ret = m_renderableType;
            break;
            }
        case EGL_CONFORMANT:
            {
            ret = m_conformant;
            break;
            }

        default:
            {
            ret = EGL_BAD_ATTRIBUTE;
            }
        }
    return ret;
    }

bool CEGLConfig::operator<( const CEGLConfig& rhs ) const
    {
    if( this == &rhs ) return false;

    return !(operator>(rhs));
    }

bool CEGLConfig::operator>( const CEGLConfig& rhs ) const
    {
    if( this == &rhs ) return false;
    
    // 1 Special: by EGL_CONFIG_CAVEAT where the precedence is EGL_NONE,
    //   EGL_SLOW_CONFIG, EGL_NON_CONFORMANT_CONFIG.    
    if( m_configCaveat != rhs.m_configCaveat )
        {
        // \note depends on EGL_NON_CONFORMANT_CONFIG > EGL_SLOW_CONFIG       
        if( rhs.m_configCaveat < m_configCaveat )
            return false;
        if( m_configCaveat > rhs.m_configCaveat )
            return true;
        }

    // 2 Special: by EGL_COLOR_BUFFER_TYPE where the precedence is EGL_-
    //   RGB_BUFFER, EGL_LUMINANCE_BUFFER.
    if( !(m_colorDescriptor.isLuminance()) && rhs.m_colorDescriptor.isLuminance() )
        return true;
    if( m_colorDescriptor.isLuminance() && !(rhs.m_colorDescriptor.isLuminance()) )
        return false;

    // 3 Special: by larger total number of color bits (for an RGB color buffer,
    //   this is the sum of EGL_RED_SIZE, EGL_GREEN_SIZE, EGL_BLUE_SIZE,
    //   and EGL_ALPHA_SIZE; for a luminance color buffer, the sum of EGL_-
    //   LUMINANCE_SIZE and EGL_ALPHA_SIZE)
    
    // \todo If the requested number of bits in attrib list for a particular color
    //       component is 0 or EGL_DONT_CARE, then the number of bits for that component
    //       is not considered.
    
    if( m_bufferSize > rhs.m_bufferSize )
        return true;
    if( rhs.m_bufferSize > m_bufferSize )
        return false;
    
    if( m_sampleBuffers > rhs.m_sampleBuffers )
        return true;
    if( rhs.m_sampleBuffers > m_sampleBuffers )
        return false;
    
    if( m_samples > rhs.m_samples )
        return true;
    if( rhs.m_samples > m_samples )
        return false;

    if( m_depthSize > rhs.m_depthSize )
        return true;
    if( rhs.m_depthSize > m_depthSize )
        return false;

    if( m_stencilSize > rhs.m_stencilSize )
        return true;
    if( rhs.m_stencilSize > m_stencilSize )
        return false;

    if( m_colorDescriptor.AlphaMaskSize() > rhs.m_colorDescriptor.AlphaMaskSize() )
        return true;
    if( rhs.m_colorDescriptor.AlphaMaskSize() > m_colorDescriptor.AlphaMaskSize() )
        return false;

    // \todo Special: by EGL_NATIVE_VISUAL_TYPE (the actual sort order is
    //       implementation-defined, depending on the meaning of native visual types).

    if( m_configId > rhs.m_configId )
        return true;
    if( rhs.m_configId > m_configId )
        return false;

    // Shouldn't reach this point.
    EGLI_ASSERT( false );
    return false;
    }

bool CEGLConfig::Match( const CEGLConfig& filter ) const
    {
    if( this == &filter ) return true;

    if( m_bufferSize < filter.m_bufferSize ) return false;

    if( m_colorDescriptor.RedSize() < filter.m_colorDescriptor.RedSize() ||
        m_colorDescriptor.GreenSize() < filter.m_colorDescriptor.GreenSize() ||
        m_colorDescriptor.BlueSize() < filter.m_colorDescriptor.BlueSize() ||
        m_colorDescriptor.AlphaSize() < filter.m_colorDescriptor.AlphaSize() ) return false;
    
    if( m_colorDescriptor.AlphaMaskSize() < filter.m_colorDescriptor.AlphaMaskSize() ) return false;
    
    if( filter.m_bindToTexRGB != EGL_DONT_CARE && (m_bindToTexRGB != filter.m_bindToTexRGB) ) return false;
    
    if( filter.m_bindToTexRGBA != EGL_DONT_CARE && (m_bindToTexRGBA != filter.m_bindToTexRGBA) ) return false;
    
    if( filter.m_colorDescriptor.Format() != EGL_DONT_CARE )
        {
        EGLint thisBufType = m_colorDescriptor.isLuminance() ? EGL_LUMINANCE_BUFFER : EGL_RGB_BUFFER;
        EGLint filterBufType = filter.m_colorDescriptor.isLuminance() ? EGL_LUMINANCE_BUFFER : EGL_RGB_BUFFER;
        if( thisBufType != filterBufType ) return false;
        }

    if( filter.m_configCaveat != EGL_DONT_CARE && (m_configCaveat != filter.m_configCaveat) ) return false;

    if( m_configId == filter.m_configId ) return false;
    
    if( (filter.m_conformant & m_conformant) != filter.m_conformant ) return false;

    if( m_depthSize < filter.m_depthSize ) return false;
    
    if( m_level != filter.m_level ) return false;
    
    if( filter.m_maxSwapInterval != EGL_DONT_CARE && (m_maxSwapInterval != filter.m_maxSwapInterval) ) return false;

    if( filter.m_minSwapInterval != EGL_DONT_CARE && (m_minSwapInterval != filter.m_minSwapInterval) ) return false;

    if( filter.m_nativeRenderable != EGL_DONT_CARE && (m_nativeRenderable != filter.m_nativeRenderable) ) return false;

    if( filter.m_nativeVisualType != EGL_DONT_CARE && (m_nativeVisualType != filter.m_nativeVisualType) ) return false;

    if( (filter.m_renderableType & m_renderableType) != filter.m_renderableType ) return false;

    if( m_sampleBuffers < filter.m_sampleBuffers ) return false;

    if( m_samples < filter.m_samples ) return false;

    if( m_stencilSize < filter.m_stencilSize ) return false;

    if( (filter.m_surfaceType & m_surfaceType) != filter.m_surfaceType ) return false;
    
    if( filter.m_transparentColor.Type() != m_transparentColor.Type() ) return false;
    
    return true;
    }

CEGLConfig& CEGLConfig::operator=( const CEGLConfig& rhs )
    {
    if( this == &rhs ) return *this;

    m_bufferSize        = rhs.m_bufferSize;
    m_colorDescriptor   = rhs.m_colorDescriptor;
    m_bindToTexRGB      = rhs.m_bindToTexRGB;
    m_bindToTexRGBA     = rhs.m_bindToTexRGBA;
    m_configCaveat      = rhs.m_configCaveat;
    m_configId          = rhs.m_configId;
    m_conformant        = rhs.m_conformant;
    m_depthSize         = rhs.m_depthSize;
    m_level             = rhs.m_level;
    m_maxPbufferWidth   = rhs.m_maxPbufferWidth;
    m_maxPbufferHeight  = rhs.m_maxPbufferHeight;
    m_maxPbufferPixels  = rhs.m_maxPbufferPixels;
    m_maxSwapInterval   = rhs.m_maxSwapInterval;
    m_minSwapInterval   = rhs.m_minSwapInterval;
    m_nativeRenderable  = rhs.m_nativeRenderable;
    m_nativeVisualId    = rhs.m_nativeVisualId;
    m_nativeVisualType  = rhs.m_nativeVisualType;
    m_renderableType    = rhs.m_renderableType;
    m_sampleBuffers     = rhs.m_sampleBuffers;
    m_samples           = rhs.m_samples;
    m_stencilSize       = rhs.m_stencilSize;
    m_surfaceType       = rhs.m_surfaceType;    
    m_transparentColor  = rhs.m_transparentColor;
    return *this;
    }

bool CEGLConfig::IsCompatible( const CEGLConfig& config, const EGLenum contextApi ) const
    {
    if( m_colorDescriptor.isLuminance() != config.m_colorDescriptor.isLuminance() )
        return false;

    if( m_colorDescriptor.isLuminance() &&
        (m_colorDescriptor.m_luminanceSize != config.m_colorDescriptor.m_luminanceSize ||
         m_colorDescriptor.m_alphaSize != config.m_colorDescriptor.m_alphaSize) )
        return false;
    else if ( m_colorDescriptor.m_redSize != config.m_colorDescriptor.m_redSize ||
              m_colorDescriptor.m_greenSize != config.m_colorDescriptor.m_greenSize ||
              m_colorDescriptor.m_blueSize != config.m_colorDescriptor.m_blueSize ||
              m_colorDescriptor.m_alphaSize != config.m_colorDescriptor.m_alphaSize )
        return false;

    if( m_samples != config.m_samples )
        return false;
    
    switch( contextApi )
        {
        case EGL_OPENVG_API:
            {
            if( m_colorDescriptor.m_alphaMaskSize != config.m_colorDescriptor.m_alphaMaskSize )
                return false;
            break;
            }
        case EGL_OPENGL_ES_API:
            {
            if( m_stencilSize != config.m_stencilSize )
                return false;
            if( m_depthSize != config.m_depthSize )
                return false;

            break;
            }
        }
    return true;
    }

