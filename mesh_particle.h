/* Definition of a triangle mesh for a particle*/

struct particle_ {
	int nov;
	int nof;
	float verts[9];
	int faces[3];
};

struct particle_ particle = {

	3, /* number of vertices */

	1, /* number of faces */

	{ /* vertices */
	0.000000, 0.000000, 1.000000,
	1.000000, 0.000000, 0.000000,
	0.000000, 1.000000, 0.000000
	},

	{ /* face indices */
	0, 1, 2
	}
};
