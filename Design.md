# Render Settings
## Render
- double - max bounces
- double - bias (0.001)
## Performance
- int - samples per iteration
- int - target sample count
- double - basic ratio
- double - hole file ratio


# Camera Settings
## Resolution
- bool - Use Viewport Resolution
- vec2 - resolution
## Attributes
- double - Vertical FOV
- point3 - position
- point3 - lookat
- point3 - vup
- double - focus distance
- double - defocus angle
- color - background

# Objects
- Quad
  - Name: ajdsiosjd 
  - Attributes:
    - position A
    - position B
    - material -> mat
  - Move, Rotate "hidden transformers"  
no delete feature
# World
- Quad A -> object
- Quad B -> object
- Box A -> object  
delete allowed

# Object class
- render hittable
- source hittable
  - Config
  - Material
- move, rotate hittables (if enabled)
- name