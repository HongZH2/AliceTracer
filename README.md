# AliceTracer
### A Ray Tracer Demo 
Currently, I build this based on the path tracing algorithm, which is a general method for scene rendering.

### Features
#### - Spatial Separation
- Naive Two-level BVH
####  - Integrator
- Vanilla Monte Carlo 
- Next Event Estimate
- Multiple Importance Sampling
#### - Materials
- Lambert Diffuse
- Perfect Reflection/Refraction
- Microfacet Specular

#### - Lights
- Rectangle Area Lights
#### - Texture
- albedo texture
- light probe (hdr)
### - TODO
- More Materials
    - Disney Materials
- Bidirectional Monte Carlo with Multiple Importance
- Multiple-Thread BVH/ Optimized BVH

### Showcases
#### Monte Carlo for Path Tracing
- Materials

  <div align="center">
  <img src="showcases/material_diffuse_50spp.png" width=180>
  <img src="showcases/material_500spp.png" width=180>
  <img src="showcases/material_1000spp_specular.png" width=180>
  <img src="showcases/material_roughmetal_50spp.png" width=180>
  </div>
  
- Cornell Box
  <div align="center">
  <img src="showcases/white_room_100spp.png" width=600>
  <img src="showcases/material_1000spp.png" width=600>
  <img src="showcases/cornel_box_albedo_texture.png" width=600>
  </div>
  
- Area Lighting
 <div align="center">
 <img src="showcases/area_lights.png" width=600>
 </div>

- Lambert Sphere
 <div align="center">
 <img src="showcases/first_demo.png" width=600>
 </div>


