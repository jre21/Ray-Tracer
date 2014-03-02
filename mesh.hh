#ifndef _MESH_HH
#define _MESH_HH 1

#include "point.hh"
#include "model.hh"
#include "surface.hh"

struct faceStruct
{
  int v1,v2,v3;
};

class mesh: public surface
{
public:
  mesh();
  mesh(const char *filename);
  mesh(Color ambient, Color diffuse, Color specular,
       double shininess, double refractive_index,
       double reflective_weight, double refractive_weight);
  mesh(const char *filename, Color ambient, Color diffuse,
       Color specular, double shininess, double refractive_index,
       double reflective_weight, double refractive_weight);
  ~mesh();
  // load mesh objects
  int load(const char *filename);
  int load(const char *filename, double scale, double rot_x, double rot_y,
	   double rot_z, double trans_x, double trans_y, double trans_z);
  // change color to reflect selected status
  void select();
  void deselect();
  // intersect a ray with bounding box
  double intersect(point orig, point dir);
  // intersect a ray with object and set vertex and normal
  double fine_intersect(point orig, point dir, point &vertex, point &normal);
protected:
  int verts, faces;           // Number of vertices, faces and normals
  point *vertList, *normList; // Vertex and Normal Lists
  faceStruct *faceList;	      // Face List
  // render the object
  void do_render();
  // handle internal dynamic structures
  void init();
  void deinit();
};

#endif /* _MESH_HH */
