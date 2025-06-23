#include "stb_image.h"
#include "gl_utils.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <string.h>
#include <time.h>
#define GL_LOG_FILE "gl.log"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "TileMap.h"
#include <fstream>

using namespace std;

GLFWwindow *g_window = NULL;

int g_gl_height = 1080;
int g_gl_width = 1920;
float ds = 1.0 / 7.0;

float xi = -1.0f;
float xf = 1.0f;
float yi = -1.0f;
float yf = 1.0f;
float w = xf - xi;
float h = yf - yi;
int tileSetCols = 7, tileSetRows = 1;

int userX = 0;
int userY = 7;

TileMap *tmap = NULL;

int loadTexture(unsigned int &texture, char *filename)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLfloat max_aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
    // set the maximum!
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);

    int width, height, nrChannels;

    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data)
    {
        if (nrChannels == 4)
        {

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else
        {

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
}

float[20] generateVertices(float th, float tw, float th2, float tileH2, float tileH, float tw2, float tileW2, float tileW)
{
    return {
        xi,
        yi + th2,
        0.0f,
        0.0f,
        tileH2,
        xi + tw2,
        yi,
        0.0f,
        tileW2,
        0.0f,
        xi + tw,
        yi + th2,
        0.0f,
        tileW,
        tileH2,
        xi + tw2,
        yi + th,
        0.0f,
        tileW2,
        tileH,
    };
}

GLuint generateVAO(float vertices[20])
{
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return VAO;
}

        tileH2,
        xi + tw2,
        yi + th,
        0.0f,
        tileW2,
        tileH,
    };

    unsigned int indices[] = {
        2, 1, 0,
        0, 3, 2};

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return VAO;
}

TileMap *readMap(char *filename)
{
    ifstream arq(filename);
    int w, h;
    arq >> w >> h;
    TileMap *tmap = new TileMap(w, h, 0);
    for (int r = 0; r < h; r++)
    {
        for (int c = 0; c < w; c++)
        {
            int tid;
            arq >> tid;
            cout << tid << " ";
            tmap->setTile(c, h - r - 1, tid);
        }
        cout << endl;
    }
    arq.close();
    return tmap;
}

