## 2D Rotation

In order to rotate elements we need 2 things:

- a pivot point
- a rotation matrix

### Pivot point

A rotation always uses the origin of the coordinate system (0,0). That means 
we need to position the element to be rotate in a way that the point we rotate around - the **pivot point** - is the origin.
For that to happen we first need to move the object such that (0,0) matches this point.

           ^ 
           | 
        *-----*
        |  |  |
        |  |  |
        |  o--|------------>
        *-----*

        o is the pivot point

1. move the object with a transformation matrix such that the pivot point becomes the origin
2. rotate using a rotation matrix
3. move to the desired endpoint with the model matrix

### Rotation Matrix

    | x * cos(a)  y * -sin(a) |   |x'|
    | x * sin(a)  y *  cos(a) |   |y'|

    a is the rotation angle in radians
          
