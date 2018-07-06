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
                 GLuint tess_ctl_shader_id, GLuint tess_eval_shader_id,
                 GLuint geom_shader_id, GLuint frag_shader_id);

// Constants
unsigned int kScreenWidth = 1024;
unsigned int kScreenHeight = 768;

// Globals
Model teapot_model;

GLuint render_program_id;
GLuint filter_program_id;

GLuint render_vao_id;
GLuint filter_vao_id;

GLuint render_pos_buffer_id;
GLuint render_normal_buffer_id;

GLuint filter_pos_buffer_id;
GLuint filter_texcoord_buffer_id;

GLuint render_fbo_id;
GLuint render_tex_id;
GLuint render_depth_rbo_id;

void Render(SDL_Window* window, SDL_GLContext* gl_context) {

  // Renders scene to texture
  glBindFramebuffer(GL_FRAMEBUFFER, render_fbo_id);
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(render_program_id);
  glBindVertexArray(render_vao_id);
  glDrawArrays(GL_TRIANGLES, 0, teapot_model.vert_count);
  glBindVertexArray(0);
  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Passes texture through filter and renders to the screen
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(filter_program_id);
  glBindTexture(GL_TEXTURE_2D, render_tex_id);
  glBindVertexArray(filter_vao_id);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  glUseProgram(0);
  
  SDL_GL_SwapWindow(window);
}

void InitShaderVariables() {

  // Sets up framebuffer for render pass
  
  glGenFramebuffers(1, &render_fbo_id);
  glBindFramebuffer(GL_FRAMEBUFFER, render_fbo_id);

  glGenTextures(1, &render_tex_id);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, render_tex_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, kScreenWidth, kScreenHeight, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_2D, render_tex_id, 0);

  glGenRenderbuffers(1, &render_depth_rbo_id);
  glBindRenderbuffer(GL_RENDERBUFFER, render_depth_rbo_id);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, kScreenWidth,
                        kScreenHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, render_depth_rbo_id);

  glBindBuffer(GL_FRAMEBUFFER, 0);
  
  // Sets uniforms for render program

  glUseProgram(render_program_id);

  GLint model_mat_loc = glGetUniformLocation(render_program_id, "model_mat");
  glm::mat4 model_mat = glm::rotate(glm::mat4(1.f), -1.f,
                                    glm::vec3(1.f, 0.f, 0.f));
  glUniformMatrix4fv(model_mat_loc, 1, GL_FALSE, glm::value_ptr(model_mat));
  
  GLint view_mat_loc = glGetUniformLocation(render_program_id, "view_mat");
  glm::mat4 view_mat = glm::translate(glm::mat4(1.f),
                                      glm::vec3(0.f, 0.f, -50.f));
  glUniformMatrix4fv(view_mat_loc, 1, GL_FALSE, glm::value_ptr(view_mat));

  GLint proj_mat_loc = glGetUniformLocation(render_program_id, "proj_mat");
  glm::mat4 proj_mat = glm::perspective(45.f,
                                        static_cast<float>(kScreenWidth) /
                                        static_cast<float>(kScreenHeight)
                                        , 0.1f, 1000.f);
  glUniformMatrix4fv(proj_mat_loc, 1, GL_FALSE, glm::value_ptr(proj_mat));

  GLint normal_mat_loc = glGetUniformLocation(render_program_id, "normal_mat");
  glm::mat4 normal_mat = glm::transpose(glm::inverse(view_mat * model_mat));
  glUniformMatrix4fv(normal_mat_loc, 1, GL_FALSE, glm::value_ptr(normal_mat));

  GLint light_pos_loc = glGetUniformLocation(render_program_id,
                                             "light_pos");
  glm::vec3 light_pos = glm::vec3(0.f, 10.f, 20.f);
  glUniform3fv(light_pos_loc, 1, glm::value_ptr(light_pos));

  GLint diffuse_param_loc = glGetUniformLocation(render_program_id,
                                                 "diffuse_param");
  glm::vec3 diffuse_param = glm::vec3(1.f, 1.f, 1.f);
  glUniform3fv(diffuse_param_loc, 1, glm::value_ptr(diffuse_param));

  GLint ambient_param_loc = glGetUniformLocation(render_program_id,
                                                 "ambient_param");
  glm::vec3 ambient_param = glm::vec3(1.f, 0.f, 0.f);
  glUniform3fv(ambient_param_loc, 1, glm::value_ptr(ambient_param));

  GLint specular_param_loc = glGetUniformLocation(render_program_id,
                                                  "specular_param");
  glm::vec3 specular_param = glm::vec3(1.f, 1.f, 1.f);
  glUniform3fv(specular_param_loc, 1, glm::value_ptr(specular_param));

  GLint shininess_loc = glGetUniformLocation(render_program_id, "shininess");
  glUniform1f(shininess_loc, 4.f);

  // Sets uniforms for filter program

  glUseProgram(filter_program_id);
  
  GLint render_texture_loc = glGetUniformLocation(filter_program_id,
                                                  "render_texture");
  glUniform1i(render_texture_loc, 0);

  GLint texture_width_loc = glGetUniformLocation(filter_program_id,
                                                 "texture_width");
  glUniform1i(texture_width_loc, kScreenWidth);

  GLint texture_height_loc = glGetUniformLocation(filter_program_id,
                                                  "texture_height");
  glUniform1i(texture_height_loc, kScreenHeight);

  GLint edge_threshold_loc = glGetUniformLocation(filter_program_id,
                                                  "edge_threshold");
  glUniform1f(edge_threshold_loc, 0.2f);

  // Loads model
  CreateModelFromFile("../assets/teapot.obj", &teapot_model);

  // Vertex specification for render program
  
  glGenBuffers(1, &render_pos_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, render_pos_buffer_id);
  glBufferData(GL_ARRAY_BUFFER, 3 * teapot_model.vert_count * sizeof(float),
               &teapot_model.positions[0][0], GL_STATIC_DRAW);

  glGenBuffers(1, &render_normal_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, render_normal_buffer_id);
  glBufferData(GL_ARRAY_BUFFER, 3 * teapot_model.vert_count * sizeof(float),
               &teapot_model.normals[0][0], GL_STATIC_DRAW);
  
  glGenVertexArrays(1, &render_vao_id);
  glBindVertexArray(render_vao_id);

  glBindBuffer(GL_ARRAY_BUFFER, render_pos_buffer_id);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, render_normal_buffer_id);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);

  // Vertices for filter program
  float filter_pos_data[] = {-1.f, -1.f, 0.f, 1.f, -1.f, 0.f, -1.f, 1.f, 0.f,
                             -1.f, 1.f, 0.f, 1.f, -1.f, 0.f, 1.f, 1.f, 0.f};
  float filter_texcoord_data[] = {0.f, 0.f, 1.f, 0.f, 0.f, 1.f,
                                  0.f, 1.f, 1.f, 0.f, 1.f, 1.f};

  // Vertex specification for filter program

  glGenBuffers(1, &filter_pos_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, filter_pos_buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(filter_pos_data), filter_pos_data,
               GL_STATIC_DRAW);

  glGenBuffers(1, &filter_texcoord_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, filter_texcoord_buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(filter_texcoord_data),
               filter_texcoord_data, GL_STATIC_DRAW);

  glGenVertexArrays(1, &filter_vao_id);
  glBindVertexArray(filter_vao_id);

  glBindBuffer(GL_ARRAY_BUFFER, filter_pos_buffer_id);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, filter_texcoord_buffer_id);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);
}

