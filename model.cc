#include <GL/gl.h>
#include "model.hh"

/* ############################## model ############################## */
model::model()
{
  state = matrix::identity();
  show_axes = 0;
  ax = 0;
  show_bound = 0;
  bound = 0;
  r = g = b = 0;
}

model::model(double rr, double gg, double bb)
{
  state = matrix::identity();
  show_axes = 0;
  ax = 0;
  show_bound = 0;
  bound = 0;
  r = rr;
  g = gg;
  b = bb;
}

model::~model()
{
  if(ax) delete ax;
  if(bound) delete bound;
}

void model::render()
{
  glPushMatrix();
  state.mult();
  // render axes, if present
  render_axes();
  // render bounding box, if present
  render_box();
  // run custom rendering code
  do_render();
  glPopMatrix();
}

void model::render(matrix transformation)
{
  transformation *= state;
  glPushMatrix();
  transformation.mult();
  // render axes, if present
  render_axes();
  // render bounding box, if present
  render_box();
  // run custom rendering code
  do_render();
  glPopMatrix();
}

void model::set_axes()
{
  show_axes = 1;
  on_set_axes();
}

void model::unset_axes()
{
  show_axes = 0;
  on_unset_axes();
}

int model::toggle_axes()
{
  show_axes ? unset_axes() : set_axes();
  return show_axes;
}

void model::set_box()
{
  show_bound = 1;
  on_set_box();
}

void model::unset_box()
{
  show_bound = 0;
  on_unset_box();
}

int model::toggle_box()
{
  show_bound ? unset_box() : set_box();
  return show_bound;
}

void model::set_color(double rr, double gg, double bb)
{
  r = rr;
  g = gg;
  b = bb;
}

void model::rotate(double theta, double vx, double vy, double vz)
{
  state = matrix::rotate(theta, vx, vy, vz) * state;
}

void model::scale(double sx, double sy, double sz)
{
  state = matrix::scale(sx, sy, sz) * state;
}

void model::translate(double tx, double ty, double tz)
{
  state = matrix::translate(tx, ty, tz) * state;
}

void model::rotate_local(double theta, double vx, double vy, double vz)
{
  state *= matrix::rotate(theta, vx, vy, vz);
}

void model::scale_local(double sx, double sy, double sz)
{
  state *= matrix::scale(sx, sy, sz);
}

void model::translate_local(double tx, double ty, double tz)
{
  state *= matrix::translate(tx, ty, tz);
}

matrix model::get_state() const
{
  return state;
}

/*
 * adapted from the M\"oller-Trumbore fast intersection paper to
 * calculate the intersection with either a parallelogram or a
 * triangle (vert1 and vert2 are both adjacent to vert0), and to not
 * return intersections "behind" the ray's origin
 */
int model::mt_intersect(point orig, point dir, double &t, double &u, double &v,
			point vert0, point vert1, point vert2, int square)
  const
{
#define EPSILON 1e-6
  point edge1 = vert1 - vert0, edge2 = vert2 - vert0,
    tvec, pvec = matrix::cross(dir) * edge2, qvec;
  double det = edge1 * pvec, inv_det;
  /* return false if the ray is nearly parallel to the polygon */
  if(det > -EPSILON && det < EPSILON)
    return 0;
  inv_det = 1.0 / det;

  /* find displacement from vert0 to origin */
  tvec = orig - vert0;

  /* calculate u and test bounds */
  u = tvec * pvec * inv_det;
  if(u < 0.0 || u > 1.0)
    return 0;

  /* calculate v and test bounds */
  qvec = matrix::cross(tvec) * edge1;
  v = dir * qvec * inv_det;
  if(square)
    {
      if(v < 0.0 || v > 1.0)
	return 0;
    }
  else
    {
      if(v < 0.0 || u + v > 1.0)
	return 0;
    }

  /* calculate distance from ray origin to intersection */
  t = edge2 * qvec * inv_det;
  return 1;
}

void model::on_set_axes() {}
void model::on_unset_axes() {}
void model::on_set_box() {}
void model::on_unset_box() {}

void model::render_axes() const
{
  if(ax && show_axes) ax->render();
}

void model::render_box() const
{
  if(show_bound && bound) bound->render();
}


/* ############################## axes ############################## */
axes::axes() {}
axes::axes(double rr, double gg, double bb) : model(rr, gg, bb) {}

void axes::do_render()
{
  // render the axes
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3d(r,g,b);
  glBegin(GL_LINES);
    point(0,0,0).render();
    point(10,0,0).render();
    point(0,0,0).render();
    point(0,10,0).render();
    point(0,0,0).render();
    point(0,0,10).render();
  glEnd();
}


// ############################## box ##############################
box::box(point min, point max) : model(0,0,1)
{
  for(int i = 0; i < 8; i++)
    {
      points[i] = point
	(
	 i & 1 ? max.get_X() : min.get_X(),
	 i & 2 ? max.get_Y() : min.get_Y(),
	 i & 4 ? max.get_Z() : min.get_Z()
	);
    }
}

double box::intersect(point orig, point dir) const
{
  double t, u, v, ret = -1.0;
  // calculate intersections with each face
  if(mt_intersect(orig, dir, t, u, v, points[0], points[1], points[2], 1)
     && t > 0.0 && (ret = -1.0 || t < ret))
    ret = t;
  if(mt_intersect(orig, dir, t, u, v, points[0], points[1], points[4], 1)
     && t > 0.0 && (ret = -1.0 || t < ret))
    ret = t;
  if(mt_intersect(orig, dir, t, u, v, points[0], points[2], points[4], 1)
     && t > 0.0 && (ret = -1.0 || t < ret))
    ret = t;
  if(mt_intersect(orig, dir, t, u, v, points[7], points[5], points[3], 1)
     && t > 0.0 && (ret = -1.0 || t < ret))
    ret = t;
  if(mt_intersect(orig, dir, t, u, v, points[7], points[6], points[3], 1)
     && t > 0.0 && (ret = -1.0 || t < ret))
    ret = t;
  if(mt_intersect(orig, dir, t, u, v, points[7], points[6], points[5], 1)
     && t > 0.0 && (ret = -1.0 || t < ret))
    ret = t;
  return ret;
}

void box::do_render()
{
  // draw the edges of the box
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3d(r,g,b);
  glBegin(GL_LINES);
  for(int i = 0; i < 8; i++)
    for(int j = 1; j < 8; j <<= 1)
      if(!(i & j))
	{
	  points[i].render();
	  points[i + j].render();
	}
  glEnd();
}
