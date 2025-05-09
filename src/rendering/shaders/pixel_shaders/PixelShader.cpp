#include "PixelShader.h"

#include <GL/glew.h>

#include <utility>

#include "core/logging/Logging.h"

namespace Vox
{
    PixelShader::PixelShader(std::string vertexShaderFilePath, std::string fragmentShaderFilePath)
    {
        Load(std::move(vertexShaderFilePath), std::move(fragmentShaderFilePath));
        Enable();
    }

    PixelShader::~PixelShader()
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    bool PixelShader::Load(std::string vertexShaderFilePath, std::string fragmentShaderFilePath)
    {
        shader = glCreateProgram();

        const std::optional<unsigned int> vertexShaderId = LoadShaderStage(std::move(vertexShaderFilePath), GL_VERTEX_SHADER);
        if (!vertexShaderId)
        {
            glDeleteProgram(shader);
            return false;
        }
        vertexShader = vertexShaderId.value();

        const std::optional<unsigned int> fragmentShaderId = LoadShaderStage(std::move(fragmentShaderFilePath), GL_FRAGMENT_SHADER);
        if (!fragmentShaderId)
        {
            glDeleteProgram(shader);
            return false;
        }
        fragmentShader = fragmentShaderId.value();

        glAttachShader(shader, vertexShader);
        glAttachShader(shader, fragmentShader);
        if (Link())
        {
            VoxLog(Display, Rendering, "ShaderProgram created successfully: ProgramId: '{}', ['{}', '{}']", shader, vertexShader, fragmentShader);
            return true;
        }
        return false;
    }
}