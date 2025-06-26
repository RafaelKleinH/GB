class TileMap
{
    float z;            // caso de eventual de vários tilemaps sobrepostos
    unsigned int tid;   // indicação do tileset utilizado
    int width, height;  // dimensões da matriz
    unsigned char *map; // mapa com ids dos tiles que formam o cenário
    unsigned int *walkable;
    char *fileName; // nome do arquivo de mapa
    int tileSetCols, tileSetRows;

    float xi, yi; // posição inicial do tile no mapa
    float xf, yf; // posição final do tile no mapa

    // Coin
    unsigned int *coins;
    int totalCoins;

public:
    TileMap(int w, int h, char *fileName, float tileSetCols, float tileSetRows, float windowsXMax, float tileXMax, float windowsYMax, float tileYMax)
    {
        this->map = new unsigned char[w * h];
        this->walkable = new unsigned int[w * h];
        this->coins = new unsigned int[w * h];
        this->width = w;
        this->height = h;
        this->z = 0.0f;
        this->tid = 0;
        this->fileName = fileName;
        this->tileSetCols = tileSetCols;
        this->tileSetRows = tileSetRows;
        this->totalCoins = 0;

        this->setupXs(windowsXMax, tileXMax, windowsYMax, tileYMax);
    }

    unsigned char *getMap()
    {
        return this->map;
    }

    int getWidth()
    {
        return this->width;
    }

    int getHeight()
    {
        return this->height;
    }

    int getTile(int col, int row)
    {
        return this->map[col + row * this->width];
    }

    int getWalkable(int col, int row)
    {
        return this->walkable[col + row * this->width];
    }

    int getCoins(int col, int row)
    {
        return this->coins[col + row * this->width];
    }

    void setTile(int col, int row, unsigned char tile) { this->map[col + row * this->width] = tile; }

    void setWalkable(int col, int row, unsigned int walkable) { this->walkable[col + row * this->width] = walkable; }

    void setCoins(int col, int row, unsigned int coin) { this->coins[col + row * this->width] = coin; }

    int getTileSet() { return this->tid; }
    float getZ() { return this->z; }
    char *getFileName() { return fileName; }
    int getTileSetCols() { return tileSetCols; }
    int getTileSetRows() { return tileSetRows; }

    int getTotalCoins() { return totalCoins; }
    void setTotalCoins(int totalCoins)
    {
        this->totalCoins = totalCoins;
    }

    void setZ(float z)
    {
        this->z = z;
    }

    void setTid(int tid)
    {
        this->tid = tid;
    }

    float getXi() const { return xi; }
    float getYi() const { return yi; }
    float getXf() const { return xf; }
    float getYf() const { return yf; }

    // Desenhar tile no mapa
    void computeDrawPosition(const int col, const int row, const float tw, const float th, float &targetx, float &targety) const
    {
        targetx = col * tw / 2 + row * tw / 2;
        targety = col * th / 2 - row * th / 2;
    }


    // Limites seguindo pixeis do arquivo de config
    void setupXs(float windowsXMax, float tileXMax, float windowsYMax, float tileYMax)
    {
        float xValues = (windowsXMax == tileXMax) ? 1.0f : 2.0f * (tileXMax / windowsXMax) - 1.0f;
        float yValues = (windowsYMax == tileYMax) ? 1.0f : 2.0f * (tileYMax / windowsYMax) - 1.0f;

        xi = xValues == 1.0f ? -1.0f : -(1.0f - fabs(xValues));
        xf = xValues == 1.0f ? 1.0f : (1.0f - fabs(xValues));
        yi = yValues == 1.0f ? -1.0f : -(1.0f - fabs(yValues));
        yf = yValues == 1.0f ? 1.0f : (1.0f - fabs(yValues));

        if (xi > xf)
        {
            xi = -xi;
            xf = fabs(xf);
        }

        if (yi > yf)
        {
            yi = -yi;
            yf = fabs(yf);
        }
    }
};
