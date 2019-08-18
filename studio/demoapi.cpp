#include "demoapi.h"

extern "C" {
#include "config.h"
#include "engine/renderer.h"
#include "engine/loader.h"
}


DemoApi DemoApi::api;

const OpenGLSettings &DemoApi::getOpenGlSettings()
{
    return opengl_settings;
}

void DemoApi::initializeGL()
{
    rInitializeRenderer();
    lInitializeLoader();
}

void DemoApi::resizeView(int w, int h)
{
    ::w = w;
    ::h = h;
    glViewport(0, 0, w, h);
}

void DemoApi::setTimeNow(double t)
{
    // TODO(ca) Implement me
    //t_now = t;
}

void DemoApi::renderFrame()
{
    lRenderLoadingScreen();
}
