#ifndef TORUS_H
#define TORUS_H
#include<GL/glew.h>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>

#include<glm/gtc/matrix_transform.hpp>

#define numVBOs 4
class Torus
{
private:
    int numVertices;
    int numIndices;
    int prec;
    int inputPointSize;
    float inner;
    float outer;
    std::vector<glm::vec3> inputs;
    std::vector<int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> sTangents;
    std::vector<glm::vec3> tTangents;
    void init();
    void initBasic();
    float toRadians(float degrees);
public:
    Torus();
    Torus(float innerRadius, float outerRadius, int prec);
    Torus(float innerRadius, float outerRadius, int prec,std::vector<glm::vec3> inputPoints);
    void addSmoothVert();
    void addSharpVert();
    void setupVertices();
    int getNumVertices();
    int getNumIndices();
    std::vector<int> getIndices();
    std::vector<glm::vec3> getVertices();
    std::vector<glm::vec2> getTexCoords();
    std::vector<glm::vec3> getNormals();
    std::vector<glm::vec3> getStangents();
    std::vector<glm::vec3> getTtangents();
    GLuint vao;
    GLuint vbo[numVBOs];
};

#endif // TORUS_H
