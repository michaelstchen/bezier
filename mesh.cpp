#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <cmath>

#include "mesh.h"

using namespace std;

vector<vector< vec3 > > patch;
float param;
int rec_depth = 10;

Triangle::Triangle(Point* p0, Point* p1, Point* p2) {
    a = p0; b = p1; c = p2;
}

Point::Point(float x_in, float y_in, float z_in, float u_in, float v_in) {
    x = x_in; y = y_in; z = z_in;
    u = u_in; v = v_in;
}

Point midPoint(Point & p0, Point & p1) {
    return Point((p0.x + p1.x) / 2.0f,
                 (p0.y + p1.y) / 2.0f,
                 (p0.z + p1.z) / 2.0f,
                 (p0.u + p1.u) / 2.0f, (p0.v + p1.v) / 2.0f);
}

float distance_pts(Point & p0, Point & p1) {
    return sqrt((p1.x - p0.x) * (p1.x - p0.x)
                + (p1.y - p0.y) * (p1.y - p0.y)
                + (p1.z - p0.z) * (p1.z - p0.z)); 
}

Point vec3topoint(vec3 & in_vec, float u, float v) {
    return Point(in_vec.x, in_vec.y, in_vec.z, u, v);
}

/* bezier curve interpretation using De Casteljau's algorithm */
vec3 bezcurveinterp(vec3 c0, vec3 c1, vec3 c2, vec3 c3, float u, vec3 & dPdu) {
    vec3 A = c0 * (1.0f - u) + c1 * u;
    vec3 B = c1 * (1.0f - u) + c2 * u;
    vec3 C = c2 * (1.0f - u) + c3 * u;

    vec3 D = A * (1.0f -u) + B * u;
    vec3 E = B * (1.0f -u) + C * u;

    dPdu = 3.0f * (E - D);

    return D * (1.0f - u) + E * u;
}

