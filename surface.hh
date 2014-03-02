#ifndef _SURFACE_HH
#define _SURFACE_HH 1

#include "model.hh"
#include "frame_buffer.hh"

class light
{
public:
  light();
  light(int type, point pos, Color color);
  // location of light if a point, direction if a vector
  point pos;
  Color color;
};

class surface : public model
{
public:
  surface();
  surface(Color ambient, Color diffuse, Color specular,
	  double shininess, double refractive_index,
	  double reflective_weight, double refractive_weight);
  void set_lighting(Color ambient, Color diffuse, Color specular,
		    double shininess, double refractive_index,
		    double reflective_weight, double refractive_weight);
  // change color to reflect selected status
  virtual void select() = 0;
  virtual void deselect() = 0;
  // index of refraction
  double index() const;
  // reflection and refraction coefficients
  double reflection() const;
  double refraction() const;
  // determine in a coarse manner where ray intersects the object
  virtual double intersect(point orig, point dir) = 0;
  // determine a fine-grained intersection of ray and object, setting
  // vertex to the world-coordinate location of intersection, and
  // normal to the world-coordinate normal at that point
  virtual double fine_intersect(point orig, point dir, point &vertex, point &normal) = 0;
  // lighting calculations
  Color phong_ambient() const;
  // calculate non-ambient local illumination, as well as reflection
  // and refraction vectors if depth > 0
  Color phong(point dir, light l, int depth, point vertex, point normal);
protected:
  void init();
  Color ambient;
  Color diffuse;
  Color specular;
  double shininess;
  double refractive_index;
  double reflective_weight;
  double refractive_weight;
};

#endif /* _SURFACE_HH */
