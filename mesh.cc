#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <math.h>
#include "mesh.hh"
#include "matrix.hh"

// ############################## mesh ##############################
mesh::mesh()
{
  init();
  ax = new axes(0,1,1);
}

mesh::mesh(const char *filename)
{
  init();
  ax = new axes(0,1,1);
  load(filename);
}

mesh::mesh(Color ambient_, Color diffuse_, Color specular_,
	   double shininess_, double refractive_index_,
	   double reflective_weight_, double refractive_weight_)
  : surface(ambient_, diffuse_, specular_, shininess_,
	    refractive_index_, reflective_weight_, refractive_weight_)
{
  init();
  ax = new axes(0,1,1);
}

mesh::mesh(const char *filename, Color ambient_,
	   Color diffuse_, Color specular_,
	   double shininess_, double refractive_index_,
	   double reflective_weight_, double refractive_weight_)
  : surface(ambient_, diffuse_, specular_, shininess_,
	    refractive_index_, reflective_weight_, refractive_weight_)
{
  init();
  ax = new axes(0,1,1);
  load(filename);
}

mesh::~mesh()
{
  deinit();
}

int mesh::load(const char *filename)
{
  return load(filename, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

// The mesh reader itself
// It can read *very* simple obj files
// return 0 on success, -1 on failure
int mesh::load(const char *filename, double sscale, double rot_x, double rot_y,
	       double rot_z, double trans_x, double trans_y, double trans_z)
{
  double x,y,z;
  int i;
  char letter;
  point v;
  point min, max; // used for bounding box
  int ix,iy,iz;
  FILE *fp;

  // clean up previous object file (if any)
  deinit();
  init();

  // apply transformations
  state = matrix::translate(trans_x, trans_y, trans_z)
    * matrix::rotate(rot_z * 180 / M_PI, 0, 0, 1)
    * matrix::rotate(rot_y * 180 / M_PI, 0, 1, 0)
    * matrix::rotate(rot_x * 180 / M_PI, 1, 0, 0)
    * matrix::scale(sscale, sscale, sscale);

  // try to open the file
  fp = fopen(filename, "r");
  if (!fp) { 
    printf("mesh::load(): Cannot open %s!\n", filename);
    return -1;
  }

  // Count the number of vertices and faces
  verts = 0;
  faces = 0;
  while(!feof(fp))
    {
      fscanf(fp,"%c %lf %lf %lf\n",&letter,&x,&y,&z);
      if (letter == 'v')
	{
	  verts++;
	}
      else
	{
	  faces++;
	}
    }

  printf("verts : %d\n", verts);
  printf("faces : %d\n", faces);

  // Dynamic allocation of vertex and face lists
  faceList = (faceStruct *)malloc(sizeof(faceStruct)*faces);
  vertList = new point[verts];
  normList = new vector[verts];

  fseek(fp, 0L, SEEK_SET);

  // Read the veritces and set min/max for bounding box
  for(i = 0;i < verts;i++)
    {
      fscanf(fp,"%c %lf %lf %lf\n",&letter,&x,&y,&z);
      vertList[i].set_X(x);
      vertList[i].set_Y(y);
      vertList[i].set_Z(z);
      if(i == 0)
	{
	  min = max = vertList[0];
	  continue;
	}
      if(x < min.get_X()) min.set_X(x);
      else if (x > max.get_X()) max.set_X(x);
      if(y < min.get_Y()) min.set_Y(y);
      else if (y > max.get_Y()) max.set_Y(y);
      if(z < min.get_Z()) min.set_Z(z);
      else if (z > max.get_Z()) max.set_Z(z);
    }
  bound = new box(min, max);

  // Read the faces
  for(i = 0;i < faces;i++)
    {
      fscanf(fp,"%c %d %d %d\n",&letter,&ix,&iy,&iz);
      // do bounds checking on vertices
      if(ix > verts || ix <= 0)
	{
	  printf("Error: %s: vertex index out of bounds: %d\n", filename, ix);
	  exit(-1);
	}
      if(iy > verts || iy <= 0)
	{
	  printf("Error: %s: vertex index out of bounds: %d\n", filename, iy);
	  exit(-1);
	}
      if(iz > verts || iz <= 0)
	{
	  printf("Error: %s: vertex index out of bounds: %d\n", filename, iz);
	  exit(-1);
	}
      // add the vertices
      faceList[i].v1 = ix - 1;
      faceList[i].v2 = iy - 1;
      faceList[i].v3 = iz - 1;
    }
  fclose(fp);

  // The part below calculates the normals of each vertex
  for(i = 0;i < faces;i++)
    {
      // find a unit vector perpendicular to faceList[i]
      v = matrix::cross(vertList[faceList[i].v2] - vertList[faceList[i].v1])
	* (vertList[faceList[i].v3] - vertList[faceList[i].v2]);
      v.normalize();
      v = v * -1.0;

      // add this unit vector to norm list for each vertex
      normList[faceList[i].v1] += v;
      normList[faceList[i].v2] += v;
      normList[faceList[i].v3] += v;
    }

  // divide normal by number of vertices
  // should we normalize instead?
  for (i = 0;i < verts;i++)
    normList[i].normalize();

  return 0;
}

void mesh::select()
{
  set_color(1,1,0);
  bound->set_color(1,0,0);
}

void mesh::deselect()
{
  set_color(0,0,1);
  bound->set_color(0,0,1);
}

double mesh::intersect(point orig, point dir)
{
  matrix trans = state.inverse();
  return bound->intersect(trans * orig, trans * dir);
}

double mesh::fine_intersect(point orig, point dir, point &vertex, point &normal)
{
  // bail fast if the ray misses the bounding box
  matrix trans = state.inverse();
  orig = trans * orig;
  dir = trans * dir;
  if(bound->intersect(orig, dir) == -1.0) return -1.0;

  // intersect with each face
  double t, u, v, t0 = -1.0, u0, v0;
  int face;
  for(int i = 0; i < faces; i++)
    {
      if(mt_intersect(orig, dir, t, u, v, vertList[faceList[i].v1],
		      vertList[faceList[i].v2], vertList[faceList[i].v3], 0)
	 && t >= 0.2 && (t0 == -1.0 || t < t0))
	{
	  t0 = t;
	  u0 = u;
	  v0 = v;
	  face = i;
	}
    }
  // set vertex and normal based on intersection
  if(t0 == -1.0)
    {
      return -1.0;
    }
  vertex = state * point::combine(vertList[faceList[face].v1],
				  vertList[faceList[face].v2], u0,
				  vertList[faceList[face].v3], v0);
  normal = state * point::combine(normList[faceList[face].v1],
				  normList[faceList[face].v2], u0,
				  normList[faceList[face].v3], v0);
  return t0;
}

void mesh::do_render()
{
  // If we've read in a model from a file, render it
  if(vertList)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glColor3d(r,g,b);
      glBegin(GL_TRIANGLES);
      for(int i = 0; i < faces; i++)
	{
	  vertList[faceList[i].v1].render();
	  vertList[faceList[i].v2].render();
	  vertList[faceList[i].v3].render();
	}
      glEnd();
      //#define DEBUG_MESH_NORMS 1
#ifdef DEBUG_MESH_NORMS
      glBegin(GL_LINES);
      for(int i = 0; i < faces; i++)
	{
	  vertList[faceList[i].v1].render();
	  (vertList[faceList[i].v1] + normList[faceList[i].v1]).render();
	  vertList[faceList[i].v2].render();
	  (vertList[faceList[i].v2] + normList[faceList[i].v2]).render();
	  vertList[faceList[i].v3].render();
	  (vertList[faceList[i].v3] + normList[faceList[i].v3]).render();
	}
      glEnd();
#endif /* DEBUG_MESH_NORMS */
    }
}

void mesh::init()
{
  faceList = 0;
  vertList = normList = 0;
  bound = 0;
}

void mesh::deinit()
{
  if(vertList)
    delete[] vertList;
  if(normList)
    delete[] normList;
  if(faceList) free(faceList);
}
