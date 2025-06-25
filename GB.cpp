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
#include "Character.h"
#include "Coin.h"
#include <fstream>

using namespace std;

GLFWwindow *g_window = NULL;

int g_gl_height = 1080;
int g_gl_width = 1920;

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

GLuint generateTileVAO(float th, float tw, float th2, float tileH2, float tileH, float tw2, float tileW2, float tileW, TileMap tmap)
{

    float vertices[] = {
        tmap.getXi(),
        tmap.getYi() + th2,
        0.0f,
        0.0f,
        tileH2,

        tmap.getXi() + tw2,
        tmap.getYi(),
        0.0f,
        tileW2,
        0.0f,

        tmap.getXi() + tw,
        tmap.getYi() + th2,
        0.0f,
        tileW,
        tileH2,

        tmap.getXi() + tw2,
        tmap.getYi() + th,
        0.0f,
        tileW2,
        tileH
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
    char* tileName = new char[256];
    float tilesetXFrames;
    int totalW, totalH;
    arq >> tileName >> tilesetXFrames >> totalH >> totalW >> w >> h;

    cout << "Map dimensions: " << w << "x" << h << endl;
    cout << "Tile name: " << tileName << endl;
    cout << "Tileset X Frames: " << tilesetXFrames << endl;
    cout << "Total Width: " << totalW << ", Total Height: " << totalH << endl;
    TileMap *tmap = new TileMap(w, h, tileName, tilesetXFrames, 1, (float)g_gl_width, (float)totalW, (float)g_gl_height, (float)totalH);
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
    cout << "Tile name: " << tmap->getFileName() << endl;
    loadTexture(tid, tmap->getFileName());
    tmap->setTid(tid);

    float w = tmap->getXf() - tmap->getXi();
    float h = tmap->getYf() - tmap->getYi();
    float tw = w / tmap->getWidth();
    float th = tw / 2.0f;
    float tw2 = th;
    float th2 = th / 2.0f;
    float tileW = 1.0f / (float)tmap->getTileSetCols();
    float tileW2 = tileW / 2.0f;
    float tileH = 1.0f / (float)tmap->getTileSetRows();
    float tileH2 = tileH / 2.0f;

    GLuint tileVAO = generateTileVAO(th, tw, th2, tileH2, tileH, tw2, tileW2, tileW, *tmap);

    Character character = Character::generateModel();
    GLuint tex;
    loadTexture(tex, character.getFileName());
    character.setTexture(tex);
    GLuint characterVAO = character.generateCharacterVAO(tmap->getXi(), tmap->getYi(), tw, tw);

    Coin coin = Coin::generateModel();
    GLuint coinTex;
    loadTexture(coinTex, coin.getFileName());
    coin.setTexture(coinTex);
    GLuint coinVAO = coin.generateCoinVAO(tmap->getXi(), tmap->getYi(), tw, tw);

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

    
    int coinFrame = 0;
    int action = 3;
    float previous = glfwGetTime();
    bool animating = false;

    float targetX = 0.0f;
    float targetY = 0.0f;

    float previousX = 0.0f;
    float previousY = 0.0f;

    while (!glfwWindowShouldClose(g_window))
    {

        _update_fps_counter(g_window);
        double current_seconds = glfwGetTime();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, g_gl_width, g_gl_height);

        glUseProgram(shaderProgram);

        glBindVertexArray(tileVAO);
        float x, y;
   

        for (int r = 0; r < tmap->getHeight(); r++)
        {
            for (int c = 0; c < tmap->getWidth(); c++)
            {
                int t_id = (int)tmap->getTile(c, r);
                int u = t_id % tmap->getTileSetCols();
                int v = t_id / tmap->getTileSetCols();

                tmap->computeDrawPosition(c, r, tw, th, x, y);

                float offsetx = u * tileW;
                if (character.getCharacterX() == c && character.getCharacterY() == r && animating && character.getX() == previousX && character.getY() == previousY)
                {
                    previousX = character.getX();
                    previousY = character.getY();
                    targetX = x;
                    targetY = y + 1.0f;
                } else if(character.getCharacterX() == c && character.getCharacterY() == r && !animating) {
                    character.setX(x);
                    character.setY(y + 1.0f);
                    targetX = character.getX();
                    targetY = character.getY();
                    previousX = character.getX();
                    previousY = character.getY();
                }

                
                glUniform1f(glGetUniformLocation(shaderProgram, "offsetx"), offsetx);
                glUniform1f(glGetUniformLocation(shaderProgram, "offsety"), v * tileH);
                glUniform1f(glGetUniformLocation(shaderProgram, "layer_z"), 0.55f);
                glUniform1f(glGetUniformLocation(shaderProgram, "tx"), x);
                glUniform1f(glGetUniformLocation(shaderProgram, "ty"), y + 1.0);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tmap->getTileSet());
                glUniform1i(glGetUniformLocation(shaderProgram, "sprite"), 0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

        double elapsed = current_seconds - previous;
        double characterElapsed = current_seconds - character.getCharacterPreviousTimeAnimation();

        if (characterElapsed > (0.08) && animating) {
            if (character.getX() < targetX || character.getX() > targetX) {
                character.setX(character.getX() + (targetX - previousX) / 8.0f);
            }

            if (character.getY() < targetY || character.getY() > targetY) {
                character.setY(character.getY() + (targetY - previousY) / 8.0f);
            }

            const float EPSILON = 0.0001f;
            if (fabs(character.getX() - targetX) < EPSILON && fabs(character.getY() - targetY) < EPSILON) {
                action = 3; // Reset action to idle; // Reset elapsed time
                animating = false;
                previousX = targetX;
                previousY = targetY;
                character.setCharacterFrame(7); // Reset character frame to idle
            }
        }

        if (characterElapsed > (0.08) && animating) {
            character.setCharacterPreviousTimeAnimation(current_seconds);

            if (character.getCharacterFrame() == 8)
            {
                action = (4 + (action - 1)) % 4;
                character.setCharacterFrame(0);
            }
            else
            {
                character.setCharacterFrame((character.getCharacterFrame() + 1) % 8);
            }
        }  

        character.setOffsetX((float)character.getCharacterXFrames() * (float)character.getCharacterFrame());
        character.setOffsetY((float)character.getCharacterYFrames() * (float)action);

        glBindVertexArray(characterVAO);
        glUniform1f(glGetUniformLocation(shaderProgram, "offsetx"), character.getOffsetX());
        glUniform1f(glGetUniformLocation(shaderProgram, "offsety"), character.getOffsetY());
        glUniform1f(glGetUniformLocation(shaderProgram, "layer_z"), 0.50f);
        glUniform1f(glGetUniformLocation(shaderProgram, "tx"), character.getX());
        glUniform1f(glGetUniformLocation(shaderProgram, "ty"), character.getY());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, character.getTexture());
        glUniform1i(glGetUniformLocation(shaderProgram, "sprite"), 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        if (elapsed > (0.2)) {
            previous = current_seconds;

            if (coinFrame == 9)
            {
                coinFrame = 0;
            }
            else
            {
                coinFrame = (coinFrame + 1) % 9;
            }
        }

        coin.setOffsetX(coin.getCoinXFrames() * (float)coinFrame);

        glBindVertexArray(coinVAO);
        glUniform1f(glGetUniformLocation(shaderProgram, "offsetx"), coin.getOffsetX());
        glUniform1f(glGetUniformLocation(shaderProgram, "offsety"), coin.getOffsetY());
        glUniform1f(glGetUniformLocation(shaderProgram, "layer_z"), 0.51f);
        glUniform1f(glGetUniformLocation(shaderProgram, "tx"), coin.getX());
        glUniform1f(glGetUniformLocation(shaderProgram, "ty"), coin.getY());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, coin.getTexture());
        glUniform1i(glGetUniformLocation(shaderProgram, "sprite"), 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



        glfwPollEvents();

        glfwSwapBuffers(g_window);


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
        if (e == GLFW_PRESS && character.getCharacterX() < tmap->getWidth() - 1 && characterElapsed > (0.16) && !animating)
        {
            animating = true;
            character.setCharacterPreviousTimeAnimation(current_seconds);
            character.setCharacterX(character.getCharacterX() + 1);
            action = 1;
            // cout << "E" << endl;
        }

        // Q NO
        if (q == GLFW_PRESS && character.getCharacterY() > 0 && characterElapsed > (0.16) && !animating)
        {
            animating = true;
            character.setCharacterPreviousTimeAnimation(current_seconds);
            character.setCharacterY(character.getCharacterY() - 1);
            action = 0;
            // cout << "Q" << endl;
        }

        // C SE
        if (c == GLFW_PRESS && character.getCharacterY() < tmap->getHeight() - 1 && characterElapsed > (0.16) && !animating)
        {
            animating = true;
            character.setCharacterPreviousTimeAnimation(current_seconds);
            character.setCharacterY(character.getCharacterY() + 1);
            action = 1;
            // cout << "C" << endl;
        }

        // Z SO
        if (z == GLFW_PRESS && character.getCharacterX() > 0 && characterElapsed > (0.16) && !animating)
        {
            animating = true;
            character.setCharacterPreviousTimeAnimation(current_seconds);
            character.setCharacterX(character.getCharacterX() - 1);
            action = 0;
            // cout << "Z" << endl;
        }

        // S
        if (s == GLFW_PRESS && d != GLFW_PRESS && a != GLFW_PRESS && character.getCharacterY() < tmap->getHeight() - 1 && character.getCharacterX() > 0 && characterElapsed > (0.16) && !animating)
        {
            animating = true;
            character.setCharacterPreviousTimeAnimation(current_seconds);
            character.setCharacterY(character.getCharacterY() + 1);
            character.setCharacterX(character.getCharacterX() - 1);
            action = 3;
            // cout << "S" << endl;
        }

        // W
        if (w == GLFW_PRESS && d != GLFW_PRESS && a != GLFW_PRESS && character.getCharacterY() > 0 && character.getCharacterX() < tmap->getWidth() - 1 && characterElapsed > (0.16) && !animating)
        {
            animating = true;
            character.setCharacterPreviousTimeAnimation(current_seconds);
            character.setCharacterY(character.getCharacterY() - 1);
            character.setCharacterX(character.getCharacterX() + 1);
            action = 2;
            // cout << "W" << endl;
        }

        // A
        if (a == GLFW_PRESS && character.getCharacterX() > 0 && character.getCharacterY() > 0 && characterElapsed > (0.16) && !animating)
        {
            animating = true;
            character.setCharacterPreviousTimeAnimation(current_seconds);
            character.setCharacterX(character.getCharacterX() - 1);
            character.setCharacterY(character.getCharacterY() - 1);
            action = 0;
            // cout << "A" << endl;
        }

        // D
        if (d == GLFW_PRESS && character.getCharacterX() < tmap->getWidth() - 1 && character.getCharacterY() < tmap->getHeight() - 1 && characterElapsed > (0.16) && !animating)
        {
            animating = true;
            character.setCharacterPreviousTimeAnimation(current_seconds);
            character.setCharacterX(character.getCharacterX() + 1);
            character.setCharacterY(character.getCharacterY() + 1);
            action = 1;
            // cout << "D" << endl;
        }
    }

    glfwDestroyWindow(g_window);
    glfwTerminate();
    return 0;
}