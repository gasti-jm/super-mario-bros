#include "gamedata.h"

cfg Resources::Config;
fonts Resources::font_type[2];
body *Resources::bodyData;
mapData **Resources::MapData;
mapInfo Resources::MapInfo;
grhData *Resources::GrhData;
NPC *Resources::NPCs;
MapOBJ *Resources::OBJs;

Resources::Resources(){}

Resources::~Resources(){

}

void Resources::LoadGrhData(){
    ifstream file("Resources/graphics.ind", ios::in|ios::binary);

    if (!file.is_open()){
        cout << "No existe el archivo 'graphics.ind'.";
        return;
    }

    int grhCount, Grh = 0, tempint;
    float tempfloat;
    short tempshort;

    file.read((char *)&tempint, sizeof(int)); // version del archivo (esto es solo para pruebas).
    file.read((char *)&grhCount, sizeof(int));

    GrhData = new grhData[grhCount + 1];

    while (Grh < grhCount){
        file.read((char *)&tempint, sizeof(int));
        Grh = tempint;

        file.read((char *)&tempshort, sizeof(short));
        GrhData[Grh].numFrames = tempshort;

        if (GrhData[Grh].numFrames <= 0){
            cout << "ERROR IN THE GRHINDEX:" << Grh << endl;
            return;
        }

        GrhData[Grh].frames = new int[GrhData[Grh].numFrames];
        if (GrhData[Grh].numFrames > 1){
            for(int frame = 1; frame <= GrhData[Grh].numFrames; frame++){
                file.read((char *)&tempint, sizeof(int));
                GrhData[Grh].frames[frame] = tempint;
                if (GrhData[Grh].frames[frame] <= 0){
                    cout << "ERROR IN THE GRHINDEX:" << Grh << endl;
                    return;
                }
            }

            file.read((char *)&tempfloat, sizeof(float));
            if (tempfloat <= 0){
                cout << "ERROR IN THE GRHINDEX:" << Grh << endl;
                return;
            }
            GrhData[Grh].speed = tempfloat;

            GrhData[Grh].pixelHeight = GrhData[GrhData[Grh].frames[1]].pixelHeight;
            if (GrhData[Grh].pixelHeight <= 0){
                cout << "ERROR IN THE GRHINDEX:" << Grh << endl;
                return;
            }

            GrhData[Grh].pixelWidth = GrhData[GrhData[Grh].frames[1]].pixelWidth;
            if (GrhData[Grh].pixelWidth <= 0){
                cout << "ERROR IN THE GRHINDEX:" << Grh << endl;
                return;
            }

            GrhData[Grh].tileWidth = GrhData[GrhData[Grh].frames[1]].tileWidth;
            if (GrhData[Grh].tileWidth <= 0){
                cout << "ERROR IN THE GRHINDEX:" << Grh << endl;
                return;
            }

            GrhData[Grh].tileHeight = GrhData[GrhData[Grh].frames[1]].tileHeight;
            if (GrhData[Grh].tileHeight <= 0){
                cout << "ERROR IN THE GRHINDEX:" << Grh << endl;
                return;
            }

        } else {

            file.read((char *)&tempint, sizeof(int));
            GrhData[Grh].filenum = tempint;
            if (GrhData[Grh].filenum <= 0){
                cout << "ERROR IN THE GrhIndex filenum: " << Grh << endl;
                return;
            }

            file.read((char *)&tempshort, sizeof(short));
            GrhData[Grh].sX = tempshort;
            if (GrhData[Grh].sX < 0){
                cout << "ERROR IN THE GrhIndex sX: " << Grh << endl;
                return;
            }

            file.read((char *)&tempshort, sizeof(short));
            GrhData[Grh].sY = tempshort;
            if (GrhData[Grh].sY < 0){
                cout << "ERROR IN THE GrhIndex sY: " << Grh << endl;
                return;
            }

            file.read((char *)&tempshort, sizeof(short));
            GrhData[Grh].pixelWidth = tempshort;
            if (GrhData[Grh].pixelWidth <= 0){
                cout << "ERROR IN THE GrhIndex pW: " << Grh << endl;
                return;
            }

            file.read((char *)&tempshort, sizeof(short));
            GrhData[Grh].pixelHeight = tempshort;
            if (GrhData[Grh].pixelHeight <= 0){
                cout << "ERROR IN THE GrhIndex pH: " << Grh << endl;
                return;
            }

            GrhData[Grh].tileWidth = (float)GrhData[Grh].pixelWidth / 32;
            GrhData[Grh].tileHeight = (float)GrhData[Grh].pixelHeight / 32;

            GrhData[Grh].frames[1] = Grh;
        }
    }

    file.close();
}

void Resources::LoadBodys(){
    int maxBodys;
    ifstream file("Resources/bodys.bin", ios::in|ios::binary);

    if (!file.is_open()){
        cout << "No existe el archivo 'bodys.bin'.";
        return;
    }

    file.read((char *)&maxBodys, sizeof(int));
    bodyData = new body[maxBodys];

    for (int i = 0; i < maxBodys; i++){
        file.read((char *)&bodyData[i].numBody[0], sizeof(short));
        file.read((char *)&bodyData[i].numBody[1], sizeof(short));
        file.read((char *)&bodyData[i].numBody[2], sizeof(short));
        file.read((char *)&bodyData[i].numBody[3], sizeof(short));

        if (bodyData[i].numBody[0] != 0){
            InitGrh(bodyData[i].posWalk[0], bodyData[i].numBody[0], false);
            InitGrh(bodyData[i].posWalk[1], bodyData[i].numBody[1], false);
            InitGrh(bodyData[i].posWalk[2], bodyData[i].numBody[2], false);
            InitGrh(bodyData[i].posWalk[3], bodyData[i].numBody[3], false);
        }
    }

    file.close();
}

