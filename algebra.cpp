#define _USE_MATH_DEFINES // To get M_PI defined
#include <math.h>
#include <stdio.h>
#include "algebra.h"

Vector CrossProduct(Vector a, Vector b) {
	Vector v = { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x };
	return v;
}

float DotProduct(Vector a, Vector b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector Subtract(Vector a, Vector b) {
	Vector v = { a.x-b.x, a.y-b.y, a.z-b.z };
	return v;
}    

Vector Add(Vector a, Vector b) {
	Vector v = { a.x+b.x, a.y+b.y, a.z+b.z };
	return v;
}    

float Length(Vector a) {
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

Vector Normalize(Vector a) {
	float len = Length(a);
	Vector v = { a.x/len, a.y/len, a.z/len };
	return v;
}

Vector ScalarVecMul(float t, Vector a) {
	Vector b = { t*a.x, t*a.y, t*a.z };
	return b;
}

HomVector MatVecMul(Matrix a, Vector b) {
	HomVector h;
	h.x = b.x*a.e[0] + b.y*a.e[4] + b.z*a.e[8] + a.e[12];
	h.y = b.x*a.e[1] + b.y*a.e[5] + b.z*a.e[9] + a.e[13];
	h.z = b.x*a.e[2] + b.y*a.e[6] + b.z*a.e[10] + a.e[14];
	h.w = b.x*a.e[3] + b.y*a.e[7] + b.z*a.e[11] + a.e[15];
	return h;
}

Vector Homogenize(HomVector h) {
	Vector a;
	if (h.w == 0.0) {
		fprintf(stderr, "Homogenize: w = 0\n");
		a.x = a.y = a.z = 9999999;
		return a;
	}
	a.x = h.x / h.w;
	a.y = h.y / h.w;
	a.z = h.z / h.w;
	return a;
}

Matrix MatMatMul(Matrix a, Matrix b) {
	Matrix c;
	int i, j, k;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			c.e[j*4+i] = 0.0;
			for (k = 0; k < 4; k++)
				c.e[j*4+i] += a.e[k*4+i] * b.e[j*4+k];
		}
	}
	return c;
}



Vector TriangleNormal(Vector a, Vector b, Vector c) {
	Vector sub_1 = Subtract(b, a);
	Vector sub_2 = Subtract(c, a);
	Vector result = CrossProduct(sub_1, sub_2);
	Vector normalised_result = Normalize(result);
	return normalised_result;
}



Vector TranslationVector(Vector a, float dx, float dy, float dz) {
	Matrix T;
	T.e[0] = 1.0f; T.e[4] = 0.0f; T.e[8] = 0.0f; T.e[12] = dx;
	T.e[1] = 0.0f; T.e[5] = 1.0f; T.e[9] = 0.0f; T.e[13] = dy;
	T.e[2] = 0.0f; T.e[6] = 0.0f; T.e[10] = 1.0f; T.e[14] = dz;
	T.e[3] = 0.0f; T.e[7] = 0.0f; T.e[11] = 0.0f; T.e[15] = 1.0f;
	HomVector new_vector = MatVecMul(T, a);
	Vector result = Homogenize(new_vector);
	return result;


}


Vector ScalingVector(Vector a, float sx, float sy, float sz) {
	Matrix S;
	S.e[0] = sx; S.e[4] = 0.0f; S.e[8] = 0.0f; S.e[12] = 0.0f;
	S.e[1] = 0.0f; S.e[5] = sy; S.e[9] = 0.0f; S.e[13] = 0.0f;
	S.e[2] = 0.0f; S.e[6] = 0.0f; S.e[10] = sz; S.e[14] = 0.0f;
	S.e[3] = 0.0f; S.e[7] = 0.0f; S.e[11] = 0.0f; S.e[15] = 1.0f;
	HomVector new_vector = MatVecMul(S, a);
	Vector result = Homogenize(new_vector);
	return result;
}

Vector RotationXVector(Vector a, float ang_a) {
	ang_a = ang_a * -1;
	ang_a = Rad(ang_a);
	Matrix R;
	R.e[0] = 1.0f; R.e[4] = 0.0f; R.e[8] = 0.0f; R.e[12] = 0.0f;
	R.e[1] = 0.0f; R.e[5] = cos(ang_a); R.e[9] = -1 * sin(ang_a); R.e[13] = 0.0f;
	R.e[2] = 0.0f; R.e[6] = sin(ang_a); R.e[10] = cos(ang_a); R.e[14] = 0.0f;
	R.e[3] = 0.0f; R.e[7] = 0.0f; R.e[11] = 0.0f; R.e[15] = 1.0f;
	HomVector new_vector = MatVecMul(R, a);
	Vector result = Homogenize(new_vector);
	return result;
}

