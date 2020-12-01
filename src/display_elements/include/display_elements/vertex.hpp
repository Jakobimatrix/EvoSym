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

  // offsets are only valide if the has_xxx template parameter is true;
  static constexpr int POSITION_OFFSET = 0;
  static constexpr int POSITION_OFFSET_BIT = POSITION_OFFSET * sizeof(float);
  static constexpr int NORMAL_OFFSET = POSITION_OFFSET + VertexPosition<has_position>::NUM;
  static constexpr int NORMAL_OFFSET_BIT = NORMAL_OFFSET * sizeof(float);
  static constexpr int TANGENT_OFFSET = NORMAL_OFFSET + VertexNormal<has_normal>::NUM;
  static constexpr int TANGENT_OFFSET_BIT = TANGENT_OFFSET * sizeof(float);
  static constexpr int BITANGENT_OFFSET = TANGENT_OFFSET + VertexTangent<has_tangent>::NUM;
  static constexpr int BITANGENT_OFFSET_BIT = BITANGENT_OFFSET * sizeof(float);
  static constexpr int TEXTURE_OFFSET =
      BITANGENT_OFFSET + VertexBitangent<has_bitangent>::NUM;
  static constexpr int TEXTURE_OFFSET_BIT = TEXTURE_OFFSET * sizeof(float);
  static constexpr int COLOR_OFFSET = TEXTURE_OFFSET + VertexTexture<has_texture>::NUM;
  static constexpr int COLOR_OFFSET_BIT = COLOR_OFFSET * sizeof(float);


  static constexpr int ARRAY_LENGTH =
      COLOR_OFFSET + VertexColor<has_color, num_color_values>::NUM;

  Vertex() {}

  Vertex(const std::array<float, ARRAY_LENGTH>& v) { this->v = v; }

  std::array<float, ARRAY_LENGTH> v;

  float* getPosition() {
    static_assert(has_position,
                  "This Vertex does not have a position. Calling this function "
                  "will result in undefined behaviour");
    return &v[POSITION_OFFSET];
  }

  int getPositionSize() const { return VertexPosition<has_position>::NUM; }

  float* getNormal() {
    static_assert(has_normal,
                  "This Vertex does not have a normal. Calling this function "
                  "will result in undefined behaviour");

    return &v[NORMAL_OFFSET];
  }

  int getNormalSize() const { return VertexNormal<has_normal>::NUM; }

  float* getTangent() {
    static_assert(has_tangent,
                  "This Vertex does not have a tangent. Calling this function "
                  "will result in undefined behaviour");

    return &v[TANGENT_OFFSET];
  }

  int getTangentSize() const { return VertexTangent<has_tangent>::NUM; }

  float* getBitangent() {
    static_assert(has_bitangent,
                  "This Vertex does not have a bitangent. Calling this "
                  "function will result in undefined behaviour");

    return &v[BITANGENT_OFFSET];
  }

  int getBitangentSize() const { return VertexBitangent<has_bitangent>::NUM; }

  float* getTexturePosition() {
    static_assert(has_texture,
                  "This Vertex does not have a texture. Calling this function "
                  "will result in undefined behaviour");

    return &v[TEXTURE_OFFSET];
  }

  int getTexturePositionSize() const { return VertexTexture<has_texture>::NUM; }

  float* getColor() {
    static_assert(has_color,
                  "This Vertex does not have a color. Calling this function "
                  "will result in undefined behaviour");
    return &v[COLOR_OFFSET];
  }

  int getColorSize() const {
    return VertexColor<has_color, num_color_values>::NUM;
  }
};


#endif
