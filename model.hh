#ifndef _MODEL_HH
#define _MODEL_HH 1

#include "matrix.hh"

class axes;
class box;

class model
{
public:
  // constructor
  model();
  model(double r, double g, double b);
  ~model();
  // render the object
  void render();
  void render(matrix transformation);
  // turn axes on or off
  virtual void set_axes();
  virtual void unset_axes();
  virtual int toggle_axes();
  // turn bounding boxes on or off
  void set_box();
  void unset_box();
  int toggle_box();
  // set color
  void set_color(double r, double g, double b);
  // transform object about global axes
  virtual void rotate(double theta, double vx, double vy, double vz);
  virtual void scale(double sx, double sy, double sz);
  virtual void translate(double tx, double ty, double tz);
  // transform object about local axes
  virtual void rotate_local(double theta, double vx, double vy, double vz);
  virtual void scale_local(double sx, double sy, double sz);
  virtual void translate_local(double tx, double ty, double tz);
  // get transformation matrices
  virtual matrix get_state() const;
protected:
  matrix state;
  int show_axes;
  axes *ax;
  int show_bound;
  box *bound;
  // color
  double r;
  double g;
  double b;
  // Intersect a ray with a triangle (square=false) or parallelogram
  // (square=true).  The point of intersection has coordinates
  // orig+t*dir, (1-u-v)*vert0+u*vert1+v*vert2
  int mt_intersect(point orig, point dir, double &t, double &u, double &v,
		   point vert0, point vert1, point vert2, int square) const;
  // overrideable methods to add handling code
  virtual void on_set_axes();
  virtual void on_unset_axes();
  // overrideable methods to add handling code
  virtual void on_set_box();
  virtual void on_unset_box();
  // rendering methods
  virtual void do_render() = 0;
  virtual void render_axes() const;
  virtual void render_box() const;
};

class axes: public model
{
public:
  axes();
  axes(double r, double g, double b);
protected:
  // render the axes
  void do_render();
};

class box: public model
{
public:
  box(point min, point max);
  // intersect box with a ray
  // return the distance along the ray to the first intersection,
  // or -1 if they fail to intersect
  double intersect(point orig, point dir) const;
protected:
  point points[8];
  void do_render();
};

#endif /* _OBJECT_HH */
