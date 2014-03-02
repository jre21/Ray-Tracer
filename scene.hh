#ifndef _SCENE_HH
#define _SCENE_HH

#include "mesh.hh"
#include "sphere.hh"

class scene: public model
{
public:
  scene();
  // preload scene from a file
  scene(const char *filename);
  ~scene();
  // load scene from a file
  int load(const char *filename);
  // transform object about global axes
  void rotate(double theta, double vx, double vy, double vz);
  void scale(double sx, double sy, double sz);
  void translate(double tx, double ty, double tz);
  // transform object about local axes
  void rotate_local(double theta, double vx, double vy, double vz);
  void scale_local(double sx, double sy, double sz);
  void translate_local(double tx, double ty, double tz);
  // select the nearest model (if any) which intersects a given ray
  void intersection(point orig, point dir);
  // perform a ray-tracing step (if depth = 0, just calculate local lighting)
  Color ray_trace(point orig, point dir, double index, int depth);
  // get transformation matrices
  matrix get_state();
protected:
  light * lights;
  int num_lights;
  sphere *spheres;
  int num_spheres;
  mesh *meshes;
  int num_meshes;
  int num_surfaces;
  int selected; // selected object
  // reflect and refract
  point reflect(point incoming, point normal);
  point refract(point incoming, point normal, double n1, double n2);
  // clean up
  void unload();
  // fetch specified surface
  surface * get_surface(int i);
  // propegate changing of axes to children
  void on_set_axes();
  void on_unset_axes();
  void on_set_box();
  void on_unset_box();
  // select an object
  void select(int object);
  // render the scene
  void do_render();
};

#endif /* _SCENE_HH */
