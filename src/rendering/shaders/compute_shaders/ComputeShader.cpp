#include "ComputeShader.h"

#include <GL/glew.h>

#include "core/logging/Logging.h"

namespace Vox
{
    ComputeShader::ComputeShader(const std::string& shaderFilePath)
    {
        shader = glCreateProgram();

        const std::optional<unsigned int> computeShaderId = LoadShaderStage(shaderFilePath, GL_COMPUTE_SHADER);
        if (!computeShaderId)
        {
            glDeleteProgram(shader);
            return;
        }

        glAttachShader(shader, computeShaderId.value());
        if (Link())
        {
            VoxLog(Display, Rendering, "Compute shader created successfully: ProgramId: '{}'", shader);
        }
    }
}
