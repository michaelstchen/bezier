#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>

#include "mesh.h"

using namespace std;

vector<vector< vec3 > > patch;
float param;
mat4 Bz;

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
    vec3 dPdv; vec3 dPdu;

    vec3 vc0 = bezcurveinterp(patch[0][0], patch[0][1],
                              patch[0][2], patch[0][3], u, dPdu);
    vec3 vc1 = bezcurveinterp(patch[1][0], patch[1][1],
                              patch[1][2], patch[1][3], u, dPdu);
    vec3 vc2 = bezcurveinterp(patch[2][0], patch[2][1],
                              patch[2][2], patch[2][3], u, dPdu);
    vec3 vc3 = bezcurveinterp(patch[3][0], patch[3][1],
                              patch[3][2], patch[3][3], u, dPdu);

    vec3 uc0 = bezcurveinterp(patch[0][0], patch[1][0],
                              patch[2][0], patch[3][0], v, dPdv);
    vec3 uc1 = bezcurveinterp(patch[0][1], patch[1][1],
                              patch[2][1], patch[3][1], v, dPdv);
    vec3 uc2 = bezcurveinterp(patch[0][2], patch[1][2],
                              patch[2][2], patch[3][2], v, dPdv);
    vec3 uc3 = bezcurveinterp(patch[0][3], patch[1][3],
                              patch[2][3], patch[3][3], v, dPdv);

    ret_v = bezcurveinterp(vc0, vc1, vc2, vc3, v, dPdv);
    bezcurveinterp(uc0, uc1, uc2, uc3, u, dPdu);
    
    glm::normalize(dPdv);
    glm::normalize(dPdu);

    ret_n = glm::cross(dPdu, dPdv);

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


bool samePoint(vec3 & v0, vec3 & v1) {
    if (v0.x == v1.x) {
        if (v0.y == v1.y) {
            if (v0.z == v1.z) {
                return true;
            }
        }
    }

    return false;
}

