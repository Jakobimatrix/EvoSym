#ifndef BASIC_SHAPES_HPP
#define BASIC_SHAPES_HPP

#include <math.h>

#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <display_elements/vertex.hpp>
#include <utils/eigen_conversations.hpp>
#include <utils/math.hpp>

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
    v.texture_pos[0] = 0;
    v.texture_pos[1] = 0;
    // static_assert(!VertexType::HAS_TEXTURE, "tangent not supported (yet)");
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
                   bool norm_z_pos = true) {
  const float z_normal_direction = (norm_z_pos) ? 1.f : -1.f;
  const bool revert_winding = (norm_z_pos) ? false : true;
  const Vector3f normal(0, 0, z_normal_direction);
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


    unsigned int next_index = (i < last_index - 1) ? i + 1 : first_index;
    unsigned int this_index = i;

    if (revert_winding) {
      std::swap(next_index, this_index);
    }

    indices_vector.push_back(center_index);
    indices_vector.push_back(this_index);  // this one
    indices_vector.push_back(next_index);  // the next one
  }
}

inline void getCircleInformation(unsigned int &num_vertices,
                                 unsigned int &num_triangles,
                                 unsigned int resolution) {
  num_triangles = resolution;
  num_vertices = resolution + 1;
}


// cylinder in x-y plane and height in z conterclockwize, center (0,0,0) in the center of the bottom circle.
template <class VertexType>
inline void cylinder(std::vector<VertexType> &vertices,
                     IndicesVector &indices_vector,
                     float radius,
                     unsigned int resolution,
                     float height,
                     const Matrix<float, VertexType::NUM_COLOR, 1> color =
                         Matrix<float, VertexType::NUM_COLOR, 1>::Zero()) {
  height = std::abs(height);
  // Get two circles for top and bottom
  const unsigned int start_index_top_circle = vertices.size();
  circle(vertices, indices_vector, radius, resolution, color, height, true);
  const unsigned int start_index_bot_circle = vertices.size();
  circle(vertices, indices_vector, radius, resolution, color, 0, false);
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

// Cone in x-y plane and height in z conterclockwize, center (0,0,0) at the bottom of the cone.
template <class VertexType>
inline void cone(std::vector<VertexType> &vertices,
                 IndicesVector &indices_vector,
                 float radius,
                 unsigned int resolution,
                 float height,
                 const Matrix<float, VertexType::NUM_COLOR, 1> color =
                     Matrix<float, VertexType::NUM_COLOR, 1>::Zero()) {
  const bool bool_look_up = height > 0;
  // The circle first vertex created will be the center vertex and
  // thus its index will be the size of the current vertice vector.
  const unsigned int index_circle_center = vertices.size();
  circle(vertices, indices_vector, radius, resolution, color, 0.f, !bool_look_up);
  const unsigned int index_circle_start = index_circle_center + 1;
  const unsigned int index_circle_stop = vertices.size() - 1;

  const Eigen::Vector3f tip(vertices[index_circle_center].position[0],
                            vertices[index_circle_center].position[1],
                            vertices[index_circle_center].position[2] + height);

  unsigned int index = vertices.size();
  for (int i = index_circle_start; i <= index_circle_stop; i++) {

    const unsigned int right_neigbour_id = (i < index_circle_stop) ? i + 1 : index_circle_start;
    const Eigen::Vector3f left_v(vertices[i].position);
    const Eigen::Vector3f right_v(vertices[right_neigbour_id].position);
    const Eigen::Vector3f normale = eigen_utils::getTrianglesNormal(tip, left_v, right_v);

    pushVertexTypeBack(vertices, tip, normale, color);
    pushVertexTypeBack(vertices, left_v, normale, color);
    pushVertexTypeBack(vertices, right_v, normale, color);

    indices_vector.push_back(index++);
    indices_vector.push_back(index++);
    indices_vector.push_back(index++);
  }
}

inline void getConeInformation(unsigned int &num_vertices,
                               unsigned int &num_triangles,
                               unsigned int resolution) {

  getCircleInformation(num_vertices, num_triangles, resolution);

  num_triangles = resolution * 2;
  num_vertices = resolution * 4 + 1;
}



// Arrow in x-y plane and height in z conterclockwize, center (0,0,0) at the bottom of the arrow.
template <class VertexType>
inline void Arrow(std::vector<VertexType> &vertices,
                  IndicesVector &indices_vector,
                  float radius,
                  unsigned int resolution,
                  float length,
                  const Matrix<float, VertexType::NUM_COLOR, 1> color =
                      Matrix<float, VertexType::NUM_COLOR, 1>::Zero()) {

  const float inner_radius = radius / static_cast<float>(GOLDEN_RATIO);

  const float tip_length = radius * static_cast<float>(GOLDEN_RATIO);
  const float cylinder_length = length - tip_length;

  cylinder(vertices, indices_vector, inner_radius, resolution, cylinder_length, color);
  const unsigned int first_tip_index = vertices.size();

  // get a cone and move it up the length of the cylinder
  cone(vertices, indices_vector, radius, resolution, tip_length, color);
  for (int i = first_tip_index; i < vertices.size(); i++) {
    vertices[i].position[2] += cylinder_length;
  }
}

inline void getArrowInformation(unsigned int &num_vertices,
                                unsigned int &num_triangles,
                                unsigned int resolution) {

  getCylinderInformation(num_vertices, num_triangles, resolution);
  unsigned int cone_num_vertices;
  unsigned int cone_num_triangles;
  getConeInformation(cone_num_vertices, cone_num_triangles, resolution);

  num_triangles += cone_num_triangles;
  num_vertices += cone_num_vertices;
}


// Arrow in x-y plane and height in z conterclockwize, center (0,0,0) at the bottom of the arrow.
template <class VertexType>
inline void coordXYZ(std::vector<VertexType> &vertices,
                     IndicesVector &indices_vector,
                     float radius,
                     unsigned int resolution,
                     float length,
                     const Matrix<float, VertexType::NUM_COLOR, 1> color_x =
                         Matrix<float, VertexType::NUM_COLOR, 1>::Zero(),
                     const Matrix<float, VertexType::NUM_COLOR, 1> color_y =
                         Matrix<float, VertexType::NUM_COLOR, 1>::Zero(),
                     const Matrix<float, VertexType::NUM_COLOR, 1> color_z =
                         Matrix<float, VertexType::NUM_COLOR, 1>::Zero()) {


  Arrow(vertices, indices_vector, radius, resolution, length, color_z);
  const unsigned int first_index_y = vertices.size();
  Arrow(vertices, indices_vector, radius, resolution, length, color_y);
  const unsigned int first_index_x = vertices.size();
  Arrow(vertices, indices_vector, radius, resolution, length, color_x);
  const unsigned int end = vertices.size();

  // rotate every pose and every normal around x 90 deg
  // z' = -y
  // y' = z
  for (int i = first_index_y; i < first_index_x; i++) {
    std::swap(vertices[i].position[2], vertices[i].position[1]);
    vertices[i].position[2] = -vertices[i].position[2];

    std::swap(vertices[i].normal[2], vertices[i].normal[1]);
    vertices[i].normal[2] = -vertices[i].normal[2];
  }
  // rotate every pose and every normal around y 90 deg
  // z' = -x
  // x' = z
  for (int i = first_index_x; i < end; i++) {
    std::swap(vertices[i].position[2], vertices[i].position[0]);
    vertices[i].position[2] = -vertices[i].position[2];

    std::swap(vertices[i].normal[2], vertices[i].normal[0]);
    vertices[i].normal[2] = -vertices[i].normal[2];
  }
}

inline void getcoordXYZInformation(unsigned int &num_vertices,
                                   unsigned int &num_triangles,
                                   unsigned int resolution) {

  getArrowInformation(num_vertices, num_triangles, resolution);
  num_triangles *= 3;
  num_vertices *= 3;
}

// circle in x-y plane conterclockwize, center (0,0,0).
template <class VertexType>
inline void rectangle(std::vector<VertexType> &vertices,
                      IndicesVector &indices_vector,
                      float radius,
                      unsigned int resolution,
                      const Matrix<float, VertexType::NUM_COLOR, 1> color =
                          Matrix<float, VertexType::NUM_COLOR, 1>::Zero(),
                      float z = 0.f,
                      bool norm_z_pos = true) {
  const float z_normal_direction = (norm_z_pos) ? 1.f : -1.f;
  const Vector3f normal(0, 0, z_normal_direction);
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

inline void getRectangleInformation(unsigned int &num_vertices,
                                    unsigned int &num_triangles,
                                    unsigned int resolution) {
  num_triangles = resolution;
  num_vertices = resolution + 1;
}


#endif
