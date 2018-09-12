# CHANGELOG

## Added 
* A changelog.
* A Texture class used for loading image data from a number sources.
* Support for multitexturing
  *  Use Object.addTexture to add a texture to an object.
  * One ob ject can have uo to 16 textures attached.
  * Textures can be transformed individually per object
* A Screen class used to add subscreens to the window.
  * Use Object.render, specifying a screen to render to that dcreen.
  * Screens can be translated and scaled, but not rotated.
* Added the Window class which ac ts as a singleton screen representing the window.
  * Several functions which were previously naked in the jgl namespace are now methods of Window
    * Including setClearColor, camera translation, and window size controls.
  
## Removed  
* Temporarily removed lighting functionality

## Changed
* Re-wrote the rendering code for increased performance for users with dedicated GPUs.
* Most jgl functions were moved to other classes.
* JGL now uses GLSL version 440 with OpenGL 4.4
  * This may cause issues on older hardware but compatibility changes are in the works.
