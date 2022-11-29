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
- Perfect reflection/refraction
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
- Cornell Box
- ![](showcases/material.png)
- ![](showcases/cornel_box_albedo_texture.png)
![](showcases/white_room_monkey_100spp.png)
- Area Lighting
![](showcases/area_lights.png)
- Lambert Sphere
![](showcases/first_demo.png)

