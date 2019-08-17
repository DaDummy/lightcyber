#ifndef CONFIG_TYPES_H
#define CONFIG_TYPES_H

enum OpenGLProfile
{
	CoreProfile,
	CompatibilityProfile,
};

enum SwapChain
{
	DoubleBuffer,
	TripleBuffer,
};

struct OpenGLSettings
{
	enum OpenGLProfile profile;
	enum SwapChain swapChain;

	int majorVersion;
	int minorVersion;

	int useSRGB;

	int redBits;
	int greenBits;
	int blueBits;
	int alphaBits;

	int depthBits;
	int stencilBits;

	int samples;
};

#endif
