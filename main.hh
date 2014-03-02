// The display function. It is called whenever the window needs
// redrawing (ie: overlapping window moves, resize, maximize)
// You should redraw your polygons here
void display(void);

// This function is called whenever the window is resized. 
// Parameters are the new dimentions of the window
void	resize(int x,int y);

// This function is called whenever the mouse is pressed or released
// button is a number 0 to 2 designating the button
// state is 1 for release 0 for press event
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseButton(int button,int state,int x,int y);

//This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseMotion(int x, int y);

// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void	keyboard(unsigned char key, int x, int y);

// Here's the main
int main(int argc, char* argv[]);

// clean up global structures and exit
void do_exit(void);
