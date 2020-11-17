#ifndef DISP_UTILS_H
#define DISP_UTILS_H

#include "mesh.hpp"
#include "shader.hpp"
#include <memory>

namespace disp_utils {

using MeshShaderPair = std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Shader>>;

inline static const MeshShaderPair makeMeshShaderPair(const Mesh& m, const Shader& s) {
    return std::make_pair<std::shared_ptr<Mesh>, std::shared_ptr<Shader>>(
        std::make_shared<Mesh>(m), std::make_shared<Shader>(s));
}

inline static const MeshShaderPair makeMeshShaderPair(const std::shared_ptr<Mesh> m, const std::shared_ptr<Shader> s) {
    return std::make_pair<std::shared_ptr<Mesh>, std::shared_ptr<Shader>>(
        std::make_shared<Mesh>(*m.get()), std::make_shared<Shader>(*s.get()));
}

}
#endif
