#ifndef _SPHERE_HH
#define _SPHERE_HH 1

#include "point.hh"
#include "model.hh"
#include "surface.hh"

class sphere : public surface
{
public:
  sphere();
  sphere(Color ambient, Color diffuse, Color specular,
	 double shininess, double refractive_index,
	 double reflective_weight, double refractive_weight);
  void select();
  void deselect();
  // intersect a ray with sphere
  double intersect(point orig, point dir);
  // intersect a ray with sphere and set vertex and normal
  double fine_intersect(point orig, point dir, point &vertex, point &normal);
protected:
  // intersect a ray with sphere.  Set vertex and normal if store is true
  double do_intersect(point orig, point dir, point &vertex, point &normal, int store);
  void do_render();
};

#endif /* _SPHERE_HH */
