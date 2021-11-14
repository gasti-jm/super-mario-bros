#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#include "character.h"

// Defines
#define TILE_PIXEL_SIZE 32

using namespace std;

class Resources{
    public:
        static cfg Config;
        static fonts font_type[2];
        static body *bodyData;
        static mapData **MapData;
        static mapInfo MapInfo;
        static grhData *GrhData;
        static NPC *NPCs;
        static MapOBJ *OBJs;

        Resources();
        ~Resources();

        static void LoadGrhData();
        static void LoadMap();
        static void LoadBodys();
        static void InitGrh(grh&, short, bool);
        static void LoadFonts();
        static void LoadConfig();
        static void ToRewriteConfig();
};
