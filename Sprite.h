#pragma once
class Sprite
{
    int id;
    char *fileName;
    unsigned int texture;
    float offsetx, offsety, x, y;

public:
    // Getters
    int getId() const { return id; }
    char* getFileName() const { return fileName; }
     int getTexture() const { return texture; }
    float getOffsetX() const { return offsetx; }
    float getOffsetY() const { return offsety; }
    float getX() const { return x; }
    float getY() const { return y; }

    // Setters
    void setId(int value) { id = value; }
    void setFileName(char* value) { fileName = value; }
    void setTexture(unsigned int value) { texture = value; }
    void setOffsetX(float value) { offsetx = value; }
    void setOffsetY(float value) { offsety = value; }
    void setX(float value) { x = value; }
    void setY(float value) { y = value; }

};