void checkTriangle(vec3 a, vec3 b, vec3 c,
                   float u_vals[], float v_vals[], int depth,
                   vector<vec3> & out_v, vector<vec3> & out_n) {

    vec3 midAB = (a + b) / 2.0f;
    vec3 midBC = (b + c) / 2.0f;
    vec3 midCA = (c + a) / 2.0f;

    float midAB_u = (u_vals[0] + u_vals[1]) / 2.0f;
    float midAB_v = (v_vals[0] + v_vals[1]) / 2.0f;
    vec3 bezmidAB; vec3 beznormAB;
    bezpatchinterp(midAB_u, midAB_v, bezmidAB, beznormAB);

    float midBC_u = (u_vals[1] + u_vals[2]) / 2.0f;
    float midBC_v = (v_vals[1] + v_vals[2]) / 2.0f;
    vec3 bezmidBC; vec3 beznormBC;
    bezpatchinterp(midBC_u, midBC_v, bezmidBC, beznormBC);

    float midCA_u = (u_vals[2] + u_vals[0]) / 2.0f;
    float midCA_v = (v_vals[2] + v_vals[0]) / 2.0f;
    vec3 bezmidCA; vec3 beznormCA;
    bezpatchinterp(midCA_u, midCA_v, bezmidCA, beznormCA );

    bool splitAB; bool splitBC; bool splitCA;
    if (depth < 0) {
        //printf("depth: %d\n", depth);
        splitAB = false;
        splitBC = false;
        splitCA = false;
    } else {
        depth -= 1;
        /*
        splitAB = true;
        splitBC = false;
        splitCA = false;
        */
        splitAB = glm::length(midAB - bezmidAB) > param;
        //printf("midAB: %f %f %f\tbezmidAB: %f %f %f\n", midAB.x, midAB.y, midAB.z, bezmidAB.x, bezmidAB.y, bezmidAB.z);
        splitBC = glm::length(midBC - bezmidBC) > param;
        //printf("midBC: %f %f %f\tbezmidBC: %f %f %f\n", midBC.x, midBC.y, midBC.z, bezmidBC.x, bezmidBC.y, bezmidBC.z);
        splitCA = glm::length(midCA - bezmidCA) > param;
        //printf("midCA: %f %f %f\tbezmidCA: %f %f %f\n", midCA.x, midCA.y, midCA.z, bezmidCA.x, bezmidCA.y, bezmidCA.z);
    }

    

    if (splitAB && splitBC && splitCA){
        //printf("split AB, BC, CA\n\n");
        float u_1[3] = {u_vals[0], midAB_u, midCA_u};
        float v_1[3] = {v_vals[0], midAB_v, midCA_v};
        checkTriangle(a, bezmidAB, bezmidCA, u_1, v_1, depth,
                      out_v, out_n);

        float u_2[3] = {u_vals[1], midBC_u, midAB_u};
        float v_2[3] = {v_vals[1], midBC_v, midAB_v};
        checkTriangle(b, bezmidBC, bezmidAB, u_2, v_2, depth,
                      out_v, out_n);

        float u_3[3] = {u_vals[2], midCA_u, midBC_u};
        float v_3[3] = {v_vals[2], midCA_v, midBC_v};
        checkTriangle(c, bezmidCA, bezmidBC, u_3, v_3, depth,
                      out_v, out_n);

        float u_4[3] = {midAB_u, midBC_u, midCA_u};
        float v_4[3] = {midAB_v, midBC_v, midCA_v};
        checkTriangle(bezmidAB, bezmidBC, bezmidCA, u_4, v_4, depth,
                      out_v, out_n);
        
    } else if (splitAB && splitBC) {
        //printf("split AB, BC\n\n");
        float u_1[3] = {u_vals[1], midBC_u, midAB_u};
        float v_1[3] = {v_vals[1], midBC_v, midAB_v};
        checkTriangle(b, bezmidBC, bezmidAB, u_1, v_1, depth,
                      out_v, out_n);

        float u_2[3] = {u_vals[2], midAB_u, midBC_u};
        float v_2[3] = {v_vals[2], midAB_v, midBC_v};
        checkTriangle(c, bezmidAB, bezmidBC, u_2, v_2, depth,
                      out_v, out_n);

        float u_3[3] = {u_vals[0], midAB_u, u_vals[2]};
        float v_3[3] = {v_vals[0], midAB_v, v_vals[2]};
        checkTriangle(a, bezmidAB, c, u_3, v_3, depth,
                      out_v, out_n);

    } else if (splitBC && splitCA) {
        //printf("split BC, CA\n\n");
        float u_1[3] = {u_vals[2], midCA_u, midBC_u};
        float v_1[3] = {v_vals[2], midCA_v, midBC_v};
        checkTriangle(c, bezmidCA, bezmidBC, u_1, v_1, depth,
                      out_v, out_n);

        float u_2[3] = {u_vals[0], midBC_u, midCA_u};
        float v_2[3] = {v_vals[0], midBC_v, midCA_v};
        checkTriangle(a, bezmidBC, bezmidCA, u_2, v_2, depth,
                      out_v, out_n);

        float u_3[3] = {u_vals[1], midBC_u, u_vals[0]};
        float v_3[3] = {v_vals[1], midBC_v, v_vals[0]};
        checkTriangle(b, bezmidBC, a, u_3, v_3, depth,
                      out_v, out_n);

    } else if (splitCA && splitAB) {
        //printf("split AB, CA\n\n");
        float u_1[3] = {u_vals[0], midAB_u, midCA_u};
        float v_1[3] = {v_vals[0], midAB_v, midCA_v};
        checkTriangle(a, bezmidAB, bezmidCA, u_1, v_1, depth,
                      out_v, out_n);

        float u_2[3] = {u_vals[1], midCA_u, midAB_u};
        float v_2[3] = {v_vals[1], midCA_v, midAB_v};
        checkTriangle(b, bezmidCA, bezmidAB, u_2, v_2, depth,
                      out_v, out_n);

        float u_3[3] = {u_vals[2], midCA_u, u_vals[1]};
        float v_3[3] = {v_vals[2], midCA_v, v_vals[1]};
        checkTriangle(c, bezmidCA, b, u_3, v_3, depth,
                      out_v, out_n);
        
    } else if (splitAB) {
        //printf("split AB\n\n");
        float u_1[3] = {u_vals[0], midAB_u, u_vals[2]};
        float v_1[3] = {v_vals[0], midAB_v, v_vals[2]};
        checkTriangle(a, bezmidAB, c, u_1, v_1, depth,
                      out_v, out_n);

        float u_2[3] = {u_vals[1], u_vals[2], midAB_u};
        float v_2[3] = {v_vals[1], v_vals[2], midAB_v};
        checkTriangle(b, c, bezmidAB, u_2, v_2, depth,
                      out_v, out_n);

    } else if (splitBC) {
        //printf("split BC\n\n");
        float u_1[3] = {u_vals[1], midBC_u, u_vals[0]};
        float v_1[3] = {v_vals[1], midBC_v, v_vals[0]};
        checkTriangle(b, bezmidBC, a, u_1, v_1, depth,
                      out_v, out_n);

        float u_2[3] = {u_vals[2], u_vals[0], midBC_u};
        float v_2[3] = {v_vals[2], v_vals[0], midBC_v};
        checkTriangle(c, a, bezmidBC, u_2, v_2, depth,
                      out_v, out_n);

    } else if (splitCA) {
        //printf("split CA\n\n");
        float u_1[3] = {u_vals[2], midCA_u, u_vals[1]};
        float v_1[3] = {v_vals[2], midCA_v, v_vals[1]};
        checkTriangle(c, bezmidCA, b, u_1, v_1, depth,
                      out_v, out_n);

        float u_2[3] = {u_vals[0], v_vals[1], midCA_u};
        float v_2[3] = {v_vals[0], v_vals[1], midCA_v};
        checkTriangle(a, b, bezmidCA, u_2, v_2, depth,
                      out_v, out_n);

    } else {
        vec3 temp;
        vec3 a_norm; vec3 b_norm; vec3 c_norm;
        //printf("NO SPLIT\n\n");        
        bezpatchinterp(u_vals[0], v_vals[0], temp, a_norm);
        bezpatchinterp(u_vals[1], v_vals[1], temp, b_norm);
        bezpatchinterp(u_vals[2], v_vals[2], temp, c_norm);
        

        out_v.push_back(a);
        out_v.push_back(b);
        out_v.push_back(c);

        out_n.push_back(a_norm);
        out_n.push_back(b_norm);
        out_n.push_back(c_norm);
        
    }

}


/* adaptive subdivision of a patch */
void adaptiveSubdivision(vector<vec3> & out_v, vector<vec3> & out_n) {

    float v_vals1[3] = {0, 0, 1};
    float u_vals1[3] = {0, 1, 0};
    checkTriangle(patch[0][0], patch[0][3], patch[3][0],
                  u_vals1, v_vals1, 5,
                  out_v, out_n);

    float v_vals2[3] = {0, 1, 1};
    float u_vals2[3] = {1, 1, 0};
    checkTriangle(patch[0][3], patch[3][3], patch[3][0],
                  u_vals2, v_vals2, 5,
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

