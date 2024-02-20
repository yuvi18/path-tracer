#include "material.h"
#include "../ui/TraceUI.h"
#include "light.h"
#include "ray.h"
extern TraceUI *traceUI;

#include "../fileio/images.h"
#include <glm/gtx/io.hpp>
#include <iostream>

// Added this here.
#include "../SceneObjects/trimesh.h"

using namespace std;
extern bool debugMode;

Material::~Material() {}

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
glm::dvec3 Material::shade(Scene *scene, const ray &r, const isect &i) const
{
  // TODO: Add some sort of normal mapping code here (replace i.getN()?)
    glm::dvec3 newN = i.getN();
  bool hasNormalMap = _kn.mapped();
  if(hasNormalMap){
      // ADDED FOR NORMAL MAP
      glm::dvec3 mapN = kn(i) * 2.0 - 1.0;
      // distorted normal = normalMap.r * tangent + normalMap.g * bitangent + normalMap.b * normal
      newN = mapN.r * i.getTangent() + mapN.g * i.getBiTangent() + mapN.b * i.getN();
  }

  glm::dvec3 pointOfImpact = r.at(i);
  glm::dvec3 finalShade(0, 0, 0);
  // Always add ambient light.
  glm::dvec3 ambientTerm = ka(i) * scene->ambient();
  glm::dvec3 diffuseTerm(0, 0, 0);
  glm::dvec3 specularTerm(0, 0, 0);
  for (const auto &pLight : scene->getAllLights())
  {
    glm::dvec3 firePos = pointOfImpact + i.getN() * RAY_EPSILON * 3.0;
    glm::dvec3 fireDirection = pLight->getDirection(firePos);
    glm::dvec3 fireWeight = glm::dvec3(1.0, 1.0, 1.0);
    ray shadowRay(firePos, fireDirection, fireWeight, ray::SHADOW);

    // Diffusion Term
    glm::dvec3 contributionD = pLight->shadowAttenuation(shadowRay, firePos);
    contributionD *= pLight->distanceAttenuation(pointOfImpact);
    contributionD *= kd(i);
    contributionD *= glm::abs(glm::dot(newN, pLight->getDirection(pointOfImpact)));
    diffuseTerm += contributionD;
    // Specular Term
    glm::dvec3 contributionS = pLight->shadowAttenuation(shadowRay, firePos);
    contributionS *= pLight->distanceAttenuation(pointOfImpact);
    contributionS *= ks(i);
    glm::dvec3 v = -1.0 * r.getDirection();
    glm::dvec3 r = glm::reflect(-1.0 * pLight->getDirection(pointOfImpact), newN);
    contributionS *= glm::pow(glm::max(0.0, glm::dot(v, r)), shininess(i));
    specularTerm += contributionS;
  }

  finalShade += ambientTerm;
  finalShade += diffuseTerm;
  finalShade += specularTerm;
  finalShade += ke(i);
  return finalShade;
}

TextureMap::TextureMap(string filename)
{
  data = readImage(filename.c_str(), width, height);
  if (data.empty())
  {
    width = 0;
    height = 0;
    string error("Unable to load texture map '");
    error.append(filename);
    error.append("'.");
    throw TextureMapException(error);
  }
}

glm::dvec3 TextureMap::getMappedValue(const glm::dvec2 &coord) const
{
  // YOUR CODE HERE
  //
  // In order to add texture mapping support to the
  // raytracer, you need to implement this function.
  // What this function should do is convert from
  // parametric space which is the unit square
  // [0, 1] x [0, 1] in 2-space to bitmap coordinates,
  // and use these to perform bilinear interpolation
  // of the values.
  int w = width, h = height;
  double x = coord[0] * double(w - 1), y = coord[1] * double(h - 1);
  double x1 = double(int(x)), y1 = double(int(y)); // cast back to int?
  double x2 = x1 + 1, y2 = y1 + 1;
  glm::dvec3 color11 = getPixelAt(int(x1), int(y1));
  glm::dvec3 color12 = getPixelAt(int(x1), int(y2));
  glm::dvec3 color21 = getPixelAt(int(x2), int(y1));
  glm::dvec3 color22 = getPixelAt(int(x2), int(y2));
  glm::dvec3 color = color11 * (x2 - x) * (y2 - y) +
                     color21 * (x - x1) * (y2 - y) +
                     color12 * (x2 - x) * (y - y1) +
                     color22 * (x - x1) * (y - y1);
  return color;
}

glm::dvec3 TextureMap::getPixelAt(int x, int y) const
{
  // YOUR CODE HERE
  // In order to add texture mapping support to the
  // raytracer, you need to implement this function.
  int w = width, h = height;
  if (x < 0 || x >= w || y < 0 || y >= h || data.size() < 3 * w * h)
    return glm::dvec3();
  int startI = (x + y * w) * 3;
  return glm::dvec3(double(data[startI]), double(data[startI + 1]), double(data[startI + 2])) / 255.0;
}

glm::dvec3 MaterialParameter::value(const isect &is) const
{
  if (0 != _textureMap)
    return _textureMap->getMappedValue(is.getUVCoordinates());
  else
    return _value;
}

double MaterialParameter::intensityValue(const isect &is) const
{
  if (0 != _textureMap)
  {
    glm::dvec3 value(_textureMap->getMappedValue(is.getUVCoordinates()));
    return (0.299 * value[0]) + (0.587 * value[1]) + (0.114 * value[2]);
  }
  else
    return (0.299 * _value[0]) + (0.587 * _value[1]) + (0.114 * _value[2]);
}

/*

// ADDED FOR NORMAL MAP
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
glm::dvec3 normalMap(TextureMap *normalMap, TrimeshFace *tri, const isect &is)
{
  // 1. sample color at current coordinates (0 to 1 space), convert it to a normal
  // vector space (-1 to 1), which is the normal vector in tangent space
  glm::dvec3 color(normalMap->getMappedValue(is.getUVCoordinates()));
  glm::dvec3 mapN = color * 2.0 - 1.0;
  // 2. use the edges of our triangle to find a tangent and bitangent vector using
  // the triangle normal and edges
  return glm::dvec3();
}

*/