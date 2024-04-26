#include "ofApp.h"
#include <cmath>
#include "ray.h"

using std::sqrt;
//--------------------------------------------------------------
void ofApp::setup() {
    
    
    bMousePressed   = false;
    Selected  = false;
    Right = Left = Up = Down = In = Out  = false;
    ScaleUp = ScaleDown = false;
    RotateX = RotateY = RotateZ= false;
    camera_animate = light_animate = false;
    
    // time set up
    clock_t  start, end;
    start = clock();
    
    // ########### Viewpoint Setting ############ //
    
    // The resolution
    w = 640;
    h = 480;
    
    view_pnt = vec(0,0,-1,1);
    light = vec(0,5,0,1);
    float focal_length = 1;
    double pi = M_PI;
    
//    vec view_pln_u = vec(width,0,0,0);  // make it to depend of the up vector
//    vec view_pln_v = vec(0,-height,0,0);
//    vec delta_u = view_pln_u / w;
//    vec delta_v = view_pln_v / h;
//    vec delta_u = view_pln_u / w;
//    vec delta_v = view_pln_v / h;
//    vec up_left = view_pnt + vec(0,0,1,0) - (view_pln_u/2) - (view_pln_v/2);
//    
    //up_left.print(); // up left corner
    
    upVector = vec(0,1,0,0);
    viewDir = vec(0,0,1,0);
    binormal = upVector.cross_product(viewDir);
    binormal.normalized();
    viewPlnU = (binormal * width);
    upVector.normalized();
    viewPlnV = (upVector * (-height));
    vec delta_u = viewPlnU / w;
    vec delta_v = viewPlnV / h;
    vec up_left = view_pnt + viewDir - (viewPlnU/2) - (viewPlnV/2);
    
    
    binormal.print();
    viewPlnU.print();
    viewPlnV.print();

    first_pixel = up_left + ((delta_u + delta_v)* 0.5);
    //first_pixel.print(); //  the up left pixel
    
    // ########### build objects ############ //


    // plane
    //q = quad(vec(2,-1.4,3,1), vec(2,-1.4,-1,1),vec(-2,-1.4,-1,1),vec(-2,-1.4,3,1),vec(0.5,0.5,0.5,0));
    q = quad(vec(2,-1.4,2,1), vec(2,-1.4,-2,1),vec(-2,-1.4,-2,1),vec(-2,-1.4,2,1),vec(0.5,0.5,0.5,0));
    
    // spheres
    sphere s1 = sphere(vec(0,-0.8,0.15,1),0.3, vec(1,0,0,0));
    sphere s2 = sphere(vec(0,-0.4,1,1),0.7, vec(0,1,0,0));
    sphere s3 = sphere(light, 0.02, vec(1,1,1,0));
    s[0] = s1; s[1] = s2; s[2] = s3;
    
    // ellipsoids
    ellipsoid e1 = ellipsoid(vec(-1.1,-0.55,0.9-0.3,1),0.3, 0.6, 0.3, vec(0.91,0.212,0.349,0),0);
    //ellipsoid e1 = ellipsoid(vec(0,0,1,1),0.3, 0.6, 0.3, vec(0.91,0.212,0.349,0),0);
    ellipsoid e2 = ellipsoid(vec(-0.7,-0.2,0.3,1),0.1, 0.7, 0.2, vec(0,1,0,0),0);
    e[0] = e1; e[1] = e2;
    
    // triangles
    triangle tr1 = triangle(vec(0,1,-1.5,1),vec(-0.5,0,-1.5,1),vec(0.5,0,-1.5,1), vec(0,0,1,0));  // behind  the camera
    triangle tr2 = triangle(vec(0+0.5,0.7,0.7-0.3,1),vec(-0.3+0.5,0,0.7-0.3,1),vec(0.3+0.5,0,0.7-0.3,1), vec(0,0,1,0));
    //triangle tr2 = triangle(vec(0,1.0,0,1),vec(-0.5,-0.1,0,1),vec(0.5,-0.1,0,1), vec(0,0,1,0));
    tr[0] = tr2; tr[1] = tr1;
    
    //cube
    cube c1 = cube(vec(-0.6,-1.15,0.2,1),0.3,0.3,0.3,vec(0.5,0.7,1,0), 0.0);
    c[0] = c1;
    
    colorPixels.allocate(w, h, OF_PIXELS_RGB);
    
    // ########### Iterate over each pixel to find the ray intersection with the objects ############ //
    
    for (int j = 0; j < h; ++j) {
        for (int i = 0; i < w; ++i) {
            vec pixel_center = first_pixel + (delta_u * i) + (delta_v * j);
            //compute the ray:
            //view_pnt = pixel_center - vec(0,0,1,0);           // $$$$$$$ If you uncomment => parallel projectio  $$$$$$$
            vec ray_direction = pixel_center - view_pnt;
            ray r =  ray(view_pnt, ray_direction);
            vec pixel_color;
            // find the intersection, closest point
            hittable hit = r.ray_intersect(r,q,s,e,tr,c);
            if ((hit.get_n()).w()!= 1){ // if intersection
            //find intersection for shadow
                vec point = hit.get_point();
                ray r2  =  ray(point, light - point);
                bool shadow  = r2.shadowRay_intersect(r2, point,q,s,e,tr,c);
            //vec color = vec(0.8,0.2,0.2,0);
                       if (shadow == true){ // if intersection
                           pixel_color = r.shadow(hit, pixel_center);
            //pixel_color.print();
                       }else{
                           pixel_color = r.shade(hit, pixel_center, r);
                       }
             }
//           else{
//                 pixel_color = r.background(r);
//             }
            colorPixels.setColor(i, j, ofColor(pixel_color.x(),pixel_color.y(),pixel_color.z()));
        }
    }
    texColor.allocate(colorPixels);
    // End of the code stored previously in the set up:
    
    
    end =  clock();
    
    // Calculating total time taken by the program.
    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    std::cout << "\n\nTime taken by program is : " << fixed << time_taken << setprecision(5);
    std::cout << " sec " << endl;
}



