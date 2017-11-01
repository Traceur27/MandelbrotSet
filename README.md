# Mandelbrot set
Program generating one of most famous fractal - Mandelbrot set. It's cross-compilation with c and x86-64 assembly. In addition it uses AVX to parallel computations.

## Usage
Requirements:
```bash
sudo apt-get install liballegro5-dev nasm
```
Run example:
```bash
git clone https://github.com/kczarnota/MandelbrotSet
cd MandelbrotSet
make
./mandelbrot 1 -1 -2 1 512 256 600
```
Parameters:
* x1, y1, x2, y2 - rectangle vertices, where to zoom the image
* width, height - size of generated image
* maxIterations - max number of iteration in algorithm

## Screenshots
![](result.jpg?raw=true)