Vector RotationYVector(Vector a, float ang_b) {
	ang_b = ang_b * -1;
	ang_b = Rad(ang_b);
	Matrix R;
	R.e[0] = cos(ang_b); R.e[4] = 0.0f; R.e[8] = sin(ang_b); R.e[12] = 0.0f;
	R.e[1] = 0.0f; R.e[5] = 1.0f; R.e[9] = 0.0f; R.e[13] = 0.0f;
	R.e[2] = -1 * sin(ang_b); R.e[6] = 0.0f; R.e[10] = cos(ang_b); R.e[14] = 0.0f;
	R.e[3] = 0.0f; R.e[7] = 0.0f; R.e[11] = 0.0f; R.e[15] = 1.0f;
	HomVector new_vector = MatVecMul(R, a);
	Vector result = Homogenize(new_vector);
	return result;
}

Vector RotationZVector(Vector a, float ang_y) {
	ang_y = ang_y * -1;
	ang_y = Rad(ang_y);
	Matrix R;
	R.e[0] = cos(ang_y); R.e[4] = -1 * sin(ang_y); R.e[8] = 0.0f; R.e[12] = 0.0f;
	R.e[1] = sin(ang_y); R.e[5] = cos(ang_y); R.e[9] = 0.0f; R.e[13] = 0.0f;
	R.e[2] = 0.0f; R.e[6] = 0.0f; R.e[10] = 1.0f; R.e[14] = 0.0f;
	R.e[3] = 0.0f; R.e[7] = 0.0f; R.e[11] = 0.0f; R.e[15] = 1.0f;
	HomVector new_vector = MatVecMul(R, a);
	Vector result = Homogenize(new_vector);
	return result;
}

float Rad(float angle) {
	return angle * M_PI / 180.0;
}

Matrix TranslationCamera(float dx, float dy, float dz) {
	Matrix T;
	T.e[0] = 1.0f; T.e[4] = 0.0f; T.e[8] = 0.0f; T.e[12] = dx;
	T.e[1] = 0.0f; T.e[5] = 1.0f; T.e[9] = 0.0f; T.e[13] = dy;
	T.e[2] = 0.0f; T.e[6] = 0.0f; T.e[10] = 1.0f; T.e[14] = dz;
	T.e[3] = 0.0f; T.e[7] = 0.0f; T.e[11] = 0.0f; T.e[15] = 1.0f;
	return T;
}

Matrix RotationXMatrix(float ang_a) {
	ang_a = ang_a * -1;
	ang_a = Rad(ang_a);
	Matrix R;
	R.e[0] = 1.0f; R.e[4] = 0.0f; R.e[8] = 0.0f; R.e[12] = 0.0f;
	R.e[1] = 0.0f; R.e[5] = cos(ang_a); R.e[9] = -1 * sin(ang_a); R.e[13] = 0.0f;
	R.e[2] = 0.0f; R.e[6] = sin(ang_a); R.e[10] = cos(ang_a); R.e[14] = 0.0f;
	R.e[3] = 0.0f; R.e[7] = 0.0f; R.e[11] = 0.0f; R.e[15] = 1.0f;
	return R;
}


Matrix RotationYMatrix(float ang_b) {
	ang_b = ang_b * -1;
	ang_b = Rad(ang_b);
	Matrix R;
	R.e[0] = cos(ang_b); R.e[4] = 0.0f; R.e[8] = sin(ang_b); R.e[12] = 0.0f;
	R.e[1] = 0.0f; R.e[5] = 1.0f; R.e[9] = 0.0f; R.e[13] = 0.0f;
	R.e[2] = -1 * sin(ang_b); R.e[6] = 0.0f; R.e[10] = cos(ang_b); R.e[14] = 0.0f;
	R.e[3] = 0.0f; R.e[7] = 0.0f; R.e[11] = 0.0f; R.e[15] = 1.0f;
	return R;
}


