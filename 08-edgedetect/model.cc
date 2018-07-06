#include "model.h"

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <utility>

#include "glm/glm.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

bool CreateModelFromFile(const std::string& path, Model* model) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string err_msg;
  bool load_result = tinyobj::LoadObj(&attrib, &shapes, &materials, &err_msg,
                                      path.c_str());

  if (!err_msg.empty()) {
    std::cerr << err_msg << std::endl;
  }

  if (!load_result) {
    std::cerr << "Failed to load model: " << path << std::endl;
    return false;
  }

  if (shapes.size() != 1) {
    std::cerr << "Does not support more than 1 shape." << std::endl;
    return false;
  }

  for (auto num_verts : shapes[0].mesh.num_face_vertices) {
    if (num_verts != 3) {
      std::cerr << "Only supports triangles as faces." << std::endl;
      return false;
    }
  }

  size_t vert_count = shapes[0].mesh.indices.size();
  
  model->positions.resize(vert_count);
  model->normals.resize(vert_count);
  model->texcoords.resize(vert_count);
  model->faces.clear(); // Not used since we don't use indexed drawing

  for (size_t i = 0; i < vert_count; ++i) {
    size_t v = shapes[0].mesh.indices[i].vertex_index;
    model->positions[i][0] = attrib.vertices[3 * v + 0];
    model->positions[i][1] = attrib.vertices[3 * v + 1];
    model->positions[i][2] = attrib.vertices[3 * v + 2];

    size_t vn = shapes[0].mesh.indices[i].normal_index;
    model->normals[i][0] = attrib.normals[3 * vn + 0];
    model->normals[i][1] = attrib.normals[3 * vn + 1];
    model->normals[i][2] = attrib.normals[3 * vn + 2];

    size_t vt = shapes[0].mesh.indices[i].texcoord_index;
    model->texcoords[i][0] = attrib.texcoords[2 * vt + 0];
    model->texcoords[i][1] = attrib.texcoords[2 * vt + 1];
  }

  model->vert_count = vert_count;
  model->face_count = shapes[0].mesh.num_face_vertices.size();
  model->indexed_drawing = false;
  
  return true;
}


Model CreateModelCube(float length) {
  float n = length / 2;

  Model model;
  
  model.positions = {{n, n, n}, {n, -n, n}, //Positive yz plane
                     {n, -n, -n}, {n, n, -n},
                     {-n, n, -n}, {-n, -n, -n}, //Negative yz plane
                     {-n, -n, n}, {-n, n, n},
                     {-n, n, -n}, {-n, n, n}, //Positive xz plane
                     {n, n, n}, {n, n, -n},
                     {n, -n, n}, {n, -n, -n}, //Negative xz plane
                     {-n, -n, -n}, {-n, -n, n},
                     {-n, n, n}, {-n, -n, n}, //Positive xy plane
                     {n, -n, n}, {n, n, n},
                     {n, n, -n}, {n, -n, -n}, //Negative xy Plane
                     {-n, -n, -n}, {-n, n, -n}};
    
  model.normals = {{1.f, 0.f, 0.f}, {1.f, 0.f, 0.f},
                   {1.f, 0.f, 0.f}, {1.f, 0.f, 0.f},
                   {-1.f, 0.f, 0.f}, {-1.f, 0.f, 0.f},
                   {-1.f, 0.f, 0.f}, {-1.f, 0.f, 0.f},
                   {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
                   {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f},
                   {0.f, -1.f, 0.f}, {0.f, -1.f, 0.f},
                   {0.f, -1.f, 0.f}, {0.f, -1.f, 0.f},
                   {0.f, 0.f, 1.f}, {0.f, 0.f, 1.f},
                   {0.f, 0.f, 1.f}, {0.f, 0.f, 1.f},
                   {0.f, 0.f, -1.f}, {0.f, 0.f, -1.f},
                   {0.f, 0.f, -1.f}, {0.f, 0.f, -1.f}};

  model.texcoords = {};
  
  model.faces = {{0, 1, 3}, {3, 1, 2},
                 {4, 5, 7}, {7, 5, 6},
                 {8, 9, 11}, {11, 9, 10},
                 {12, 13, 15}, {15, 13, 14},
                 {16, 17, 19}, {19, 17, 18},
                 {20, 21, 23}, {23, 21, 22}};

  model.vert_count = 24;
  model.face_count = 12;
  model.indexed_drawing = true;

  return std::move(model);
}
