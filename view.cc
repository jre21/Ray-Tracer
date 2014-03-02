#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "view.hh"

extern int window_width, window_height;

#define FB_SIZE 64
#define PROJECTION 0x1 /* 0 = orthographic, 1 = perspective */
#define ORIGIN 0x2 /* 0 = object, 1 = world */

// ############################## view ##############################

view::view() : FrameBuffer(FB_SIZE, FB_SIZE)
{
  ax = new axes(0,1,0);
  scn = new scene();
  bf = 0;
  width = 6.0;
  depth = 8.0;
  near = 0.5;
  far = 20.0;
  state = matrix::look_at(point(0,0,8),point(0,0,0),vector(0,1,0));
  perspective();
}

view::view(const char *filename) : FrameBuffer(FB_SIZE, FB_SIZE)
{
  ax = new axes(0,1,0);
  scn = new scene(filename);
  bf = 0;
  width = 6.0;
  depth = 8.0;
  near = 0.5;
  far = 20.0;
  state = matrix::look_at(point(0,0,8),point(0,0,0),vector(0,1,0));
  perspective();
}

view::~view()
{
  delete scn;
}

scene *view::get_scene() const
{
  return scn;
}

void view::ortho()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-width, width, -width, width, near, far);
  glMatrixMode(GL_MODELVIEW);
  bf &= ~PROJECTION;
}

void view::perspective()
{ 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(atan(width/depth) * 360 * M_1_PI, window_width/window_height, near, far);
  glMatrixMode(GL_MODELVIEW);
  bf |= PROJECTION;
}

int view::toggle_perspective()
{
  if(bf & PROJECTION) ortho();
  else perspective();
  return bf & PROJECTION;
}

int view::refresh()
{
  if(!(bf & PROJECTION)) ortho();
  else perspective();
  return bf & PROJECTION;
}

int view::snap()
{
  point loc = state.inverse() * point(),
    focus = bf & ORIGIN ? scn->get_state() * point() : point();
  vector up = state.inverse() * vector(0,1,0);
  state = matrix::look_at(loc, focus, up);
  return bf ^= ORIGIN;
}

void view::do_render()
{
  scn->render();
}

void view::inc_width()
{
  width *= 1.1;
  if(width > 100.0) width = 100.0;
  refresh();
}

void view::dec_width()
{
  width /= 1.1;
  if(width < 1.0) width = 1.0;
  refresh();
}

void view::inc_depth()
{
  depth *= 1.1;
  if(depth > 100.0) depth = 100.0;
  refresh();
}

void view::dec_depth()
{
  depth /= 1.1;
  if(depth < 1.0) depth = 1.0;
  refresh();
}

void view::move_near_plane(double delta)
{
  near += delta;
  if(near < 0.5) near = 0.5;
  else if (near > far - 1.0) near = far - 1.0;
  refresh();
}

void view::move_far_plane(double delta)
{
  far += delta;
  if(far > 100.0) far = 100.0;
  else if(far < near + 1.0) far = near + 1.0;
  refresh();
}

void view::select(int x, int y)
{
  x = 2 * width * (double)x / window_width - width;
  y = width - 2 * width * (double)y / window_height;
  point orig, dir;
  cast_ray(x, y, orig, dir);
  scn->intersection(orig, dir);
}

void view::render_from_buffer()
{
  fill_buffer();
  render_buffer();
}

void view::on_set_axes()
{
  if(scn) scn->set_axes();
}

void view::on_unset_axes()
{
  if(scn) scn->unset_axes();
}

void view::on_set_box()
{
  if(scn) scn->set_box();
}

void view::on_unset_box()
{
  if(scn) scn->unset_box();
}

void view::fill_buffer()
{
  point orig, dir;
  for(int i = 0; i < GetWidth(); i++)
    for(int j = 0; j < GetHeight(); j++)
      {
	double u = 2 * width * (double)i / GetWidth() - width,
	  v = 2 * width * (double)j / GetHeight() - width;
	cast_ray(u, v, orig, dir);
	SetPixel( i, j, scn->ray_trace(orig, dir, 1.0, 4) );
      }
}

void view::cast_ray(double x, double y, point &orig, point &dir)
{
  matrix inv_state = state.inverse();
  if(bf & PROJECTION)
    {
      // choose a ray extending from the camera origin
      orig = inv_state * point(0,0,0);
      dir = inv_state * vector(x, y, - depth);
    }
  else
    {
      // choose a ray perpendicular to image plane
      orig = inv_state * point(x, y, 0);
      dir = inv_state * vector(0,0,-depth);
    }
}


// ############################## orbital_view ##############################
orbital_view::orbital_view()
{
  r = 20;
  theta = phi = 0;
}
orbital_view::orbital_view(const char *filename)
  : view(filename)
{
  r = 20;
  theta = phi = 0;
}

void orbital_view::rotate(double dphi, double dtheta)
{
  theta -= rate * dtheta;
  while(theta >= 2 * M_PI) theta -= 2 * M_PI;
  while(theta < 0) theta += 2 * M_PI;
  phi -= rate * dphi;
  if(phi >= 2 * M_PI) phi -= 2 * M_PI;
  else if(phi < 0) phi += 2 * M_PI;
}

void orbital_view::zoom(double dr)
{
  r += rate * dr;
  if(r < 1) r = 1;
  if(r > 100) r = 100;
  refresh();
}

void orbital_view::do_render()
{
  point
    obs(r * sin(theta) * cos(phi), r * sin(theta) * sin(phi), r * cos(theta)),
    orig(0,0,0);
  vector up
    (
     sin(theta - M_PI_2) * cos(phi),
     sin(theta - M_PI_2) * sin(phi),
     cos(theta - M_PI_2)
    );
  glPushMatrix();
  matrix::look_at(obs, orig, up).mult();
  scn->render();
  glPopMatrix();
}
