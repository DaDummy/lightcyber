#ifndef DEMOAPI_H
#define DEMOAPI_H

extern "C" {
#include "config_types.h"
}


struct OpenGLSettings;

class DemoApi
{
public:
	static DemoApi api;

public:
	const OpenGLSettings& getOpenGlSettings();
	void initializeGL();
	void resizeView(int w, int h);

	void setTimeNow(double t_now);
	void renderFrame();

private:
	DemoApi() = default;
};

#endif // DEMOAPI_H
