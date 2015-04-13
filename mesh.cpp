#include <stdio.h>
#include <string>
#include <cstring>

#include "mesh.h"

using namespace std;

/* given a control patch and (u, v) values, find the surface pt and norm */
void bezpatchinterp(vector<vector< vec3 > > & p,
                    float u, float v,
                    vector<vec3> & out_v,
                    vector<vec3> & out_n,
                    vector<vec3> & temp_rv,
                    vector<vec3> & temp_rn) {
    
}
                    
/* uniform subdivision of a patch */
void uniformSubdivision(vector<vector< vec3 > > & p,
                        vector<vec3> & out_v,
                        vector<vec3> & out_n) {
    float step = 0.1;
    float numdiv = ((1 + 0.000001) / step);
    vector<vector< vec3 > > temp_v;
    vector<vector< vec3 > > temp_n;

    for (float iu = 0; iu < numdiv; iu += step) {
        float u = iu * step;
        
        vector<vec3> temp_rv;
        vector<vec3> temp_rn;
        for (float iv = 0; iv < numdiv; iv += step) {
            float v = iv * step;
            
            bezpatchinterp(p, u, v, out_v, out_n, temp_rv, temp_rn);
        }

        temp_v.push_back(temp_rv);
        temp_n.push_back(temp_rn);
    }
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



bool loadBEZ(char* filename,
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
            
            uniformSubdivision(patch, out_vertices, out_normals);

        }

    }

    return true;
}


bool loadVertices(char* filename,
                  vector<vec3> & out_vertices,
                  vector<vec3> & out_normals) {
    if (strstr(filename, ".obj") != NULL) {
        return loadOBJ(filename, out_vertices, out_normals);
    } else if (strstr(filename, ".bez") != NULL) {
        return loadBEZ(filename, out_vertices, out_normals);
    } else {
        fprintf(stderr, "ERROR: '%s' format not supported\n", filename);
        return false;
    }
}

