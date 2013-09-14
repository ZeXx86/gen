#include "gen.h"
#include "octree.h"
#include "terrain.h"

static octree_t *root;

octree_t *octree_root_get ()
{
	return root;
}
int cnt = 0;
octree_t *octree_node_alloc (octree_t *parent, int id)
{
	if (parent && parent->children[id])
		return 0;

	octree_t *node = (octree_t *) malloc (sizeof (octree_t));
	
	if (parent) {

		node->size_x = parent->size_x / 2;
		node->size_y = parent->size_y / 2;
		node->size_z = parent->size_z / 2;

		node->origin_x = parent->origin_x - node->size_x / 2;
		node->origin_y = parent->origin_y - node->size_y / 2;
		node->origin_z = parent->origin_z - node->size_z / 2;

		switch (id) {
			case 0:
				break;
			case 1:
				node->origin_x += node->size_x;
				break;
			case 2:
				node->origin_y += node->size_y;
				break;
			case 3:
				node->origin_x += node->size_x;
				node->origin_y += node->size_y;
				break;
			case 4:
				node->origin_z += node->size_z;
				break;
			case 5:
				node->origin_x += node->size_x;
				node->origin_z += node->size_z;
				break;
			case 6:
				node->origin_y += node->size_y;
				node->origin_z += node->size_z;
				break;
			case 7:
				node->origin_x += node->size_x;
				node->origin_y += node->size_y;
				node->origin_z += node->size_z;
				break;
		}

		parent->children[id] = node;
	} else {
		terrain_t *t = terrain_get ();

		node->size_x = t->dim_x;
		node->size_y = t->dim_y;
		node->size_z = t->dim_z;

		node->origin_x = node->size_x / 2;
		node->origin_y = node->size_y / 2;
		node->origin_z = node->size_z / 2;
	}
	
	if (node->size_x == 8) {
	cnt ++;
	printf ("%d: %d %d %d, %d %d %d, %d\n", cnt, node->origin_x, node->origin_y, node->origin_z, node->size_x, node->size_y, node->size_z, id);
	}
	node->children = (octree_t **) malloc (sizeof (octree_t *) * 8);
	
	for (int i = 0; i < 8; i ++)
		node->children[i] = NULL;
		
	node->parent = parent;

	//node->children = (octree_t *) malloc (sizeof (octree_t) * 8);
}

void octree_build_children (octree_t *parent, int id)
{
	octree_t *node = octree_node_alloc (parent, id);

	/* continue until node exist */
	if (!node)
		return;
	
	octree_build (node);
}

void octree_build (octree_t *parent)
{
	int o_x = parent->size_x / 2;		
	int o_y = parent->size_y / 2;
	int o_z = parent->size_z / 2;	

	if (o_x == 0 || o_y == 0 || o_z == 0)
		return;

	terrain_t *terrain = terrain_get ();
	
		for (int x = 0; x < terrain->dim_x; x ++) {
			for (int y = 0; y < terrain->dim_y; y ++) {
				for (int z = 0; z < terrain->dim_z; z ++) {
					voxel_t *v = terrain_voxel_get (x, y, z);
					
					if (v->value <= 0.0f)
						continue;
		
					/* Nachazi se voxel uvnitr bounding boxu konkretniho rodice? */
					if (x >= (parent->origin_x - o_x) && x <= (parent->origin_x) &&
					    y >= (parent->origin_y - o_y) && y <= (parent->origin_y) &&
					    z >= (parent->origin_z - o_z) && z <= (parent->origin_z))
						octree_build_children (parent, 0);
					if (x >= (parent->origin_x) && x <= (parent->origin_x + o_x) &&
					    y >= (parent->origin_y - o_y) && y <= (parent->origin_y) &&
					    z >= (parent->origin_z - o_z) && z <= (parent->origin_z))
						octree_build_children (parent, 1);
						
					if (x >= (parent->origin_x - o_x) && x <= (parent->origin_x) &&
					    y >= (parent->origin_y) && y <= (parent->origin_y + o_y) &&
					    z >= (parent->origin_z - o_z) && z <= (parent->origin_z))
						octree_build_children (parent, 2);
					if (x >= (parent->origin_x) && x <= (parent->origin_x + o_x) &&
					    y >= (parent->origin_y) && y <= (parent->origin_y + o_y) &&
					    z >= (parent->origin_z - o_z) && z <= (parent->origin_z))
						octree_build_children (parent, 3);
						
						
					if (x >= (parent->origin_x - o_x) && x <= (parent->origin_x) &&
					    y >= (parent->origin_y - o_y) && y <= (parent->origin_y) &&
					    z >= (parent->origin_z) && z <= (parent->origin_z + o_z))
						octree_build_children (parent, 4);
					if (x >= (parent->origin_x) && x <= (parent->origin_x + o_x) &&
					    y >= (parent->origin_y - o_y) && y <= (parent->origin_y) &&
					    z >= (parent->origin_z) && z <= (parent->origin_z + o_z))
						octree_build_children (parent, 5);
						
					if (x >= (parent->origin_x - o_x) && x <= (parent->origin_x) &&
					    y >= (parent->origin_y) && y <= (parent->origin_y + o_y) &&
					    z >= (parent->origin_z) && z <= (parent->origin_z + o_z))
						octree_build_children (parent, 6);
					if (x >= (parent->origin_x) && x <= (parent->origin_x + o_x) &&
					    y >= (parent->origin_y) && y <= (parent->origin_y + o_y) &&
					    z >= (parent->origin_z) && z <= (parent->origin_z + o_z))
						octree_build_children (parent, 7);
				}
			}
		}
}


bool octree_init ()
{
	root = octree_node_alloc (NULL, 0);
	
	octree_build (root);
	
	return true;
}
