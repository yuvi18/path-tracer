# Ray Tracing Lab

###Code by Yuvraj Baweja and Rohith Vishwajith

All features are working as intended currently. Some of the refract examples have outlines that don't match,
however the visual clarity is working as intended (and verified with Professor Vouga).

For our Phong Illumination model, we opted to use the abs function for diffuse illumination, and the max function
for specular illumination (as depicted on the slides).

We do a variety of different position rearrangements with RAY_EPSILION, since we were opting for correctness. For
the most part however, we take advantage of the normals (or its inverse) to adjust the position of a secondary ray.

We fire a reflect ray on every material that is reflective, and a refract ray on every material that is transculent.

We do NOT reflect off the back face of a reflective and transulcent material, as we believe this is a better
approximiation of what we would see in real life.

Distance attenuiation is clamped to [0,1], since this provided cleaner output.

Finally, we decided not to impelement backface culling, as we wanted to prioritize correctness over speed of our raytracer.