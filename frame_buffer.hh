#ifndef _FRAME_BUFFER_HH
#define _FRAME_BUFFER_HH

class Color
{
public:
  double r,g,b;

  Color();
  Color(double rr, double gg, double bb);
  Color(const Color &c);
  Color operator=(Color c);
  Color operator*(double num) const;
  Color operator*=(double num);
  Color operator*(Color c) const;
  Color operator*=(Color c);
  Color operator+(Color c) const;
  Color operator+=(Color c);
  Color operator-(Color c) const;
  Color operator-=(Color c);
};

class Pixel
{
public:
  Color color;
  double z_value;

  Pixel();
  Pixel(Color cc);
  Pixel(Color cc, double depth);
};

class FrameBuffer
{
public:
  Pixel **buffer;
  int x_res, y_res;

  // pixels can be accessed as fb->buffer[x][y]
  // or by fb->GetPixel(x,y) and fb->SetPixel(x,y)
  // GetPixel and SetPixel are bounds-checked

  FrameBuffer(int x_dimension, int y_dimension);
  ~FrameBuffer();

  // resizing the buffer deletes its current contents!
  void Resize(int x_dimension, int y_dimension);
  Pixel GetPixel(int x, int y);
  void SetPixel(int x, int y, Color c);
  void SetPixel(int x, int y, Color c, double depth);
  int GetWidth();
  int GetHeight();
  void BresenhamLine(int x_1, int y_1, int x_2, int y_2, Color c);
  void render_buffer();
protected:
  Pixel *storage_array;
  void drawRect(double x, double y, double w, double h);
  void linePosSteep(int x_1, int y_1, int x_2, int y_2, Color c);
  void linePosShallow(int x_1, int y_1, int x_2, int y_2, Color c);
  void lineNegShallow(int x_1, int y_1, int x_2, int y_2, Color c);
  void lineNegSteep(int x_1, int y_1, int x_2, int y_2, Color c);
};

#endif /* _FRAME_BUFFER_HH */
