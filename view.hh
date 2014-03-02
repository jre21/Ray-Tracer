#ifndef _VIEW_HH
#define _VIEW_HH

#include "scene.hh"
#include "frame_buffer.hh"

class view: public model, public FrameBuffer
{
public:
  view();
  // preload with a new model generated from the named file
  view(const char *filename);
  ~view();
  // get pointers to internal objects
  scene *get_scene() const;
  // perspective functions
  void ortho(); // use orthogonal projection
  void perspective(); // use perspective projection
  int toggle_perspective(); // toggle projection type and return new type
  int refresh(); // refresh projection and return type
  // snap to world or object origin
  int snap();
  // adjust width or depth of image plane
  void inc_width();
  void dec_width();
  void inc_depth();
  void dec_depth();
  // adjust depth of near and far planes
  void move_near_plane(double delta);
  void move_far_plane(double delta);
  // select object at specific point
  void select(int x, int y);
  void render_from_buffer();
protected:
  scene *scn;
  int bf; // bitfield used to store boolean variables
  double width, depth; // radius of the image plane and distance from camera
  double near, far; // near and far viewing planes
  // propagate state changes of axes
  void on_set_axes();
  void on_unset_axes();
  void on_set_box();
  void on_unset_box();
  // render the scene
  virtual void do_render();
  void fill_buffer();
  // calculate ray from pixel coordinates
  void cast_ray(double x, double y, point &orig, point &dir);
};

class orbital_view : public view
{
public:
  orbital_view();
  orbital_view(const char *filename);
  // manipulate the view
  void rotate(double dphi, double dtheta); // rotate around the origin
  void zoom(double dr); // move toward or away from the object
protected:
  double r; // radial coordinate
  double theta; // polar angle
  double phi; // azimuthal angle
  static const double rate = 3.1415927 / 40;
  // render the scene
  void do_render();
};

#endif /* _VIEW_HH */