void Resources::LoadMap(){
    ifstream file("Resources/world.map", ios::in|ios::binary);

    if (!file.is_open()){
        cout << "No existe el archivo 'world.map'.";
        return;
    }

    file.read((char *)&MapInfo.max_tiles_x, sizeof(int));
    file.read((char *)&MapInfo.max_tiles_y, sizeof(int));
    file.read((char *)&MapInfo.typeMap, sizeof(int));
    file.read((char *)&MapInfo.Mario_PosX, sizeof(int));
    file.read((char *)&MapInfo.Mario_PosY, sizeof(int));
    file.read((char *)&MapInfo.TOTAL_NPCs, sizeof(int));
    file.read((char *)&MapInfo.TOTAL_OBJs, sizeof(int));
    file.read((char *)&MapInfo.PosCastle, sizeof(int));

    NPCs = new NPC[MapInfo.TOTAL_NPCs];
    OBJs = new MapOBJ[MapInfo.TOTAL_OBJs];

    MapData = new mapData * [MapInfo.max_tiles_x];
    for(int i = 0; i < MapInfo.max_tiles_x; i++) MapData[i] = new mapData[MapInfo.max_tiles_y];

    for (int y = 0; y < MapInfo.max_tiles_y; y++){
        for (int x = 0; x < MapInfo.max_tiles_x; x++){
            file.read((char *)&MapData[x][y].Layer[0].grhIndex, sizeof(MapData[x][y].Layer[0].grhIndex));
            if(MapData[x][y].Layer[0].grhIndex != 0){
                InitGrh(MapData[x][y].Layer[0], MapData[x][y].Layer[0].grhIndex, true);
            }

            file.read((char *)&MapData[x][y].Layer[1].grhIndex, sizeof(MapData[x][y].Layer[1].grhIndex));
            if(MapData[x][y].Layer[1].grhIndex != 0){
                InitGrh(MapData[x][y].Layer[1], MapData[x][y].Layer[1].grhIndex, true);
            }

            file.read((char *)&MapData[x][y].blocked, sizeof(MapData[x][y].blocked));

            file.read((char *)&MapData[x][y].typeNPC, sizeof(MapData[x][y].typeNPC));
            file.read((char *)&MapData[x][y].NPCIndex, sizeof(MapData[x][y].NPCIndex));

            if(MapData[x][y].typeNPC != tNPC::NONE_CHAR){
                NPCs[MapData[x][y].NPCIndex - 1] = NPC(x, y, 2, MapData[x][y].typeNPC);
            }

            file.read((char *)&MapData[x][y].typeOBJ, sizeof(MapData[x][y].typeOBJ));
            file.read((char *)&MapData[x][y].OBJIndex, sizeof(MapData[x][y].OBJIndex));

            if(MapData[x][y].typeOBJ != NONE_OBJ){
               OBJs[MapData[x][y].OBJIndex - 1] = MapOBJ(x, y, MapData[x][y].typeOBJ);
            }
        }
    }

    file.close();
}

void Resources::InitGrh(grh &Grh, short grhindex, bool started){
    Grh.grhIndex = grhindex;
    Grh.frameCounter = 1;

    if (started){
        if (GrhData[Grh.grhIndex].numFrames > 1){
            Grh.started = true;
        } else {
            Grh.started = false;
        }
    } else {
        if (GrhData[Grh.grhIndex].numFrames == 1) started = false;
        Grh.started = started;
    }

    if (Grh.started){
        Grh.loops = -1;
    } else {
        Grh.loops = 0;
    }

    Grh.frameCounter = 1;
    Grh.speed = GrhData[Grh.grhIndex].speed;
}

void Resources::LoadFonts(){
    ifstream file("Resources/fonts.bin", ios::in|ios::binary);

    if (!file.is_open()){
        cout << "No existe el archivo 'fonts.bin'.";
        return;
    }

    for (int i = 0; i < 255; i++){
        file.read((char *)&font_type[0].ascii_code[i], sizeof(int));
        file.read((char *)&font_type[1].ascii_code[i], sizeof(int));
    }

    file.close();
}

void Resources::LoadConfig(){
    ifstream file("Resources/cfg.bin", ios::in|ios::binary);

    if (!file.is_open()){
        cout << "No existe el archivo 'cfg.bin'.";
        return;
    }

    file.read((char *)&Config.screen_width, sizeof(int));
    file.read((char *)&Config.screen_height, sizeof(int));
    file.read((char *)&Config.volume, sizeof(int));
    file.read((char *)&Config.fullscreen, sizeof(bool));
    file.read((char *)&Config.capFPS, sizeof(bool));
    file.read((char *)&Config.vsync, sizeof(bool));

    file.close();
}

void Resources::ToRewriteConfig(){
    ofstream file ("Resources/cfg.bin", ios::out | ios::binary);

	if (file.fail()){
		cout << "ERROR: No se pudo abrir el archivo cfg.bin" << endl;
		return;
	}

	file.seekp(0, ios::beg); // pos file.

	file.write((char *)&Config.screen_width, sizeof(Config.screen_width));
	file.write((char *)&Config.screen_height, sizeof(Config.screen_height));
	file.write((char *)&Config.volume, sizeof(Config.volume));
	file.write((char *)&Config.fullscreen, sizeof(Config.fullscreen));
	file.write((char *)&Config.capFPS, sizeof(Config.capFPS));
	file.write((char *)&Config.vsync, sizeof(Config.vsync));

	file.close(); // cerramos el archivo
}
