#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED

struct cfg{
    int screen_width, screen_height, volume;
    bool fullscreen, vsync, capFPS;
};

enum renderType{
    RENDER_MENU, // Menu
    RENDER_BLACKSCREEN, // inicio de juego o game over
    RENDER_GAME // juego.
};

enum OPTION_MENU{
    INICIAR_JUEGO,
    OPCIONES,
    SALIR
};

enum OPTION_SETTINGS{
    VOLUME_AUDIO,
    FULLSCREEN,
    VSYNC,
    FPSLIMIT,
    QUIT
};

// Mario
enum Evolutions{
    DEAD,
    SMALL,
    BIG,
    POWER_FLOWER,
    POWER_STAR
};

struct grhData{
    short sX;
    short sY;
    short filenum;
    short pixelWidth;
    short pixelHeight;
    float tileWidth;
    float tileHeight;
    short numFrames;
    int *frames;
    float speed;
};

struct grh{
    short grhIndex;
    float frameCounter;
    float speed;
    bool started;
    int loops;
    float angle;
};

struct body{
    grh posWalk[4];
    short numBody[3];
};

enum tNPC{
    NONE_CHAR,
    HONGE, // HONGO
    BEETLE, // ESCARABAJO
    TURTLE_GREEN, // TORTUGA VERDE
    TURTLE_WINGS_GREEN, // TORTUGA CON ALAS VERDE
    TURTLE_RED, // TORTUGA ROJA
    TURTLE_WINGS_RED, // TORTUGA CON ALAS ROJA
    SPIKEY, // TORTUGA PUNTIAGUDA
    HAMMER_BRO, // TORTUGA CON MARTILLO
    SQUID, // CALAMAR
    CHEEP, // PAJARO
    PLANT, // PLANTA
    FIRE, // FUEGO
    BULLET_BILL, // CAÑON
    FINAL_BOSS, // TORUGA GIGANTE BOSS FINAL DEL JUEGO
    PEACH // PRINCESA
};

// KIND OF OBJETS IN ONE MAP!
enum tOBJ{
    NONE_OBJ,
    FREECOIN, // monedas libres por el mapa
    BOX_COIN, // cajita "?" con X monedas
    BOX_HONGE_OR_FLOWER, // CAJA QUE SPAWNEA UN HONGO O UNA FLOR
    BOX_HONGE_UP, // HONGO VERDE QUE REGALA 1 VIDA.
    HONGE_OR_FLOWER, // cajita "?" con hongo o flor (sale flor si mario es grande)
    BOX_INVISIBLE_HONGE_OR_FLOWER, // cajita invisible con hongo u flor.
    BLOCK, // ladrillo que se rompe
    BLOCK_COIN, // ladrillo que da muchas monedas
    BLOCK_STAR, // ladrillo que dropea la estrellita.
    FLAG,
    MINI_FLAG,
    CASTLE_FLAG,
    RED_HONGE,
    GREEN_HONGE
};

struct mapData{
    grh Layer[2];
    bool blocked;
    int typeOBJ, typeNPC, NPCIndex, OBJIndex;
};

struct mapInfo{
    int max_tiles_x, max_tiles_y; // Para el array bidimencional de tipo puntero.
    int typeMap; // esto define las texturas de monedas, objetos, musica de fondo, etc.
    int TOTAL_NPCs, TOTAL_OBJs; // para la creacion de objetos despues de cargar el mapa.
    int Mario_PosX, Mario_PosY, PosCastle;
};

enum tMAP{
    OVERWORLD,
    CASTLE,
    UNDERGROUND,
    UNDERWATER
};

struct fonts{
    int ascii_code[255];
};

// INDICE BODYS INDEXADOS
enum bChar{
    BODY_MARIO_SMALL,
    BODY_HONGE,
    BODY_TURTLE_GREEN,
    BODY_TURTLE_WINGS_GREEN
};

#endif
