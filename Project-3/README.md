# Project 3 — WebGL Shaders

This project implements several WebGL applications using vertex and fragment shaders to procedurally generate geometric shapes.

## Table of Contents

- [Wireframe Triangle](triangle.html)
- [10-Sided Filled Polygon](polygon.html)
- [Five-Pointed Star](star.html)
- [Rotating Star](star-rotating.html)
 
## Description of Each Version

**Wireframe Triangle** - A procedurally generated equilateral triangle drawn as a wireframe using `gl.LINE_LOOP` primitive type. This version uses vertex and fragment shaders to compute vertex positions.

**10-Sided Filled Polygon** - A filled convex polygon created using a triangle fan (`gl.TRIANGLE_FAN`). This version generates 10 vertices around a circle to form a disk-like shape.

**Five-Pointed Star** - A filled five-pointed star created by alternating the radius of vertices - even vertices are positioned at the outer radius while odd vertices are positioned closer to the center, creating the star's pointy appearance.

**Rotating Star** - The same five-pointed star with rotation animation. This version uses a time uniform variable to continuously rotate the star by modifying the angle calculation in the vertex shader.
