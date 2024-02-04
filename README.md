# Ray Tracing Lab

putting backup code here since i'm lazy:
//		    cout << "Seg fault happens here" << endl;
//		    double indexRatioEntry = 1 / m.index(i);
//		    double cosEntry = glm::dot(normal, r.getDirection());
//		    double sinSquaredEntry = glm::max(0.0, 1 - glm::pow(cosEntry, 2));
//		    double sinSquaredInternal = glm::pow(indexRatioEntry, 2) * sinSquaredEntry;
//		    glm::dvec3 dummy(0, 0, 0);
//		    ray refractRay(dummy, dummy, dummy, ray::REFRACTION);
//		    glm::dvec3 entryPos = r.at(i);
//		    //Cases of Refraction
//            if(sinSquaredEntry < RAY_EPSILON || m.index(i) == 1){
//                //No Deflection
//                refractRay = ray(r.at(i.getT() + RAY_EPSILON), r.getDirection(), glm::dvec3(1.0, 1.0, 1.0), ray::REFRACTION);
//            }
//            else if(sinSquaredInternal >= 1){
//                //Perfect Reflection
//                glm::dvec3 reflectDir = glm::reflect(r.getDirection(), normal);
//                refractRay = ray(r.at(i.getT() + RAY_EPSILON), reflectDir, glm::dvec3(1.0, 1.0, 1.0), ray::REFRACTION);
//            }
//            else{
//                //Normal Refraction
//                glm::dvec3 refractDir = glm::refract(r.getDirection(), normal, indexRatioEntry);
//                refractRay = ray(r.at(i.getT() + RAY_EPSILON), refractDir, glm::dvec3(1.0, 1.0, 1.0), ray::REFRACTION);
//            }
//            //Get exit point
//            isect exitPoint;
//            scene->intersect(refractRay, exitPoint);
//            glm::dvec3 exitPos = refractRay.at(exitPoint);
//            double d = glm::distance(entryPos, exitPos);
//            glm::dvec3 newNormal = -exitPoint.getN();
//            const Material &newM = exitPoint.getMaterial();
//            cout << r.getDirection() << endl;
//            cout << refractRay.getDirection() << endl;
//            cout << exitPoint.getT() << endl;
//            //Add color from the exit
//            colorC += newM.shade(scene.get(), refractRay, exitPoint) * glm::pow(m.kt(i), glm::dvec3(d));
//
//            //Now do what we did above again
//            double indexRatioExit =  m.index(i) / 1;
//            double cosExit = glm::dot(newNormal, refractRay.getDirection());
//            double sinSquaredInternal2 = glm::max(0.0, 1 - glm::pow(cosExit, 2));
//            double sinSquaredExit = glm::pow(indexRatioExit, 2) * sinSquaredInternal2;
//            ray exitRay(dummy, dummy, dummy, ray::REFRACTION);
//            //Cases of Refraction
//            if(sinSquaredInternal2 < RAY_EPSILON || newM.index(exitPoint) == 1){
//                //No Deflection
//                exitRay = ray(refractRay.at(exitPoint.getT() + RAY_EPSILON), refractRay.getDirection(), glm::dvec3(1.0, 1.0, 1.0), ray::REFRACTION);
//            }
//            else if(sinSquaredExit >= 1){
//                //Perfect Reflection
//                glm::dvec3 reflectDir = glm::reflect(refractRay.getDirection(), newNormal);
//                exitRay = ray(refractRay.at(exitPoint.getT() + RAY_EPSILON), reflectDir, glm::dvec3(1.0, 1.0, 1.0), ray::REFRACTION);
//            }
//            else{
//                //Normal Refraction
//                glm::dvec3 refractDir = glm::refract(refractRay.getDirection(), newNormal, indexRatioExit);
//                exitRay = ray(refractRay.at(exitPoint.getT() + RAY_EPSILON), refractDir, glm::dvec3(1.0, 1.0, 1.0), ray::REFRACTION);
//            }
//            glm::dvec3 refractResult = glm::pow(m.kt(i), glm::dvec3(d)) * traceRay(exitRay, thresh, depth - 1, t);
//            colorC += refractResult;