/* given a control patch and (u, v) values, find the surface pt and norm */
void bezpatchinterp(float u, float v,
                    vec3 & ret_v,
                    vec3 & ret_n) {
    vec3 dPdv; vec3 dPdu; vec3 temp;

    vec3 vc0 = bezcurveinterp(patch[0][0], patch[0][1],
                              patch[0][2], patch[0][3], u, temp);
    vec3 vc1 = bezcurveinterp(patch[1][0], patch[1][1],
                              patch[1][2], patch[1][3], u, temp);
    vec3 vc2 = bezcurveinterp(patch[2][0], patch[2][1],
                              patch[2][2], patch[2][3], u, temp);
    vec3 vc3 = bezcurveinterp(patch[3][0], patch[3][1],
                              patch[3][2], patch[3][3], u, temp);

    vec3 uc0 = bezcurveinterp(patch[0][0], patch[1][0],
                              patch[2][0], patch[3][0], v, temp);
    vec3 uc1 = bezcurveinterp(patch[0][1], patch[1][1],
                              patch[2][1], patch[3][1], v, temp);
    vec3 uc2 = bezcurveinterp(patch[0][2], patch[1][2],
                              patch[2][2], patch[3][2], v, temp);
    vec3 uc3 = bezcurveinterp(patch[0][3], patch[1][3],
                              patch[2][3], patch[3][3], v, temp);

    ret_v = bezcurveinterp(vc0, vc1, vc2, vc3, v, dPdv);
    bezcurveinterp(uc0, uc1, uc2, uc3, u, dPdu);
    
    if (dPdv.x == 0 && dPdv.y == 0 && dPdv.z == 0) {
        printf("BEFORE dPdv equals 0\n");
    } else if (dPdu.x == 0 && dPdu.y == 0 && dPdu.z == 0) {
        printf("BEFORE dPdu equals 0\n");
    }

    if (dPdu.x == 0 && dPdu.y == 0 && dPdu.z == 0) {
        
        vec3 vc0_0 = bezcurveinterp(patch[0][0], patch[0][1],
                                  patch[0][2], patch[0][3], 0.0, temp);
        vec3 vc1_0 = bezcurveinterp(patch[1][0], patch[1][1],
                                  patch[1][2], patch[1][3], 0.0, temp);
        vec3 vc2_0 = bezcurveinterp(patch[2][0], patch[2][1],
                                  patch[2][2], patch[2][3], 0.0, temp);
        vec3 vc3_0 = bezcurveinterp(patch[3][0], patch[3][1],
                                  patch[3][2], patch[3][3], 0.0, temp);

        vec3 vc0_1 = bezcurveinterp(patch[0][0], patch[0][1],
                                  patch[0][2], patch[0][3], 1.0, temp);
        vec3 vc1_1 = bezcurveinterp(patch[1][0], patch[1][1],
                                  patch[1][2], patch[1][3], 1.0, temp);
        vec3 vc2_1 = bezcurveinterp(patch[2][0], patch[2][1],
                                  patch[2][2], patch[2][3], 1.0, temp);
        vec3 vc3_1 = bezcurveinterp(patch[3][0], patch[3][1],
                                  patch[3][2], patch[3][3], 1.0, temp);

        bezcurveinterp(vc0_0, vc1_0, vc2_0, vc3_0, v, dPdv);
        bezcurveinterp(vc0_1, vc1_1, vc2_1, vc3_1, v, dPdu);
        
    } else if (dPdv.x == 0 && dPdv.y == 0 && dPdv.z == 0) {
        
        vec3 uc0_0 = bezcurveinterp(patch[0][0], patch[1][0],
                                  patch[2][0], patch[3][0], 0.0, temp);
        vec3 uc1_0 = bezcurveinterp(patch[0][1], patch[1][1],
                                  patch[2][1], patch[3][1], 0.0, temp);
        vec3 uc2_0 = bezcurveinterp(patch[0][2], patch[1][2],
                                  patch[2][2], patch[3][2], 0.0, temp);
        vec3 uc3_0 = bezcurveinterp(patch[0][3], patch[1][3],
                                  patch[2][3], patch[3][3], 0.0, temp);

        vec3 uc0_1 = bezcurveinterp(patch[0][0], patch[1][0],
                                  patch[2][0], patch[3][0], 1.0, temp);
        vec3 uc1_1 = bezcurveinterp(patch[0][1], patch[1][1],
                                  patch[2][1], patch[3][1], 1.0, temp);
        vec3 uc2_1 = bezcurveinterp(patch[0][2], patch[1][2],
                                  patch[2][2], patch[3][2], 1.0, temp);
        vec3 uc3_1 = bezcurveinterp(patch[0][3], patch[1][3],
                                  patch[2][3], patch[3][3], 1.0, temp);

        bezcurveinterp(uc0_0, uc1_0, uc2_0, uc3_0, u, dPdu);
        bezcurveinterp(uc0_1, uc1_1, uc2_1, uc3_1, u, dPdv);
        
    }
    
    glm::normalize(dPdv);
    glm::normalize(dPdu);

    //ret_n = glm::cross(dPdu, dPdv);
    ret_n = glm::normalize(glm::cross(dPdu, dPdv));
    
}
                    
/* uniform subdivision of a patch */
void uniformSubdivision(vector<vec3> & out_v, vector<vec3> & out_n) {

    float step = param;

    float numdiv = ((1.0f + 0.00001)/ step);
    vector<vector< vec3 > > temp_v;
    vector<vector< vec3 > > temp_n;

    for (float iu = 0; iu < numdiv; iu += 1.0f) {
        
        float u = iu * step;
        if (numdiv - iu < 1.0) u = 1.0f;
        
        vector<vec3> temp_rv;
        vector<vec3> temp_rn;
        for (float iv = 0; iv < numdiv; iv += 1.0f) {
            float v = iv * step;
            if (numdiv - iv < 1.0) v = 1.0f;
            vec3 vector; vec3 normal;
            bezpatchinterp(u, v, vector, normal);
            temp_rv.push_back(vector);
            temp_rn.push_back(normal);
        }

        temp_v.push_back(temp_rv);
        temp_n.push_back(temp_rn);

    }

    for (int i = 0; i < temp_v.size() - 1; i++) {
        for (int j = 0; j < temp_v[i].size() - 1; j++) {
            
            out_v.push_back(temp_v[i][j]);
            out_v.push_back(temp_v[i][j+1]);
            out_v.push_back(temp_v[i+1][j]);

            out_v.push_back(temp_v[i][j+1]);
            out_v.push_back(temp_v[i+1][j+1]);
            out_v.push_back(temp_v[i+1][j]);

            out_n.push_back(temp_n[i][j]);
            out_n.push_back(temp_n[i][j+1]);
            out_n.push_back(temp_n[i+1][j]);

            out_n.push_back(temp_n[i][j+1]);
            out_n.push_back(temp_n[i+1][j+1]);
            out_n.push_back(temp_n[i+1][j]);
            
        }
    }
}


