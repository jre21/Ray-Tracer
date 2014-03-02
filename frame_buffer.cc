#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>

#include "frame_buffer.hh"

/* ############################# Color ############################# */
Color::Color()
{
  r = 0;
  g = 0;
  b = 0;
}

Color::Color(double rr, double gg, double bb)
{
  r = rr;
  g = gg;
  b = bb;
}

Color::Color(const Color &c)
{
  r = c.r;
  g = c.g;
  b = c.b;
}

Color Color::operator=(Color c)
{
  r = c.r;
  g = c.g;
  b = c.b;
  return *this;
}

Color Color::operator*(double num) const
{
  return Color(r * num, g * num, b * num);
}

Color Color::operator*=(double num)
{
  r *= num;
  g *= num;
  b *= num;
  return *this;
}

Color Color::operator*(Color c) const
{
  return Color(r * c.r, g * c.g, b * c.b);
}

Color Color::operator*=(Color c)
{
  r *= c.r;
  g *= c.g;
  b *= c.b;
  return *this;
}

Color Color::operator+(Color c) const
{
  return Color(r + c.r, g + c.g, b + c.b);
}

Color Color::operator+=(Color c)
{
  r += c.r;
  g += c.g;
  b += c.b;
  return *this;
}

Color Color::operator-(Color c) const
{
  return Color(r - c.r, g - c.g, b - c.b);
}

Color Color::operator-=(Color c)
{
  r -= c.r;
  g -= c.g;
  b -= c.b;
  return *this;
}


/* ############################# Pixel ############################# */
Pixel::Pixel()
{
  color = Color(0,0,0);
  z_value = 0;
}

Pixel::Pixel(Color cc)
{
  color = cc;
  z_value = 0;
}

Pixel::Pixel(Color cc, double depth)
{
  color = cc;
  z_value = depth;
}

/* ########################## FrameBuffer ######################### */
FrameBuffer::FrameBuffer(int x_dimension, int y_dimension)
{
  storage_array = new Pixel[x_dimension * y_dimension];
  buffer = new Pixel *[x_dimension];

  for(int n = 0; n < x_dimension; ++n)
    buffer[n] = &storage_array[n * x_dimension];

  x_res = x_dimension;
  y_res = y_dimension;

}

FrameBuffer::~FrameBuffer()
{
  delete [] buffer[0];
  delete [] buffer;
}

void FrameBuffer::Resize(int x_dimension, int y_dimension)
{
  delete [] buffer[0];
  delete [] buffer;

  storage_array = new Pixel[x_dimension * y_dimension];
  buffer = new Pixel *[x_dimension];

  for(int n = 0; n < x_dimension; ++n)
    buffer[n] = &storage_array[n * x_dimension];

  x_res = x_dimension;
  y_res = y_dimension;
}

Pixel FrameBuffer::GetPixel(int x, int y)
{
  if(x < 0 || x >= x_res || y < 0 || y >= y_res)
    {
      printf("FrameBuffer::GetPixel(): array index out of bounds\n");
      exit(-1);
    }
  return buffer[x][y];
}

void FrameBuffer::SetPixel(int x, int y, Color c)
{
  if(x < 0 || x >= x_res || y < 0 || y >= y_res)
    {
      printf("FrameBuffer::SetPixel(): array index out of bounds\n");
      exit(-1);
    }

  buffer[x][y].color = c;
  buffer[x][y].z_value = 0;
}

void FrameBuffer::SetPixel(int x, int y, Color c, double depth)
{
  if(x < 0 || x >= x_res || y < 0 || y >= y_res)
    {
      printf("FrameBuffer::SetPixel(): array index out of bounds\n");
      exit(-1);
    }

  buffer[x][y].color = c;
  buffer[x][y].z_value = depth;
}

int FrameBuffer::GetWidth()
{
  return x_res;
}

int FrameBuffer::GetHeight()
{
  return y_res;
}

void FrameBuffer::BresenhamLine(int x_1, int y_1, int x_2, int y_2, Color c)
{
  double deltax, deltay;	// For Bresenham's
  double slope;			// Ditto

  deltax = x_2 - x_1;		// Change in x
  deltay = y_2 - y_1;		// Change in y

  if (deltax == 0)		// Need to avoid div by zero
    linePosSteep(x_1, y_1, x_2, y_2, c);

  else {
    slope = deltay / deltax;

    if (slope > 0.0) {
      if (slope > 1.0)
	linePosSteep(x_1, y_1, x_2, y_2, c);
      else
	linePosShallow(x_1, y_1, x_2, y_2, c);
    } else {
      if (slope > -1.0)
	lineNegShallow(x_1, y_1, x_2, y_2, c);
      else
	lineNegSteep(x_1, y_1, x_2, y_2, c);
    }
  }
}

