# Starrider Engine
A work in progress game engine written in c++. The goal of the project is to do as much from scratch as possible. That being said, compromises have to be made. For example: for now I use FreeType 2 for font rendering because I wanted to be able to render TTF fonts early on. But i'm hoping in the future to maybe also provide my own TTF (or other font-types) rendering solutions.

## Features
- Software rasterization
- 32 bit texture mapping
- Texture alpha blending
- TTF and Bitmap font support 

## Requirements
- Freetype 2
- A system with Xlib and MIT-SHM headers available. Most unix systems should be fine with x-server installed.

## Contributing
As much as i like the idea of working with multiple people on a game engine, I won't be accepting
any contributions as of now. The purpose of this project is mainly to learn low level graphics programming
and game development and to have a bit of fun with that. Maybe if the Engine is in a more mature state, I will
allow contributions.
