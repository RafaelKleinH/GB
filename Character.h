#include "Sprite.h"

class Character : public Sprite
{

    int characterFrame; // Frame of the character sprite
    float characterXFrames;
    float characterYFrames;
    float characterPreviousTimeAnimation = 0.0f;

    int characterX, characterY;

public:

    // Getters
    int getCharacterFrame() const { return characterFrame; } 
    float getCharacterXFrames() const { return characterXFrames; }
    float getCharacterYFrames() const { return characterYFrames; }
    float getCharacterPreviousTimeAnimation() const { return characterPreviousTimeAnimation; }
    int getCharacterX() const { return characterX; }
    int getCharacterY() const { return characterY; }


    // Setters
    void setCharacterFrame(int value) { characterFrame = value; }
    void setCharacterPreviousTimeAnimation(float value) { characterPreviousTimeAnimation = value; }
    void setCharacterX(int value) { characterX = value; }
    void setCharacterY(int value) { characterY = value; }

    static Character generateModel() {
        Character character;
        character.setFileName("sprite.png");
        character.setOffsetX(3.0);
        character.setOffsetY(3.0);
        character.setX(0.0);
        character.setY(0.0);
        character.characterXFrames = 1.0f / 8.0f; // 8 frames in the X direction
        character.characterYFrames = 1.0f / 4.0f; //
        character.characterX = 0;
        character.characterY = 0;
        character.setCharacterFrame(7); // Default frame
        return character;
    }

    GLuint generateCharacterVAO(float xi, float yi, float th, float tw)
{
    float vertices[] = {
        xi + tw,
        yi + th + th / 4.0f,
        0.0f,
        characterXFrames,
        characterYFrames,

        xi,
        yi + th / 4.0f,
        0.0f,
        0.0f,
        0.0f,

        xi + tw,
        yi + th / 4.0f,
        0.0f,
        characterXFrames,
        0.0f,

        xi,
        yi + th + th / 4.0f,
        0.0f,
        0.0f,
        characterYFrames,

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