Matrix RotationZMatrix(float ang_y) {
	ang_y = ang_y * -1;
	ang_y = Rad(ang_y);
	Matrix R;
	R.e[0] = cos(ang_y); R.e[4] = -1 * sin(ang_y); R.e[8] = 0.0f; R.e[12] = 0.0f;
	R.e[1] = sin(ang_y); R.e[5] = cos(ang_y); R.e[9] = 0.0f; R.e[13] = 0.0f;
	R.e[2] = 0.0f; R.e[6] = 0.0f; R.e[10] = 1.0f; R.e[14] = 0.0f;
	R.e[3] = 0.0f; R.e[7] = 0.0f; R.e[11] = 0.0f; R.e[15] = 1.0f;
	return R;
}

Matrix ScalingMatrix(float sx, float sy, float sz) {
	Matrix S;
	S.e[0] = sx; S.e[4] = 0.0f; S.e[8] = 0.0f; S.e[12] = 0.0f;
	S.e[1] = 0.0f; S.e[5] = sy; S.e[9] = 0.0f; S.e[13] = 0.0f;
	S.e[2] = 0.0f; S.e[6] = 0.0f; S.e[10] = sz; S.e[14] = 0.0f;
	S.e[3] = 0.0f; S.e[7] = 0.0f; S.e[11] = 0.0f; S.e[15] = 1.0f;
	return S;
}



Matrix TransformCamera(float ang_a, float ang_b, float ang_y, float cx, float cy, float cz) {
	Matrix M_t = TranslationCamera(-cx, -cy, -cz);
	Matrix R_x = RotationXMatrix(ang_a);
	Matrix R_y = RotationYMatrix(ang_b);
	Matrix R_z = RotationZMatrix(ang_y);
	Matrix new_a = MatMatMul(R_x, M_t);
	Matrix new_b = MatMatMul(R_y, new_a);
	Matrix new_c = MatMatMul(R_z, new_b);
	return new_c;
}

float cotan(float i) { return(cos(i) / sin(i)); }

Matrix ProjectionOrth(float left, float right, float bottom, float top, float near, float far) {
	Matrix P;
	P.e[0] = 2 / (right - left); P.e[4] = 0.0f; P.e[8] = 0.0f; P.e[12] = -1 * (right + left) / (right - left);
	P.e[1] = 0.0f; P.e[5] = 2 / (top - bottom); P.e[9] = 0.0f; P.e[13] = -1 * (top + bottom) / (top - bottom);
	P.e[2] = 0.0f; P.e[6] = 0.0f; P.e[10] = 2 / (near - far); P.e[14] = -1 * (far + near) / (far - near);
	P.e[3] = 0.0f; P.e[7] = 0.0f; P.e[11] = 0.0f; P.e[15] = 1.0f;
	return P;
}

Matrix ProjectionPer(float fovy, float aspect, float near, float far) {
	Matrix P;
	fovy = Rad(fovy);
	P.e[0] = cotan(fovy / 2) / aspect; P.e[4] = 0.0f; P.e[8] = 0.0f; P.e[12] = 0.0f;
	P.e[1] = 0.0f; P.e[5] = cotan(fovy / 2); P.e[9] = 0.0f; P.e[13] = 0.0f;
	P.e[2] = 0.0f; P.e[6] = 0.0f; P.e[10] = (far + near) / (near - far); P.e[14] = (2 * far*near) / (near - far);
	P.e[3] = 0.0f; P.e[7] = 0.0f; P.e[11] = -1.0f; P.e[15] = 0.0f;
	return P;
}

//Carefull with minuses
Matrix TransformModelMatrix(Vector s, Vector r, Vector t) {
	Matrix S = ScalingMatrix(s.x, s.y, s.z);
	Matrix R_z = RotationZMatrix(r.z);
	Matrix R_y = RotationYMatrix(r.y);
	Matrix R_x = RotationXMatrix(r.x);
	Matrix T = TranslationCamera(t.x, t.y, t.z);
	Matrix M_1 = MatMatMul(R_z, S);
	Matrix M_2 = MatMatMul(R_y, M_1);
	Matrix M_3 = MatMatMul(R_x, M_2);
	Matrix W = MatMatMul(T, M_3);
	return W;

}



void PrintVector(const char *name, Vector a) {
	printf("%s: %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z);
}

void PrintHomVector(char *name, HomVector a) {
	printf("%s: %6.5lf %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z, a.w);
}

void PrintMatrix(const char *name, Matrix a) { 
	int i,j;

	printf("%s:\n", name);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			printf("%6.5lf ", a.e[j*4+i]);
		}
		printf("\n");
	}
}
