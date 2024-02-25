# verbose-octo-computing-machine
A simple c++ project to graph a singular contour of an arbitary 3D function.\
This projects is a place for me to experiment with optimisation.
## Performance
### Mirco optimisations
All tests were run with the command line arguments `0.00001 -5 2 5 -2 2560 1024 16 0.1 100` and were conpiled in release mode.\
On an i5 1534p the results are as follows:\
v1: 31ms\
v2: 33ms\
v3: 34ms\
While I did not record the results on much older i7 5820k the results were flipped with v3 being the fastest.