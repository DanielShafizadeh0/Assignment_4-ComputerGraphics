#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2/SOIL2.h>
#include <stack>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtx/quaternion.hpp>
#include "Torus.h"
#include "Utils.h"
using namespace std;

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1
#define numVBOs 4
std::stack<glm::mat4> mdlViewStck;
float cameraX, cameraY, cameraZ;
float torLocX, torLocY, torLocZ;
float arrLocX, arrLocY, arrLocZ;
GLuint renderingProgram, cubeRenderer;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint brickTexture, metalTexture, paintTexture, skyBoxTexture,stoneTexture;
float rotAmt = 0.0f;
bool mouseHeld = false;
bool rMouseHeld=false;
int initMouseX, initMouseY=0;
// variable allocation for display
GLuint mvLoc, projLoc, vLoc;
int width, height;
float aspect;
float elev,azim,baseElev,baseAzim=0.0f;
float scale=0.001f;
float baseScale= 0.001f;
float xang=0.0f;
glm::mat4 pMat, vMat, mMat, mvMat,M, transMat, rotMat, v1Mat, v2Mat, normMat;
int j=0;
float l,r,t,b,f,n=0.0f;
//lightstuff
float globalAmbient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };

float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
bool ambGSwitch, ambSwitch, diffuseSwitch, specularSwitch, radAttenSwitch, specNSwitch=1;
bool heatButton,baseMat=0;
bool tintButton=1;
float* matAmb = Utils::goldAmbient();
float* matDif = Utils::goldDiffuse();
float* matSpe = Utils::goldSpecular();
float matShi = Utils::goldShininess();
float textureScale=1.0f;
glm::vec3 currentLightPos, transformed;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc, heatLoc, tintLoc, radAttLoc, nLoc, sampLoc;
float lightPos[3];
Torus basicTorus(0.8f, 0.4f, 48);
void installLights(glm::mat4 vMatrix) {
    transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
    lightPos[0] = transformed.x;
    lightPos[1] = transformed.y;
    lightPos[2] = transformed.z;

    // get the locations of the light and material fields in the shader
    globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
    ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
    diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
    specLoc = glGetUniformLocation(renderingProgram, "light.specular");
    posLoc = glGetUniformLocation(renderingProgram, "light.position");
    mambLoc = glGetUniformLocation(renderingProgram, "material.ambient");
    mdiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
    mspecLoc = glGetUniformLocation(renderingProgram, "material.specular");
    mshiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

    //  set the uniform light and material values in the shader
    glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
    glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
    glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
    glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
    glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);
    glProgramUniform4fv(renderingProgram, mambLoc, 1, matAmb);
    glProgramUniform4fv(renderingProgram, mdiffLoc, 1, matDif);
    glProgramUniform4fv(renderingProgram, mspecLoc, 1, matSpe);
    glProgramUniform1f(renderingProgram, mshiLoc, matShi);
}
struct cubenet{
    std::vector<int> indices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tTangents;
    std::vector<glm::vec3> sTangents;
    GLuint vao;
    GLuint vbo[numVBOs];
    GLuint texture;
    void initBox(float x, float y, float z){
        //front
        int i=0;

        vertices.push_back(glm::vec3(x,y,z));
        texCoords.push_back(glm::vec2(1,1));
        normals.push_back(glm::vec3(0.0f,0.0f,-1.0f));
        indices.push_back(i);
        i++;
           vertices.push_back(glm::vec3(-x,y,z));
        texCoords.push_back(glm::vec2(0,1));
           normals.push_back(glm::vec3(0.0f,0.0f,-1.0f));
        indices.push_back(i);
        i++;
           vertices.push_back(glm::vec3(x,-y,z));
           texCoords.push_back(glm::vec2(1,0));
           normals.push_back(glm::vec3(0.0f,0.0f,-1.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(-x,-y,z));
           texCoords.push_back(glm::vec2(0,0));
           normals.push_back(glm::vec3(0.0f,0.0f,-1.0f));
           indices.push_back(i);
           i++;
           indices.push_back(0xFFFF);
           //back
           vertices.push_back(glm::vec3(x,y,-z));
           texCoords.push_back(glm::vec2(1,0));
           normals.push_back(glm::vec3(0.0f,0.0f,1.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(-x,y,-z));
           texCoords.push_back(glm::vec2(0,0));
           normals.push_back(glm::vec3(0.0f,0.0f,1.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(x,-y,-z));
           texCoords.push_back(glm::vec2(1,1));
           normals.push_back(glm::vec3(0.0f,0.0f,1.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(-x,-y,-z));
           texCoords.push_back(glm::vec2(0,1));
           normals.push_back(glm::vec3(0.0f,0.0f,1.0f));
           indices.push_back(i);
           i++;
            indices.push_back(0xFFFF);
           //left
           vertices.push_back(glm::vec3(-x,y,-z));
           texCoords.push_back(glm::vec2(1,0));
           normals.push_back(glm::vec3(-1.0f,0.0f,0.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(-x,y,z));
           texCoords.push_back(glm::vec2(1,1));
           normals.push_back(glm::vec3(-1.0f,0.0f,0.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(-x,-y,-z));
           texCoords.push_back(glm::vec2(0,0));
           normals.push_back(glm::vec3(-1.0f,0.0f,0.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(-x,-y,z));
           texCoords.push_back(glm::vec2(1,0));
           normals.push_back(glm::vec3(-1.0f,0.0f,0.0f));
           indices.push_back(i);
           i++;
            indices.push_back(0xFFFF);
           //right
           vertices.push_back(glm::vec3(x,y,-z));
           texCoords.push_back(glm::vec2(1,0));
           normals.push_back(glm::vec3(1.0f,0.0f,0.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(x,y,z));
           texCoords.push_back(glm::vec2(1,1));
           normals.push_back(glm::vec3(1.0f,0.0f,0.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(x,-y,-z));
           texCoords.push_back(glm::vec2(0,0));
           normals.push_back(glm::vec3(1.0f,0.0f,0.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(x,-y,z));
           texCoords.push_back(glm::vec2(1,0));
           normals.push_back(glm::vec3(1.0f,0.0f,0.0f));
           indices.push_back(i);
           i++;
            indices.push_back(0xFFFF);
           //bottom
           vertices.push_back(glm::vec3(-x,-y,-z));
           texCoords.push_back(glm::vec2(1,0));
           normals.push_back(glm::vec3(0.0f,-1.0f,0.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(-x,-y,z));
           texCoords.push_back(glm::vec2(1,1));
           normals.push_back(glm::vec3(0.0f,-1.0f,0.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(x,-y,-z));
           texCoords.push_back(glm::vec2(0,0));
           normals.push_back(glm::vec3(0.0f,-1.0f,0.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(x,-y,z));
           texCoords.push_back(glm::vec2(1,0));
           normals.push_back(glm::vec3(0.0f,-1.0f,0.0f));
           indices.push_back(i);
           i++;
           indices.push_back(0xFFFF);

           vertices.push_back(glm::vec3(-x,y,-z));
           texCoords.push_back(glm::vec2(1,0));
           normals.push_back(glm::vec3(0.0f,1.0f,0.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(-x,y,z));
           texCoords.push_back(glm::vec2(1,1));
           normals.push_back(glm::vec3(0.0f,1.0f,0.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(x,y,-z));
           texCoords.push_back(glm::vec2(0,0));
           normals.push_back(glm::vec3(0.0f,1.0f,0.0f));
           indices.push_back(i);
           i++;
           vertices.push_back(glm::vec3(x,y,z));
           texCoords.push_back(glm::vec2(1,0));
           normals.push_back(glm::vec3(0.0f,1.0f,0.0f));
           indices.push_back(i);
           i++;
           indices.push_back(0xFFFF);



    }
    void initBoxFaces(float x, float y, float z){
        //front
        vertices.push_back(glm::vec3(x,y,z));
        texCoords.push_back(glm::vec2(1,1));
        vertices.push_back(glm::vec3(-x,y,z));
        texCoords.push_back(glm::vec2(0,1));
        vertices.push_back(glm::vec3(x,-y,z));
        texCoords.push_back(glm::vec2(1,0));
        vertices.push_back(glm::vec3(-x,-y,z));
        texCoords.push_back(glm::vec2(0,0));
        //back
        vertices.push_back(glm::vec3(x,y,-z));
        texCoords.push_back(glm::vec2(1,0));
        vertices.push_back(glm::vec3(-x,y,-z));
        texCoords.push_back(glm::vec2(0,0));
        vertices.push_back(glm::vec3(x,-y,-z));
        texCoords.push_back(glm::vec2(1,1));
        vertices.push_back(glm::vec3(-x,-y,-z));
        texCoords.push_back(glm::vec2(0,1));
        //left



    }
    void setupVertices(void) {


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
        glGenBuffers(numVBOs, vbo);
        glBindVertexArray(vao);


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
    void initCustom(std::vector<glm::vec3> face, float offset){
        int faceUpperSize=face.size()/2;
        int facePointSize=face.size();
        for(int i=0;i<facePointSize;i++){
            vertices.push_back(face[i]);
        }
        for(int i=0;i<facePointSize;i++){
            i<faceUpperSize?texCoords.push_back(glm::vec2((float)i/faceUpperSize,0)):texCoords.push_back(glm::vec2((i-faceUpperSize)/faceUpperSize,1));
        }
        for(int i=0;i<facePointSize;i++){
            vertices.push_back(glm::vec3(face[i][0],face[i][1],offset));
            i<faceUpperSize?texCoords.push_back(glm::vec2((float)i/faceUpperSize,0)):texCoords.push_back(glm::vec2(((float)i-faceUpperSize)/faceUpperSize,1));
        }
        for(int i=0;i<faceUpperSize;i++){
            indices.push_back(i);
            indices.push_back(i+faceUpperSize);
        }
        indices.push_back(faceUpperSize-1);
        indices.push_back(facePointSize);
        for(int i=0;i<faceUpperSize;i++){
            indices.push_back(face.size()-i-faceUpperSize);
            indices.push_back(face.size()-i);
        }
        indices.push_back(0xFFFF);

        for(int i=0;i<faceUpperSize;i++){
            indices.push_back(faceUpperSize-i-1);
                indices.push_back(facePointSize+faceUpperSize-i-1);
        }
        for(int i=0;i<faceUpperSize;i++){
            indices.push_back(faceUpperSize+i);
            indices.push_back(faceUpperSize+facePointSize+i);
        }
        indices.push_back(0xFFFF);
        }
    void initCustomNet(std::vector<glm::vec3> face, float tFact){
        //face1
        int ind=0;
        int offset=face.size();
        for(int i=0;i<face.size();i++){
            vertices.push_back(face[i]);
            normals.push_back(glm::vec3(0.0f,0.0f,-1.0f));
            i<offset/2?texCoords.push_back(glm::vec2((float)i/(offset/2),0)):texCoords.push_back(glm::vec2((i-offset/2)/(offset/2),1));
            indices.push_back(ind);
            ind++;

        }
        indices.push_back(0xFFFF);
        //face2
        for(int i=0;i<face.size();i++){
            vertices.push_back(glm::vec3(face[i][0],face[i][1],tFact));
            normals.push_back(glm::vec3(0.0f,0.0f,1.0f));
            i<offset/2?texCoords.push_back(glm::vec2((float)i/(offset/2),0)):texCoords.push_back(glm::vec2((i-offset/2)/(offset/2),1));
            indices.push_back(ind);
            ind++;

        }
        indices.push_back(0xFFFF);
        //left face
        vertices.push_back(face[offset]);
        normals.push_back(glm::vec3(-1.0f,0.0f,0.0f));
        vertices.push_back(face[0]);
        normals.push_back(glm::vec3(-1.0f,0.0f,0.0f));
        vertices.push_back(face[offset+offset/2]);
        normals.push_back(glm::vec3(-1.0f,0.0f,0.0f));
        vertices.push_back(face[offset/2]);
        normals.push_back(glm::vec3(-1.0f,0.0f,0.0f));
        texCoords.push_back(glm::vec2(0.0f,1.0f));
         texCoords.push_back(glm::vec2(1.0f,1.0f));
          texCoords.push_back(glm::vec2(0.0f,0.0f));
           texCoords.push_back(glm::vec2(1.0f,0.0f));
          indices.push_back(ind);
          ind++;
          indices.push_back(ind);
          ind++;
          indices.push_back(ind);
          ind++;
          indices.push_back(ind);
          ind++;
          indices.push_back(0xFFFF);
        //right face
        vertices.push_back(face[offset/2-1]);
        normals.push_back(glm::vec3(1.0f,0.0f,0.0f));
        vertices.push_back(face[offset+offset/2-1]);
        normals.push_back(glm::vec3(1.0f,0.0f,0.0f));
        vertices.push_back(face[offset-1]);
        normals.push_back(glm::vec3(1.0f,0.0f,0.0f));
        vertices.push_back(face[offset*2-1]);
        normals.push_back(glm::vec3(1.0f,0.0f,0.0f));
        texCoords.push_back(glm::vec2(0.0f,1.0f));
        texCoords.push_back(glm::vec2(1.0f,1.0f));
        texCoords.push_back(glm::vec2(0.0f,0.0f));
        texCoords.push_back(glm::vec2(1.0f,0.0f));
        indices.push_back(ind);
        ind++;
        indices.push_back(ind);
        ind++;
        indices.push_back(ind);
        ind++;
        indices.push_back(ind);
        ind++;
        indices.push_back(0xFFFF);
        //lower face
        for(int i=0;i<face.size();i++){
            vertices.push_back(face[i<face.size()/2?i+offset/2:i+offset+offset/2]);
            normals.push_back(glm::vec3(0.0f,-1.0f,0.0f));
            texCoords.push_back(glm::vec2(i<face.size()/2?i/(offset/2):(i-offset)/(offset/2),i<face.size()/2?1:0));
            indices.push_back(ind);
            ind++;
        }
        indices.push_back(0xFFFF);

        //upperface
        for(int i=0;i<face.size();i++){
            vertices.push_back(face[i<face.size()/2?i:i+offset]);
            normals.push_back(glm::vec3(0.0f,1.0f,0.0f));
             texCoords.push_back(glm::vec2(i<face.size()/2?i/(offset/2):(i-offset)/(offset/2),i<face.size()/2?1:0));
            indices.push_back(ind);
            ind++;
        }
        indices.push_back(0xFFFF);

    }
        void initStock(std::vector<glm::vec3> stock, float offset){
            int faceUpperSize=stock.size()/2;
            int facePointSize=stock.size();

            for(int i=0;i<facePointSize;i++){
                vertices.push_back(stock[i]);
                texCoords.push_back(glm::vec2(i/faceUpperSize,i%2==0?1:0));
            }
            for(int i=0;i<facePointSize;i++){
                vertices.push_back(glm::vec3(stock[i][0],stock[i][1],offset));
                texCoords.push_back(glm::vec2((i/faceUpperSize)%faceUpperSize,i%2==0?1:0));

            }
            for(int i=0;i<facePointSize;i++){
                indices.push_back(i);
            }
            for(int i=0;i<facePointSize;i++){
                indices.push_back(facePointSize*2-i-1);
            }
            indices.push_back(0xFFFF);
            for(int i=0;i<facePointSize;i+=2){
                indices.push_back(facePointSize-i-1);
                indices.push_back(facePointSize*2-i-1);
            }
            for(int i=1;i<facePointSize;i+=2){
                indices.push_back(i);
                indices.push_back(facePointSize+i);
            }
            indices.push_back(0xFFFF);

        }
        void initStockNet(std::vector<glm::vec3> stock, float offset){
            int faceUpperSize=stock.size()/2;
            int facePointSize=stock.size();
            int indpoints=0;
            for(int i=0;i<facePointSize;i++){
                vertices.push_back(stock[i]);
                texCoords.push_back(glm::vec2(i/faceUpperSize,i%2==0?1:0));
                normals.push_back(glm::vec3(0.0f,0.0f,-1.0f));
                indices.push_back(indpoints);
                indpoints++;
            }
            indices.push_back(0xFFFF);
            for(int i=0;i<facePointSize;i++){
                vertices.push_back(glm::vec3(stock[i][0],stock[i][1],offset));
                texCoords.push_back(glm::vec2((i/faceUpperSize)%faceUpperSize,i%2==0?1:0));
                normals.push_back(glm::vec3(0.0f,0.0f,1.0f));
                indices.push_back(indpoints);
                indpoints++;

            }
             indices.push_back(0xFFFF);
            //lower face
            for(int i=0;i<facePointSize;i+=2){
                vertices.push_back(stock[facePointSize-i-1]);
                vertices.push_back(stock[facePointSize*2-i-1]);
                texCoords.push_back(glm::vec2(i/faceUpperSize,1));
                texCoords.push_back(glm::vec2(i/faceUpperSize,0));
                normals.push_back(glm::vec3(0.0f,-1.0f,0.0f));
                normals.push_back(glm::vec3(0.0f,-1.0f,0.0f));
                indices.push_back(indpoints);
                indpoints++;
                indices.push_back(indpoints);
                indpoints++;
            }
             indices.push_back(0xFFFF);
            for(int i=1;i<facePointSize;i+=2){
                vertices.push_back(stock[i]);
                vertices.push_back(stock[facePointSize+i]);
                texCoords.push_back(glm::vec2(i/faceUpperSize,1));
                texCoords.push_back(glm::vec2(i/faceUpperSize,0));
                normals.push_back(glm::vec3(0.0f,1.0f,0.0f));
                normals.push_back(glm::vec3(0.0f,1.0f,0.0f));
                indices.push_back(indpoints);
                indpoints++;
                indices.push_back(indpoints);
                indpoints++;
            }
            indices.push_back(0xFFFF);


        }
        void addSmoothVert(){
            //face 1
            for(int i=0;i<vertices.size();i++) normals.push_back(glm::vec3(0.0f,0.0f,1.0f));
            for(int i=0;i<4;i++) normals.push_back(glm::vec3(1.0f,0.0f,0.0f));
             for(int i=0;i<vertices.size();i++) normals.push_back(glm::vec3(0.0f,0.0f,1.0f));



            //right linking face

            //face 2
            //upper linking face
            //left linking face
            //lower linking face
        }
        void addSharpVert(){

        }
        void addSmoothVertStock(){

        }
        void addSharpVertStock(){

        }



};
void setupCubeVertices(void) {
    float cubeVertexPositions[108] =
        {	-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
            1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
            1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
        };


    glGenBuffers(numVBOs, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexPositions), cubeVertexPositions, GL_STATIC_DRAW);


}
cubenet cheek,spoke, stock, axbx,plane, skyBox;
std::vector<glm::vec3> pointList={{.10f,0.0f,0.0f},{.105f,.10f,0.0f},{1.08f, 0.05f, 0.0f},{1.08f,0.06f,0.0f},{1.10f,0.06f,0.0f},{1.10f,0.05f,0.0f}};
std::vector<glm::vec3> arrowPoints={{0.0f,0.0f,0.0f},{0.0f,.010f,0.0f},{.80f,.01f,0.0f}, {.80f,.02f,0.0f},{.9f,.00f,0.0f}};
std::vector<glm::vec3> hubPoints={{0.01f,0.0f,0.0f},{0.025f,0.0f,0.0f},{0.025f,0.025f,0.0f},{0.025f,0.08f,0.0f},{0.06f,0.08f,0.0f},{0.1f,0.1f,0.0f},{0.12f,0.1f,0.0f},{0.12f,0.0f,0.0f}};
//can use a similar method to the cheek or the box method
std::vector<glm::vec3> stockPoints={{0.0f,0.2f,0.0f},{.25f,0.0f,0.0f},{0.1f,0.07f,0.0f},{0.2f,0.22f,0.0f},{0.05f,0.05f,0.0f},{0.4f,0.25f,0.0f},{1.62f,0.3f,0.0f},{1.6f,0.5f,0.0f},{2.2f,0.3f,0.0f},{2.15f,.15f,0.0f}};
std::vector<glm::vec3> trunPoints={{-0.1f,0.05f,0.0f},{0.1f,0.05f,0.0f}};
std::vector<glm::vec3> rimPoints={{0.0f,.40f,0.0f},{0.02f,0.4f,0.0f},{0.0f,.6f,0.0f},{0.02f,.6f,0.0f}};
//
std::vector<glm::vec3> spokePoints={{0.0f,0.25f,0.0f},{0.2f,0.25f,0.0f},{0.0f,0.8f,0.0f},{0.2f,0.8f,0.0f}};
//for the cheek, just use x,0 for points to finish 2d face then  copy the  points of the face that are translated by .2 in z, then draw between them to generate shape
std::vector<glm::vec3> cheekPoints={{0.0f,0.03f,0.0f},{0.08f,0.05f,0.0f},{0.10f,0.05f,0.0f},{.105f,0.043f,0.0f},{.1125f,0.04f,0.0f},{.12f,0.043f,0.0f},{.125f,0.05f,0.0f},{.142f,0.05f,0.0f} };
std::vector<glm::vec3> axlBxPoints={{0.0f,0.2f,0.0f},{02.0f,0.2f,0.0f},{2.0f,0.6f,0.0f},{0.0f,0.6f,0.0f}, {0.0f,0.2f,2.0f},{02.0f,0.2f,2.0f},{2.0f,0.6f,2.0f},{0.0f,0.6f,2.0f}};
std::vector<glm::vec3> elevScrwPoints={{0.0f,0.015f,0.0f},{0.05f,0.015f,0.0f}};
//fix this to work with the rotation
std::vector<glm::vec3> hndlPoints={{0.0f,0.0f,0.0f},{0.012f,0.0f,0.0f},{0.0f,0.01f,0.0f},{0.012f,0.01f,0.0f},{0.02f,0.0025f,0.0f},{0.035f,0.005f,0.0f},{0.05f,0.0f,0.0f}};
std::vector<glm::vec3> columnPoints{{0.0f,0.0f,0.0f},{0.0f,0.06f,0.0f},{0.03f,0.05f,0.0f},{0.45f,0.05f,0.0f},{0.475f,0.06f,0.0f},{0.45f,0.050f,0.0f}};
std::vector<glm::vec3> orbPoints{{0.5f,0.0f,0.0f},{0.512f,0.02f,0.0f},{0.54f,0.042f,0.0f},{0.57f,0.055f,0.0f},{0.605f,0.042f,0.0f},{0.638f,0.02f,0.0f},{0.65f,0.0f,0.0f}};
//plan for objects with faces:
//create object face with points
//copy face points and translate them
//draw triangles between and on faces
Torus myTorus(0.5f, 0.2f, 48,pointList);
Torus arrowTorus(0.5f,0.2f,48,arrowPoints);
Torus  hub(0.5f,0.2f,48,hubPoints);
Torus  elevScrw(0.5f,0.2f,48,elevScrwPoints);
Torus  hndle(0.5f,0.2f,48,hndlPoints);
Torus  trun(0.5f,0.2f,48,trunPoints);
Torus  rim(0.7f,0.05f,48,rimPoints);
Torus column(0.5f, 0.2f, 48,columnPoints);
Torus lightOrb(0.5f, 0.2f, 48,orbPoints);
std::vector<int> stockpts;
std::vector<glm::vec2> stocktex;
std::vector<int> cheekpts;
std::vector<glm::vec2> cheektex;
std::vector<int> spokepts;
std::vector<glm::vec2> spoketex;

float xamn, yamn,yrot,trunAngle,elevScrewT,hubxrot,hubyrot,scrwRotX=0.0;
float xhub= .02f;
float spokeZ=0.02f;
glm::vec3 position=glm::vec3(0.0f,0.0f,0.0f);
glm::quat quaternion=glm::quat(1.0f,0.0f,0.0f,0.0f);
const char* vertexShaderSource=
    "#version 430\n"
    "layout(location = 0) in vec3 pos;\n"
    "layout(location = 1) in vec2 tecCoord;\n"
    "out vec2 tc;\n"
    "uniform mat4 proj_matrix;\n"
    "uniform mat4 mv_matrix;\n"
    "layout (binding=0) uniform sampler2D s;\n"
    "void main(void)\n"
    "{gl_Position = proj_matrix * mv_matrix * vec4(pos, 1.0); tc = tecCoord;}";
const char* fragmentShaderSource=
    "#version 430\n"
    "in vec2 tc;\n"
    "out vec4 color;\n"
    "uniform mat4 mv_matrix;\n"
    "uniform mat4 proj_matrix;\n"
    "layout(binding = 0) uniform sampler2D samp;\n"
    "void main(void)\n"
    "{color = texture(samp, tc);}";

GLuint createShaderProgram(const char *vp, const char *fp) {
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vp, NULL);
    glCompileShader(vShader);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fp, NULL);
    glCompileShader(fShader);
    GLuint vfprogram = glCreateProgram();
    glAttachShader(vfprogram, vShader);
    glAttachShader(vfprogram, fShader);
    glLinkProgram(vfprogram);
    return vfprogram;
}
GLuint loadTexture(const char *texImagePath)
{
    GLuint textureRef;
    textureRef = SOIL_load_OGL_texture(texImagePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    if (textureRef == 0) cout << "didnt find texture file " << texImagePath << endl;
    // ----- mipmap/anisotropic section
    glBindTexture(GL_TEXTURE_2D, textureRef);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
    /*if (glewIsSupported("GL_EXT_texture_filter_anisotropic")) {
        GLfloat anisoset = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoset);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoset);
    }*/

    return textureRef;
}




//in the case the object doesnt have the points required to generate triangle strips with respect to the x axis, adds points to do so
void addXAxisPts(std::vector<glm::vec3> &inputPointList){
    int size =inputPointList.size();
    for(int i=0;i<size;i++){
        inputPointList.push_back(glm::vec3(inputPointList[i][0],0.0f,0.0f));

    }
}




void mouse_callback(GLFWwindow* window, int button, int action, int mods){
    double x_pos = 0.0;
    double y_pos = 0.0;
    // Save mouse position to variables x_pos and y_pos
    glfwGetCursorPos(window, &x_pos, &y_pos);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouseHeld = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mouseHeld = false;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        rMouseHeld = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        rMouseHeld = false;
    }
}
void movement_callback(GLFWwindow* window, double xpos, double ypos){

    if(rMouseHeld){
        baseMat? baseScale+=(ypos-initMouseY)*0.25: scale+=(ypos-initMouseY)*0.25f;
    }
    if(mouseHeld){
        baseMat? baseElev+=(xpos-initMouseX)*0.0025f:elev+=(xpos-initMouseX)*0.0025f;
        baseMat? baseAzim+=(ypos-initMouseY)*0.0025f:azim+=(ypos-initMouseY)*0.0025f;
    }
    initMouseX=xpos;
    initMouseY=ypos;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //update m accordingly based on press
    //x: world - apply translation to left local - apply translation to right
    //z: world - apply rotation left local - apply rotation right
    //y: local - apply rotation to right
    transMat=glm::mat4(1.0f);
    rotMat=glm::mat4(1.0f);
    xang=0.0f;
    //m needs to be set in memory to properly rotate


        //absolute transformations
        //UP/DOWN: absolute +x/-x
        //RIGHT/LEFT: absolute +z/-z
    //glfwGetKey()
    if (key==GLFW_KEY_UP&&action == GLFW_PRESS){

        position[0]+=0.1;
        hubxrot=1.0f;

            //M=transMat*M;

}

        if (key== GLFW_KEY_LEFT&&action == GLFW_PRESS){
            //angles need to be updated in a way that preserves previous changes, changing direction causes the shape to be modified
            //currently the angles have to be updated multiple times in the opposite direction to change direction

            quaternion=quaternion*glm::quat(glm::cos(-.1f/2),0,0,glm::sin(-.1f/2));
        }
        if (key==GLFW_KEY_DOWN&&action == GLFW_PRESS){
           position[0]-=0.1;
            hubxrot=-1.0f;

        }
     if (key==GLFW_KEY_RIGHT&&action == GLFW_PRESS){

            quaternion=quaternion*glm::quat(glm::cos(.1f/2),0,0,glm::sin(.1f/2));
     }
        //local transformations
        //W/S: local +x/-x
        //D/A: local  +z/-z
        //1/0: local +y/-y
       if (key==GLFW_KEY_W&&action == GLFW_PRESS){


       }
        if (key==GLFW_KEY_A&&action == GLFW_PRESS){

         yrot-=1.0f;
            hubyrot-=1.0f;
        }
       if (key==GLFW_KEY_S&&action == GLFW_PRESS){

       }
        if (key==GLFW_KEY_D&&action == GLFW_PRESS){

            yrot+=1.0f;
            hubyrot+=1.0f;
        }
        if (key==GLFW_KEY_1&&action == GLFW_PRESS){
            elevScrewT+=.1f;
            scrwRotX+=1.0f;
            trunAngle+=.10f;
        }
       if (key==GLFW_KEY_0&&action == GLFW_PRESS){
             elevScrewT-=.1f;
           scrwRotX-=1.0f;
             trunAngle-=.10f;
       }
            //camera elevation:
            //T - increase angle | Y - decrease angle
      if (key==GLFW_KEY_T&&action == GLFW_PRESS){
            xang+=1.00f;
      }
       if (key==GLFW_KEY_Y&&action == GLFW_PRESS){
            xang-=1.00f;
       }
       if(key==GLFW_KEY_F&&action==GLFW_PRESS){
           t=t*1.25;
           b=b*1.25;
           l=l*1.25;
           r=r*1.25;
       }
       if(key==GLFW_KEY_V&&action==GLFW_PRESS){
           t=t*0.8;
           b=b*0.8;
           l=l*0.8;
           r=r*0.8;
       }
       if(key==GLFW_KEY_2&&action==GLFW_PRESS){
           switch(heatButton){
           case 0:
               heatButton=1;
               break;
           case 1:
               heatButton=0;
               break;
           }
       }
       if(key==GLFW_KEY_3&&action==GLFW_PRESS){
           switch(tintButton){
           case 0:
               tintButton=1;
               break;
           case 1:
               tintButton=0;
               break;
           }
       }
           if(key==GLFW_KEY_4&&action==GLFW_PRESS){
               switch(ambGSwitch){
               case 0:

                   globalAmbient[0]=0.7f;
                   globalAmbient[1]=0.7f;
                   globalAmbient[2]=0.7f;
                   globalAmbient[3]=1.0f;
                   break;
               case 1:

                   globalAmbient[0]=0.0f;
                  globalAmbient[1]=0.0f;
                   globalAmbient[2]=0.0f;
                  globalAmbient[3]=1.0f;
                   break;
               }
           }
               if(key==GLFW_KEY_5&&action==GLFW_PRESS){
                   switch(ambSwitch){
                   case 0:

                       lightAmbient[0]=0.2f;
                       lightAmbient[1]=0.2f;
                       lightAmbient[2]=0.2f;
                       lightAmbient[3]=1.0f;
                       break;
                   case 1:

                       lightAmbient[0]=0.0f;
                       lightAmbient[1]=0.0f;
                       lightAmbient[2]=0.0f;
                      lightAmbient[3]=1.0f;
                       break;
                   }
       }
               if(key==GLFW_KEY_6&&action==GLFW_PRESS){
                   switch(diffuseSwitch){
                   case 0:

                       lightDiffuse[0]=1.0f;
                       lightDiffuse[1]=1.0f;
                       lightDiffuse[2]=1.0f;
                       lightDiffuse[3]=1.0f;
                       break;
                   case 1:

                       lightDiffuse[0]=0.0f;
                       lightDiffuse[1]=0.0f;
                       lightDiffuse[2]=0.0f;
                       lightDiffuse[3]=1.0f;
                       break;
                   }
               }
               if(key==GLFW_KEY_7&&action==GLFW_PRESS){
                   switch(specularSwitch){
                   case 0:
                       lightSpecular[0]=1.0f;
                       lightSpecular[1]=1.0f;
                       lightSpecular[2]=1.0f;
                       lightSpecular[3]=1.0f;
                       break;
                   case 1:
                       lightSpecular[0]=0.0f;
                       lightSpecular[1]=0.0f;
                       lightSpecular[2]=0.0f;
                       lightSpecular[3]=1.0f;
                       break;
                   }
               }
               if(key==GLFW_KEY_8&&action==GLFW_PRESS){
                   switch(radAttenSwitch){
                   case 0:
                       radAttenSwitch=1;
                       break;
                   case 1:
                       radAttenSwitch=0;
                       break;
                   }
               }
               if(key==GLFW_KEY_9&&action==GLFW_PRESS){
                   switch(specNSwitch){
                   case 0:
                       matShi=Utils::goldShininess();
                       break;
                   case 1:
                       matShi= 1.0f;
                       break;
                   }
               }
               if(key==GLFW_KEY_TAB&&action==GLFW_PRESS){
                   switch(baseMat){
                   case 0:
                       baseMat=1;
                       break;
                   case 1:
                       baseMat=0;
                       break;
                   }
               }

       vMat=glm::rotate(vMat,toRadians(xang),glm::vec3(1.0f,0.0f,0.0f));
    mvMat=vMat*mMat;

    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glProgramUniform1i(renderingProgram,heatLoc,heatButton);
     glProgramUniform1i(renderingProgram,tintLoc,tintButton);
    glProgramUniform1i(renderingProgram,radAttLoc,radAttenSwitch);
    glDrawElements(GL_TRIANGLE_STRIP, myTorus.getIndices().size(), GL_UNSIGNED_INT, 0);
}
void drawObject(GLsizei numIndices, GLuint texture, GLuint vao, glm::mat4 mvMatrix, glm::mat4 normMatrix) {
    glBindVertexArray(vao);

    GLint mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    GLint normLoc = glGetUniformLocation(renderingProgram, "norm_matrix");

    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
    glUniformMatrix4fv(normLoc, 1, GL_FALSE, glm::value_ptr(normMatrix));

    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void init(GLFWwindow* window) {
    renderingProgram = Utils::createShaderProgram("../../PhongShaders/vertShader.glsl", "../../PhongShaders/fragShader.glsl");
    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 0.2f;
    torLocX = 0.0f; torLocY = 0.0f; torLocZ = -0.5f;
    arrLocX=0.0f; arrLocY=0.0f; arrLocZ=0.0f;
     M=glm::mat4(1.0f);
    rotMat=glm::mat4(1.0f);
     xang=0.0f;
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
   // pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
    myTorus.setupVertices();
    arrowTorus.setupVertices();
        hub.setupVertices();
    elevScrw.setupVertices();
        hndle.setupVertices();
    trun.setupVertices();
        rim.setupVertices();
    column.setupVertices();
        lightOrb.setupVertices();


    addXAxisPts(cheekPoints);
        cheek.initCustomNet(cheekPoints,0.06);
    cheek.setupVertices();
        stock.initStockNet(stockPoints,0.15);
    stock.setupVertices();
        spoke.initBox(0.01f,0.04f,0.01f);
    spoke.setupVertices();
        axbx.initBox(.04f,0.04f,0.44f);
    axbx.setupVertices();
        plane.initBox(0.25,0.01,0.25);
    plane.setupVertices();



    vMat=glm::mat4(1.0f);
    vMat = glm::translate(vMat, glm::vec3(-cameraX, -cameraY, -cameraZ));
cubeRenderer = Utils::createShaderProgram("../../vertCShader.glsl", "../../fragCShader.glsl");
    setupCubeVertices();
glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
skyBoxTexture = Utils::loadCubeMap("../../cubeMap");


    brickTexture = loadTexture("../../rustedcopper.jpg");
    metalTexture=loadTexture("../../paintedblackmetal.jpg");
    paintTexture=loadTexture("../../paintedolivewood.jpg");
    stoneTexture=loadTexture("../../brick1.jpg");
    glBindTexture(GL_TEXTURE_2D, brickTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, metalTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, paintTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, stoneTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(0xFFFF);
    b=-0.25;
    t=0.25;
    n=-02.5;
    f=02.50;
    l=b;
    r=t;
    setupCubeVertices();
    pMat=glm::mat4(1.0f);
    pMat[0][0]=(2*n)/(r-l);
    pMat[0][2]=(r+l)/(r-l);
    pMat[1][1]=(2*n)/(t-b);
    pMat[1][2]=(t+b)/(t-b);
    pMat[2][2]=(-(f+n))/(f-n);
    pMat[2][3]=(-(2*f*n))/(f-n);
    pMat[3][2]=-1.0f;
    GLint status;
    glGetShaderiv(renderingProgram, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint logLength = 0;
        glGetShaderiv(renderingProgram, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 0) {
            std::vector<GLchar> log(logLength);
            glGetShaderInfoLog(renderingProgram, logLength, nullptr, log.data());
            std::cerr << "Shader compile error:\n" << log.data() << std::endl;
        } else {
            std::cerr << "Shader compile error, but no log information is available." << std::endl;
        }
    }

}
float scrwX=0.1f;
float scrwY=0.3f;
void display(GLFWwindow* window, double currentTime) {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    v1Mat=glm::inverse(glm::rotate(glm::mat4(1.0f),azim,glm::vec3(0,1,0))*glm::rotate(glm::mat4(1.0f),elev,glm::vec3(1,0,0))*glm::translate(glm::mat4(1.0f),glm::vec3(0,0,1/scale)));
    v2Mat=glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(torLocX,torLocY,torLocZ))*glm::rotate(glm::mat4(1.0f),baseAzim,glm::vec3(0,1,0))*glm::rotate(glm::mat4(1.0f),baseElev,glm::vec3(1,0,0))*glm::translate(glm::mat4(1.0f),glm::vec3(0,0,1/baseScale)));
    vMat=baseMat?v2Mat:v1Mat;
    // Setup matrices

    glm::mat4 viewMatrix = glm::mat4(glm::mat3(vMat)); // Remove translation for skybox
    glm::mat4 projectionMatrix = pMat;

    // Render Skybox
    glUseProgram(cubeRenderer);
    GLint vLoc = glGetUniformLocation(cubeRenderer, "v_matrix");
    GLint projLoc = glGetUniformLocation(cubeRenderer, "proj_matrix");

    glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTexture);

    glDisable(GL_DEPTH_TEST);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glEnable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glUseProgram(0);
    glUseProgram(renderingProgram);

    // Get uniform locations
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");

    // Debug uniform locations
    if (projLoc == -1 || nLoc == -1 || mvLoc == -1) {
        std::cerr << "Error: Failed to get uniform locations." << std::endl;
    }

    // Push and update the model-view stack
    mdlViewStck.push(vMat);
    mdlViewStck.push(mdlViewStck.top());
    mMat = glm::mat4(1.0f); // Identity matrix
    mdlViewStck.top() *= mMat;

    // Install lights and calculate matrices
    installLights(mdlViewStck.top());
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(normMat));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, stoneTexture);
    glUniform1i(glGetUniformLocation(renderingProgram, "s"), 0);


    // Render the objects
   // glBindTexture(GL_TEXTURE_2D, stoneTexture);
    drawObject(column.getIndices().size(), stoneTexture, column.vao, mvMat, normMat);

    // Clean up the stack
    mdlViewStck.pop();

    // Ensure depth test and culling are enabled
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
    aspect = (float)newWidth / (float)newHeight;
    glViewport(0, 0, newWidth, newHeight);
    gluPerspective(45.0, 16.0/9.0*float(newWidth)/float(newHeight), 0.1, 100.0);
    n=-25;
    f=25;
    l=b;
    r=t;

    pMat=glm::mat4(1.0f);
    pMat[0][0]=(2*n)/(r-l);
    pMat[0][2]=(r+l)/(r-l);
    pMat[1][1]=(2*n)/(t-b);
    pMat[1][2]=(t+b)/(t-b);
    pMat[2][2]=(-(f+n))/(f-n);
    pMat[2][3]=(-(2*f*n))/(f-n);
    pMat[3][2]=-1.0f;
    //pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void) {
    if (!glfwInit()) { exit(EXIT_FAILURE); }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(800*16.0/9.0, 800, "Assignment 3", NULL, NULL);

    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
    glfwSwapInterval(1);
    //transMat=glm::mat4(1.0f);
    //transMat[3][0]=1.0f;
    //transMat[3][1]=1.0f;
    //transMat[3][2]=1.0f;
    glfwSetWindowSizeCallback(window, window_size_callback);
glfwSetKeyCallback(window,key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
glfwSetCursorPosCallback(window, movement_callback);
    init(window);

    while (!glfwWindowShouldClose(window)) {

        glProvokingVertex(GL_TRIANGLE_STRIP);

        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
