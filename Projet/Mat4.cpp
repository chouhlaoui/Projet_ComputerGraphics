#include "mat4.h"

mat4::mat4() {
    for (int i = 0; i < 16; i++) {
        if (i % 5 == 0) {
            matrice[i] = 1.0f; 
        }
        else {
            matrice[i] = 0.0f; 
        }
    }
}

float& mat4::operator()(int row, int col) {
    return matrice[row + col * 4]; 
}

const float& mat4::operator()(int row, int col) const {
    return matrice[row + col * 4];
}

mat4 mat4::operator*(const mat4& B) const {
    mat4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result(i, j) = 0.0f;
            for (int k = 0; k < 4; k++) {
                result(i, j) += (*this)(i, k) * B(k, j);
            }
        }
    }
    return result;
}

mat4 mat4::operator+(const mat4& B) const {
    mat4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result(i, j) = (*this)(i, j) + B(i, j);
        }
    }
    return result;
}

mat4 mat4::operator-(const mat4& B) const {
    mat4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result(i, j) = (*this)(i, j) - B(i, j);
        }
    }
    return result;
}

mat4 mat4::translate(const mat4& mat, const std::array<float, 3>& translation) {
    mat4 result = mat;
    result(0, 3) += translation[0];
    result(1, 3) += translation[1];
    result(2, 3) += translation[2];
    return result;
}

mat4 mat4::scale(const mat4& mat, const std::array<float, 3>& scale) {
    mat4 result = mat;
    result(0, 0) *= scale[0];
    result(1, 1) *= scale[1];
    result(2, 2) *= scale[2];
    return result;
}
