# CHANGELOG

## Added
New class CompositeTexture replaces TextureManifold in allowing multitexturing of an object.  
    * Texture pointers are loaded in descrete slots on the CompositeTexture.  
        * The number of available slots is determined by hw limits.  
        * Slots are aligned in 4-slot segments which can be indivdually blended (blender phase 0)  
        * Each segment pair can have an indidual blnder set (blender phase 1)  
  
New class Image for loading and manipulating raw image data.  
  
  
## Changed
Textures are now immutable once rendered.
