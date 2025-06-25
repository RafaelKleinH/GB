class TileMap {
    float z;               // caso de eventual de vários tilemaps sobrepostos
    unsigned int tid;      // indicação do tileset utilizado
    int width, height;     // dimensões da matriz
    unsigned char *map; // mapa com ids dos tiles que formam o cenário
    char *fileName; // nome do arquivo de mapa
    int tileSetCols, tileSetRows;

    float xi, yi; // posição inicial do tile no mapa
    float xf, yf; // posição final do tile no mapa
    
public:
    TileMap(int w, int h, char *fileName, float tileSetCols, float tileSetRows, float windowsXMax, float tileXMax, float windowsYMax, float tileYMax) {
        this->map = new unsigned char [w*h];
        this->width = w;
        this->height = h;
        this->z = 0.0f;
        this->tid = 0;
        this->fileName = fileName;
        this->tileSetCols = tileSetCols;
        this->tileSetRows = tileSetRows;

        this->setupXs(windowsXMax, tileXMax, windowsYMax, tileYMax);
    }

    unsigned char* getMap() {
        return this->map;
    }
    
    int getWidth() {
        return this->width;
    }
    
    int getHeight() {
        return this->height;
    }
    
    int getTile(int col, int row) {
        return this->map[col + row * this->width];
    }
    
    void setTile(int col, int row, unsigned char tile) { this->map[col + row * this->width] = tile; }

    int getTileSet() { return this->tid; }
    float getZ() { return this->z; }
    char* getFileName() { return fileName; }
    int getTileSetCols() { return tileSetCols; }
    int getTileSetRows() { return tileSetRows; }

    void setZ(float z){
        this->z = z;
    }
    
    void setTid(int tid) {
        this->tid = tid;
    }

    float getXi() const { return xi; }
    float getYi() const { return yi; }
    float getXf() const { return xf; }
    float getYf() const { return yf; }
    
    void computeDrawPosition(const int col, const int row, const float tw, const float th, float &targetx, float &targety) const {
        targetx = col * tw / 2 + row * tw / 2;
        targety = col * th / 2 - row * th / 2;
    }

    void setupXs(float windowsXMax, float tileXMax, float windowsYMax, float tileYMax) {

        float xValues = (windowsXMax == tileXMax) ? 1.0f : 2.0f * (tileXMax / windowsXMax) - 1.0f;
        float yValues = (windowsYMax == tileYMax) ? 1.0f : 2.0f * (tileYMax / windowsYMax) - 1.0f;
        std::cout << "tileXMax: " << tileXMax << std::endl;
        std::cout << "tileYMax: " << tileYMax << std::endl;
        std::cout << "windowsXMax: " << windowsXMax << std::endl;
        std::cout << "windowsYMax: " << windowsYMax << std::endl;

        xi = xValues == 1.0f ? -1.0f : -(1.0f- fabs(xValues));
        xf = xValues == 1.0f ? 1.0f : (1.0f - fabs(xValues));
        yi = yValues == 1.0f ? -1.0f : -(1.0f - fabs(yValues));
        yf = yValues == 1.0f ? 1.0f : (1.0f - fabs(yValues));
        
        if (xi > xf) {
            xi = -xi;
            xf = fabs(xf);
        }

         if (yi > yf) {
            yi = -yi;
            yf = fabs(yf);
        }

        std::cout << "xi: " << xi << std::endl;
        std::cout << "xf: " << xf << std::endl;
        std::cout << "yi: " << yi << std::endl;
        std::cout << "yf: " << yf << std::endl;
    }
};

