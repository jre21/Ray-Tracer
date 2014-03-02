#ifndef _MATRIX_HH
#define _MATRIX_HH 1

#include "point.hh"

class matrix
{
public:
  // constructors
  matrix();
  matrix(const matrix& m);
  matrix operator = (const matrix& m);
  // add two matrices
  matrix operator + (const matrix& param) const;
  matrix operator += (const matrix& param);
  // subtract two matrices
  matrix operator - (const matrix& param) const;
  matrix operator -= (const matrix& param);
  // multiply two matrices
  matrix operator * (const matrix& param) const;
  matrix operator *= (const matrix& param);
  // scalar multiplication
  matrix operator * (double param) const;
  matrix operator *= (double param);
  // invert a matrix which is a composition of rotations,
  // translations, and uniform scalings
  matrix inverse() const;
  matrix invert();
  // transform a point
  point operator * (const point& param) const;
  // load into current gl matrix
  void load() const;
  // multiply into current gl matrix
  void mult() const;
  // matrix equivalent to cross product
  static matrix cross (const point& param);
  // identity matrix
  static matrix identity();
  // rotate by angle theta round vector (vx,vy,vz)
  static matrix rotate(double theta, double vx, double vy, double vz);
  // scale each direction d by factor sd
  static matrix scale(double sx, double sy, double sz);
  // translate by the vector (tx,ty,tx)
  static matrix translate(double tx, double ty, double tz);
  // position the camera at origin, pointing toward focus, with up
  // as the direction toward the top of the screen
  static matrix look_at(point origin, point focus, point up);
  // perspective matrices
  static matrix persp(double near, double far, double image);
  static matrix inv_persp(double near, double far, double image);
protected:
  double array[4][4];
};

#endif /* _MATRIX_HH */
