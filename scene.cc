#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "scene.hh"

scene::scene()
{
  lights = 0;
  spheres = 0;
  meshes = 0;
  num_lights = num_spheres = num_meshes = num_surfaces = 0;
  selected = -1;
}

scene::scene(const char *filename)
{
  lights = 0;
  spheres = 0;
  meshes = 0;
  num_lights = num_spheres = num_meshes = num_surfaces = 0;
  selected = -1;
  load(filename);
}

scene::~scene()
{
  unload();
}

int scene::load(const char *filename)
{
  unload();
  int ret = 0;
  FILE *fp = fopen(filename, "r");
  if (!fp) { 
    printf("scene::load(): Cannot open %s!\n", filename);
    return -1;
  }

  fscanf(fp, "%d %d %d\n", &num_lights, &num_spheres, &num_meshes);
  num_surfaces = num_spheres + num_meshes;
  lights = new light[num_lights];
  spheres = new sphere[num_spheres];
  meshes = new mesh[num_meshes];

  char mesh_file[255];
  int ltype;
  double scale_, rot_x, rot_y, rot_z, trans_x, trans_y, trans_z,
    r_ambient, g_ambient, b_ambient, r_diffuse, g_diffuse, b_diffuse,
    r_specular, g_specular, b_specular, k_ambient, k_diffuse, k_specular,
    shininess, index, k_reflective, k_refractive;

  for(int i = 0; i < num_lights; i++)
    {
      if(feof(fp))
	{
	  ret = -1;
	  break;
	}
      fscanf
	(
	 fp, "L %d %lf %lf %lf %lf %lf %lf\n", &ltype,
	 &trans_x, &trans_y, &trans_z, &r_ambient, &g_ambient, &b_ambient
	);
      lights[i] = light(ltype, point(trans_x, trans_y, trans_z),
			Color(r_ambient, g_ambient, b_ambient));
    }
  for(int i = 0; i < num_spheres; i++)
    {
      if(feof(fp))
	{
	  ret = -1;
	  break;
	}
      fscanf
	(
	 fp, "S %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
	 &trans_x, &trans_y, &trans_z, &scale_,
	 &r_ambient, &g_ambient, &b_ambient, &r_diffuse,
	 &g_diffuse, &b_diffuse, &r_specular, &g_specular, &b_specular,
	 &k_ambient, &k_diffuse, &k_specular, &shininess, &index,
	 &k_reflective, &k_refractive
	);
      spheres[i].set_lighting
	(
	 Color(r_ambient, g_ambient, b_ambient) * k_ambient,
	 Color(r_diffuse, g_diffuse, b_diffuse) * k_diffuse,
	 Color(r_specular, g_specular, b_specular) * k_specular,
	 shininess, index, k_reflective, k_refractive
	);
      spheres[i].scale(scale_, scale_, scale_);
      spheres[i].translate(trans_x, trans_y, trans_z);
    }
  for(int i = 0; i < num_meshes; i++)
    {
      if(feof(fp))
	{
	  ret = -1;
	  break;
	}
      fscanf
	(
	 fp, "M %s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
	 mesh_file, &scale_, &rot_x, &rot_y, &rot_z, &trans_x, &trans_y,
	 &trans_z, &r_ambient, &g_ambient, &b_ambient, &r_diffuse,
	 &g_diffuse, &b_diffuse, &r_specular, &g_specular, &b_specular,
	 &k_ambient, &k_diffuse, &k_specular, &shininess, &index,
	 &k_reflective, &k_refractive
	);
      meshes[i].set_lighting
	(
	 Color(r_ambient, g_ambient, b_ambient) * (k_ambient),
	 Color(r_diffuse, g_diffuse, b_diffuse) * (k_diffuse),
	 Color(r_specular, g_specular, b_specular) * (k_specular),
	 shininess, index, k_reflective, k_refractive
	);
      ret |= meshes[i].load
	(mesh_file, scale_, rot_x, rot_y, rot_z, trans_x, trans_y, trans_z);
    }
  return ret;
}

point scene::reflect(point incoming, point normal)
{
  return incoming - normal * (2 * (incoming * normal));
}

point scene::refract(point incoming, point normal, double n1, double n2)
{
  double IdotN = normal * incoming;
  point tangent = (incoming - normal * IdotN) * (n2 / n1);
  double tangent2 = tangent * tangent;
  return tangent2 >= 1 ? vector()
    : tangent + (IdotN > 0 ? normal : -normal) * sqrt(1 - tangent2);
}