void checkTriangle(Point & a, Point & b, Point & c, int depth,
                   vector<vec3> & out_v, vector<vec3> & out_n) {
    Point midAB = midPoint(a, b);
    Point midBC = midPoint(b, c);
    Point midCA = midPoint(c, a);

    vec3 bezmidAB_t; vec3 beznormAB_t;
    bezpatchinterp(midAB.u, midAB.v, bezmidAB_t, beznormAB_t);
    Point bezmidAB = vec3topoint(bezmidAB_t, midAB.u, midAB.v); 

    vec3 bezmidBC_t; vec3 beznormBC_t;
    bezpatchinterp(midBC.u, midBC.v, bezmidBC_t, beznormBC_t);
    Point bezmidBC = vec3topoint(bezmidBC_t, midBC.u, midBC.v); 

    vec3 bezmidCA_t; vec3 beznormCA_t;
    bezpatchinterp(midCA.u, midCA.v, bezmidCA_t, beznormCA_t);
    Point bezmidCA = vec3topoint(bezmidCA_t, midCA.u, midCA.v); 

    bool splitAB; bool splitBC; bool splitCA;
    if (depth < 0) {
        splitAB = false; splitBC = false; splitCA = false;
    } else {
        depth -= 1;
        splitAB = distance_pts(midAB, bezmidAB) > param;
        splitBC = distance_pts(midBC, bezmidBC) > param;
        splitCA = distance_pts(midCA, bezmidCA) > param;

    }

    if (!splitAB && !splitBC && !splitCA) {
        printf("adding triangles\n");
        vec3 a_vert; vec3 b_vert; vec3 c_vert;
        vec3 a_norm; vec3 b_norm; vec3 c_norm;

        bezpatchinterp(a.u, a.v, a_vert, a_norm);
        printf("a_norm: %f %f %f\n", a_norm.x, a_norm.y, a_norm.z);
        bezpatchinterp(b.u, b.v, b_vert, b_norm);
        printf("b_norm: %f %f %f\n", b_norm.x, b_norm.y, b_norm.z);
        bezpatchinterp(c.u, c.v, c_vert, c_norm);
        printf("c_norm: %f %f %f\n\n", c_norm.x, c_norm.y, c_norm.z);
        
        out_v.push_back(a_vert);
        out_v.push_back(b_vert);
        out_v.push_back(c_vert);

        out_n.push_back(a_norm);
        out_n.push_back(b_norm);
        out_n.push_back(c_norm);

    } else if (splitAB && !splitBC && !splitCA) {
        checkTriangle(a, bezmidAB, c, depth, out_v, out_n);
        checkTriangle(bezmidAB, b, c, depth, out_v, out_n);

    } else if(!splitAB && splitBC && !splitCA) {
        checkTriangle(a, b, bezmidBC, depth, out_v, out_n);
        checkTriangle(a, bezmidBC, c, depth, out_v, out_n);

    } else if (!splitAB && !splitBC && splitCA) {
        checkTriangle(a, b, bezmidCA, depth, out_v, out_n);
        checkTriangle(b, c, bezmidCA, depth, out_v, out_n);

    } else if (splitAB && splitBC && !splitCA) {
        checkTriangle(b, bezmidBC, bezmidAB, depth, out_v, out_n);
        checkTriangle(c, a, bezmidBC, depth, out_v, out_n);
        checkTriangle(a, bezmidAB, bezmidBC, depth, out_v, out_n);       

    } else if (!splitAB && splitBC && splitCA) {
        checkTriangle(a, b, bezmidCA, depth, out_v, out_n);
        checkTriangle(bezmidCA, b, bezmidBC, depth, out_v, out_n);
        checkTriangle(bezmidCA, bezmidBC, c, depth, out_v, out_n);        
        
    } else if (splitAB && !splitBC && splitCA) {
        checkTriangle(a, bezmidAB, bezmidCA, depth, out_v, out_n);
        checkTriangle(bezmidAB, b, c, depth, out_v, out_n);
        checkTriangle(c, bezmidCA, bezmidAB, depth, out_v, out_n);

    } else if (splitAB && splitBC && splitCA) {
        checkTriangle(a, bezmidAB, bezmidCA, depth, out_v, out_n);
        checkTriangle(b, bezmidBC, bezmidAB, depth, out_v, out_n);
        checkTriangle(c, bezmidCA, bezmidBC, depth, out_v, out_n);
        checkTriangle(bezmidAB, bezmidBC, bezmidCA, depth, out_v, out_n);

    }

}


