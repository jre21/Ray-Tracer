#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "point.hh"

// ############################## point ##############################
point::point()
{
  array[0] = 0.0;
  array[1] = 0.0;
  array[2] = 0.0;
  array[3] = 1.0;
}

point::point(double x, double y, double z)
{
  array[0] = x;
  array[1] = y;
  array[2] = z;
  array[3] = 1.0;
}

point::point(const point& p)
{
  for(int i = 0; i < 4; i++)
    array[i] = p.array[i];
}

double &point::operator[] (int i)
{
  return array[i];
}

point point::operator= (const point& p)
{
  for(int i = 0; i < 4; i++)
    array[i] = p.array[i];
  return *this;
}

double point::get_X() const
{
  return array[3] ? array[0] / array[3] : array[0];
}

double point::set_X(double x)
{
  return array[0] = array[3] ? x * array[3] : x;
}

double point::get_Y() const
{
  return array[3] ? array[1] / array[3] : array[1];
}

double point::set_Y(double y)
{
  return array[1] = array[3] ? y * array[3] : y;
}

double point::get_Z() const
{
  return array[3] ? array[2] / array[3] : array[2];
}

double point::set_Z(double z)
{
  return array[2] = array[3] ? z * array[3] : z;
}

double point::get_x() const
{
  return array[0];
}

double point::set_x(double x)
{
  return array[0] = x;
}

double point::get_y() const
{
  return array[1];
}

double point::set_y(double y)
{
  return array[1] = y;
}

double point::get_z() const
{
  return array[2];
}

double point::set_z(double z)
{
  return array[2] = z;
}

double point::get_w() const
{
  return array[3];
}

double point::set_w(double w)
{
  return array[3] = w;
}

int point::nonzero() const
{
  return array[0] || array[1] || array[2];
}

int point::is_vector() const
{
  return !array[3];
}

double point::norm() const
{
  double ret = 0;
  if(array[3])
    printf("Warning: norm called on non-vector\n");
  for(int i = 0; i < 4; i++)
    ret += array[i] * array[i];
  return sqrt(ret);
}

// Any votes for i = 0x5f375a86 - (i >> 1)?
point point::normalize()
{
  double tmp;
  if( (tmp = norm()) )
    for(int i = 0; i < 3; i++)
      array[i] /= tmp;
  return *this;
}

double point::operator* (const point &param) const
{
  double ret = 0;
  if(array[3] || param.array[3])
    printf("Warning: dot product called on non-vectors\n");
  for(int i = 0; i < 4; i++)
    ret += array[i] * param.array[i];
  return ret;
}

point point::operator- () const
{
  point ret(-array[0], -array[1], -array[2]);
  ret.array[3] = array[3];
  return ret;
}

point point::operator* (double param) const
{
  point ret(array[0] * param, array[1] * param, array[2] * param);
  ret.array[3] = array[3];
  return ret;
}

point point::operator*= (double param)
{
  array[0] *= param;
  array[1] *= param;
  array[2] *= param;
  return *this;
}

point point::operator/ (double param) const
{
  point ret(array[0] / param, array[1] / param, array[2] / param);
  ret.array[3] = array[3];
  return ret;
}

point point::operator/= (double param)
{
  array[0] /= param;
  array[1] /= param;
  array[2] /= param;
  return *this;
}

point point::operator+ (const point &param) const
{
  point ret;
  // Vectors can be added, but if one is a point we should deal with
  // homogeneous coordinates.  Two points cannot be added.
  if(!array[3] && !param.array[3])
    for(int i = 0; i < 4; i++)
      ret.array[i] = array[i] + param.array[i];
  else if(!array[3])
    for(int i = 0; i < 4; i++)
      ret.array[i] = param.array[3] * array[i] + param.array[i];
  else if(!param.array[3])
    for(int i = 0; i < 4; i++)
      ret.array[i] = array[i] + array[3] * param.array[i];
  else
    {
      printf("Error: incompatible point addition: %f %f\n",
	     array[3], param.array[3]);
      *(char*)0 = 0;
    }
  return ret;
}

point point::operator+= (const point &param)
{
  *this = *this + param;
  return *this;
}

point point::operator- (const point &param) const
{
  vector ret;
  // cannot subtract a point from a vector
  if(!array[3] && param.array[3])
    {
      printf("Error: incompatible point subtraction: %f %f\n",
	     array[3], param.array[3]);
      *(char*)0 = 0;
    }
  // subtract two points, accounting for homogeneous coordinates
  if(param.array[3])
    for(int i = 0; i < 3; i++)
      ret.array[i] = array[i] / array[3] - param.array[i] / param.array[3];
  // subtract a vector from a point
  else if(array[3])
    for(int i = 0; i < 4; i++)
      ret.array[i] = array[i] - array[3] * param.array[i];
  // subtract two vectors
  else
    for(int i = 0; i < 3; i++)
      ret.array[i] = array[i] - param.array[i];
  return ret;
}

point point::operator-= (const point &param)
{
  *this = *this - param;
  return *this;
}

point point::combine(const point &a, const point &b, double t)
{
  point ret;
  for(int i = 0; i < 4; i++)
    ret.array[i] = a.array[i] + t * (b.array[i] - a.array[i]);
  return ret;
}

point point::combine(const point &a, const point &b, double t,
		     const point &c, double u)
{
  point ret;
  for(int i = 0; i < 4; i++)
    ret.array[i] = a.array[i] + t * (b.array[i] - a.array[i])
      + u * (c.array[i] - a.array[i]);
  return ret;
}

void point::render() const
{
  if(!array[3])
    {
      printf("Warning: attempted to render vector {%g, %g, %g, %g}\n",
	     array[0], array[1], array[2], array[3]);
      return;
    }
  glVertex3d(array[0] / array[3], array[1] / array[3], array[2] / array[3]);
}

// ############################## vector ##############################
vector::vector()
{
  array[3] = 0.0;
}

vector::vector(double x, double y, double z) : point(x, y, z)
{
  array[3] = 0.0;
}

vector::vector(const point& p) : point (p)
{
  array[3] = 0.0;
  if(p.array[3] && p.array[3] != 1.0)
    for(int i = 0; i < 3; i++)
      array[i] /= p.array[3];
}

vector vector::operator = (const point& p)
{
  for(int i = 0; i < 3; i++)
    array[i] = p.array[i];
  array[4] = 0.0;
  if(p.array[3] && p.array[3] != 1.0)
    for(int i = 0; i < 3; i++)
      array[i] /= p.array[3];
  return *this;
}
