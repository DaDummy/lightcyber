#ifndef SHADER_H
#define SHADER_H

#include "platform.h"
#include "renderer.h"


struct Uniform
{
    GLint* location;
    const char* name;
};

struct ShaderProgram
{
    GLuint handle;

    // GL_FRAGMENT_SHADER
    const char* fragment_source;
    GLuint fragment_handle;

    struct Uniform* uniforms;
};


static GLuint sCompileShader(const char* source, GLuint type)
{
    GLuint shaderHandle = glCreateShader(type);
    glShaderSource(shaderHandle, 1, (const GLchar **)&source, NULL);
    glCompileShader(shaderHandle);

    return shaderHandle;
}

static void sCompileShaderProgram(struct ShaderProgram* shaderProgram)
{
    GLuint programHandle = glCreateProgram();

    if (shaderProgram->fragment_source != NULL)
    {
        shaderProgram->fragment_handle = sCompileShader(shaderProgram->fragment_source, GL_FRAGMENT_SHADER);
        glAttachShader(programHandle, shaderProgram->fragment_handle);
    }

    glLinkProgram(programHandle);
    glUseProgram(programHandle);

    for (struct Uniform* uniform = shaderProgram->uniforms; uniform->name != NULL; ++uniform)
    {
        *uniform->location = glGetUniformLocation(programHandle, uniform->name);
    }

    shaderProgram->handle = programHandle;
}

#endif // SHADER_H
