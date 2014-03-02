#include <math.h>
#include "surface.hh"

/* ############################ light ############################ */
light::light()
{
  pos = point();
  color = Color();
}

light::light(int type, point pos_, Color color_)
{
  pos = point(pos_);
  pos[3] = type;
  color = color_;
}


/* ########################### surface ########################### */
surface::surface() : model(0,0,1)
{
  ambient = diffuse = specular = Color(1.0, 1.0, 1.0);
  shininess = refractive_index = reflective_weight = refractive_weight = 0.0;
}

surface::surface(Color ambient_, Color diffuse_, Color specular_,
		 double shininess_, double refractive_index_,
		 double reflective_weight_, double refractive_weight_)
  : model(0,0,1)
{
  ambient = ambient_;
  diffuse = diffuse_;
  specular = specular_;
  shininess = shininess_;
  refractive_index = refractive_index_;
  reflective_weight = reflective_weight_;
  refractive_weight = refractive_weight_;
}

void surface::set_lighting(Color ambient_, Color diffuse_, Color specular_,
			   double shininess_, double refractive_index_,
			  double reflective_weight_, double refractive_weight_)
{
  ambient = ambient_;
  diffuse = diffuse_;
  specular = specular_;
  shininess = shininess_;
  refractive_index = refractive_index_;
  reflective_weight = reflective_weight_;
  refractive_weight = refractive_weight_;
}

double surface::index() const
{
  return refractive_index;
}

double surface::reflection() const
{
  return reflective_weight;
}

double surface::refraction() const
{
  return refractive_weight;
}

Color surface::phong_ambient() const
{
  return ambient * Color(0.3, 0.3, 0.3);
}

Color surface::phong(point dir, light l, int depth, point vertex, point normal)
{
  point lightdir = (l.pos[3] ? l.pos - vertex : l.pos).normalize(),
    view = -dir.normalize();
  if(normal * view < 0.0) normal = - normal;
  double NdotL = normal * lightdir;
  Color color;
  if(NdotL > 0.0)
    {
      // calculate diffuse lighting
      color = diffuse * l.color * NdotL;

      // calculate specular highlights
      // calculate halfway vector, L+V
      point halfway = (lightdir + view).normalize();
      float NdotH = normal * halfway;
      if(NdotH > 0.0)
	color += specular * l.color
	  * pow(NdotH, shininess);
    }
  return color;
}
