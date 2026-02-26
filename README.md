# Particle Simulation to Learn C++ and OpenGL

A simple real-time 3D particle simulation built with C++, OpenGL, GLFW, GLAD, and GLM.

---

## Features

- 3D particles with mass and radius  
- Velocity Verlet integration  
- Inverse-square gravity  
- Elastic particle collisions  
- Boundary sphere containment  
- Free-look camera  

---

## Project Structure

ParticleSimulation/
├── main.cpp
├── library/
│   ├── Particle.h
│   ├── Particle3D.h
│   ├── Camera.h
│   └── Render.h
├── output/
└── app/

---

## Physics

Position update:
x = x + v·dt + 0.5a·dt²  

Velocity update:
v = v + 0.5(a_old + a_new)·dt  

Gravity:
a = G / r²

---

## Requirements

- C++17+
- OpenGL
- GLFW
- GLAD
- GLM

---

## How to Run

git clone https://github.com/jahaanb7/Space-Simulation.git  
cd ParticleSimulation  
mkdir build  
cd build  
cmake ..  
make  
./simulation  

---

## Purpose

Built as a learning project for real-time physics simulation and 3D graphics programming.