int main()
{

    start_gl();
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS);

    tmap = readMap("terreno.tmap");
    GLuint tid;
    loadTexture(tid, "tilesetIso.png");
    tmap->setTid(tid);

    float tw = w / tmap->getWidth();
    float th = tw / 2.0f;
    float tw2 = th;
    float th2 = th / 2.0f;
    float tileW = 1.0f / (float)tileSetCols;
    float tileW2 = tileW / 2.0f;
    float tileH = 1.0f / (float)tileSetRows;
    float tileH2 = tileH / 2.0f;

    GLuint VAO = generateVAO(th, tw, th2, tileH2, tileH, tw2, tileW2, tileW);

    char vertex_shader[1024 * 256];
    char fragment_shader[1024 * 256];
    parse_file_into_str("vs.glsl", vertex_shader, 1024 * 256);
    parse_file_into_str("fs.glsl", fragment_shader, 1024 * 256);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const GLchar *p = (const GLchar *)vertex_shader;
    glShaderSource(vs, 1, &p, NULL);
    glCompileShader(vs);

    // check for compile errors
    int params = -1;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params)
    {
        fprintf(stderr, "ERROR: GL shader index %i did not compile\n", vs);
        print_shader_info_log(vs);
        return 1; // or exit or something
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    p = (const GLchar *)fragment_shader;
    glShaderSource(fs, 1, &p, NULL);
    glCompileShader(fs);

    // check for compile errors
    glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params)
    {
        fprintf(stderr, "ERROR: GL shader index %i did not compile\n", fs);
        print_shader_info_log(fs);
        return 1; // or exit or something
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fs);
    glAttachShader(shaderProgram, vs);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &params);
    if (GL_TRUE != params)
    {
        fprintf(stderr, "ERROR: could not link shader programme GL index %i\n",
                shaderProgram);
        return false;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float previous = glfwGetTime();

    while (!glfwWindowShouldClose(g_window))
    {

        _update_fps_counter(g_window);
        double current_seconds = glfwGetTime();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, g_gl_width, g_gl_height);

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);
        float x, y;
        for (int r = 0; r < tmap->getHeight(); r++)
        {
            for (int c = 0; c < tmap->getWidth(); c++)
            {
                int t_id = (int)tmap->getTile(c, r);
                int u = t_id % tileSetCols;
                int v = t_id / tileSetCols;

                tmap->computeDrawPosition(c, r, tw, th, x, y);

                float offsetx = u * tileW;
                if (userX == c && userY == r)
                {

                    offsetx = 6.0f * tileW; // desloca o tile selecionado para a direita
                }

                glUniform1f(glGetUniformLocation(shaderProgram, "offsetx"), offsetx);
                glUniform1f(glGetUniformLocation(shaderProgram, "offsety"), v * tileH);
                glUniform1f(glGetUniformLocation(shaderProgram, "tx"), x);
                glUniform1f(glGetUniformLocation(shaderProgram, "ty"), y + 1.0);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tmap->getTileSet());
                glUniform1i(glGetUniformLocation(shaderProgram, "sprite"), 0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

        glfwPollEvents();

        int w = glfwGetKey(g_window, GLFW_KEY_W);
        int a = glfwGetKey(g_window, GLFW_KEY_A);
        int s = glfwGetKey(g_window, GLFW_KEY_S);
        int d = glfwGetKey(g_window, GLFW_KEY_D);
        int e = glfwGetKey(g_window, GLFW_KEY_E);
        int q = glfwGetKey(g_window, GLFW_KEY_Q);
        int c = glfwGetKey(g_window, GLFW_KEY_C);
        int z = glfwGetKey(g_window, GLFW_KEY_Z);

        if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE))
        {

            glfwSetWindowShouldClose(g_window, 1);
        }

        // E NE
        if (e == GLFW_PRESS && userX < tmap->getWidth() - 1 && (current_seconds - previous) > (0.16))
        {
            previous = current_seconds;
            userX += 1;
            cout << "E" << endl;
        }

        // Q NO
        if (q == GLFW_PRESS && userY > 0 && (current_seconds - previous) > (0.16))
        {
            previous = current_seconds;
            userY -= 1;
            cout << "Q" << endl;
        }

        // C SE
        if (c == GLFW_PRESS && userY < tmap->getHeight() - 1 && (current_seconds - previous) > (0.16))
        {
            previous = current_seconds;
            userY += 1;
            cout << "C" << endl;
        }

        // Z SO
        if (z == GLFW_PRESS && userX > 0 && (current_seconds - previous) > (0.16))
        {
            previous = current_seconds;
            userX -= 1;
            cout << "Z" << endl;
        }

        // S
        if (s == GLFW_PRESS && d != GLFW_PRESS && a != GLFW_PRESS && userY < tmap->getHeight() - 1 && userX > 0 && (current_seconds - previous) > (0.16))
        {
            previous = current_seconds;
            userY += 1;
            userX -= 1;
            cout << "S" << endl;
        }
        glfwSetKeyCallback(g_window, NULL);
        // W
        if (w == GLFW_PRESS && d != GLFW_PRESS && a != GLFW_PRESS && userY > 0 && userX < tmap->getWidth() - 1 && (current_seconds - previous) > (0.16))
        {
            previous = current_seconds;
            userY -= 1;
            userX += 1;
            cout << "W" << endl;
        }

        // A
        if (a == GLFW_PRESS && userX > 0 && userY > 0 && (current_seconds - previous) > (0.16))
        {
            previous = current_seconds;
            userX -= 1;
            userY -= 1;
            cout << "A" << endl;
        }

        // D
        if (d == GLFW_PRESS && userX < tmap->getWidth() - 1 && userY < tmap->getHeight() - 1 && (current_seconds - previous) > (0.16))
        {
            previous = current_seconds;
            userX += 1;
            userY += 1;
            cout << "D" << endl;
        }

        glfwSwapBuffers(g_window);
    }

    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(g_window);
    glfwTerminate();
    return 0;
}
