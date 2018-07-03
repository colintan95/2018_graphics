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

#include "model.h"

// Forward Declarations
bool CompileShader(GLuint shader_id, const std::string& path);
bool LinkProgram(GLuint program_id, GLuint vert_shader_id,
                 GLuint geom_shader_id, GLuint frag_shader_id);

// Constants
unsigned int kScreenWidth = 1024;
unsigned int kScreenHeight = 768;

// Globals
Model teapot_model;
GLuint program_id;
GLuint vertex_array_id;
GLuint position_buffer_id;
GLuint normal_buffer_id;

void Render(SDL_Window* window, SDL_GLContext* gl_context) {
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(program_id);

  glBindVertexArray(vertex_array_id);
  glDrawArrays(GL_TRIANGLES, 0, teapot_model.vert_count);

  glBindVertexArray(0);
  glUseProgram(0);
  
  SDL_GL_SwapWindow(window);
}

void InitShaderVariables() {
  
  // Set uniforms
  GLint light_pos_loc = glGetUniformLocation(program_id,
                                             "light_pos");
  glm::vec3 light_pos = glm::vec3(0.f, 10.f, 20.f);
  glUniform3fv(light_pos_loc, 1, glm::value_ptr(light_pos));

  GLint diffuse_param_loc = glGetUniformLocation(program_id, "diffuse_param");
  glm::vec3 diffuse_param = glm::vec3(1.f, 1.f, 1.f);
  glUniform3fv(diffuse_param_loc, 1, glm::value_ptr(diffuse_param));

  GLint ambient_param_loc = glGetUniformLocation(program_id, "ambient_param");
  glm::vec3 ambient_param = glm::vec3(1.f, 0.f, 0.f);
  glUniform3fv(ambient_param_loc, 1, glm::value_ptr(ambient_param));

  GLint specular_param_loc = glGetUniformLocation(program_id,
                                                  "specular_param");
  glm::vec3 specular_param = glm::vec3(1.f, 1.f, 1.f);
  glUniform3fv(specular_param_loc, 1, glm::value_ptr(specular_param));

  GLint shininess_loc = glGetUniformLocation(program_id, "shininess");
  glUniform1f(shininess_loc, 4.f);
  
  GLint model_mat_loc = glGetUniformLocation(program_id, "model_mat");
  glm::mat4 model_mat = glm::rotate(glm::mat4(1.f), -1.f,
                                    glm::vec3(1.f, 0.f, 0.f));
  glUniformMatrix4fv(model_mat_loc, 1, GL_FALSE, glm::value_ptr(model_mat));
  
  GLint view_mat_loc = glGetUniformLocation(program_id, "view_mat");
  glm::mat4 view_mat = glm::translate(glm::mat4(1.f),
                                      glm::vec3(0.f, 0.f, -50.f));
  glUniformMatrix4fv(view_mat_loc, 1, GL_FALSE, glm::value_ptr(view_mat));

  GLint proj_mat_loc = glGetUniformLocation(program_id, "proj_mat");
  glm::mat4 proj_mat = glm::perspective(45.f,
                                        static_cast<float>(kScreenWidth) /
                                        static_cast<float>(kScreenHeight)
                                        , 0.1f, 1000.f);
  glUniformMatrix4fv(proj_mat_loc, 1, GL_FALSE, glm::value_ptr(proj_mat));

  GLint normal_mat_loc = glGetUniformLocation(program_id, "normal_mat");
  glm::mat4 normal_mat = glm::transpose(glm::inverse(view_mat * model_mat));
  glUniformMatrix4fv(normal_mat_loc, 1, GL_FALSE, glm::value_ptr(normal_mat));

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

  GLint line_info_width_loc = glGetUniformLocation(program_id,
                                                   "line_info.width");
  glUniform1f(line_info_width_loc, 1.f);

  GLint line_info_color_loc = glGetUniformLocation(program_id,
                                                   "line_info.color");
  glm::vec4 line_info_color = {1.f, 0.f, 0.f, 1.f};
  glUniform4fv(line_info_color_loc, 1, glm::value_ptr(line_info_color));

  CreateModelFromFile("../assets/teapot.obj", &teapot_model);
  
  // Create the position vertex buffer
  glGenBuffers(1, &position_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer_id);
  glBufferData(GL_ARRAY_BUFFER, 3 * teapot_model.vert_count * sizeof(float),
               &teapot_model.positions[0][0], GL_STATIC_DRAW);

  // Create the normal vertex buffer
  glGenBuffers(1, &normal_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_id);
  glBufferData(GL_ARRAY_BUFFER, 3 * teapot_model.vert_count * sizeof(float),
               &teapot_model.normals[0][0], GL_STATIC_DRAW);
  
  // Create the vertex array object
  glGenVertexArrays(1, &vertex_array_id);
  glBindVertexArray(vertex_array_id);

  // Vertex array specification
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer_id);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_id);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);
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
}

void CreateProgram() {
  // Create and compile shaders
  GLuint vert_shader_id = glCreateShader(GL_VERTEX_SHADER);
  GLuint geom_shader_id = glCreateShader(GL_GEOMETRY_SHADER);
  GLuint frag_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
  if (!CompileShader(vert_shader_id, "wireframe.vs")) {
    std::cerr << "Could not compile vertex shader" << std::endl;
  }
  if (!CompileShader(geom_shader_id, "wireframe.gs")) {
    std::cerr << "Could not compile geometry shader" << std::endl;
  }
  if (!CompileShader(frag_shader_id, "wireframe.fs")) {
    std::cerr << "Could not compile fragment shader" << std::endl;
  }

  // Create and link program
  program_id = glCreateProgram();
  if (!LinkProgram(program_id, vert_shader_id,
                   geom_shader_id, frag_shader_id)) {
    std::cerr << "Could not link program" << std::endl;
    exit(1);
  }
  glDeleteShader(vert_shader_id);
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

// Geom shader id should be 0 if unsed
bool LinkProgram(GLuint program_id, GLuint vert_shader_id,
                 GLuint geom_shader_id,  GLuint frag_shader_id) {

  // (TODO:) Validate program_id and shaders
  
  glAttachShader(program_id, vert_shader_id);
  glAttachShader(program_id, frag_shader_id);
  if (geom_shader_id > 0) {
    glAttachShader(program_id, geom_shader_id);
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
