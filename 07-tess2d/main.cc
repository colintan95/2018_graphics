#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iterator>

#include <SDL2/SDL.h>
#include <OpenGL/gl3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// Forward Declarations
bool CompileShader(GLuint shader_id, const std::string& path);
bool LinkProgram(GLuint program_id, GLuint vert_shader_id,
                 GLuint tess_ctl_shader_id, GLuint tess_eval_shader_id,
                 GLuint geom_shader_id, GLuint frag_shader_id);

// Constants
unsigned int kScreenWidth = 1024;
unsigned int kScreenHeight = 768;

// Globals
GLuint program_id;
GLuint vertex_array_id;
GLuint position_buffer_id;

void Render(SDL_Window* window, SDL_GLContext* gl_context) {
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(program_id);

  glBindVertexArray(vertex_array_id);
  glDrawArrays(GL_PATCHES, 0, 4);

  glBindVertexArray(0);
  glUseProgram(0);
  
  SDL_GL_SwapWindow(window);
}

void InitShaderVariables() {
  
  // Set uniforms
  GLint outer_tess_level_loc = glGetUniformLocation(program_id,
                                                    "outer_tess_level");
  glUniform1i(outer_tess_level_loc, 8);

  GLint inner_tess_level_loc = glGetUniformLocation(program_id,
                                                    "inner_tess_level");
  glUniform1i(inner_tess_level_loc, 8);
  
  GLint mvp_mat_loc = glGetUniformLocation(program_id, "mvp_mat");
  glm::mat4 model_mat = glm::mat4(1.f);
  glm::mat4 view_mat = glm::translate(glm::mat4(1.f),
                                      glm::vec3(0.f, 0.f, -5.f));
  glm::mat4 proj_mat = glm::perspective(45.f,
                                        static_cast<float>(kScreenWidth) /
                                        static_cast<float>(kScreenHeight)
                                        , 0.1f, 1000.f);
  glm::mat4 mvp_mat = proj_mat * view_mat * model_mat;
  glUniformMatrix4fv(mvp_mat_loc, 1, GL_FALSE, glm::value_ptr(mvp_mat));

  GLint viewport_mat_loc = glGetUniformLocation(program_id, "viewport_mat");
  float l = 0.f;
  float r = static_cast<float>(kScreenWidth);
  float t = 0.f;
  float b = static_cast<float>(kScreenHeight);
  float f = 1.f;
  float n = 0.f;
  glm::mat4 viewport_mat = {(r-l)/2.f, 0, 0, (r+l)/2.f,
                            0, (t-b)/2, 0, (t+b)/2.f,
                            0, 0, (f-n)/2.f, (f+n)/2.f,
                            0, 0, 0, 1};
  glUniformMatrix4fv(viewport_mat_loc, 1, GL_FALSE,
                     glm::value_ptr(viewport_mat));

  GLint line_width_loc = glGetUniformLocation(program_id, "line_width");
  glUniform1f(line_width_loc, 2.f);
  
  GLint line_color_loc = glGetUniformLocation(program_id, "line_color");
  glm::vec4 line_color = glm::vec4(1.f, 0.f, 0.f, 1.f);
  glUniform4fv(line_color_loc, 1, glm::value_ptr(line_color));

  GLint quad_color_loc = glGetUniformLocation(program_id, "quad_color");
  glm::vec4 quad_color = glm::vec4(1.f, 1.f, 1.f, 1.f);
  glUniform4fv(quad_color_loc, 1, glm::value_ptr(quad_color));

  float pos_data[] = {-1.f, -1.f, 1.f, -1.f,
                      1.f, 1.f, -1.f, 1.f};
  
  // Create the position vertex buffer
  glGenBuffers(1, &position_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(pos_data), pos_data, GL_STATIC_DRAW);
  
  // Create the vertex array object
  glGenVertexArrays(1, &vertex_array_id);
  glBindVertexArray(vertex_array_id);

  // Vertex array specification
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer_id);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);
}

void DestroyShaderVariables() {
  glDeleteBuffers(1, &position_buffer_id);
  glDeleteVertexArrays(1, &vertex_array_id);
  glDeleteProgram(program_id);
}

