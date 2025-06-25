#include "Sprite.h"

class Coin : public Sprite
{

    float coinXFrames = 1.0 / 9.0f;
    float coinYFrames = 1.0f;

public:

    // Getters
    float getCoinXFrames() const { return coinXFrames; }
    float getCoinYFrames() const { return coinYFrames; }

    static Coin generateModel() {
        Coin coin;
        coin.setFileName("coin.png");
        coin.setOffsetX(0.0);
        coin.setOffsetY(0.0);
        return coin;
    }

    GLuint generateCoinVAO(float xi, float yi, float th, float tw)
{
    float vertices[] = {
        xi +  tw / 4.0f,
        yi + th / 1.5f,
        0.0f,
        coinXFrames,
        coinYFrames,

        xi,
        yi + th / 4.0f,
        0.0f,
        0.0f,
        0.0f,

        xi + tw / 4.0f,
        yi + th / 4.0f,
        0.0f,
        coinXFrames,
        0.0f,

        xi,
        yi + th / 1.5f,
        0.0f,
        0.0f,
        coinYFrames,

    };

    unsigned int indices[] = {
        2, 1, 0,
        0, 3, 1};

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
};