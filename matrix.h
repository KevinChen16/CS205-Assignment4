//
// Created by jackchen on 2020/12/3.
//

#ifndef ASS4_MATRIX_H
#define ASS4_MATRIX_H

#include <immintrin.h>
#include <omp.h>

class Matrix {
private:
    int row;
    int column;
    float* data;
public:
    Matrix();
    Matrix(const Matrix& m);
    Matrix(int r, int c, float*d=nullptr);
    ~Matrix();
    Matrix& operator=(const Matrix& m);
    Matrix& operator*(const Matrix& m);
    Matrix& operator*(float b);

    friend std::istream& operator>>(std::istream&is, Matrix& m);
    friend std::ostream& operator<<(std::ostream& os, const Matrix& m);
    friend Matrix& operator*(float b, const Matrix& m);

};

Matrix::Matrix(): row(0), column(0), data(nullptr) {}

Matrix::Matrix(const Matrix &m) {
    row = m.row;
    column = m.column;
    data = m.data;
}

Matrix::Matrix(int r, int c, float *d): row(r), column(c), data(d) {}

Matrix::~Matrix() {
    delete[] data;
}

Matrix& Matrix::operator=(const Matrix &m) {
    if (this==&m)
        return *this;
    row = m.row;
    column = m.column;
    delete[] data;
    data = m.data;
    return *this;
}

Matrix& Matrix::operator*(const Matrix &m) {
    if (column != m.row) {
        std::cout << "Illegal!" << std::endl;
        return *this;
    }
    if (column % 8 != 0) {
        Matrix* res = new Matrix(row, m.column);
        float* d = new float[row * m.column];
        int point = 0;
        for (int i = 0; i < res->row; i++) {
            for (int j = 0; j < res->column; j++) {
                d[point] = 0;
#pragma omp parallel for
                for (int k = 0; k < this->column; k++) {
                    d[point] += this->data[i * this->column + k] * m.data[k * m.column + j];
                }
                point++;
            }
        }
        res->data = d;
        Matrix& r = *res;
        return r;
    }
    float* temp = new float[m.row * m.column];
    for (int i = 0; i < m.row; i++) {
#pragma omp parallel for
        for (int j = 0; j < m.column; j++) {
            temp[i * m.row + j] = m.data[j * m.row + i];
        }
    }
    size_t bound = column;
    Matrix* res = new Matrix(row, m.column);
    res->data = new float[row * m.column];
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < m.column; j++) {
            float sum[8] = { 0 };
            __m256 aa, bb;
            __m256 c = _mm256_setzero_ps();
#pragma omp parallel for
            for (size_t k = 0; k < bound; k += 8) {
                aa = _mm256_load_ps(&data[i] + k);
                bb = _mm256_load_ps(&temp[j] + k);
                c = _mm256_add_ps(c, _mm256_mul_ps(aa, bb));
            }
            _mm256_store_ps(sum, c);
            res->data[i * row + j] = sum[0] + sum[1] + sum[2] + sum[3] + sum[4] + sum[5] + sum[6] + sum[7];
        }
    }
    delete[] temp;
    Matrix& r = *res;
    return r;
}

Matrix& Matrix::operator*(float b) {
    Matrix *res = new Matrix(row, column);
    float* d = new float[row*column];
    int point = 0;
    while (point<row*column) {
        d[point]=data[point]*b;
        point++;
    }
    res->data=d;
    Matrix &r = *res;
    return r;
}

std::istream& operator>>(std::istream& is, Matrix& m) {
    int r, c;
    std::cout << "row: ";
    is >> r;
    m.row = r;
    std::cout << "column: ";
    is >> c;
    m.column = c;
    float* d = new float[r * c];
    if (m.data != nullptr) delete[] m.data;
    std::cout << "data: " << std::endl;
    int point = 0;
    for (int i = 0; i < m.row; i++) {
        for (int j = 0; j < m.column; j++) {
            is >> d[point];
            point++;
        }
    }
    m.data = d;
    return is;
}

std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    int point = 0;
    for (int i=0; i<m.row; i++) {
        for (int j=0; j<m.column; j++) {
            os << "\t" << m.data[point];
            point++;
        }
        os<<std::endl;
    }
    return os;
}

Matrix& operator*(float b, const Matrix& m) {
    int point = 0;
    Matrix *res = new Matrix(m.row, m.column);
    float* d = new float[m.row*m.column];
    while (point<m.row*m.column) {
        d[point]=m.data[point]*b;
        point++;
    }
    res->data=d;
    Matrix &r = *res;
    return r;
}

#endif //ASS4_MATRIX_H
