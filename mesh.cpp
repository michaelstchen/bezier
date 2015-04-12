#include <stdio.h>
#include <string>
#include <cstring>

#include "mesh.h"


bool loadOBJ(char* filename,
                  std::vector<glm::vec3> & out_vertices,
                  std::vector<glm::vec3> & out_normals) {

    FILE* finput = fopen(filename, "r");
    if (finput == NULL) {
        fprintf(stderr, "ERROR: cannot open obj file '%s'\n", filename);
        return false;
    }

    std::vector< vec3 > temp_v;

    char line[100];
    while(fscanf(finput, "%s", line) != EOF) {

        if (strcmp(line, "v") == 0 ){
            glm::vec3 vertex;
            fscanf(finput, "%f %f %f\n",
                   &vertex.x, &vertex.y, &vertex.z );
            temp_v.push_back(vertex);
        } else if (strcmp(line, "f") == 0 ){
            int vInd[3];
            int matches = fscanf(finput, "%d %d %d\n",
                                 &vInd[0], &vInd[1], &vInd[2]);
            for (int i = 0; i < 3; i++) {
                out_vertices.push_back(temp_v[vInd[i] - 1]);
            }

        } else {
            fprintf(stderr, "ERROR: reading obj line '%s'\n", line);
        }
    }

    return true;
}



bool loadBEZ(char* filename,
             std::vector<glm::vec3> & out_vertices,
             std::vector<glm::vec3> & out_normals) {
    return false;
}


bool loadVertices(char* filename,
                  std::vector<glm::vec3> & out_vertices,
                  std::vector<glm::vec3> & out_normals) {
    if (strstr(filename, ".obj") != NULL) {
        return loadOBJ(filename, out_vertices, out_normals);
    } else if (strstr(filename, ".bez") != NULL) {
        return loadOBJ(filename, out_vertices, out_normals);
    } else {
        fprintf(stderr, "ERROR: '%s' format not supported\n", filename);
        return false;
    }
}

