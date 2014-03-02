#include <math.h>
#include <GL/gl.h>
#include "sphere.hh"

#define EPSILON 1e-6

sphere::sphere() {}

sphere::sphere(Color ambient_, Color diffuse_, Color specular_,
	       double shininess_, double refractive_index_,
	       double reflective_weight_, double refractive_weight_)
  : surface(ambient_, diffuse_, specular_, shininess_,
	    refractive_index_, reflective_weight_, refractive_weight_) {}

void sphere::select()
{
  set_color(1,1,0);
}
void sphere::deselect()
{
  set_color(0,0,1);
}

double sphere::intersect(point orig, point dir)
{
  point vertex;
  vector normal;
  return do_intersect(orig, dir, vertex, normal, 0);
}

double sphere::fine_intersect(point orig, point dir, point &vertex, point &normal)
{
  return do_intersect(orig, dir, vertex, normal, 1);
}

double sphere::do_intersect(point orig, point dir, point &vertex, point &normal, int store)
{
  matrix inv_state = state.inverse();
  // translate to object coordinates
  orig = inv_state * orig;
  dir = inv_state * dir;

  // compute intersection with perpendicular plane through sphere
  // center as a multiple of vector length
  double t = - (dir * vector(orig)) / (dir * dir);

  // compute distance from center of sphere to intersection
  double u = vector(orig + dir * t).norm();

  // we don't intersect if distance is greater than radius
  if(u > 1) return -1;

  // find distance from t to intersections with sphere as a multiple
  // of vector length
  u = sqrt(1 - u * u) / dir.norm();

  // return nearest intersection point which is in front of ray origin
  if(t > EPSILON)
    if(u < t - EPSILON)
      t -= u;
    else t += u;
  else
    if(u > EPSILON - t)
      t += u;
    else
      t = -1.0;
  if(store && t != -1.0)
    {
      // set vertex and normal based on intersection
      point v = orig + dir * t;
      vertex = state * v;
      normal = (state * vector(v)).normalize();
    }
  return t;
}

void sphere::do_render()
{
  // render an octahedron of the appropriate radius
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3d(r,g,b);
  glBegin(GL_TRIANGLES);
  for(int i = 0; i < 8; i++)
    {
      ( i & 1 ? point(0,0,1) : point(0,0,-1) ).render();
      ( i & 2 ? point(0,1,0) : point(0,-1,0) ).render();
      ( i & 4 ? point(1,0,0) : point(-1,0,0) ).render();
    }
  glEnd();
}
