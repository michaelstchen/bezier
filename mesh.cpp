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
    std::vector<vec3*> temp_n;

    char line[100];
    while(fscanf(finput, "%s", line) != EOF) {

        if (strcmp(line, "v") == 0 ){
            vec3 vertex;
            fscanf(finput, "%f %f %f\n",
                   &vertex.x, &vertex.y, &vertex.z );
            temp_v.push_back(vertex);
            temp_n.push_back(new vec3(0.0,0.0,0.0));
        } else if (strcmp(line, "f") == 0 ){
            int vInd[3];
            int matches = fscanf(finput, "%d %d %d\n",
                                 &vInd[0], &vInd[1], &vInd[2]);
            for (int i = 0; i < 3; i++) {
                out_vertices.push_back(temp_v[vInd[i] - 1]);
                out_normals.push_back(*temp_n[vInd[i] - 1]);
            }

            vec3 facenorm = glm::normalize(glm::cross(temp_v[vInd[1] - 1] - temp_v[vInd[0] - 1], temp_v[vInd[2] - 1] - temp_v[vInd[0] - 1]));

            float angle0 = glm::angle(glm::normalize(temp_v[vInd[1] - 1] - temp_v[vInd[0] - 1]), 
                                      glm::normalize(temp_v[vInd[2] - 1] - temp_v[vInd[0] - 1]));
            float angle1 = glm::angle(glm::normalize(temp_v[vInd[2] - 1] - temp_v[vInd[1] - 1]),
                                      glm::normalize(temp_v[vInd[0] - 1] - temp_v[vInd[1] - 1]));
            float angle2 = PI - angle0 - angle1;

            *temp_n[vInd[0] - 1] = *temp_n[vInd[0] - 1] + facenorm * (angle0 / (2 * PI));
            *temp_n[vInd[0] - 1] = *temp_n[vInd[0] - 1] + facenorm * (angle1 / (2 * PI));
            *temp_n[vInd[0] - 1] = *temp_n[vInd[0] - 1] + facenorm * (angle2 / (2 * PI));
            

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

