#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -10.0f;  
float pitch = 0.0f;  
float lastX = 400.0f;
float lastY = 300.0f; 
float fov = 45.0f;

bool firstMouse = true; 
float deltaTime = 0.0f; 
float lastFrame = 0.0f; 

GLuint cubeVAO, cubeVBO, cubeEBO;
GLuint HumanVAO, HumanVBO, HumanEBO;
GLuint Wolf1VAO, Wolf1VBO, Wolf1EBO;
GLuint Wolf2VAO, Wolf2VBO, Wolf2EBO;
GLFWwindow* window;
int width, height;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= (float)yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    float currentSpeed = 5.0f * deltaTime; 

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {

        printf("%d", GLFW_PRESS);
        cameraPos += currentSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        printf("%d", GLFW_PRESS);
        cameraPos -= currentSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        printf("%d", GLFW_PRESS);
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * currentSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        printf("%d", GLFW_PRESS);
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * currentSpeed;
    }
}

static bool Initiate(int w, int h, const char* title)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(w, h, title, nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    glEnable(GL_DEPTH_TEST);

    
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetScrollCallback(window, scroll_callback);

    return true;
}

static void Terminate() {
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    glDeleteVertexArrays(1, &HumanVAO);
    glDeleteBuffers(1, &HumanVBO);
    glDeleteBuffers(1, &HumanEBO);

    glDeleteVertexArrays(1, &Wolf1VAO);
    glDeleteBuffers(1, &Wolf1VBO);
    glDeleteBuffers(1, &Wolf1EBO);

    glDeleteVertexArrays(1, &Wolf2VAO);
    glDeleteBuffers(1, &Wolf2VBO);
    glDeleteBuffers(1, &Wolf2EBO);

    glfwDestroyWindow(window);
    glfwTerminate();
}

std::string readShaderCode(const char* filePath) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        exit(EXIT_FAILURE);
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    return shaderStream.str();
}

GLuint compileShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }

    return shader;
}

GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode = readShaderCode(vertexPath);
    std::string fragmentCode = readShaderCode(fragmentPath);

    GLuint vertexShader = compileShader(vertexCode, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader linking failed: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Load model using TinyOBJLoader
bool loadModel(const std::string& path, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
    if (!warn.empty()) std::cerr << "WARN: " << warn << std::endl;
    if (!err.empty()) std::cerr << "ERR: " << err << std::endl;
    if (!success) return false;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            vertices.push_back(attrib.normals[3 * index.normal_index + 0]);
            vertices.push_back(attrib.normals[3 * index.normal_index + 1]);
            vertices.push_back(attrib.normals[3 * index.normal_index + 2]);

            if (index.texcoord_index >= 0) {
                vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
            }
            else {
                vertices.push_back(0.0f); 
                vertices.push_back(0.0f);
            }

            indices.push_back(indices.size());
        }
    }

    return true;
}

GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        printf("Texture loaded: %s size : (%d,%d)\n", path, width, height);
        GLenum format = GL_RGB;
        if (nrChannels == 1)      format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }

    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

static void Setup(GLuint& VAO, GLuint& VBO, GLuint& EBO, const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}


int main() {
 
    if (!Initiate(800, 600, "Computer Graphics Project")) {
        return -1;
    }
    
    std::vector<float> cubeVertices;
    std::vector<unsigned int> cubeIndices;
    if (!loadModel("Objects\\Cottage\\cottage_obj.obj", cubeVertices, cubeIndices)) {
        std::cerr << "Failed to load obj" << std::endl;
        return -1;
    }

    std::vector<float> HumanVertices;
    std::vector<unsigned int> HumanIndices;
    if (!loadModel("Objects\\OBJ\\OBJ.obj", HumanVertices, HumanIndices)) {
        std::cerr << "Failed to load obj" << std::endl;
        return -1;
    }

    std::vector<float> Wolf1Vertices;
    std::vector<unsigned int> Wolf1Indices;
    if (!loadModel("Objects\\Wolf\\Wolf_obj.obj", Wolf1Vertices, Wolf1Indices)) {
        std::cerr << "Failed to load obj" << std::endl;
        return -1;
    }

    std::vector<float> Wolf2Vertices;
    std::vector<unsigned int> Wolf2Indices;
    if (!loadModel("Objects\\Wolf\\Wolf_obj.obj", Wolf2Vertices, Wolf2Indices)) {
        std::cerr << "Failed to load obj" << std::endl;
        return -1;
    }

    // Cube
    Setup(cubeVAO, cubeVBO, cubeEBO, cubeVertices, cubeIndices);

    // Human
    Setup(HumanVAO, HumanVBO, HumanEBO, HumanVertices, HumanIndices);

    // Wolf1
    Setup(Wolf1VAO, Wolf1VBO, Wolf1EBO, Wolf1Vertices, Wolf1Indices);

    // Wolf2
    Setup(Wolf2VAO, Wolf2VBO, Wolf2EBO, Wolf2Vertices, Wolf2Indices);

   
    GLuint shaderProgram = createShaderProgram("vertex_shader.glsl", "fragment_shader.glsl");

    GLuint cubeTexture = loadTexture("Objects\\Texture_Old_paint.jpg");

    // Set up camera
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    printf("CameraPos: %d , %d , %d", cameraPos.x, cameraPos.y, cameraPos.z);

    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    glUseProgram(shaderProgram);
    glUniform3f(glGetUniformLocation(shaderProgram, "light.position"), lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "light.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"), 1.0f, 1.0f, 1.0f);

    glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 1.0f, 0.5f, 0.31f);
    glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 1.0f, 0.5f, 0.31f);
    glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.5f, 0.5f, 0.5f);
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);


    while (!glfwWindowShouldClose(window)) {


        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection = glm::perspective(glm::radians(fov),
            (float)width / (float)height,
            0.1f,
            100.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));

        // Animate light position
        lightPos.x = 4.0f * cos(glfwGetTime());
        lightPos.z = 4.9f * sin(glfwGetTime());
        glUniform3f(glGetUniformLocation(shaderProgram, "light.position"), lightPos.x, lightPos.y, lightPos.z);

        glUseProgram(shaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        
               
        // Bind texture for cottage
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "material.diffuse"), 0);

        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.2f, -0.6f, 0.0f));

            model = glm::rotate(model, glm::radians(-115.0f), glm::vec3(0, 1, 0));

            model = glm::scale(model, glm::vec3(0.06f));

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 0.8f, 0.2f);

            glBindVertexArray(cubeVAO);
            glDrawElements(GL_TRIANGLES, (GLsizei)cubeIndices.size(), GL_UNSIGNED_INT, 0);
        }


        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -0.35f, 2.0f));
            model = glm::scale(model, glm::vec3(0.001f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.8f, 0.7f, 0.6f);

            glBindVertexArray(HumanVAO);
            glDrawElements(GL_TRIANGLES, (GLsizei)HumanIndices.size(), GL_UNSIGNED_INT, 0);
        }

 
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(1.5f, -0.5f, -1.0f));
            model = glm::scale(model, glm::vec3(0.6f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.8f, 0.8f, 0.2f);

            glBindVertexArray(Wolf1VAO);
            glDrawElements(GL_TRIANGLES, (GLsizei)Wolf1Indices.size(), GL_UNSIGNED_INT, 0);
        }


        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.5f, -0.5f, -2.0f));
            model = glm::scale(model, glm::vec3(0.6f));

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.2f, 0.5f, 0.2f);

            glBindVertexArray(Wolf2VAO);
            glDrawElements(GL_TRIANGLES, (GLsizei)Wolf2Indices.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    Terminate();
    return 0;
}