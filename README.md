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

  <img src="showcases/material_diffuse_50spp.png" width=150>
  <img src="showcases/material_500spp.png" width=150>
  <img src="showcases/material_1000spp_specular.png" width=150>
  <img src="showcases/material_roughmetal_50spp.png" width=150>

- Cornell Box
![](showcases/white_room_100spp.png)
![](showcases/material_1000spp.png)
![](showcases/cornel_box_albedo_texture.png)
- Area Lighting
![](showcases/area_lights.png)
- Lambert Sphere
![](showcases/first_demo.png)

