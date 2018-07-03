#ifndef MODEL_H_
#define MODEL_H_

#include <string>
#include <vector>
#include <utility>

#include "glm/glm.hpp"

struct Model {
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texcoords;
  std::vector<glm::uvec3> faces; // Used for indexed drawing

  unsigned int vert_count = 0;
  unsigned int face_count = 0;

  bool indexed_drawing = false;

  Model() {}
  Model(const Model&) = delete; // To prevent any unintended copying
  Model(Model&& o) : positions(std::move(o.positions)),
                     normals(std::move(o.normals)),
                     texcoords(std::move(o.texcoords)),
                     faces(std::move(o.faces)),
                     vert_count(o.vert_count),
                     face_count(o.face_count),
                     indexed_drawing(o.indexed_drawing) {}
};

bool CreateModelFromFile(const std::string& path, Model *model);

Model CreateModelCube(float length);

#endif
