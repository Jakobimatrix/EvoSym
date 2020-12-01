#ifndef VERTEX_H
#define VERTEX_H

#include <array>

template <bool has_position = true>
struct VertexPosition;

template <>
struct VertexPosition<true> {
  static constexpr int NUM = 3;
};

template <>
struct VertexPosition<false> {
  static constexpr int NUM = 0;
};

template <bool has_normal = true>
struct VertexNormal;

template <>
struct VertexNormal<true> {
  static constexpr int NUM = 3;
};

template <>
struct VertexNormal<false> {
  static constexpr int NUM = 0;
};

template <bool has_tangent = true>
struct VertexTangent;

template <>
struct VertexTangent<true> {
  static constexpr int NUM = 3;
};

template <>
struct VertexTangent<false> {
  static constexpr int NUM = 0;
};

template <bool has_bitangent = true>
struct VertexBitangent;

template <>
struct VertexBitangent<true> {
  static constexpr int NUM = 3;
};

template <>
struct VertexBitangent<false> {
  static constexpr int NUM = 0;
};

template <bool has_texture = true>
struct VertexTexture;

template <>
struct VertexTexture<true> {
  static constexpr int NUM = 2;
};

template <>
struct VertexTexture<false> {
  static constexpr int NUM = 0;
};

template <bool has_color = true, int num_pigments = 3>
struct VertexColor;

template <int num_pigments>
struct VertexColor<true, num_pigments> {
  static constexpr int NUM = num_pigments;
};

template <>
struct VertexColor<false, 0> {
  static constexpr int NUM = 0;
};


template <bool has_position, bool has_normal, bool has_tangent, bool has_bitangent, bool has_texture, bool has_color, int num_color_values>
struct Vertex {

  static constexpr int NUM_POSITION = VertexPosition<has_position>::NUM;
  static constexpr int NUM_NORMAL = VertexNormal<has_normal>::NUM;
  static constexpr int NUM_TANGENT = VertexTangent<has_tangent>::NUM;
  static constexpr int NUM_BITANGENT = VertexBitangent<has_bitangent>::NUM;
  static constexpr int NUM_TEXTURE = VertexTexture<has_texture>::NUM;
  static constexpr int NUM_COLOR = VertexColor<has_color, num_color_values>::NUM;

  // offsets are only valide if the has_xxx template parameter is true;
  static constexpr int POSITION_OFFSET = 0;
  static constexpr int POSITION_OFFSET_BYTE = POSITION_OFFSET * sizeof(float);
  static constexpr int NORMAL_OFFSET = POSITION_OFFSET + NUM_POSITION;
  static constexpr int NORMAL_OFFSET_BYTE = NORMAL_OFFSET * sizeof(float);
  static constexpr int TANGENT_OFFSET = NORMAL_OFFSET + NUM_NORMAL;
  static constexpr int TANGENT_OFFSET_BYTE = TANGENT_OFFSET * sizeof(float);
  static constexpr int BITANGENT_OFFSET = TANGENT_OFFSET + NUM_TANGENT;
  static constexpr int BITANGENT_OFFSET_BYTE = BITANGENT_OFFSET * sizeof(float);
  static constexpr int TEXTURE_OFFSET = BITANGENT_OFFSET + NUM_BITANGENT;
  static constexpr int TEXTURE_OFFSET_BYTE = TEXTURE_OFFSET * sizeof(float);
  static constexpr int COLOR_OFFSET = TEXTURE_OFFSET + NUM_TEXTURE;
  static constexpr int COLOR_OFFSET_BYTE = COLOR_OFFSET * sizeof(float);

  static constexpr int ARRAY_LENGTH = COLOR_OFFSET + NUM_COLOR;

  Vertex() {}

  Vertex(const std::array<float, ARRAY_LENGTH>& v) {
    unsigned int pointer = 0;
    std::copy(std::begin(v) + pointer, std::begin(v) + pointer + NUM_POSITION, position);
    pointer += NUM_POSITION;

    std::copy(std::begin(v) + pointer, std::begin(v) + pointer + NUM_NORMAL, normal);
    pointer += NUM_NORMAL;

    std::copy(std::begin(v) + pointer, std::begin(v) + pointer + NUM_TANGENT, tangent);
    pointer += NUM_TANGENT;

    std::copy(std::begin(v) + pointer, std::begin(v) + pointer + NUM_BITANGENT, bitangent);
    pointer += NUM_BITANGENT;

    std::copy(std::begin(v) + pointer, std::begin(v) + pointer + NUM_TEXTURE, texture_pos);
    pointer += NUM_TEXTURE;

    std::copy(std::begin(v) + pointer, std::begin(v) + pointer + NUM_COLOR, color);
  }

  float position[NUM_POSITION];
  float normal[NUM_NORMAL];
  float tangent[NUM_TANGENT];
  float bitangent[NUM_BITANGENT];
  float texture_pos[NUM_TEXTURE];
  float color[NUM_COLOR];
};


#endif
