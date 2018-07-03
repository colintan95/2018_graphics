#include "model.h"

#include <vector>
#include <utility>

#include "glm/glm.hpp"

Model CreateModelCube(float length) {
  float n = length / 2;

  Model model;
  
  model.positions = {{n, n, n, 1.f}, {n, -n, n, 1.f}, //Positive yz plane
                     {n, -n, -n, 1.f}, {n, n, -n, 1.f},
                     {-n, n, -n, 1.f}, {-n, -n, -n, 1.f}, //Negative yz plane
                     {-n, -n, n, 1.f}, {-n, n, n, 1.f},
                     {-n, n, -n, 1.f}, {-n, n, n, 1.f}, //Positive xz plane
                     {n, n, n, 1.f}, {n, n, -n, 1.f},
                     {n, -n, n, 1.f}, {n, -n, -n, 1.f}, //Negative xz plane
                     {-n, -n, -n, 1.f}, {-n, -n, n, 1.f},
                     {-n, n, n, 1.f}, {-n, -n, n, 1.f}, //Positive xy plane
                     {n, -n, n, 1.f}, {n, n, n, 1.f},
                     {n, n, -n, 1.f}, {n, -n, -n, 1.f}, //Negative xy Plane
                     {-n, -n, -n, 1.f}, {-n, n, -n, 1.f}};
    
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

  return std::move(model);
}
