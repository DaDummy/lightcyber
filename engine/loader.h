#ifndef LOADER_H
#define LOADER_H

#include "renderer.h"
#include "shader.h"

// Generated resources
#include "gfx/load.h"

// --- Begin of section that should be moved to gfx/load.h ---
// Loading bar shader and uniforms
static GLint load_progress_location, load_time_location, load_resolution_location;
static struct Uniform load_uniforms[] = {
    { &load_progress_location, "iProgress" },
    { &load_time_location, "iTime" },
    { &load_resolution_location, "iResolution" },
    { NULL, NULL }
};

static struct ShaderProgram loadingBar = {
    0,
    // TODO(ca) Make load_frag a compile-time constant
    NULL, 0,
    load_uniforms,
};
// --- End of section that should be moved to gfx/load.h ---


static float progress = 0.f;

static void lInitializeLoader()
{
    // Load loading bar shader
    loadingBar.fragment_source = load_frag;
    sCompileShaderProgram(&loadingBar);
}

static void lRenderLoadingScreen()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(loadingBar.handle);
    glUniform2f(load_resolution_location, w, h);
    glUniform1f(load_progress_location, progress);

    quad();
}

#endif // LOADER_H
