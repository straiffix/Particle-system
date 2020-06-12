#ifndef _ALGEBRA_H_
#define _ALGEBRA_H_

typedef struct { float x, y, z; } Vector;
typedef struct { float x, y, z, w; } HomVector;

/* Column-major order are used for the matrices here to be compatible with OpenGL.
** The indices used to access elements in the matrices are shown below.
**  _                _
** |                  |
** |   0   4   8  12  |
** |                  |
** |   1   5   9  13  |
** |                  |
** |   2   6  10  14  |
** |                  |
** |   3   7  11  15  |
** |_                _|
*/
typedef struct matrix { float e[16]; } Matrix;

Vector Add(Vector a, Vector b);
Vector Subtract(Vector a, Vector b);
Vector CrossProduct(Vector a, Vector b);
float DotProduct(Vector a, Vector b);
float Length(Vector a);
Vector Normalize(Vector a);
Vector ScalarVecMul(float t, Vector a);
HomVector MatVecMul(Matrix a, Vector b);
Vector Homogenize(HomVector a);
Matrix MatMatMul(Matrix a, Matrix b);
void PrintMatrix(const char *name, Matrix m);
void PrintVector(const char *name, Vector v);
void PrintHomVector(char *name, HomVector h);


Vector TriangleNormal(Vector a, Vector b, Vector c);


Vector TranslationVector(Vector a, float dx, float dy, float dz);
Matrix TranslationCamera(float dx, float dy, float dz);
Vector ScalingVector(Vector a, float cx, float cy, float cz);
Vector RotationXVector(Vector a, float ang_a);
Vector RotationYVector(Vector a, float ang_b);
Vector RotationZVector(Vector a, float ang_y);


Matrix RotationXMatrix(float ang_a);
Matrix RotationYMatrix(float ang_b);
Matrix RotationZMatrix(float ang_y);
Matrix TransformCamera(float ang_a, float ang_b, float ang_y, float cx, float cy, float cz);
Matrix ScalingMatrix(float sx, float sy, float sz);

float Rad(float angle);
float cotan(float i);

Matrix ProjectionOrth(float left, float right, float bottom, float top, float near, float far);
Matrix ProjectionPer(float fovy, float aspect, float near, float far);

Matrix TransformModelMatrix(Vector s, Vector r, Vector t);

#endif

