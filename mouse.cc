#include "mouse.hh"

mouse::mouse()
{
  x = y = 0.0;
  field = 0;
}

void mouse::set(int button)
{
  field |= 1 << button;
}

void mouse::unset(int button)
{
  field &= ~(1 << button);
}

int mouse::is_set(int button)
{
  return field & 1 << button;
}

void mouse::set_coords(int xx, int yy)
{
  x = xx;
  y = yy;
}

int mouse::get_x()
{
  return x;
}

int mouse::get_y()
{
  return y;
}