//--------------------------------------------------------------
void ofApp::update() {
    //double t = ofGetElapsedTimeMillis();
    matrix cam;
    double pi = M_PI;
    if (camera_animate){
        //std::cout << "animate" << endl;
        view_pnt = cam.RotateY(pi/12)*view_pnt;
        viewDir  = cam.RotateY(pi/12)*viewDir;
        upVector = cam.RotateY(pi/12)*upVector;
    }if (light_animate){
        if (light == vec(0,5,0,1)){
            light = vec(5,5,0,1);
        }
        std::cout << "animate" << endl;
        light = cam.RotateY(pi/12)*light;
    }

    
    if (!Selected && Up){
        std::cout << "up?" << endl;
        view_pnt = cam.translate(0,0.2,0)*view_pnt;
        Up = false;
    }if (!Selected && Down){
        std::cout << "down?" << endl;
        view_pnt = cam.translate(0,-0.2,0)*view_pnt;
        Down = false;
    }if (!Selected && Right){
        std::cout << "right?" << endl;
        view_pnt = cam.translate(0.2,0,0)*view_pnt;
        Right = false;
    }if (!Selected && Left){
        std::cout << "left?" << endl;
        view_pnt = cam.translate(-0.2,0,0)*view_pnt;
        Left = false;
    }if (!Selected && In){
        std::cout << "Dolly?" << endl;
        view_pnt = cam.translate(0,0,0.2)*view_pnt;
        In = false;
    }if (!Selected && Out){
        std::cout << "out?" << endl;
        view_pnt = cam.translate(0,0,-0.2)*view_pnt;
        Out = false;
    }if (!Selected && RotateX){
        std::cout << "rotate?" << endl;
        view_pnt = cam.RotateX(pi/12)*view_pnt;
        viewDir  = cam.RotateX(pi/12)*viewDir;
        upVector = cam.RotateX(pi/12)*upVector;
        RotateX = false;
    }if (!Selected && RotateY){
        std::cout << "rotate?" << endl;
        view_pnt = cam.RotateY(pi/12)*view_pnt;
        viewDir  = cam.RotateY(pi/12)*viewDir;
        upVector = cam.RotateY(pi/12)*upVector;
        RotateY = false;
    }if (!Selected && RotateZ){
        std::cout << "rotate?" << endl;
        view_pnt = cam.RotateZ(pi/12)*view_pnt;
        viewDir  = cam.RotateZ(pi/12)*viewDir;
        upVector = cam.RotateZ(pi/12)*upVector;
        RotateZ = false;
    }
    
    
    binormal = upVector.cross_product(viewDir);
    binormal.normalized();
    viewPlnU = (binormal * width);
    upVector.normalized();
    viewPlnV = (upVector * (-height));
    
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetHexColor(0xffffff);
	texColor.draw(0, 0, w, h);
    
    
    
    matrix cam;
    double pi = M_PI;
    vec delta_u = viewPlnU / w;
    vec delta_v = viewPlnV / h;
    vec up_left = view_pnt + viewDir - (viewPlnU/2) - (viewPlnV/2);
    first_pixel = up_left + ((delta_u + delta_v)* 0.5);
    
    
    hittable hit;
    if(bMousePressed){
        std::cout << "mouse pressed" << endl;
        // Is any object selected?
        vec pixel_center = first_pixel + (delta_u * x) + (delta_v * y);
        vec d = pixel_center - view_pnt;
        ray select = ray(view_pnt, d);
        hit = select.ray_intersect(select,q,s,e,tr,c);
        // Found an object, select it => select mode:
        if (hit.get_t() != -1){
            Selected_obj  = hit.get_clr();
            Selected = true;
            // show it by highlight or change color               ********* in progress *********
        }else{ // Unselect
            Selected = false;
            std::cout << "Unselect" << endl;
        }
    }
    
    if (Selected){
        
                                         /*  **** Transformation Matrices: ****  */
        
        matrix m;
        
        // Translation
        matrix right = m.translate(0.05,0,0);
        matrix left  = m.translate(-0.05,0,0);
        matrix up    = m.translate(0,0.05,0);
        matrix down  = m.translate(0,-0.05,0);
        matrix in    = m.translate(0,0,0.05);
        matrix out    = m.translate(0,0,-0.05);
        
        // Scaling  (add non unoform scalling, maybe asking for an input from user?)  // create the keys for it
        matrix scaleUp_uniform    = m.scale(1.05, 1.05, 1.05);
        matrix scaleDown_uniform  = m.scale(0.95, 0.95, 0.95);
        
        // Rotation // create keys for it
        matrix rotate_x = m.RotateX(pi/12.00);
        matrix rotate_y = m.RotateY(pi/12.00);
        matrix rotate_z = m.RotateZ(pi/12.00);
        
        
        
                                      /*  **** Transforming the Objects: ****  */
        
        // Sphere 1
        if(Selected_obj == vec(1,0,0,0)){
            
            // Translation
            if (Right){
                vec new_center = right*(s[0].get_center());
                s[0].center = new_center;
                Right = false;
            }if (Left){
                vec new_center = left*(s[0].get_center());
                s[0].center = new_center;
                Left = false;
            }if (Up){
                vec new_center = up*(s[0].get_center());
                s[0].center = new_center;
                Up = false;
            }if (Down){
                vec new_center = down*(s[0].get_center());
                s[0].center = new_center;
                Down = false;
            }if (In){
                vec new_center = in*(s[0].get_center());
                s[0].center = new_center;
                In = false;
            }if (Out){
                vec new_center = out*(s[0].get_center());
                s[0].center = new_center;
                Out = false;
            }
            
            // Scaling
            if (ScaleUp){
                float new_radius = (s[0].get_radius())*1.2;
                s[0].radius = new_radius;
                ScaleUp = false;
            }if (ScaleDown){
                float new_radius = (s[0].get_radius())*(0.8);
                s[0].radius = new_radius;
                ScaleDown = false;
            }
            
        }
        // Triangle 1
        if(Selected_obj == vec(0,0,1,0)){
            if (Right){
                tr[0].a = right*(tr[0].get_a());
                tr[0].b = right*(tr[0].get_b());
                tr[0].c = right*(tr[0].get_c());
                Right = false;
            }if (Left){
                tr[0].a = left*(tr[0].get_a());
                tr[0].b = left*(tr[0].get_b());
                tr[0].c = left*(tr[0].get_c());
                Left = false;
            }if (Up){
                tr[0].a = up*(tr[0].get_a());
                tr[0].b = up*(tr[0].get_b());
                tr[0].c = up*(tr[0].get_c());
                Up = false;
            }if (Down){
                tr[0].a = down*(tr[0].get_a());
                tr[0].b = down*(tr[0].get_b());
                tr[0].c = down*(tr[0].get_c());
                Down = false;
            }if (In){
                tr[0].a = in*(tr[0].get_a());
                tr[0].b = in*(tr[0].get_b());
                tr[0].c = in*(tr[0].get_c());
                In = false;
            }if (Out){
                tr[0].a = out*(tr[0].get_a());
                tr[0].b = out*(tr[0].get_b());
                tr[0].c = out*(tr[0].get_c());
                Out = false;
            }if (RotateZ){
                // get center
                vec triCenter = tr[0].TriCenter();
                matrix trans =  m.translate(-triCenter.x(), -triCenter.y(), -triCenter.z());
                matrix inv_trans =  m.translate(triCenter.x(), triCenter.y(), triCenter.z());

                tr[0].a = (inv_trans)*(rotate_z*(trans*(tr[0].get_a())));
                tr[0].b = (inv_trans)*(rotate_z*(trans*(tr[0].get_b())));
                tr[0].c = (inv_trans)*(rotate_z*(trans*(tr[0].get_c())));
                RotateZ = false;
            }if (RotateX){
                vec triCenter = tr[0].TriCenter();
                matrix trans =  m.translate(-triCenter.x(), -triCenter.y(), -triCenter.z());
                matrix inv_trans =  m.translate(triCenter.x(), triCenter.y(), triCenter.z());
                
                tr[0].a = (inv_trans)*(rotate_x*(trans*(tr[0].get_a())));
                tr[0].b = (inv_trans)*(rotate_x*(trans*(tr[0].get_b())));
                tr[0].c = (inv_trans)*(rotate_x*(trans*(tr[0].get_c())));
                RotateX = false;
            }if (RotateY){
                
                vec triCenter = tr[0].TriCenter();
                matrix trans =  m.translate(-triCenter.x(), -triCenter.y(), -triCenter.z());
                matrix inv_trans =  m.translate(triCenter.x(), triCenter.y(), triCenter.z());
                tr[0].a = (inv_trans)*(rotate_y*(trans*(tr[0].get_a())));
                tr[0].b = (inv_trans)*(rotate_y*(trans*(tr[0].get_b())));
                tr[0].c = (inv_trans)*(rotate_y*(trans*(tr[0].get_c())));
                RotateY = false;
            }
            // Scaling
            if (ScaleUp){
                matrix scale = m.scale(1.1, 1.1, 1);
                vec triCenter = tr[0].TriCenter();
                matrix trans =  m.translate(-triCenter.x(), -triCenter.y(), -triCenter.z());
                matrix inv_trans =  m.translate(triCenter.x(), triCenter.y(), triCenter.z());
                tr[0].a = inv_trans*(scale*(trans*(tr[0].get_a())));
                tr[0].b = inv_trans*(scale*(trans*(tr[0].get_b())));
                tr[0].c = inv_trans*(scale*(trans*(tr[0].get_c())));
                ScaleUp = false;
            }if (ScaleDown){
                matrix scale = m.scale(0.9, 0.9, 1);
                vec triCenter = tr[0].TriCenter();
                matrix trans =  m.translate(-triCenter.x(), -triCenter.y(), -triCenter.z());
                matrix inv_trans =  m.translate(triCenter.x(), triCenter.y(), triCenter.z());
                tr[0].a = inv_trans*(scale*(trans*(tr[0].get_a())));
                tr[0].b = inv_trans*(scale*(trans*(tr[0].get_b())));
                tr[0].c = inv_trans*(scale*(trans*(tr[0].get_c())));
                ScaleDown = false;
            }
        }
        // Sphere 2
        if(Selected_obj == vec(0,1,0,0)){
            if (Right){
                vec new_center = right*(s[1].get_center());
                s[1].center = new_center;
                Right = false;
            }if (Left){
                vec new_center = left*(s[1].get_center());
                s[1].center = new_center;
                Left = false;
            }if (Up){
                vec new_center = up*(s[1].get_center());
                s[1].center = new_center;
                Up = false;
            }if (Down){
                vec new_center = down*(s[1].get_center());
                s[1].center = new_center;
                Down = false;
            }if (In){
                vec new_center = in*(s[1].get_center());
                s[1].center = new_center;
                In = false;
            }if (Out){
                vec new_center = out*(s[1].get_center());
                s[1].center = new_center;
                Out = false;
            }
            // Scaling
            if (ScaleUp){
                float new_radius = (s[1].get_radius())*1.2;
                s[1].radius = new_radius;
                ScaleUp = false;
            }if (ScaleDown){
                float new_radius = (s[1].get_radius())*(0.8);
                s[1].radius = new_radius;
                ScaleDown = false;
            }
        }
        // Cube 1
        if(Selected_obj == vec(0.5,0.7,1,0)){
            if (Right){
                vec new_center = right*(c[0].get_origin());
                c[0].origin = new_center;
                Right = false;
            }if (Left){
                vec new_center = left*(c[0].get_origin());
                c[0].origin = new_center;
                Left = false;
            }if (Up){
                vec new_center = up*(c[0].get_origin());
                c[0].origin = new_center;
                Up = false;
            }if (Down){
                vec new_center = down*(c[0].get_origin());
                c[0].origin = new_center;
                Down = false;
            }if (In){
                vec new_center = in*(c[0].get_origin());
                c[0].origin = new_center;
                In = false;
            }if (Out){
                vec new_center = out*(c[0].get_origin());
                c[0].origin = new_center;
                Out = false;
            }
            // Scaling
            if (ScaleUp){
//                vec O = c[0].get_origin();
//                matrix trans =  m.translate(-O.x(), -O.y(), -O.z());
//                matrix inv_trans =  m.translate(O.x(), O.y(), O.z());
                c[0].width *=1.05;
                c[0].height *=1.05;
                c[0].depth *=1.05;
//                c[0].origin = new_origin;
                ScaleUp = false;
            }if (ScaleDown){
                c[0].width *=0.95;
                c[0].height *=0.95;
                c[0].depth *=0.95;
                ScaleDown = false;
            }
            if (RotateZ){
                c[0].theta += pi/12;
                RotateZ = false;
            }
        }
        // Ellipsoid 1
        if(Selected_obj == vec(0.91,0.212,0.349,0)){
            if (Right){
                vec new_center = right*(e[0].get_center());
                e[0].center = new_center;
                Right = false;
            }if (Left){
                vec new_center = left*(e[0].get_center());
                e[0].center = new_center;
                Left = false;
            }if (Up){
                vec new_center = up*(e[0].get_center());
                e[0].center = new_center;
                Up = false;
            }if (Down){
                vec new_center = down*(e[0].get_center());
                e[0].center = new_center;
                Down = false;
            }if (In){
                vec new_center = in*(e[0].get_center());
                e[0].center = new_center;
                In = false;
            }if (Out){
                vec new_center = out*(e[0].get_center());
                e[0].center = new_center;
                Out = false;
            } 
            // Scaling
            if (ScaleUp){
                e[0].a = (e[0].a)*1.1;
                e[0].b = (e[0].b)*1.1;
                e[0].c = (e[0].c)*1.1;
                ScaleUp = false;
            }if (ScaleDown){
                e[0].a = (e[0].a)*0.9;
                e[0].b = (e[0].b)*0.9;
                e[0].c = (e[0].c)*0.9;
                ScaleDown = false;
            }
            
            // Rotation
            if (RotateZ){
                e[0].theta += pi/12;
                RotateZ = false;
            }if (RotateY){
                e[0].theta += pi/12;
                RotateY = false;
            }
        }
    }
    
    // Render:
    colorPixels.allocate(w, h, OF_PIXELS_RGB);
    
    // ########### Iterate over each pixel to find the ray intersection with the objects ############ //
    
    for (int j = 0; j < h; ++j) {
        for (int i = 0; i < w; ++i) {
            vec pixel_center = first_pixel + (delta_u * i) + (delta_v * j);
            //compute the ray:
            //view_pnt = pixel_center - vec(0,0,1,0);           // $$$$$$$ If you uncomment => parallel projection  $$$$$$$
            vec ray_direction = pixel_center - view_pnt;
            ray r =  ray(view_pnt, ray_direction);
            vec pixel_color;
            // find the intersection, closest point
            hittable hit = r.ray_intersect(r,q,s,e,tr,c);
            if ((hit.get_n()).w()!= 1){ // if intersection
            //find intersection for shadow
                vec point = hit.get_point();
                ray r2  =  ray(point, light - point);
                bool shadow  = r2.shadowRay_intersect(r2, point,q,s,e,tr,c);
            //vec color = vec(0.8,0.2,0.2,0);
                       if (shadow == true){ // if intersection
                           pixel_color = r.shadow(hit, pixel_center);
            //pixel_color.print();
                       }else{
                           pixel_color = r.shade(hit, pixel_center, r);
                       }
             }
//           else{
//                 pixel_color = r.background(r);
//             }
            colorPixels.setColor(i, j, ofColor(pixel_color.x(),pixel_color.y(),pixel_color.z()));
        }
    }
    texColor.allocate(colorPixels);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
        std::cout << "selected and key pressed" << endl;
        switch(key) {
            case OF_KEY_RIGHT:
                Right = true;
                std::cout << "Right" << endl;
                break;
            case OF_KEY_LEFT:
                Left = true;
                break;
            case OF_KEY_UP:
                Up = true;
                break;
            case OF_KEY_DOWN:
                Down = true;
                break;
            case 'j':
                In = true;
                break;
            case 'k':
                Out = true;
                break;
            case 'b':
                ScaleUp = true;
                break;
            case 's':
                ScaleDown = true;
                break;
            case 'z':
                RotateZ = true;
                break;
            case 'y':
                RotateY = true;
                break;
            case 'x':
                RotateX = true;
                break;
            case 'a':
                camera_animate = true;
                break;
            case 'l':
                light_animate = true;
                break;
            case 'r':
                view_pnt = vec(0,0,-1,1);
                upVector = vec(0,1,0,0);
                viewDir = vec(0,0,1,0);
                light = vec(0,5,0,1);
                double pi = M_PI;
                // plane
                //q = quad(vec(2,-1.4,3,1), vec(2,-1.4,-1,1),vec(-2,-1.4,-1,1),vec(-2,-1.4,3,1),vec(0.5,0.5,0.5,0));
                q = quad(vec(2,-1.4,2,1), vec(2,-1.4,-1,1),vec(-2,-1.4,-1,1),vec(-2,-1.4,2,1),vec(0.5,0.5,0.5,0));
                
                // spheres
                sphere s1 = sphere(vec(0,-0.8,0.15,1),0.3, vec(1,0,0,0));
                sphere s2 = sphere(vec(0,-0.4,1,1),0.7, vec(0,1,0,0));
                sphere s3 = sphere(light, 0.02, vec(1,1,1,0));
                s[0] = s1; s[1] = s2; s[2] = s3;
                
                // ellipsoids
                ellipsoid e1 = ellipsoid(vec(-1.1,-0.55,0.9-0.3,1),0.3, 0.6, 0.3, vec(0.91,0.212,0.349,0),0);
                ellipsoid e2 = ellipsoid(vec(-0.7,-0.2,0.3,1),0.1, 0.7, 0.2, vec(0,1,0,0),0);
                e[0] = e1; e[1] = e2;
                
                // triangles
                triangle tr1 = triangle(vec(0,1,-1.5,1),vec(-0.5,0,-1.5,1),vec(0.5,0,-1.5,1), vec(0,0,1,0));  // behind  the camera
                triangle tr2 = triangle(vec(0+0.5,0.7,0.7-0.3,1),vec(-0.3+0.5,0,0.7-0.3,1),vec(0.3+0.5,0,0.7-0.3,1), vec(0,0,1,0));
                //triangle tr2 = triangle(vec(0,1.0,0,1),vec(-0.5,-0.1,0,1),vec(0.5,-0.1,0,1), vec(0,0,1,0));
                tr[0] = tr2; tr[1] = tr1;
                
                //cube
                cube c1 = cube(vec(-0.6,-1.15,0.2,1),0.3,0.3,0.3,vec(0.5,0.7,1,0), 0.0);
                c[0] = c1;
                camera_animate = false;
                light_animate = false;
            
                // this resets the camera but not the new positions
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    //Right = Left = Up = Down = In = Out  = false;

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    //save them 
    this ->x = x; this-> y = y;
    std::cout << x << endl;
    bMousePressed = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    bMousePressed = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
