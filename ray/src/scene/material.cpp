#include "material.h"
#include "../ui/TraceUI.h"
#include "light.h"
#include "ray.h"
extern TraceUI *traceUI;

#include "../fileio/images.h"
#include <glm/gtx/io.hpp>
#include <iostream>

using namespace std;
extern bool debugMode;

Material::~Material() {}

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
glm::dvec3 Material::shade(Scene *scene, const ray &r, const isect &i) const {
  // YOUR CODE HERE

  // For now, this method just returns the diffuse color of the object.
  // This gives a single matte color for every distinct surface in the
  // scene, and that's it.  Simple, but enough to get you started.
  // (It's also inconsistent with the phong model...)

  // Your mission is to fill in this method with the rest of the phong
  // shading model, including the contributions of all the light sources.
  // You will need to call both distanceAttenuation() and
  // shadowAttenuation()
  // somewhere in your code in order to compute shadows and light falloff.
  //	if( debugMode )
  //		std::cout << "Debugging Phong code..." << std::endl;

  // When you're iterating through the lights,
  // you'll want to use code that looks something
  // like this:
  //
  // for ( const auto& pLight : scene->getAllLights() )
  // {
  //              // pLight has type Light*
  // 		.
  // 		.
  // 		.
  // }
  glm::dvec3 pointOfImpact = r.at(i.getT());
  glm::dvec3 finalShade(0, 0, 0);
  //Always add ambient light.
  glm::dvec3 ambientTerm = ka(i) * scene->ambient();
  glm::dvec3 diffuseTerm(0, 0, 0);
  glm::dvec3 specularTerm(0, 0, 0);
  for ( const auto& pLight : scene->getAllLights() ){
        //TODO: Check colision
        if(true){
            //Diffusion Term
            glm::dvec3 contributionD = pLight->getColor() * pLight->distanceAttenuation(pointOfImpact);
            contributionD *= kd(i);
            contributionD *= glm::abs(glm::dot(i.getN(), pLight->getDirection(pointOfImpact)));
            diffuseTerm += contributionD;

            //Specular Term
            glm::dvec3 contributionS = pLight->getColor() * pLight->distanceAttenuation(pointOfImpact);
            contributionS *= ks(i);
            glm::dvec3 v = -1.0 * r.getDirection();
            glm::dvec3 r = glm::reflect(-1.0 * pLight->getDirection(pointOfImpact), i.getN());
            contributionS *= glm::pow(glm::max(0.0, glm::dot(v, r)), shininess(i));
            specularTerm += contributionS;
        }
  }

  finalShade += ambientTerm;
  finalShade += diffuseTerm;
  finalShade += specularTerm;
  return finalShade;
}

TextureMap::TextureMap(string filename) {
  data = readImage(filename.c_str(), width, height);
  if (data.empty()) {
    width = 0;
    height = 0;
    string error("Unable to load texture map '");
    error.append(filename);
    error.append("'.");
    throw TextureMapException(error);
  }
}

glm::dvec3 TextureMap::getMappedValue(const glm::dvec2 &coord) const {
  // YOUR CODE HERE
  //
  // In order to add texture mapping support to the
  // raytracer, you need to implement this function.
  // What this function should do is convert from
  // parametric space which is the unit square
  // [0, 1] x [0, 1] in 2-space to bitmap coordinates,
  // and use these to perform bilinear interpolation
  // of the values.

  return glm::dvec3(1, 1, 1);
}

glm::dvec3 TextureMap::getPixelAt(int x, int y) const {
  // YOUR CODE HERE
  //
  // In order to add texture mapping support to the
  // raytracer, you need to implement this function.

  return glm::dvec3(1, 1, 1);
}

glm::dvec3 MaterialParameter::value(const isect &is) const {
  if (0 != _textureMap)
    return _textureMap->getMappedValue(is.getUVCoordinates());
  else
    return _value;
}

double MaterialParameter::intensityValue(const isect &is) const {
  if (0 != _textureMap) {
    glm::dvec3 value(_textureMap->getMappedValue(is.getUVCoordinates()));
    return (0.299 * value[0]) + (0.587 * value[1]) + (0.114 * value[2]);
  } else
    return (0.299 * _value[0]) + (0.587 * _value[1]) + (0.114 * _value[2]);
}
