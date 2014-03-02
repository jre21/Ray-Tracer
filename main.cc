#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#include "view.hh"
#include "mouse.hh"
#include "frame_buffer.hh"
#include "main.hh"

// Global variables
int window_width, window_height; // Window dimensions
int mode = 0; // track movement mode vs selection mode
view *viewer = 0; // scene to be rendered
mouse *mouse0 = 0;

// The display function. It is called whenever the window needs
// redrawing (ie: overlapping window moves, resize, maximize)
// You should redraw your polygons here
void	display(void)
{
  // Clear the background
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // render the scene
  glLoadIdentity();
  if(mode) viewer->render();
  else viewer->render_from_buffer();

  // (Note that the origin is lower left corner)
  // (Note also that the window spans (0,1) )
  // Finish drawing, update the frame buffer, and swap buffers
  glutSwapBuffers();
}


// This function is called whenever the window is resized. 
// Parameters are the new dimentions of the window
void	resize(int x,int y)
{
  glViewport(0,0,x,y);
  window_width = x;
  window_height = y;
  viewer->refresh();
  printf("Resized to %d %d\n",x,y);
}


// This function is called whenever the mouse is pressed or released
// button is a number 0 to 2 designating the button
// state is 1 for release 0 for press event
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseButton(int button,int state,int x,int y)
{
  // update coordinates to current position
  mouse0->set_coords(x,y);
  // set/unset mouse button
  if(state) mouse0->unset(button);
  else mouse0->set(button);
  // handle selection mode
  if(mode && !state && button == 0)
    {
      viewer->select(x, y);
      glutPostRedisplay();
    }
  //printf("Mouse click at %d %d, button: %d, state %d\n",x,y,button,state);
}


//This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseMotion(int x, int y)
{
  // calculate difference from previous position.  Only update if
  // we've moved at least five pixels, to reduce lag.
  int dx = x - mouse0->get_x(), dy = y - mouse0->get_y();
  if(dx * dx + dy * dy >= 25)
    {
      double scale = M_PI / 40;
      // set the new coordinates
      mouse0->set_coords(x,y);
      if(!mode)
	{
	  // rotate if left mouse button is down
	  if(mouse0->is_set(0))
	    {
	      viewer->rotate(scale / 5 * dx, 0, 1, 0);
	      viewer->rotate(scale / 5 * dy, 1, 0, 0);
	    }
	  // zoom if right mouse button is down
	  if(mouse0->is_set(1)) viewer->translate(- scale * dx, scale * dy, 0);
	  if(mouse0->is_set(2)) viewer->translate(0, 0, - scale * dy);
	  // redraw
	  glutPostRedisplay();
	} /* if(!mode) */
      else
	{
	  if(mouse0->is_set(1))
	    viewer->move_near_plane(- scale * dy);
	  if(mouse0->is_set(2))
	    viewer->move_far_plane(- scale * dy);
	  glutPostRedisplay();
	} /* if(mode) */
    } /* if(dx * dx + dy * dy >= 25) */
  //printf("Mouse is at %d, %d\n", x,y);
}


// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void	keyboard(unsigned char key, int x, int y)
{
  double rot_scale = M_PI / 40, trans_scale = .5;
  switch(key) {
    // image properties
  case 'r':
  case 'R':
    // no-op, because we redraw at any keypress
    break;
    // resolution
  case '-':
  case '_':
    viewer->Resize(viewer->GetHeight()/2, viewer->GetWidth()/2);
    break;
  case '=':
  case '+':
    viewer->Resize(viewer->GetHeight()*2, viewer->GetWidth()*2);
    break;
    // image plane properties
  case ']':
  case '}':
    viewer->inc_depth();
    break;
  case '[':
  case '{':
    viewer->dec_depth();
    break;
  case ',':
  case '<':
    viewer->dec_width();
    break;
  case '.':
  case '>':
    viewer->inc_width();
    break;
    // world coordinate transformations
  case '4':
    viewer->get_scene()->translate(-trans_scale,0,0);
    break;
  case '6':
    viewer->get_scene()->translate(trans_scale,0,0);
    break;
  case '2':
    viewer->get_scene()->translate(0,-trans_scale,0);
    break;
  case '8':
    viewer->get_scene()->translate(0,trans_scale,0);
    break;
  case '1':
    viewer->get_scene()->translate(0,0,-trans_scale);
    break;
  case '9':
    viewer->get_scene()->translate(0,0,trans_scale);
    break;
  case 'w':
  case 'W':
    viewer->get_scene()->rotate(-rot_scale,1,0,0);
    break;
  case 'e':
  case 'E':
    viewer->get_scene()->rotate(rot_scale,1,0,0);
    break;
  case 's':
  case 'S':
    viewer->get_scene()->rotate(-rot_scale,0,1,0);
    break;
  case 'd':
  case 'D':
    viewer->get_scene()->rotate(rot_scale,0,1,0);
    break;
  case 'x':
  case 'X':
    viewer->get_scene()->rotate(-rot_scale,0,0,1);
    break;
  case 'c':
  case 'C':
    viewer->get_scene()->rotate(rot_scale,0,0,1);
    break;
  case 'f':
  case 'F':
    viewer->get_scene()->scale(20.0/19,20.0/19,20.0/19);
    break;
  case 'v':
  case 'V':
    viewer->get_scene()->scale(19.0/20,19.0/20,19.0/20);
    break;
    // local coordinate transformations
  case 'y':
  case 'Y':
    viewer->get_scene()->rotate_local(-rot_scale,1,0,0);
    break;
  case 'u':
  case 'U':
    viewer->get_scene()->rotate_local(rot_scale,1,0,0);
    break;
  case 'h':
  case 'H':
    viewer->get_scene()->rotate_local(-rot_scale,0,1,0);
    break;
  case 'j':
  case 'J':
    viewer->get_scene()->rotate_local(rot_scale,0,1,0);
    break;
  case 'n':
  case 'N':
    viewer->get_scene()->rotate_local(-rot_scale,0,0,1);
    break;
  case 'm':
  case 'M':
    viewer->get_scene()->rotate_local(rot_scale,0,0,1);
    break;
    // miscellaneous keys
  case '':                           /* Quit */
    exit(1);
    break;
  case 'a':
  case 'A':
    viewer->toggle_axes();
    break;
  case 'b':
  case 'B':
    viewer->toggle_box();
    break;
  case 'g':
  case 'G':
    viewer->snap();
    break;
  case 'z':
  case 'Z':
    mode = !mode;
    if(mode) printf("Entered selection mode\n");
    else printf("Entered camera mode\n");
    break;
  case 'p':
  case 'P':
    // Toggle Projection Type (orthogonal, perspective)
    viewer->toggle_perspective();
    break;
  case 'q':
  case 'Q':
    exit(0);
    break;
  default:
    break;
  }

  // Schedule a new display event
  glutPostRedisplay();
}


// Here's the main
int main(int argc, char* argv[])
{
  atexit(do_exit);

  // Initialize GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowPosition(100,100);
  glutInitWindowSize(512,512);
  window_width = window_height = 320;
  glutCreateWindow("Assignment 7");
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutMouseFunc(mouseButton);
  glutMotionFunc(mouseMotion);
  glutKeyboardFunc(keyboard);

  // Initialize GL
  glEnable(GL_DEPTH_TEST);

  mouse0 = new mouse();
  viewer = new view("scene1.rtl");

  // Switch to main loop
  glutMainLoop();
  return 0;        
}

// clean up global structures and exit
void do_exit(void)
{
  if(viewer) delete viewer;
  if(mouse0) delete mouse0;
}
