#include <iostream>
#include <cstdlib>
#include <iterator>
#include <fstream>
#include <string>
#include <vector>
#include <SDL2/SDL.h>

#include <OpenGL/gl3.h>

using namespace std;

GLuint vertex_array_id;
GLuint vertex_buffer_id;
GLuint program_id;

void Render(SDL_Window* window, SDL_GLContext* gl_context) {
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(program_id);

  glBindVertexArray(vertex_array_id);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void*>(0));

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray(0);
  
  SDL_GL_SwapWindow(window);
}

bool CompileShader(GLuint shader_id, const std::string& path) {
  std::ifstream shader_fin(path);
  std::string shader_source =
    std::string(std::istreambuf_iterator<char>(shader_fin),
                std::istreambuf_iterator<char>());

  const char* shader_source_ptr = shader_source.c_str();
  glShaderSource(shader_id, 1, &shader_source_ptr, NULL);
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
                                        1024, // window width
                                        768, // window height
                                        SDL_WINDOW_OPENGL);

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  
  if (gl_context == NULL) {
    cout << "OpenGL context could not be created: " << SDL_GetError() << endl;
    exit(1);
  }

  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << std::endl;

  GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

  CompileShader(vertex_shader_id, "simple.vs");
  CompileShader(fragment_shader_id, "simple.fs");

  program_id = glCreateProgram();
  glAttachShader(program_id, vertex_shader_id);
  glAttachShader(program_id, fragment_shader_id);
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
    exit(1);
  }
  
  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  glGenVertexArrays(1, &vertex_array_id);
  glBindVertexArray(vertex_array_id);

  float vertices[] = {-0.5f,  0.5f, 0.f,
                      -0.5f, -0.5f, 0.f,
                       0.5f, -0.5f, 0.f};

  std::cout << sizeof(vertices) << std::endl;
  
  glGenBuffers(1, &vertex_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
               static_cast<float*>(vertices), GL_STATIC_DRAW);
  

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

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
