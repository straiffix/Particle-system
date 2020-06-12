#include <stdlib.h>
#include <stdio.h>
#include "mesh.h"

float rnd() {
	return 2.0f * float(rand()) / float(RAND_MAX) - 1.0f;
}

void insertModel(Mesh **list, int nv, float * vArr, int nt, int * tArr, float scale) {
	Mesh * mesh = (Mesh *) malloc(sizeof(Mesh));
	mesh->nv = nv;
	mesh->nt = nt;	
	mesh->vertices = (Vector *) malloc(nv * sizeof(Vector));
	mesh->vnorms = (Vector *)malloc(nv * sizeof(Vector));
	mesh->triangles = (Triangle *) malloc(nt * sizeof(Triangle));
	
	// set mesh vertices
	for (int i = 0; i < nv; i++) {
		mesh->vertices[i].x = vArr[i*3] * scale;
		mesh->vertices[i].y = vArr[i*3+1] * scale;
		mesh->vertices[i].z = vArr[i*3+2] * scale;
	}

	// set mesh triangles
	for (int i = 0; i < nt; i++) {
		mesh->triangles[i].vInds[0] = tArr[i*3];
		mesh->triangles[i].vInds[1] = tArr[i*3+1];
		mesh->triangles[i].vInds[2] = tArr[i*3+2];
	}


	// Assignment 1: 
	// Calculate and store suitable vertex normals for the mesh here.
	// Replace the code below that simply sets some arbitrary normal values	

	//Ublock for normal, rotated, parallel, orth. 

	/*for (int i = 0; i < nv; i++) {
		mesh->vnorms[i].x = 0.80078125f;
		mesh->vnorms[i].y = 0.34765625f;
		mesh->vnorms[i].z = 0.1796875f;
	}*/

	//Unblock for 4 and 5

	for (int i = 0; i < nv; i++) {
		mesh->vnorms[i].x = 0.0f;
		mesh->vnorms[i].y = 0.0f;
		mesh->vnorms[i].z = 0.0f;
	}

	for (int i = 0; i < nt; i++) {
		int vert_1 = mesh->triangles[i].vInds[0];
		int vert_2 = mesh->triangles[i].vInds[1];
		int vert_3 = mesh->triangles[i].vInds[2];

		Vector A = mesh->vertices[vert_1];
		Vector B = mesh->vertices[vert_2];
		Vector C = mesh->vertices[vert_3];

		Vector face_normal = TriangleNormal(A, B, C);

		mesh->vnorms[vert_1] = Add(mesh->vnorms[vert_1], face_normal);
		mesh->vnorms[vert_2] = Add(mesh->vnorms[vert_2], face_normal);
		mesh->vnorms[vert_3] = Add(mesh->vnorms[vert_3], face_normal);
	}

	for (int i = 0; i < nv; i++) {
		mesh->vnorms[i] = Normalize(mesh->vnorms[i]);
	}

	mesh->next = *list;
	*list = mesh;	
}
