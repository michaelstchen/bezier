#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>

#include "mesh.h"

using namespace std;

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
void bezpatchinterp(vector<vector< vec3 > > & p,
                    float u, float v,
                    vector<vec3> & out_v,
                    vector<vec3> & out_n,
                    vector<vec3> & temp_rv,
                    vector<vec3> & temp_rn) {
    vec3 dPdv = vec3(0.0,0.0,0.0);
    vec3 dPdu = vec3(0.0,0.0,0.0);

    vec3 vc0 = bezcurveinterp(p[0][0], p[0][1], p[0][2], p[0][3], u, dPdu);
    vec3 vc1 = bezcurveinterp(p[1][0], p[1][1], p[1][2], p[1][3], u, dPdu);
    vec3 vc2 = bezcurveinterp(p[2][0], p[2][1], p[2][2], p[2][3], u, dPdu);
    vec3 vc3 = bezcurveinterp(p[3][0], p[3][1], p[3][2], p[3][3], u, dPdu);

    vec3 uc0 = bezcurveinterp(p[0][0], p[1][0], p[2][0], p[3][0], v, dPdv);
    vec3 uc1 = bezcurveinterp(p[0][1], p[1][1], p[2][1], p[3][1], v, dPdv);
    vec3 uc2 = bezcurveinterp(p[0][2], p[1][2], p[2][2], p[3][2], v, dPdv);
    vec3 uc3 = bezcurveinterp(p[0][3], p[1][3], p[2][3], p[3][3], v, dPdv);

    vec3 point = bezcurveinterp(vc0, vc1, vc2, vc3, v, dPdv);
    bezcurveinterp(uc0, uc1, uc2, uc3, u, dPdu);
    
    vec3 norm = glm::normalize(glm::cross(dPdu, dPdv));

    temp_rv.push_back(point);
    temp_rn.push_back(norm);
}
                    
/* uniform subdivision of a patch */
void uniformSubdivision(vector<vector< vec3 > > & p, float step,
                        vector<vec3> & out_v,
                        vector<vec3> & out_n) {

    float numdiv = ((1.0f + 0.00001)/ step);
    vector<vector< vec3 > > temp_v;
    vector<vector< vec3 > > temp_n;

    for (float iu = 0; iu < numdiv; iu += 1.0f) {
        float u = iu * step;
        
        vector<vec3> temp_rv;
        vector<vec3> temp_rn;
        for (float iv = 0; iv < numdiv; iv += 1.0f) {
            float v = iv * step;
            
            bezpatchinterp(p, u, v, out_v, out_n, temp_rv, temp_rn);
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


/* adaptive subdivision of a patch */
void adaptiveSubdivision(vector<vector< vec3 > > & p, float param,
                        vector<vec3> & out_v,
                        vector<vec3> & out_n) {
    
}


bool loadBEZ(char* filename, float param, char* a,
             vector<vec3> & out_vertices,
             vector<vec3> & out_normals) {

    FILE* finput = fopen(filename, "r");
    if (finput == NULL) {
        fprintf(stderr, "ERROR: cannot open obj file '%s'\n", filename);
        return false;
    }

    vector<vector< vec3 > > patch;
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
                adaptiveSubdivision(patch, param, out_vertices, out_normals);
            } else {
                uniformSubdivision(patch, param, out_vertices, out_normals);
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
        float param = (float) atof(argv[2]);

        char* a;
        (argc > 3) ? a = argv[3] : a = NULL;

        return loadBEZ(filename, param, a, out_vertices, out_normals);
    } else {
        fprintf(stderr, "ERROR: '%s' format not supported\n", filename);
        return false;
    }
}

