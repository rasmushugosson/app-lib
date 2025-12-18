#pragma once

#include <ext.hpp>
#include <glm.hpp>
#include <gtc/constants.hpp>
#include <iostream>

inline std::ostream &operator<<(std::ostream &os, const glm::vec1 &v)
{
    os << "vec1[" << v.x << "]";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::vec2 &v)
{
    os << "vec2[" << v.x << ", " << v.y << "]";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::vec3 &v)
{
    os << "vec3[" << v.x << ", " << v.y << ", " << v.z << "]";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::vec4 &v)
{
    os << "vec4[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::dvec1 &v)
{
    os << "dvec1[" << v.x << "]";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::dvec2 &v)
{
    os << "dvec2[" << v.x << ", " << v.y << "]";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::dvec3 &v)
{
    os << "dvec3[" << v.x << ", " << v.y << ", " << v.z << "]";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::dvec4 &v)
{
    os << "dvec4[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::mat2x2 &v)
{
    os << "mat2x2:\n";
    os << "[" << v[0] << ", " << v[1] << "]\n";
    os << "[" << v[2] << ", " << v[3] << "]\n";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::mat3x3 &v)
{
    os << "mat3x3:\n";
    os << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]\n";
    os << "[" << v[3] << ", " << v[4] << ", " << v[5] << "]\n";
    os << "[" << v[6] << ", " << v[7] << ", " << v[8] << "]\n";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::mat4x4 &v)
{
    os << "mat4x4:\n";
    os << "[" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "]\n";
    os << "[" << v[4] << ", " << v[5] << ", " << v[6] << ", " << v[7] << "]\n";
    os << "[" << v[8] << ", " << v[9] << ", " << v[10] << ", " << v[11] << "]\n";
    os << "[" << v[12] << ", " << v[13] << ", " << v[14] << ", " << v[15] << "]\n";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::dmat2x2 &v)
{
    os << "dmat2x2:\n";
    os << "[" << v[0] << ", " << v[1] << "]\n";
    os << "[" << v[2] << ", " << v[3] << "]\n";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::dmat3x3 &v)
{
    os << "dmat3x3:\n";
    os << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]\n";
    os << "[" << v[3] << ", " << v[4] << ", " << v[5] << "]\n";
    os << "[" << v[6] << ", " << v[7] << ", " << v[8] << "]\n";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::dmat4x4 &v)
{
    os << "dmat4x4:\n";
    os << "[" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "]\n";
    os << "[" << v[4] << ", " << v[5] << ", " << v[6] << ", " << v[7] << "]\n";
    os << "[" << v[8] << ", " << v[9] << ", " << v[10] << ", " << v[11] << "]\n";
    os << "[" << v[12] << ", " << v[13] << ", " << v[14] << ", " << v[15] << "]\n";
    return os;
}