void FrameBuffer::render_buffer()
{
  double w = 10.0/GetWidth();
  double h = 10.0/GetHeight();

  Color cl;

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0,10, 0,10, -1,1);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glBegin(GL_QUADS);

  printf("width %d, height %d\n", GetWidth(), GetHeight());

  for(int y = 0; y < GetHeight(); y++)
    {
      for(int x = 0; x < GetHeight(); x++)
	{
	  cl = buffer[x][y].color;
	  glColor3d(cl.r, cl.g, cl.b);
	  drawRect(w*x, h*y, w, h);
	}
    }

  glEnd();

  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

void FrameBuffer::drawRect(double x, double y, double w, double h)
{
  glVertex2d(x,y);
  glVertex2d(x+w,y);
  glVertex2d(x+w,y+h);
  glVertex2d(x, y+h);
}

void FrameBuffer::linePosSteep(int x_1, int y_1, int x_2, int y_2, Color c)
{
  int dx, dy, p, twodx, twodxdy, x, y, yend;

  dx = abs(x_1 - x_2);
  dy = abs(y_1 - y_2);
  twodx = 2 * dx;
  p = twodx - dy;
  twodxdy = 2 * (dx - dy);
  y = y_1;		// Dummy initialization
  yend = y_2;		// Ditto
  x = x_1;

  if (x_1 == x_2)		// Vertical line
    {
      if (y_2 > y_1) {
	yend = y_2;
	y = y_1;
      } else {
	yend = y_1;
	y = y_2;
      }
      for (; y <= yend; y++)
	buffer[x_1][y] = c;
    }
  if (x_1 > x_2) {
    x = x_2;
    y = y_2;
    yend = y_1;
  }
  if (x_1 < x_2) {
    x = x_1;
    y = y_1;
    yend = y_2;
  }
  if (x_1 != x_2)
    {
      buffer[x_1][y_1] = c;
      while (y < yend) {
	y++;
	if (p < 0)
	  p += twodx;
	else {
	  x++;
	  p += twodxdy;
	}
	buffer[x][y] = c;
      }
    }
}

void FrameBuffer::linePosShallow(int x_1, int y_1, int x_2, int y_2, Color c)
{
  int dx, dy, p, twody, twodydx, x, y, xend;

  dx = abs(x_1 - x_2);
  dy = abs(y_1 - y_2);
  twody = 2 * dy;
  p = twody - dx;
  twodydx = 2 * (dy - dx);

  if (x_1 > x_2) {
    x = x_2;
    y = y_2;
    xend = x_1;
  } else {
    x = x_1;
    y = y_1;
    xend = x_2;
  }
  buffer[x][y] = c;

  while (x < xend) {
    x++;
    if (p < 0)
      p += twody;
    else {
      y++;
      p += twodydx;
    }
    buffer[x][y] = c;
  }
}

void FrameBuffer::lineNegShallow(int x_1, int y_1, int x_2, int y_2, Color c)
{
  int dx, dy, p, twody, twodydx, x, y, xend;

  dx = abs(x_2 - x_1);
  dy = abs(y_2 - y_1);
  p = 2 * dy - dx;
  twody = 2 * dy;
  twodydx = 2 * (dy - dx);

  if (x_1 > x_2) {
    x = x_2;
    y = y_2;
    xend = x_1;
  } else {
    x = x_1;
    y = y_1;
    xend = x_2;
  }
  buffer[x][y] = c;

  while (x < xend) {
    x++;
    if (p < 0)
      p += twody;
    else {
      y--;
      p += twodydx;
    }
    buffer[x][y] = c;
  }
}

void FrameBuffer::lineNegSteep(int x_1, int y_1, int x_2, int y_2, Color c)
{
  int dx, dy, p, twodx, twodxdy, x, y, yend;

  dx = abs (x_2 - x_1);
  dy = abs (y_2 - y_1);
  p = 2 * dx - dy;
  twodx = 2 * dx;
  twodxdy = 2 * (dx - dy);

  if (x_1 > x_2) {
    x = x_2;
    y = y_2;
    yend = y_1;
  } else {
    x = x_1;
    y = y_1;
    yend = y_2;
  }
  buffer[x][y] = c;

  while (y > yend) {
    y--;
    if (p < 0)
      p += twodx;
    else {
      x++;
      p += twodxdy;
    }
    buffer[x][y] = c;
  }
}
