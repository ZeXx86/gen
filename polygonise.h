#ifndef __polygonise_h__
#define __polygonise_h__

typedef struct {
	double x;
	double y;
	double z;
} vertex_t, vector_t;

typedef struct {
   vertex_t p[3];
} triangle_t;

typedef struct {
   vertex_t p[8];
   double val[8];
} gridcell_t;

#define POLY_EPSILON 	0.00001

extern vector_t polygonise_trg_norm (triangle_t t);
extern int polygonise_grid (gridcell_t grid, double isolevel, triangle_t *triangles);
extern bool polygonise_init ();

#endif

