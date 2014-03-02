#ifndef _MOUSE_HH
#define _MOUSE_HH 1

class mouse
{
public:
  mouse();
  void set(int button); // register a click
  void unset(int button); // register a release
  int is_set(int button); // is the button clicked?
  void set_coords(int x, int y); // update mouse coordinates
  int get_x(); // get x-coordinate
  int get_y(); // get y-coordinate
protected:
  int x;
  int y;
  int field; // bitfield representing the state of the mouse buttons
};

#endif /* _MOUSE_HH */
