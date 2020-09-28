# Introduction
<img src="https://media.giphy.com/media/e4d0JlTz0hU8gCKjWn/giphy.gif" >
This is a report of a project which main purpose was to implement a Particle system in OpenGL library. Particle systems are ubiquitously used in computer graphics for producing
animated effects such as fire, smoke, clouds, fireworks, etc. Particle systems have been widely used in film and games for visualizing random and unpredictable effects such
as explosions. In this project was made an attempt to build a basic particle system, which was animated as a firework.
This works concentrates more on building a particle system, which will handle firework animation. 

![](https://img.shields.io/badge/OS-Windows-informational?style=flat&logo=linux&logoColor=white&color=2bbc8a)
![](https://img.shields.io/badge/Editor-Microsoft_Visual_Studio-informational?style=flat&logo=intellij-idea&logoColor=white&color=2bbc8a)
![](https://img.shields.io/badge/Code-GLSL-informational?style=flat&logo=go&logoColor=white&color=2bbc8a)
![](https://img.shields.io/badge/Code-C\C++-informational?style=flat&logo=python&logoColor=white&color=2bbc8a)
![](https://img.shields.io/badge/Framework-OpenGL-informational?style=flat&logo=javascript&logoColor=white&color=2bbc8a)


# Relevant files

- algebra.cpp\algebra.h - my own written algebraic functions for matrices and vectors
- mesh.cpp\mesh.h - defining structures Triangle, Camera and Mesh. Triangle hosts vertices, Mesh include all vertices, nodes, normals, Material parameters and particle parameters. 
- vertex-phong.txt, fragm-phong.txt, vertex-tex.txt, frag-tex.txt - GLSL files. Program loads GLSL from txt. Handling Phong shading and Texture shading.
- mesh_particle.h - Hosts vertices for single particle.
- main.cpp - Main file, running application, loading meshes, rendering, animating. 

# Detailed description
<img src="https://i.imgur.com/FlveLCh.png">


**Particle System**
In the project particle system is a structure, which contains a pointer to the beginning of the mesh list, and some parameters, common for all particles, such as: material parameters,
required for Phong shader; initial position of all particles(emitter); variable, which is responsible to switch particles between random and defined colour; and a parameter
for radius of the field, which constraints the whole system. Mesh represents a set of parameters, which describe one single triangle: three vertices, one face, it’s normal. Each mesh has it’s own material parameters and position. As a
particle, mesh also has a direction and life. Direction in a project is a replacement for velocity, which is common parameter for particles. Life represents how long particle will
be active.
Number of particles is important. Realistic effects can contain up to ten thousands - one million particles, according to Lutz Latta. Number of particles as well as number of
their vertices can affect performance and appearance.

**Rendering**
Rendering process consists of some stages.
At the beginning View, Projection and View-Projections matrices are calculated. It is important to calculate them at the rendering, because they depend on the camera position,
which can be changed by using keyboard. In the project actually there are two processes, that can force program to redisplay: keyboard interaction and timer. Timer is represented
by function, which forces the program to redisplay each n mseconds. Redisplay in the program means calling Display function one more time. Particle states are changed during
Display, so it creates animation.
Then, background is rendered. Here has been implemented basic texture mapping, where some defined vertices are stored in the buffer, and later texture is assigned to these vertices
as to corners. For texture another shader was created; Probably it was not the optimal solution, and it was done because it was the easiest way to integrate texture to code, which already had
one shader. Also, because there is a need to use two shaders, active shader is changed during rendering - one shader is used for texture rendering, another is used for mesh
rendering.
After that, particles are rendered. Project implementation allows to use some particle systems in the scene; however, there were some problems in refreshing particles when 
some systems were rendered at the same time. In the project some particle systems are changed, next one is rendered when all particles from previous one are "dead"

**Particle Life Cycle**
There are three general phases of particle lifespan. At birth they are placed in the coordinate system, then each particle changes and moves during its lifetime, and dies
when it’s time to life has expired. By dying usually means removing particle from the system. The particles in a system can either exist permanently or only for a limited
time. In the project all particles are created at once, and before all of them ended their lives no one particle is created. That is the reason why in current implementation particles
are not removed from the system, they just ignored, since the final implementation does not require to constantly create new particles - but for such rendering it is relevant to
implement proper particle cleaning.
There are a lot of ways in which particles can behave. Each particle can have a direction of moving, which can be described by equation. The simplest one is a direct move, where
coordinates are changed in the time with a given or calculated velocity. There are also some approaches on how new position can be calculated, like Euler or Verlet integration.
In the project instead of velocity only direction is calculated. Direction represents a vector of movement. For fireworks implementation, direction for each particle is computed by
pseudo-random function. Particle position is changed during time by addition of current particle position and it’s direction.
In the project all particles are created at one position at the beginning: initial position of the particle system, or emitter(Accordingly to Reeves system, emitter is a part
of particle system, which creates the particles). Then all particles start their moving until their life becomes zero. Life is decreased by delta of time between displays. Also,
in the current implementation life can become zero if particle exceeds certain distance from original point. It was done with an assumption that all particles should move only
within a constraint field, and if there is a one distance which all particles can’t exceed, final result will represent something about sphere.

# To Do'es
There are a lot of options how work can be improved. First, memory allocation of particles is not optimized; In the project meshes are created for each particle, and, even
after implementing a mesh only with a three vertices, it is still not the best option how memory can be allocated. For example, rendering particles as point sprites can
reduce number of vertices and improve optimization. Good optimization is required for implementing systems with a huge amount of particles with realistic behaviour, such
as realistic smoke or fluids. Also, there could be implemented one shader for both Phong shading and Texture rendering. It was not important during the work, but if it was a
necessity to create some objects with texture for the scene, it would be relevant to mix shading and texture mapping.
Fireworks can be improved too. In the project was implemented the basic idea of how they can be looked, but the behaviour of the particles can be computed in more advanced
ways. For example, particles can handle an acceleration, what can be used for gravity simulation. Also, some particle systems should be able to work at one time.
