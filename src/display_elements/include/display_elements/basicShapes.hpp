#ifndef BASIC_SHAPES_HPP
#define BASIC_SHAPES_HPP

#include <math.h>

#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <display_elements/vertex.hpp>
#include <iostream>
#include <utils/eigen_conversations.hpp>

using namespace Eigen;

typedef std::vector<unsigned int> IndicesVector;

template <class VertexType>
void pushVertexTypeBack(std::vector<VertexType> &vertices,
                        const Vector3f &xyz,
                        const Vector3f &normale,
                        const Matrix<float, VertexType::NUM_COLOR, 1> &color) {
  VertexType v;
  if constexpr (VertexType::HAS_POSITION) {
    std::copy(xyz.data(), xyz.data() + 3, v.position);
  }
  if constexpr (VertexType::HAS_NORMAL) {
    std::copy(normale.data(), normale.data() + 3, v.normal);
  }
  if constexpr (VertexType::HAS_TANGENT) {
    static_assert(!VertexType::HAS_TANGENT, "tangent not supported (yet)");
    // std::copy(tangent.data(), tangent.data() + 3, v.tangent);
  }
  if constexpr (VertexType::HAS_BITANGETN) {
    static_assert(!VertexType::HAS_BITANGETN, "tangent not supported (yet)");
    // std::copy(bitangent.data(), bitangent.data() + 3, v.bitangent);
  }
  if constexpr (VertexType::HAS_TEXTURE) {
    static_assert(!VertexType::HAS_TEXTURE, "tangent not supported (yet)");
    // std::copy(texture_pos.data(), texture_pos.data() + 2, v.texture_pos);
  }
  if constexpr (VertexType::HAS_COLOR) {
    std::copy(color.data(), color.data() + VertexType::NUM_COLOR, v.color);
  }
  vertices.emplace_back(v);
}

// circle in x-y plane conterclockwize, center (0,0,0).
template <class VertexType>
inline void circle(std::vector<VertexType> &vertices,
                   IndicesVector &indices_vector,
                   float radius,
                   unsigned int resolution,
                   const Matrix<float, VertexType::NUM_COLOR, 1> color =
                       Matrix<float, VertexType::NUM_COLOR, 1>::Zero(),
                   float z = 0.f,
                   float norm_z = 1.f) {
  const Vector3f normal(0, 0, norm_z);
  const unsigned int center_index = vertices.size();
  const unsigned int first_index = center_index + 1;
  const unsigned int last_index = first_index + resolution;
  pushVertexTypeBack(vertices, Vector3f(0, 0, z), normal, color);

  for (unsigned int i = first_index; i < last_index; i++) {
    // i-first_index is necessarry that two drawn circles first vertex have the same angle.
    const float angle = static_cast<float>(M_PI * 2. * static_cast<double>(i - first_index) /
                                           static_cast<double>(resolution));
    const float x = radius * std::cos(angle);
    const float y = radius * std::sin(angle);
    pushVertexTypeBack(vertices, Vector3f(x, y, z), normal, color);


    const unsigned int next_index = (i < last_index - 1) ? i + 1 : first_index;

    indices_vector.push_back(center_index);
    indices_vector.push_back(i);           // this one
    indices_vector.push_back(next_index);  // the next one
  }
}

inline void getCircleInformation(unsigned int &num_vertices,
                                 unsigned int &num_triangles,
                                 unsigned int resolution) {
  num_triangles = resolution;
  num_vertices = resolution + 1;
}


// cylinder in x-y plane and height in z conterclockwize, center (0,0,0) in the center of the cylinder.
template <class VertexType>
inline void cylinder(std::vector<VertexType> &vertices,
                     IndicesVector &indices_vector,
                     float radius,
                     unsigned int resolution,
                     float height,
                     const Matrix<float, VertexType::NUM_COLOR, 1> color =
                         Matrix<float, VertexType::NUM_COLOR, 1>::Zero()) {
  // Get two circles for top and bottom
  const unsigned int start_index_top_circle = vertices.size();
  circle(vertices, indices_vector, radius, resolution, color, height / 2.f, 1.f);
  const unsigned int start_index_bot_circle = vertices.size();
  circle(vertices, indices_vector, radius, resolution, color, -height / 2.f, -1.f);
  const unsigned int start_index_surface = vertices.size();
  unsigned int index = start_index_surface;
  // We want a flat shaded mesh, so we need for each triangle its own normals.
  // Meaning we can not reuse the indices used for the circles since their normals look up/down.

  const unsigned int top_bot_offset = start_index_bot_circle - start_index_top_circle;
  const unsigned int index_first_vertex = start_index_top_circle + 1;
  const unsigned int index_last_vertex = start_index_bot_circle - 1;
  for (unsigned int i = index_first_vertex; i < index_last_vertex + 1; i++) {
    const unsigned int neigbour_vertex = (i != index_last_vertex) ? i + 1 : index_first_vertex;

    const Vector3f v1_top(vertices[i].position);
    const Vector3f v2_top(vertices[neigbour_vertex].position);
    const Vector3f v1_bot(vertices[i + top_bot_offset].position);
    const Vector3f v2_bot(vertices[neigbour_vertex + top_bot_offset].position);

    const Vector3f tangent = v2_bot - v1_bot;
    // rotation -90 deg around z
    // cos(-90) -sin(-90) 0
    // sin(90)   cos(90)  0
    // 0          0       1
    // -> 0 1 0   x
    //   -1 0 0 * y
    //    0 0 1   z
    Vector3f normale(tangent.y(), -tangent.x(), tangent.z());
    normale.normalize();
    pushVertexTypeBack(vertices, v1_top, normale, color);
    const unsigned int index_v1_top = index++;
    pushVertexTypeBack(vertices, v2_top, normale, color);
    const unsigned int index_v2_top = index++;
    pushVertexTypeBack(vertices, v1_bot, normale, color);
    const unsigned int index_v1_bot = index++;
    pushVertexTypeBack(vertices, v2_bot, normale, color);
    const unsigned int index_v2_bot = index++;


    // now for the triangles

    indices_vector.push_back(index_v2_top);
    indices_vector.push_back(index_v1_top);
    indices_vector.push_back(index_v1_bot);

    indices_vector.push_back(index_v2_top);
    indices_vector.push_back(index_v1_bot);
    indices_vector.push_back(index_v2_bot);
  }
}

inline void getCylinderInformation(unsigned int &num_vertices,
                                   unsigned int &num_triangles,
                                   unsigned int resolution) {

  getCircleInformation(num_vertices, num_triangles, resolution);

  const unsigned int num_outer_vertices = num_outer_vertices - 1;
  num_triangles = num_triangles * 2 + resolution * 2;
  num_vertices = num_vertices * 2 + num_outer_vertices * 4;
}



#endif
