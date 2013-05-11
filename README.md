## ModelViewer libretro

This is a simple modelviewer for libretro using the OpenGL interface.
It displays basic objects in .obj format. Objects are loaded as ROMs in RetroArch.

    retroarch -L libretro-modelviewer.so cool_thing.obj

## Controls

Models are rotated using left RetroPad analog axis. The object is scaled with right analog.

## Textures/materials

This implementation does not support MTL materials. Instead, a simple `texture` extension is used.

    texture Foo

sets the current texture to `Foo.png` relative to where the obj was loaded from.
All faces declared after will refer to this texture.

## Changing resolution

The implementation exposes a libretro core option to change internal resolution.
Resolutions can be set to a scale factor of 320x240 (up to 1920x1440).

## Example OBJ

This declares a simple textured triangle:

    texture blockDiamond
    v -1.0 -1.0 0.0
    v 1.0 -1.0 0.0
    v 0.0 1.0 0.0
    vn 0.0 0.0 -1.0
    vt 0.0 0.0
    vt 1.0 0.0
    vt 0.5 1.0

    f 1/1/1 2/2/1 3/3/1

