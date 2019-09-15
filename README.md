Minimal OpenGL-based SAT solver

Usage:
- Insert your CNF formula in [DIMACS format](http://www.satcompetition.org/2009/format-benchmarks2009.html) into the file `samples/sample_60k.cnf`
- Run the executable

Due to GLSL being restricted to 32bit Integers, only a maximum of 32 literals are supported. 
The maximum amount of clauses depends on the buffer texture sizes the underyling hardware supports, according to Khronos[\[1\]]() "\[the minimum is\] 65536, and the actual value may well be a good deal larger (generally on the order of the size of GPU memory)"

Dependencies (Developer only):
- [GLFW 3.2.1](https://www.glfw.org/download.html)
- [Glad 4.5 loader](http://glad.dav1d.de/#profile=core&language=c&specification=gl&loader=on&api=gl%3D4.5)