void InitGL() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  glViewport(0, 0, kScreenWidth, kScreenHeight);

  glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void CreateProgram() {
  // Create and compile shaders
  GLuint vert_shader_id = glCreateShader(GL_VERTEX_SHADER);
  GLuint tess_ctl_shader_id = glCreateShader(GL_TESS_CONTROL_SHADER);
  GLuint tess_eval_shader_id = glCreateShader(GL_TESS_EVALUATION_SHADER);
  GLuint geom_shader_id = glCreateShader(GL_GEOMETRY_SHADER);
  GLuint frag_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
  
  if (!CompileShader(vert_shader_id, "tess2d.vs")) {
    std::cerr << "Could not compile vertex shader" << std::endl;
  }
  if (!CompileShader(tess_ctl_shader_id, "tess2d.tcs")) {
    std::cerr << "Could not compile tesselation control shader" << std::endl;
  }
  if (!CompileShader(tess_eval_shader_id, "tess2d.tes")) {
    std::cerr << "Could not compile tesselation evaluation shader"
              << std::endl;
  }
  if (!CompileShader(geom_shader_id, "tess2d.gs")) {
    std::cerr << "Could not compile geometry shader" << std::endl;
  }  
  if (!CompileShader(frag_shader_id, "tess2d.fs")) {
    std::cerr << "Could not compile fragment shader" << std::endl;
  }

  // Create and link program
  program_id = glCreateProgram();
  if (!LinkProgram(program_id, vert_shader_id, tess_ctl_shader_id,
                   tess_eval_shader_id, geom_shader_id, frag_shader_id)) {
    std::cerr << "Could not link program" << std::endl;
    exit(1);
  }
  glDeleteShader(vert_shader_id);
  glDeleteShader(tess_ctl_shader_id);
  glDeleteShader(tess_eval_shader_id);
  glDeleteShader(geom_shader_id);
  glDeleteShader(frag_shader_id);
  glUseProgram(program_id);
}


int main() {
   
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
    SDL_Log("Failed to initialized SDL: %s", SDL_GetError());
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  // To use GLSL version 330
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                      SDL_GL_CONTEXT_PROFILE_CORE);
  
  SDL_Window* window = SDL_CreateWindow("Hello, World!",
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        kScreenWidth, // window width
                                        kScreenHeight, // window height
                                        SDL_WINDOW_OPENGL);

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  
  if (gl_context == NULL) {
    std::cout << "OpenGL context could not be created: " << SDL_GetError()
              << std::endl;
    exit(1);
  }

  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << std::endl;

  InitGL();

  CreateProgram();

  InitShaderVariables();
  
  bool should_quit = false;
  
  while (!should_quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        should_quit = true;
      }
    }

    Render(window, &gl_context);
  }

  DestroyShaderVariables();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}                

bool CompileShader(GLuint shader_id, const std::string& path) {
  std::ifstream shader_fin(path);
  std::string shader_source =
    std::string(std::istreambuf_iterator<char>(shader_fin),
                std::istreambuf_iterator<char>());

  const char* shader_source_ptr = shader_source.c_str();
  glShaderSource(shader_id, 1, &shader_source_ptr, NULL);
  std::cout << "Compiling shader: " << path << std::endl;
  glCompileShader(shader_id);

  GLint result;
  GLint infolog_length;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
  glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &infolog_length);

  if (infolog_length > 0) {
    std::vector<char> infolog_buf(infolog_length);
    glGetShaderInfoLog(shader_id, infolog_length, NULL, &infolog_buf[0]);
    std::cout << &infolog_buf[0] << std::endl;
  }

  if (result == GL_FALSE) {
    return false;
  }

  return true;
}

// Shader id should be 0 if unused
bool LinkProgram(GLuint program_id, GLuint vert_shader_id,
                 GLuint tess_ctl_shader_id, GLuint tess_eval_shader_id,
                 GLuint geom_shader_id, GLuint frag_shader_id) {

  // (TODO:) Validate program_id and shaders
  
  glAttachShader(program_id, vert_shader_id);
  glAttachShader(program_id, frag_shader_id);
  if (geom_shader_id > 0) {
    glAttachShader(program_id, geom_shader_id);
  }
  if (tess_ctl_shader_id > 0) {
    glAttachShader(program_id, tess_ctl_shader_id);
  }
  if (tess_eval_shader_id > 0) {
    glAttachShader(program_id, tess_eval_shader_id);
  }
  
  glLinkProgram(program_id);

  GLint link_result;
  GLint infolog_length;
  glGetProgramiv(program_id, GL_LINK_STATUS, &link_result);
  glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infolog_length);
    
  if (infolog_length > 0) {
    std::vector<char> infolog(infolog_length);
    glGetProgramInfoLog(program_id, infolog_length, NULL, &infolog[0]);
    std::cout << &infolog[0] << std::endl;
  }

  if (link_result == GL_FALSE) {
    return false;
  }

  return true;
}