void scene::unload()
{
  if(lights)
    {
      delete[] lights;
      lights = 0;
    }
  if(spheres)
    {
      delete[] spheres;
      spheres = 0;
    }
  if(meshes)
    {
      delete[] meshes;
      meshes = 0;
    }
  num_lights = num_spheres = num_meshes = num_surfaces = 0;
}

surface * scene::get_surface(int i)
{
  if(i < 0 || i >= num_surfaces) return 0;
  if(i < num_spheres) return spheres + i;
  return meshes + i - num_spheres;
}

// transform object about global axes
void scene::rotate(double theta, double vx, double vy, double vz)
{
  if(get_surface(selected)) get_surface(selected)->rotate(theta, vx, vy, vz);
}

void scene::scale(double sx, double sy, double sz)
{
  if(get_surface(selected)) get_surface(selected)->scale(sx,sy,sz);
}

void scene::translate(double tx, double ty, double tz)
{
  if(get_surface(selected)) get_surface(selected)->translate(tx,ty,tz);
}

// transform object about local axes
void scene::rotate_local(double theta, double vx, double vy, double vz)
{
  if(get_surface(selected)) get_surface(selected)->rotate_local(theta, vx, vy, vz);
}

void scene::scale_local(double sx, double sy, double sz)
{
  if(get_surface(selected)) get_surface(selected)->scale_local(sx,sy,sz);
}

void scene::translate_local(double tx, double ty, double tz)
{
  if(get_surface(selected)) get_surface(selected)->translate_local(tx,ty,tz);
}

void scene::intersection(point orig, point dir)
{
  int closest = -1;
  double distance, t;

  // find closest object which intersects ray
  for(int i = 0; i < num_surfaces; i++)
    {
      t = get_surface(i)->intersect(orig, dir);
      if(t != -1 && (closest == -1 || t < distance) )
	{
	  distance = t;
	  closest = i;
	}
    }

  // select said object
  if(closest != -1) select(closest);
}

// only do a lighting calculation for now
Color scene::ray_trace(point orig, point dir, double index, int depth)
{
  int closest = -1;
  double distance, t;
  Color color;
  point vert, norm, vert0, norm0;
  dir.normalize();

  // find closest object which intersects ray
  for(int i = 0; i < num_surfaces; i++)
    {
      t = get_surface(i)->fine_intersect(orig, dir, vert0, norm0);
      if(t != -1 && (closest == -1 || t < distance) )
	{
	  distance = t;
	  closest = i;
	  vert = vert0;
	  norm = norm0;
	}
    }

  // we've hit a surface
  if(closest != -1)
    {
      surface *s = get_surface(closest);
      // calculate ambient illumination
      color = s->phong_ambient();
      for(int i = 0; i < num_lights; i++)
	// calculate local illumination
	color += s->phong(dir, lights[i], depth, vert, norm);
      if(depth > 0)
	{
	  double k;
	  if( (k = s->reflection()) )
	    color += ray_trace(vert, reflect(dir, norm), index, depth - 1) * k;
	  if( (k = s->refraction()) )
	    {
	      point next_dir = refract(dir, norm, index, s->index());
	      if(next_dir.nonzero())
		color += ray_trace(vert, next_dir, s->index(), depth -1) * k;
	    }
	}
    }

  return color;
}

matrix scene::get_state()
{
  if(!get_surface(selected)) return matrix::identity();
  else return get_surface(selected)->get_state();
}

void scene::on_set_axes()
{
  for(int i = 0; i < num_surfaces; i++)
    get_surface(i)->set_axes();
}

void scene::on_unset_axes()
{
  for(int i = 0; i < num_surfaces; i++)
    get_surface(i)->unset_axes();
}

void scene::on_set_box()
{
  for(int i = 0; i < num_surfaces; i++)
    get_surface(i)->set_box();
}

void scene::on_unset_box()
{
  for(int i = 0; i < num_surfaces; i++)
    get_surface(i)->unset_box();
}

void scene::select(int object)
{
  if(get_surface(selected)) get_surface(selected)->deselect();
  if(get_surface(object))
    {
      selected = object;
      get_surface(selected)->select();
    }
  else selected = -1;
}

void scene::do_render()
{
  // render child objects, transformed appropriately
  for(int i = 0; i < num_surfaces; i++)
    get_surface(i)->render();
}
