#include "demoapi.h"

extern "C" {
#include "config.h"
}


DemoApi DemoApi::api;

const OpenGLSettings &DemoApi::getOpenGlSettings()
{
    return opengl_settings;
}

void DemoApi::initializeGL()
{
    // TODO(ca) Implement me
}

void DemoApi::resizeView(int w, int h)
{
    // TODO(ca) Implement me
}

void DemoApi::setTimeNow(double t)
{
    // TODO(ca) Implement me
    //t_now = t;
}

void DemoApi::renderFrame()
{
    // TODO(ca) Implement me
}
