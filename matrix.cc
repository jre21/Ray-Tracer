#include <math.h>
#include <stdio.h>
#include <GL/gl.h>

#include "matrix.hh"

matrix::matrix() {}
matrix::matrix(const matrix& m)
{
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      array[i][j] = m.array[i][j];
}

matrix matrix::operator= (const matrix& m)
{
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      array[i][j] = m.array[i][j];
  return *this;
}

matrix matrix::operator+ (const matrix& param) const
{
  matrix ret;
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      // fourth row must remain invarient for homogeneous coordinates to work
      ret.array[i][j] = array[i][j] + (j < 3 ? param.array[i][j] : 0);
  return ret;
}

matrix matrix::operator+= (const matrix& param)
{
  // fourth row must remain invarient for homogeneous coordinates to work
  for(int i = 0; i < 3; i++)
    for(int j = 0; j < 4; j++)
      array[i][j] += param.array[i][j];
  return *this;
}

matrix matrix::operator- (const matrix& param) const
{
  matrix ret;
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      // fourth row must remain invarient for homogeneous coordinates to work
      ret.array[i][j] = array[i][j] - (j < 3 ? param.array[i][j] : 0);
  return ret;
}

matrix matrix::operator-= (const matrix& param)
{
  // fourth row must remain invarient for homogeneous coordinates to work
  for(int i = 0; i < 3; i++)
    for(int j = 0; j < 4; j++)
      array[i][j] -= param.array[i][j];
  return *this;
}

matrix matrix::operator* (const matrix& param) const
{
  matrix ret;
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      {
	ret.array[i][j] = 0;
	for(int k = 0; k < 4; k++)
	  ret.array[i][j] += array[i][k] * param.array[k][j];
      }
  return ret;
}

matrix matrix::operator*= (const matrix& param)
{
  matrix ret;
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      {
	ret.array[i][j] = 0;
	for(int k = 0; k < 4; k++)
	  ret.array[i][j] += array[i][k] * param.array[k][j];
      }
  return *this = ret;
}

matrix matrix::operator* (double param) const
{
  matrix ret;
  for(int i = 0; i < 3; i++)
    for(int j = 0; j < 4; j++)
      ret.array[i][j] = param * array[i][j];
  // don't scale the fourth row, or we'll mess up homogeneous coordinates
  for(int j = 0; j < 4; j++)
    ret.array[3][j] = array[3][j];
  return ret;
}

matrix matrix::operator*= (double param)
{
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 3; j++)
      array[i][j] = param * array[i][j];
  // don't scale the fourth row, or we'll mess up homogeneous coordinates
  for(int j = 0; j < 4; j++)
    array[3][j] = array[3][j];
  return *this;
}

matrix matrix::inverse() const
{
  matrix ret = matrix::identity();
  double dot = 1.0 / (array[0][0] * array[0][0] + array[1][0] * array[1][0]
			+ array[2][0] * array[2][0]);
  for(int i = 0; i < 3; i++)
    for(int j = 0; j < 3; j++)
      {
	// transpose the rotation and invert the scaling
	ret.array[i][j] = dot * array[j][i];
	// reflect the translation, and convert to original basis
	ret.array[i][3] -= dot * array[j][i] * array[j][3];
      }
  return ret;
}

matrix matrix::invert()
{
  *this = this->inverse();
  return *this;
}

point matrix::operator* (const point& param) const
{
  vector ret;
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      ret.array[i] += array[i][j] * param.array[j];
  return ret;
}

void matrix::load() const
{
  double elems [16];
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      elems[4 * i + j] = array[j][i];
  glLoadMatrixd(elems);
}

void matrix::mult() const
{
  double elems [16];
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      elems[4 * i + j] = array[j][i];
  glMultMatrixd(elems);
}

