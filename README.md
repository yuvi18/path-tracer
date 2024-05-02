# Path Tracing

###Code by Yuvraj Baweja and Shobhit Gupta

For this project we did path tracing, using our original ray tracing code.

We implemented the Cook-Torrance BRDF model, Russian Roulette path tracing, and rectangular area lights.

All of the scenes that test features can be found in pathTracer.

The three scenes we demonstrate are:

1. A scene with smooth lighting / shadows
2. A scene displaying different roughness, metallicness, etc. based on the Cook-Torrance BRDF model.
3. A scene showcasing softer shadows with area lights.

The relevant code we changed is in lights.h/cpp, RayTracer.cpp, and material.h/cpp.