#include <cmath>
#include <vector>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "Torus.h"
using namespace std;

Torus::Torus() {
    prec = 48;
    inner = 0.5f;
    outer = 0.2f;
    initBasic();
}

Torus::Torus(float in, float out, int precIn) {
    prec = precIn;
    inner = in;
    outer = out;
    initBasic();
}

float Torus::toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }
Torus::Torus(float in, float out, int precIn, std::vector<glm::vec3> inputPoints){
    prec=precIn;
    inner=in;
    outer=out;
    inputs=inputPoints;
    inputPointSize=inputPoints.size();
    init();

}
void Torus::initBasic() {
    numVertices = (prec + 1) * (prec + 1);
    numIndices = prec * prec * 6;
    for (int i = 0; i < numVertices; i++) { vertices.push_back(glm::vec3()); }
    for (int i = 0; i < numVertices; i++) { texCoords.push_back(glm::vec2()); }
    for (int i = 0; i < numVertices; i++) { normals.push_back(glm::vec3()); }
    for (int i = 0; i < numVertices; i++) { sTangents.push_back(glm::vec3()); }
    for (int i = 0; i < numVertices; i++) { tTangents.push_back(glm::vec3()); }
    for (int i = 0; i < numIndices; i++) { indices.push_back(0); }

    // calculate first ring
    for (int i = 0; i < prec + 1; i++) {
        float amt = toRadians(i*360.0f / prec);

        glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::vec3 initPos(rMat * glm::vec4(0.0f, outer, 0.0f, 1.0f));

        vertices[i] = glm::vec3(initPos + glm::vec3(inner, 0.0f, 0.0f));
        texCoords[i] = glm::vec2(0.0f, ((float)i / (float)prec));

        rMat = glm::rotate(glm::mat4(1.0f), amt + (3.14159f / 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        tTangents[i] = glm::vec3(rMat * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f));

        sTangents[i] = glm::vec3(glm::vec3(0.0f, 0.0f, -1.0f));
        normals[i] = glm::cross(tTangents[i], sTangents[i]);
    }
    // rotate the first ring about Y to get the other rings
    for (int ring = 1; ring < prec + 1; ring++) {
        for (int i = 0; i < prec + 1; i++) {
            float amt = (float)toRadians((float)ring * 360.0f / (prec));

            glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
            vertices[ring*(prec + 1) + i] = glm::vec3(rMat * glm::vec4(vertices[i], 1.0f));

            texCoords[ring*(prec + 1) + i] = glm::vec2((float)ring*2.0f / (float)prec, texCoords[i].t);

            rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
            sTangents[ring*(prec + 1) + i] = glm::vec3(rMat * glm::vec4(sTangents[i], 1.0f));

            rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
            tTangents[ring*(prec + 1) + i] = glm::vec3(rMat * glm::vec4(tTangents[i], 1.0f));

            rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
            normals[ring*(prec + 1) + i] = glm::vec3(rMat * glm::vec4(normals[i], 1.0f));
        }
    }
    // calculate triangle indices
    for (int ring = 0; ring < prec; ring++) {
        for (int i = 0; i < prec; i++) {
            indices[((ring*prec + i) * 2) * 3 + 0] = ring*(prec + 1) + i;
            indices[((ring*prec + i) * 2) * 3 + 1] = (ring + 1)*(prec + 1) + i;
            indices[((ring*prec + i) * 2) * 3 + 2] = ring*(prec + 1) + i + 1;
            indices[((ring*prec + i) * 2 + 1) * 3 + 0] = ring*(prec + 1) + i + 1;
            indices[((ring*prec + i) * 2 + 1) * 3 + 1] = (ring + 1)*(prec + 1) + i;
            indices[((ring*prec + i) * 2 + 1) * 3 + 2] = (ring + 1)*(prec + 1) + i + 1;
        }
    }
}
void Torus::setupVertices(void) {


    std::vector<float> pvalues;
    std::vector<float> tvalues;
    std::vector<float> nvalues;

    for (int i = 0; i < vertices.size(); i++) {
        pvalues.push_back(vertices[i].x);
        pvalues.push_back(vertices[i].y);
        pvalues.push_back(vertices[i].z);
        tvalues.push_back(texCoords[i].s);
        tvalues.push_back(texCoords[i].t);
        nvalues.push_back(normals[i].x);
        nvalues.push_back(normals[i].y);
        nvalues.push_back(normals[i].z);
    }
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(numVBOs, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 4, &indices[0], GL_STATIC_DRAW);

}
void Torus::addSmoothVert(){
    for (int ring = 1; ring < prec + 1; ring++) {
        for (int i = 0; i < inputPointSize; i++) {
            normals[ring*(inputPointSize)+i]=glm::normalize(
glm::vec3(vertices[ring*inputPointSize+i][1]-vertices[(ring+1)*inputPointSize+i][1],vertices[(ring+1)*(inputPointSize)+i][0]-vertices[ring*inputPointSize+i][0],0.0f)
                +glm::vec3(vertices[(ring-1)*inputPointSize+i][1]-vertices[ring*(inputPointSize)+i][1],vertices[ring*inputPointSize+i][0]-vertices[(ring-1)*inputPointSize+i][0],0.0f));

        }
    }
}
void Torus::addSharpVert(){
    normals.resize(normals.size()*2);
    for (int ring = 1; ring < prec + 1; ring++) {
        for (int i = 0; i < inputPointSize; i++) {
            normals[ring*(inputPointSize)+i]=glm::vec3(vertices[(ring-1)*inputPointSize+i][1]-vertices[ring*(inputPointSize)+i][1],vertices[ring*inputPointSize+i][0]-vertices[(ring-1)*inputPointSize+i][0],0.0f);
            normals[ring*(inputPointSize)+i+1]=glm::vec3(vertices[ring*inputPointSize+i][1]-vertices[(ring+1)*inputPointSize+i][1],vertices[(ring+1)*(inputPointSize)+i][0]-vertices[ring*inputPointSize+i][0],0.0f);

        }
        }
}
void Torus::init() {
    numVertices = (prec + 1) *(inputPointSize+1);
    numIndices = (prec )*4*(inputPointSize);
    for (int i = 0; i < numVertices; i++) { vertices.push_back(glm::vec3()); }
    for (int i = 0; i < numVertices; i++) { texCoords.push_back(glm::vec2()); }
    for (int i = 0; i < numVertices; i++) { normals.push_back(glm::vec3()); }
    for (int i = 0; i < numVertices; i++) { sTangents.push_back(glm::vec3()); }
    for (int i = 0; i < numVertices; i++) { tTangents.push_back(glm::vec3()); }
    for (int i = 0; i < numIndices; i++) { indices.push_back(0); }

    // calculate first ring
   // for (int i = 0; i < prec + 1; i++) {
        for(int j=0;j<inputPointSize;j++){
        //float amt = toRadians(i*360.0f / prec);

        glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
      //  glm::vec3 initPos(rMat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
//initPos +
        vertices[j] = glm::vec3(glm::vec3(inputs[j][0], 0.0f, inputs[j][1]));
        texCoords[j] = glm::vec2(0.0f, ((float)j / ((float)inputPointSize)));

        rMat = glm::rotate(glm::mat4(1.0f), 0 + (3.14159f / 2.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        tTangents[j] = glm::vec3(rMat * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f));

        sTangents[j] = glm::vec3(glm::vec3(0.0f, 0.0f, -1.0f));
        normals[j] = glm::cross(tTangents[j], sTangents[j]);
    }
    //}
    // rotate the first ring about Y to get the other rings
    for (int ring = 1; ring < prec + 1; ring++) {
        for (int i = 0; i < inputPointSize; i++) {

            float amt = -(float)toRadians((float)ring * 360.0f / (prec));

            glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3( 1.0f, 0.0f, 0.0f));
            vertices[ring*(inputPointSize)+i] = glm::vec3(rMat * glm::vec4(vertices[i], 1.0f));

            texCoords[ring*(inputPointSize)+i] = glm::vec2(((float)i) / (((float)prec)), texCoords[i].t);

            rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(1.0f, 0.0f,  0.0f));
            sTangents[ring*(inputPointSize)+i] = glm::vec3(rMat * glm::vec4(sTangents[i], 1.0f));

            rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3( 1.0f,0.0f, 0.0f));
            tTangents[ring*(inputPointSize)+i] = glm::vec3(rMat * glm::vec4(tTangents[i], 1.0f));

            rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(1.0f,0.0f,  0.0f));
            normals[ring*(inputPointSize)+i] = glm::vec3(rMat * glm::vec4(normals[i], 1.0f));
        }
    }
    // calculate triangle indices
    for (int ring = 0; ring < prec; ring++) {

        for (int vert = 0; vert < (inputPointSize-1); vert++) {
           //depending on the winding order, the vertices are set accordingly
            int sel;
            switch(ring%2){
            case 0:
                indices[(ring*(inputPointSize)+vert)*4+0] = vert+ring*(inputPointSize);

                indices[(ring*(inputPointSize)+vert)*4+1] = vert+(ring+1)*(inputPointSize);

                indices[(ring*(inputPointSize)+vert)*4+2] = vert+(ring)*(inputPointSize)+1;

                indices[(ring*(inputPointSize)+vert)*4+3] = vert+(ring+1)*(inputPointSize)+1;


                break;
            case 1:
                indices[(ring*(inputPointSize)+vert)*4+0] = prec-vert+ring*(inputPointSize)-1;

                indices[(ring*(inputPointSize)+vert)*4+1] = vert+(ring+1)*(inputPointSize)-1;

                indices[(ring*(inputPointSize)+vert)*4+2] = vert+(ring)*(inputPointSize)-2;

                indices[(ring*(inputPointSize)+vert)*4+3] = vert+(ring+1)*(inputPointSize)-2;


                break;
            }



        } }

}

int Torus::getNumVertices() { return numVertices; }
int Torus::getNumIndices() { return numIndices; }
std::vector<int> Torus::getIndices() { return indices; }
std::vector<glm::vec3> Torus::getVertices() { return vertices; }
std::vector<glm::vec2> Torus::getTexCoords() { return texCoords; }
std::vector<glm::vec3> Torus::getNormals() { return normals; }
std::vector<glm::vec3> Torus::getStangents() { return sTangents; }
std::vector<glm::vec3> Torus::getTtangents() { return tTangents; }
