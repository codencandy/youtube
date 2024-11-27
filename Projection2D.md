# 2D Projection

What problem do we want to solve?
Our geometry or images are in a different coordinate system than the one the GPU uses. We have an application/world coordinate system and the GPU uses normalized device coordinates.

In our case (which we can chose as we like) the upper left corner in application space is 0,0 and the lower left is e.g. 600, 600.
In NDC space (on Apple platforms) the origin (0,0) is in the center of the screen with the dimension 1.0 in each direction. So the upper left corner is now -1.0, 1.0.

         0  X---------* world
         |  |         | origin in the top left 
         |  |         | corner
        `.´ |         | 
         h  *---------*
            0 ------> w

         1  *---------* NDC
            |         | origin in the center
            |    X    | 
            |         |
        -1  *---------*
           -1 ------> 1

Time for matrix operations to enter the stage. We need to transform and scale the input - that means our projection matrix combines transformation and scaling.

Scaling is achieved by multiplying the input (a,b).
Transformation is achieved by adding a constant factor (e,f).

             |x|   | x*a 0    0   e|   |x'|
             |y|   | 0   y*b  0   f|   |y'|
             |z|   | 0   0    1   1|   |0 |
             |1|   | 0   0    0   1|   |1 |

This matrix will be used to transform an input coordinate [x, y, z, 1] to an output in NDC space. For us that means we need to find the 4 parameters **a**, **b**, **e** and **f**.   

Since we are only in 2D space we are not concerned with the z value, it's always 0.

That leaves us with two functions

    f(x) = x*a + e
    f(y) = y*b + f

For the first we already know the outcome for 2 cases

    f(0)   = -1.0
    f(600) =  1.0

With this we can derive a and e like this

    f(0)  = 0*a + e           [now replace f(0) with -1.0]
    -1.0  = e

    f(600) = 600*a + (-1.0)   [now replace f(600) with 1.0]
    1.0    = 600*a - 1.0
    2.0    = 600*a
    a      = 2.0/600

For the second we also know two cases

    f(0)   =  1.0
    f(600) = -1.0

Same as before with f(y) = y*b + f

    f(0)   = 0*b + f          [replace f(0) with 1.0]
    1.0    = f

    f(600) = 600*b + 1.0      [replace f(600) with -1.0]
    -1.0   = 600*b + 1.0
    -2.0   = 600*b
    b      = -2.0/600

So now we have all 4 parameters and can construct the projection matrix where 600 in x is now the maximum width (w) and 600 in y is the maximum height (h) 

    a =  2.0/w
    b = -2.0/h
    e = -1.0
    f =  1.0

    |x|   | x*(2.0/w) 0           0   -1|   |x'|
    |y|   | 0         y*(-2.0/h)  0    1|   |y'|
    |z|   | 0         0           1    1|   |0 |
    |1|   | 0         0           0    1|   |1 |




