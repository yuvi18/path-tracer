#include <cmath>
#include <iostream>

#include "light.h"
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>

using namespace std;

double DirectionalLight::distanceAttenuation(const glm::dvec3 &) const {
  // distance to light is infinite, so f(di) goes to 0.  Return 1.
  return 1.0;
}

glm::dvec3 DirectionalLight::shadowAttenuation(const ray &r,
                                               const glm::dvec3 &p) const {
    glm::dvec3 light = getColor();
    isect point;
    ray shadowRay = r;
    scene->intersect(shadowRay, point);
    while(point.getT() < 1000){
        //We intersected a material before the light. Now we need to get the other side to find the distance.
        glm::dvec3 entry = shadowRay.at(point);
        shadowRay.setPosition(shadowRay.at(point.getT() + RAY_EPSILON));
        scene->intersect(shadowRay, point);
        glm::dvec3 exit = shadowRay.at(point);
        double distance = glm::distance(entry, exit);
        //Found distance, now do the transulcent light formula
        light *= glm::pow(point.getMaterial().kt(point), glm::dvec3(distance));
        //Great, now get the next material's intersection and continue.
        shadowRay.setPosition(shadowRay.at(point.getT() + RAY_EPSILON));
        scene->intersect(shadowRay, point);
    }
    return light;
}

glm::dvec3 DirectionalLight::getColor() const { return color; }

glm::dvec3 DirectionalLight::getDirection(const glm::dvec3 &) const {
    return -orientation;
}

glm::dvec3 DirectionalLight::getRelativeDirection(const glm::dvec3 &P) const {
    return -orientation;
}

double PointLight::distanceAttenuation(const glm::dvec3 &P) const {
  double distance = glm::distance(position, P);
  double denom = constantTerm + linearTerm * distance + quadraticTerm * glm::pow(distance, 2);
  return glm::min(1.0, 1 / denom);
}

glm::dvec3 PointLight::getColor() const { return color; }

glm::dvec3 PointLight::getDirection(const glm::dvec3 &P) const {
  return glm::normalize(position - P);
}

glm::dvec3 PointLight::getRelativeDirection(const glm::dvec3 &P) const {
    return position - P;
}

glm::dvec3 PointLight::shadowAttenuation(const ray &r,
                                         const glm::dvec3 &p) const {
    glm::dvec3 light = getColor();
    double lightT = glm::sqrt(glm::dot(position - p, position - p));
    isect point;
    ray shadowRay = r;
    scene->intersect(shadowRay, point);
    while(point.getT() < lightT){
        //We intersected a material before the light. Now we need to get the other side to find the distance.
        glm::dvec3 entry = shadowRay.at(point);
        shadowRay.setPosition(shadowRay.at(point.getT() + RAY_EPSILON));
        scene->intersect(shadowRay, point);
        glm::dvec3 exit = shadowRay.at(point);
        double distance = glm::distance(entry, exit);
        //Found distance, now do the transulcent light formula
        light *= glm::pow(point.getMaterial().kt(point), glm::dvec3(distance));
        //Great, now get the next material's intersection and continue.
        shadowRay.setPosition(shadowRay.at(point.getT() + RAY_EPSILON));
        scene->intersect(shadowRay, point);
        lightT = glm::sqrt(glm::dot(position - shadowRay.getPosition(), position - shadowRay.getPosition()));
    }
    return light;
}

//Distance attenuation is done in shadow attenuation
double RectangleAreaLight::distanceAttenuation(const glm::dvec3 &P) const {
    return 1.0;
    double distance = glm::distance(position, P);
    double denom = constantTerm + linearTerm * distance + quadraticTerm * glm::pow(distance, 2);
    return glm::min(1.0, 1 / denom);
}

//Done
glm::dvec3 RectangleAreaLight::getColor() const { return color; }

//Not done
glm::dvec3 RectangleAreaLight::getDirection(const glm::dvec3 &P) const {
    return glm::normalize(position - P);
}

// Not used
glm::dvec3 RectangleAreaLight::getRelativeDirection(const glm::dvec3 &P) const {
    return position - P;
}

//Ignore the ray that's passed in, and instead make 10 rays here
glm::dvec3 RectangleAreaLight::shadowAttenuation(const ray &r,
                                         const glm::dvec3 &p) const {
    glm::dvec3 light = getColor();
    double lightT = glm::sqrt(glm::dot(position - p, position - p));
    isect point;
    ray shadowRay = r;
    scene->intersect(shadowRay, point);
    while(point.getT() < lightT){
        //We intersected a material before the light. Now we need to get the other side to find the distance.
        glm::dvec3 entry = shadowRay.at(point);
        shadowRay.setPosition(shadowRay.at(point.getT() + RAY_EPSILON));
        scene->intersect(shadowRay, point);
        glm::dvec3 exit = shadowRay.at(point);
        double distance = glm::distance(entry, exit);
        //Found distance, now do the transulcent light formula
        light *= glm::pow(point.getMaterial().kt(point), glm::dvec3(distance));
        //Great, now get the next material's intersection and continue.
        shadowRay.setPosition(shadowRay.at(point.getT() + RAY_EPSILON));
        scene->intersect(shadowRay, point);
        lightT = glm::sqrt(glm::dot(position - shadowRay.getPosition(), position - shadowRay.getPosition()));
    }
    return light;
}

#define VERBOSE 0

