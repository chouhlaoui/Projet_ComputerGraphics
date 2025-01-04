#ifndef MAT4_H
#define MAT4_H

#include <array>
#include <cmath>

class mat4 {
public:
    std::array<float, 16> matrice;

    mat4();

    float& operator()(int row, int col);

    const float& operator()(int row, int col) const;

    mat4 operator*(const mat4& B) const;

    mat4 operator+(const mat4& B) const;

    mat4 operator-(const mat4& B) const;

    static mat4 translate(const mat4& mat, const std::array<float, 3>& translation);

    static mat4 scale(const mat4& mat, const std::array<float, 3>& scale);
};

#endif 