void DestroyShaderVariables() {
  glDeleteRenderbuffers(1, &render_depth_rbo_id);
  glDeleteTextures(1, &render_tex_id);
  glDeleteFramebuffers(1, &render_fbo_id);
  glDeleteBuffers(1, &render_pos_buffer_id);
  glDeleteBuffers(1, &render_normal_buffer_id);
  glDeleteBuffers(1, &filter_pos_buffer_id);
  glDeleteBuffers(1, &filter_texcoord_buffer_id);
  glDeleteVertexArrays(1, &render_vao_id);
  glDeleteVertexArrays(1, &filter_vao_id);
  glDeleteProgram(render_program_id);
  glDeleteProgram(filter_program_id);
}

void InitGL() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  glViewport(0, 0, kScreenWidth, kScreenHeight);
}

void CreatePrograms() {
  GLuint render_vs_id = glCreateShader(GL_VERTEX_SHADER);
  GLuint render_fs_id = glCreateShader(GL_FRAGMENT_SHADER);
  if (!CompileShader(render_vs_id, "lighting.vs")) {
    std::cerr << "Could not compile render vertex shader" << std::endl;
  }
  if (!CompileShader(render_fs_id, "lighting.fs")) {
    std::cerr << "Could not compile render fragment shader" << std::endl;
  }

  render_program_id = glCreateProgram();
  if (!LinkProgram(render_program_id, render_vs_id, 0, 0, 0, render_fs_id)) {
    std::cerr << "Could not link render program" << std::endl;
    exit(1);
  }
  glDeleteShader(render_vs_id);
  glDeleteShader(render_fs_id);

  GLuint filter_vs_id = glCreateShader(GL_VERTEX_SHADER);
  GLuint filter_fs_id = glCreateShader(GL_FRAGMENT_SHADER);
  if (!CompileShader(filter_vs_id, "edgedetect.vs")) {
    std::cerr << "Could not compile filter vertex shader" << std::endl;
  }
  if (!CompileShader(filter_fs_id, "edgedetect.fs")) {
    std::cerr << "Could not compile filter fragment shader" << std::endl;
  }

  filter_program_id = glCreateProgram();
  if (!LinkProgram(filter_program_id, filter_vs_id, 0, 0, 0,  filter_fs_id)) {
    std::cerr << "Could not link filter program" << std::endl;
    exit(1);
  }
  glDeleteShader(filter_vs_id);
  glDeleteShader(filter_fs_id);
}


int main() {
   
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
    SDL_Log("Failed to initialized SDL: %s", SDL_GetError());
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

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

  CreatePrograms();

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
