#ifndef CONFIG_H
#define CONFIG_H

#include "config_types.h"

static const struct OpenGLSettings opengl_settings = {
    CompatibilityProfile, // profile
	DoubleBuffer, // swapChain

    2, // majorVersion
    1, // minorVersion

	0, // useSRGB

	8, // redBits
	8, // greenBits
	8, // blueBits
	8, // alphaBits

	24, // depthBits
	8, // stencilBits

	1, // samples (for Multisampling)
};

#endif
