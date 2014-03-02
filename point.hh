#ifndef _POINT_HH
#define _POINT_HH 1

// three-dimensional point
class point
{
public:
  // get the point (0,0,0)
  point();
  // get the point (x,y,z)
  point(double x, double y, double z);
  point(const point& p);
  double &operator[] (int i);
  point operator = (const point& p);
  // get and set elements
  double get_X() const;
  double set_X(double x);
  double get_Y() const;
  double set_Y(double y);
  double get_Z() const;
  double set_Z(double z);
  // get and set elements
  double get_x() const;
  double set_x(double x);
  double get_y() const;
  double set_y(double y);
  double get_z() const;
  double set_z(double z);
  double get_w() const;
  double set_w(double w);
  // return true if this is not the zero point or zero vector
  int nonzero() const;
  // check whether point or vector
  int is_vector() const;
  // return the norm
  double norm() const;
  // normalize this point
  point normalize();
  // take the dot product of two points
  double operator * (const point &param) const;
  // scale by a constant
  point operator * (double param) const;
  point operator *= (double param);
  //unary -
  point operator - () const;
  // scale by the inverse of a constant
  point operator / (double param) const;
  point operator /= (double param);
  // add two points (only makes physical sense if at least one is a vector)
  point operator + (const point &param) const;
  point operator += (const point &param);
  // subtract two points (only makes physical sense if the second is a vector)
  point operator - (const point &param) const;
  point operator -= (const point &param);
  // take a linear combination of two points
  static point combine(const point &a, const point &b, double t);
  static point combine(const point &a, const point &b, double t,
		       const point &c, double u);
  // render this vertex within the current glBegin environment
  void render() const;
  // let matrices and vectors access the array
  friend class classified_point;
  friend class vector;
  friend class matrix;
protected:
  // first three coordinates are vector components
  // fourth distinguishes points from vectors
  double array[4];
};

// three-dimensional vector
class vector: public point
{
public:
  vector();
  vector(double x, double y, double z);
  vector(const point& p);
  vector operator = (const point& p);
};
#endif /* _POINT_HH */
