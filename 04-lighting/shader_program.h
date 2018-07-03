#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include <string>

#include <OpenGL/gl3.h>
#include "glm/glm.hpp"

class ShaderProgram {
 public:
  void UseProgram();

  bool AttachAndCompileVertexShader(const std::string& path);
  bool AttachAndCompileFragmentShader(const std::string& path);
  
  void SetUniform(const std::string& name, float val);
  void SetUniform(const std::string& name, const glm::vec3& val);
  void SetUniform(const std::string& name, const glm::mat3& val);
  void SetUniform(const std::string& name, const glm::mat4& val);

 private:
  bool CompileShader(GLuint shader_id, const std::string& path);
};

#endif
