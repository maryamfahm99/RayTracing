//
//  sphere.h
//  template
//
//  Created by memo2 on 9/13/23.
//
#pragma once

#ifndef sphere_h
#define sphere_h

#include "vec.h"
#include "Matrix.h"
#include "ray.h"

// This file consists of the sphere class.
// I identify the sphere by the center and its radius. 

class sphere{
   public:
    vec center;
    float radius;
    vec color;
    
    sphere(){
        //unit sphere
        radius = 0.3;
        center = vec(0,0,0,1);
        color = vec(0.5,0.5,0.5,0);
    }
    sphere(vec c, float r, vec clr){
        radius = r;
        center = c;
        color = clr;
    }
    vec get_center(){
        return center;
    }
    float get_radius(){
        return radius;
    }
    vec get_clr(){
        return color;
    }
    bool point_on_sphere(vec& point){
        if ( (point-center).length() <= radius + 1e-3){
            return true;
        }
        return false;
    }
};



#endif /* sphere_h */