/* adaptive subdivision of a patch */
void adaptiveSubdivision(vector<vec3> & out_v, vector<vec3> & out_n) {

    Point a = Point(patch[0][0].x, patch[0][0].y, patch[0][0].z, 0, 0);
    Point b = Point(patch[3][0].x, patch[3][0].y, patch[3][0].z, 0, 1);
    Point c = Point(patch[0][3].x, patch[0][3].y, patch[0][3].z, 1, 0);
    Point d = Point(patch[3][3].x, patch[3][3].y, patch[3][3].z, 1, 1);
    checkTriangle(a, b, c, rec_depth,
                  out_v, out_n);

    checkTriangle(b, d, c, rec_depth,
                  out_v, out_n);
}


bool loadBEZ(char* filename, char* a,
             vector<vec3> & out_vertices,
             vector<vec3> & out_normals) {

    FILE* finput = fopen(filename, "r");
    if (finput == NULL) {
        fprintf(stderr, "ERROR: cannot open obj file '%s'\n", filename);
        return false;
    }

    for (int i = 0; i < 4; i++) {
        vector<vec3> row;
        for (int j = 0; j < 4; j++) {
            row.push_back(vec3(0, 0, 0));
        }
        patch.push_back(row);
    }

    int numPatches = 0;
    while(fscanf(finput, "%d\n", &numPatches) != EOF) {
        for (int n = 0; n < numPatches; n++) {
            for (int i = 0; i < 4; i++) {
                fscanf(finput,
                       "%f %f %f  %f %f %f  %f %f %f  %f %f %f\n",
                       &patch[i][0].x, &patch[i][0].y, &patch[i][0].z,
                       &patch[i][1].x, &patch[i][1].y, &patch[i][1].z,
                       &patch[i][2].x, &patch[i][2].y, &patch[i][2].z,
                       &patch[i][3].x, &patch[i][3].y, &patch[i][3].z);
            }

            if (a != NULL && strcmp(a, "-a") == 0) {
                adaptiveSubdivision(out_vertices, out_normals);
            } else {
                uniformSubdivision(out_vertices, out_normals);
            }

        }

    }

    return true;
}


bool loadOBJ(char* filename,
             vector<vec3> & out_vertices,
             vector<vec3> & out_normals) {
    
    FILE* finput = fopen(filename, "r");
    if (finput == NULL) {
        fprintf(stderr, "ERROR: cannot open obj file '%s'\n", filename);
        return false;
    }

    vector< vec3 > temp_v;

    char line[100];
    while(fscanf(finput, "%s", line) != EOF) {

        if (strcmp(line, "v") == 0 ){
            vec3 vertex;
            fscanf(finput, "%f %f %f\n",
                   &vertex.x, &vertex.y, &vertex.z );
            temp_v.push_back(vertex);
        } else if (strcmp(line, "f") == 0 ){
            int vInd[3];
            int matches = fscanf(finput, "%d %d %d\n",
                                 &vInd[0], &vInd[1], &vInd[2]);

            vec3 facenorm = glm::normalize(glm::cross(temp_v[vInd[1] - 1] - temp_v[vInd[0] - 1], temp_v[vInd[2] - 1] - temp_v[vInd[0] - 1]));

            for (int i = 0; i < 3; i++) {
                out_vertices.push_back(temp_v[vInd[i] - 1]);
                out_normals.push_back(facenorm);
            }
            
        } else {
            fprintf(stderr, "ERROR: reading obj line '%s'\n", line);
        }
    }

    return true;
}



bool loadVertices(int argc, char** argv,
                  vector<vec3> & out_vertices,
                  vector<vec3> & out_normals) {
    char* filename = argv[1];

    if (strstr(filename, ".obj") != NULL) {
        return loadOBJ(filename, out_vertices, out_normals);
    } else if (strstr(filename, ".bez") != NULL) {
        param = (float) atof(argv[2]);

        char* a;
        (argc > 3) ? a = argv[3] : a = NULL;

        return loadBEZ(filename, a, out_vertices, out_normals);
    } else {
        fprintf(stderr, "ERROR: '%s' format not supported\n", filename);
        return false;
    }
}

