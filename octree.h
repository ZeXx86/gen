#ifndef __octree_h__
#define __octree_h__

typedef struct octree {
	int origin_x, origin_y, origin_z;
	
	int size_x, size_y, size_z;
	
	octree *parent;
	octree **children;
} octree_t;

extern octree_t *octree_root_get ();
extern void octree_build (octree_t *parent);
extern bool octree_init ();

#endif