matrix matrix::cross(const point& param)
{
  matrix omega;
  if(!param.is_vector())
    printf("Warning: cross called on non-vector\n");
  omega.array[0][0] = 0.0;
  omega.array[0][1] = -param.get_Z();
  omega.array[0][2] = param.get_Y();
  omega.array[0][3] = 0.0;
  omega.array[1][0] = param.get_Z();
  omega.array[1][1] = 0.0;
  omega.array[1][2] = -param.get_X();
  omega.array[1][3] = 0.0;
  omega.array[2][0] = -param.get_Y();
  omega.array[2][1] = param.get_X();
  omega.array[2][2] = 0.0;
  omega.array[2][3] = 0.0;
  omega.array[3][0] = 0.0;
  omega.array[3][1] = 0.0;
  omega.array[3][2] = 0.0;
  omega.array[3][3] = 1.0;
  return omega;
}

matrix matrix::identity()
{
  matrix ret;
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      ret.array[i][j] = i == j ? 1.0 : 0.0;
  return ret;
}

matrix matrix::translate(double tx, double ty, double tz)
{
  matrix ret;
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 3; j++)
      ret.array[i][j] = i == j ? 1.0 : 0.0;
  ret.array[0][3] = tx;
  ret.array[1][3] = ty;
  ret.array[2][3] = tz;
  ret.array[3][3] = 1.0;
  return ret;
}

matrix matrix::rotate(double theta, double vx, double vy, double vz)
{
  vector v(vx, vy, vz);
  matrix omega = cross(v.normalize());
  matrix ret = matrix::identity() + omega * sin(theta)
    + omega * omega * (1 - cos(theta));
  return ret;
}

matrix matrix::scale(double sx, double sy, double sz)
{
  matrix ret;
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 4; j++)
      ret.array[i][j] = 0.0;
  ret.array[0][0] = sx;
  ret.array[1][1] = sy;
  ret.array[2][2] = sz;
  ret.array[3][3] = 1.0;
  return ret;
}

matrix matrix::look_at(point origin, point focus, point up)
{
  int i;
  matrix ret = identity();
  // force points to the correct type
  if(!origin.array[3]) origin.array[3] = 1;
  if(!focus.array[3]) focus.array[3] = 1;
  if(up.array[3] != 0 && up.array[3] != 1)
    {
      up /= up.array[3];
      up.array[3] = 0;
    }
  // handle origin/focus degeneracy
  if(!(origin - focus).norm())
    {
      focus = origin - vector(0,0,1);
      printf("Warning: matrix::look_at(): origin and focus are same point\n");
    }
  // calculate basis vectors
  point z = (origin - focus).normalize(),
    y = (up - z * (up * z)).normalize();
  // pick a default if up in direction of origin - focus
  if(!y.norm())
    {
      printf("Warning: matrix::look_at(): degeneracy of axes\n");
      y = (vector(0,1,0) - z * (vector(0,1,0) * z)).normalize();
      if(!y.norm()) y = (vector(0,0,1) - z * (vector(0,0,1) * z)).normalize();
    }
  point x = cross(y) * z;
  // build matrix as rotation
  for(i = 0; i < 3; i++)
    {
      ret.array[0][i] = x.array[i];
      ret.array[1][i] = y.array[i];
      ret.array[2][i] = z.array[i];
    }
  // add translation component
  point shift = -(ret * origin);
  for(i = 0; i < 3; i++)
    ret.array[i][3] = shift.array[i];
  return ret;
}

// perspective transformation, based on Watt Handout
// Assume the view plane is centered at the origin and scaled to the
// output window
matrix matrix::persp(double near, double far, double image)
{
  matrix ret = identity();
  ret.array[0][0] = ret.array[1][1] = image;
  ret.array[2][2] = (near + far) / (near - far);
  ret.array[2][3] = 2 * near * far / (near - far);
  ret.array[3][2] = -1;
  ret.array[3][3] = 0;
  return ret;
}

// inverse perspective transformation, based on Watt Handout
matrix matrix::inv_persp(double near, double far, double image)
{
  matrix ret = identity();
  ret.array[0][0] = ret.array[1][1] = 1.0 / image;
  ret.array[2][2] = 0;
  ret.array[2][3] = -1;
  ret.array[3][2] = (near - far) / (2 * near * far);
  ret.array[3][3] = (near + far) / (2 * near * far);
  return ret;
}
