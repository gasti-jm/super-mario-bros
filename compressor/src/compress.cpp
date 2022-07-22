#include "compress.h"

fonts font_types[2];
body bodyData[4];
mapData MapData[224][15];
mapInfo MapInfo;
int MAX_GRAPHICS = 0;
int VersionGRHDATA = 0;
grhData *GrhData;
string linea;



void comprimirCFG(){
	cfg CFG_WRITE;

	// set initial variables.
	CFG_WRITE.screen_width = 640;
	CFG_WRITE.screen_height = 480;
	CFG_WRITE.volume = 0;
	CFG_WRITE.fullscreen = false;
	CFG_WRITE.capFPS = false;
	CFG_WRITE.vsync = false;

	ofstream file ("cfg.bin", ios::out | ios::binary);

	if (file.fail()){
		cout << "No se pudo abrir el archivo";
		return; // salir del programa.
	}

	file.seekp(0, ios::beg); // pos file.

	file.write((char *)&CFG_WRITE.screen_width, sizeof(CFG_WRITE.screen_width));
	file.write((char *)&CFG_WRITE.screen_height, sizeof(CFG_WRITE.screen_height));
	file.write((char *)&CFG_WRITE.volume, sizeof(CFG_WRITE.volume));
	file.write((char *)&CFG_WRITE.fullscreen, sizeof(CFG_WRITE.fullscreen));
	file.write((char *)&CFG_WRITE.capFPS, sizeof(CFG_WRITE.capFPS));
	file.write((char *)&CFG_WRITE.vsync, sizeof(CFG_WRITE.vsync));


	file.close(); // cerramos el archivo
}

void comprimirFonts(){
	LoadFonts();

	ofstream file ("fonts.bin", ios::out | ios::binary);

	if (file.fail()){
		cout << "No se pudo abrir el archivo";
		return;
	}

	file.seekp(0, ios::beg); // pos file.

	for (int i = 0; i < 255; i++){
		file.write((char *)&font_types[0].ascii_code[i], sizeof(font_types[0].ascii_code[i]));
		file.write((char *)&font_types[1].ascii_code[i], sizeof(font_types[1].ascii_code[i]));
	}

	file.close(); // cerramos el archivo
}

void comprimirBodys(){
    LoadBodys();
    int maxBodys = 4;

    ofstream file ("bodys.bin", ios::out | ios::binary);

	if (file.fail()){
		cout << "No se pudo abrir el archivo bodys.bin";
		return;
	}

	file.seekp(0, ios::beg); // pos 0 in the file.

	file.write((char *)&maxBodys, sizeof(maxBodys));

    for(int i = 0; i < maxBodys; i++){
        file.write((char *)&bodyData[i].numBody[0], sizeof(bodyData[i].numBody[0]));
        file.write((char *)&bodyData[i].numBody[1], sizeof(bodyData[i].numBody[1]));
        file.write((char *)&bodyData[i].numBody[2], sizeof(bodyData[i].numBody[2]));
        file.write((char *)&bodyData[i].numBody[3], sizeof(bodyData[i].numBody[3]));
    }

    file.close(); // cerramos el archivo
}

void comprimirMapa(){
    LoadMap();

    ofstream file ("world.map", ios::out | ios::binary);

	if (file.fail()){
		cout << "No se pudo abrir el archivo world.map";
		return;
	}

	file.seekp(0, ios::beg); // pos 0 in the file.

	file.write((char *)&MapInfo.max_tiles_x, sizeof(MapInfo.max_tiles_x));
    file.write((char *)&MapInfo.max_tiles_y, sizeof(MapInfo.max_tiles_y));
    file.write((char *)&MapInfo.typeMap, sizeof(MapInfo.typeMap));
    file.write((char *)&MapInfo.Mario_PosX, sizeof(MapInfo.Mario_PosX));
    file.write((char *)&MapInfo.Mario_PosY, sizeof(MapInfo.Mario_PosY));
    file.write((char *)&MapInfo.TOTAL_NPCs, sizeof(MapInfo.TOTAL_NPCs));
    file.write((char *)&MapInfo.TOTAL_OBJs, sizeof(MapInfo.TOTAL_OBJs));
    file.write((char *)&MapInfo.PosCastle, sizeof(MapInfo.PosCastle));

    for (int y = 0; y < MapInfo.max_tiles_y; y++){
        for (int x = 0; x < MapInfo.max_tiles_x; x++){
            file.write((char *)&MapData[x][y].Layer[0].grhIndex, sizeof(MapData[x][y].Layer[0].grhIndex));
            file.write((char *)&MapData[x][y].Layer[1].grhIndex, sizeof(MapData[x][y].Layer[1].grhIndex));

            file.write((char *)&MapData[x][y].blocked, sizeof(MapData[x][y].blocked));

            file.write((char *)&MapData[x][y].typeNPC, sizeof(MapData[x][y].typeNPC));
            file.write((char *)&MapData[x][y].NPCIndex, sizeof(MapData[x][y].NPCIndex));

            file.write((char *)&MapData[x][y].typeOBJ, sizeof(MapData[x][y].typeOBJ));
            file.write((char *)&MapData[x][y].OBJIndex, sizeof(MapData[x][y].OBJIndex));
        }
    }

    file.close(); // cerramos el archivo
}

void comprimirGraficos(){
    if (LoadGraphics() == false) return;

    ofstream file ("graphics.ind", ios::out | ios::binary);

    if (file.fail()){
		cout << "No se pudo abrir el archivo";
		return;
	}

	file.seekp(0, ios::beg); // pos file.

	VersionGRHDATA++;
	file.write((char *)&VersionGRHDATA, sizeof(int)); // version del archivo (esto es solo para pruebas).
    file.write((char *)&MAX_GRAPHICS, sizeof(int));

    for (int i = 1; i <= MAX_GRAPHICS; i++){
        file.write((char *)&i, sizeof(int));
        file.write((char *)&GrhData[i].numFrames, sizeof(short));

        if (GrhData[i].numFrames > 1){
            for(int frame = 1; frame <= GrhData[i].numFrames; frame++){
                file.write((char *)&GrhData[i].frames[frame], sizeof(int));
            }

            file.write((char *)&GrhData[i].speed, sizeof(float));
        } else {
            file.write((char *)&GrhData[i].filenum, sizeof(int));
            file.write((char *)&GrhData[i].sX, sizeof(short));
            file.write((char *)&GrhData[i].sY, sizeof(short));
            file.write((char *)&GrhData[i].pixelWidth, sizeof(short));
            file.write((char *)&GrhData[i].pixelHeight, sizeof(short));
        }
    }

	file.close();
}

void LoadFonts(){
    // primero setteo a todos como 0 y luego le asigno.
    for (int i = 0; i < 255; i++){
        font_types[0].ascii_code[i] = 0;
        font_types[1].ascii_code[i] = 0;
    }

    // chars according to ascii code
    // font_types[0] - principal font
    // font_types[1] - small font for score.

    // example:  font_types[typefont].ascii_code[ASCII CODE] = GRHINDEX;

    // numbers - 0 1 2 3 4 5 6 7 8 9
    font_types[0].ascii_code[48] = 67;
    font_types[0].ascii_code[49] = 68;
    font_types[0].ascii_code[50] = 69;
    font_types[0].ascii_code[51] = 70;
    font_types[0].ascii_code[52] = 71;
    font_types[0].ascii_code[53] = 72;
    font_types[0].ascii_code[54] = 73;
    font_types[0].ascii_code[55] = 74;
    font_types[0].ascii_code[56] = 75;
    font_types[0].ascii_code[57] = 76;

    // abcdario
    for (int a = 0; a <= 25; a++){
        font_types[0].ascii_code[a + 97] = 84 + a; // minuscula
        font_types[0].ascii_code[a + 65] = 84 + a; // mayuscula
    }

    // x minuscula
    font_types[0].ascii_code[120] = 113;

    // special chars
    font_types[0].ascii_code[43] = 62;
    font_types[0].ascii_code[44] = 63;
    font_types[0].ascii_code[45] = 64;
    font_types[0].ascii_code[46] = 65;
    font_types[0].ascii_code[47] = 66;
    font_types[0].ascii_code[58] = 77;
    font_types[0].ascii_code[59] = 78;
    font_types[0].ascii_code[60] = 79;
    font_types[0].ascii_code[61] = 80;
    font_types[0].ascii_code[62] = 81;
    font_types[0].ascii_code[63] = 82;
    font_types[0].ascii_code[184] = 83;
    font_types[0].ascii_code[34] = 110;
    font_types[0].ascii_code[95] = 111;

    // small font for the score.
    // numbers.
    font_types[1].ascii_code[48] = 203;
    font_types[1].ascii_code[49] = 204;
    font_types[1].ascii_code[50] = 205;
    font_types[1].ascii_code[51] = 206;
    font_types[1].ascii_code[52] = 207;
    font_types[1].ascii_code[53] = 208;
    font_types[1].ascii_code[54] = 209;
    font_types[1].ascii_code[55] = 210;
    font_types[1].ascii_code[56] = 211;
    font_types[1].ascii_code[57] = 212;

    font_types[1].ascii_code[117] = 225; // u
    font_types[1].ascii_code[85] = 225; // U
    font_types[1].ascii_code[112] = 226; //p
    font_types[1].ascii_code[80] = 226; //P

}

void LoadBodys(){
    // mario small
    bodyData[0].numBody[0] = 6; // jump
    bodyData[0].numBody[1] = 11; // crouched or dead
    bodyData[0].numBody[2] = 21; // walk
    bodyData[0].numBody[3] = 9; // caught on the flag

    // NPC hongo
    bodyData[1].numBody[0] = 55; // jump (does not have)
    bodyData[1].numBody[1] = 55; // crouched or dead
    bodyData[1].numBody[2] = 182; // walk
    bodyData[1].numBody[3] = 55; // NOTHING

    // NPC Tortuga
    bodyData[2].numBody[0] = 193; // jump (does not have)
    bodyData[2].numBody[1] = 193; // crouched or dead
    bodyData[2].numBody[2] = 194; // walk
    bodyData[2].numBody[3] = 193; // NOTHING

    // NPC Tortuga ALAS
    bodyData[3].numBody[0] = 193; // jump (does not have)
    bodyData[3].numBody[1] = 193; // crouched or dead
    bodyData[3].numBody[2] = 195; // walk
    bodyData[3].numBody[3] = 193; // NOTHING
}

bool LoadGraphics(){
    ifstream file("graphics.ini");
    string param, numGrh, VersionFile;

    if (!file.is_open()){
		cout << "No se pudo abrir el archivo 'graphics.ini'" << endl;
		return false;
	}

    getline(file, linea);
	getline(file, linea);
	stringstream stream1(linea);
	getline(stream1, param, '='); // from
	getline(stream1, param, '\n'); // to
	numGrh = param;

	getline(file, linea);
	stringstream stream2(linea);
	getline(stream2, param, '='); // from
	getline(stream2, param, '\n'); // to
	VersionFile = param;

	MAX_GRAPHICS = atoi(numGrh.c_str());
	VersionGRHDATA = atoi(VersionFile.c_str());
	GrhData = new grhData[MAX_GRAPHICS + 1];

	getline(file, linea);
    getline(file, linea);
    getline(file, linea);

    for (int i = 1; i <= MAX_GRAPHICS; i++){
        //cout << "Grh" << i << "=";
        GrhData[i].numFrames = atoi(getValueFile('=', '-').c_str());
        //cout << GrhData[i].numFrames << "-";

        GrhData[i].frames = new int[GrhData[i].numFrames];
        if (GrhData[i].numFrames > 1){
            for(int frame = 1; frame <= GrhData[i].numFrames; frame++){
                GrhData[i].frames[frame] = atoi(getValueFile('-', '-', false).c_str());
                //cout << GrhData[i].frames[frame] << "-";
            }
            GrhData[i].speed = atoi(getValueFile('-', '-', false).c_str());
            //cout << GrhData[i].speed;

            GrhData[i].pixelHeight = GrhData[GrhData[i].frames[1]].pixelHeight;
            GrhData[i].pixelWidth = GrhData[GrhData[i].frames[1]].pixelWidth;
            GrhData[i].tileWidth = GrhData[GrhData[i].frames[1]].tileWidth;
            GrhData[i].tileHeight = GrhData[GrhData[i].frames[1]].tileHeight;
        } else {
            GrhData[i].filenum = atoi(getValueFile('-', '-', false).c_str());
            //cout << GrhData[i].filenum << "-";

            GrhData[i].sX = atoi(getValueFile('-', '-', false).c_str());
            //cout << GrhData[i].sX << "-";

            GrhData[i].sY = atoi(getValueFile('-', '-', false).c_str());
            //cout << GrhData[i].sY << "-";

            GrhData[i].pixelWidth = atoi(getValueFile('-', '-', false).c_str());
            //cout << GrhData[i].pixelWidth << "-";

            GrhData[i].pixelHeight = atoi(getValueFile('-', '-', false).c_str());
            //cout << GrhData[i].pixelHeight;

            GrhData[i].tileWidth = (float)GrhData[i].pixelWidth / 32;
            GrhData[i].tileHeight = (float)GrhData[i].pixelHeight / 32;
            GrhData[i].frames[1] = i;
        }

        //cout << endl;
        getline(file, linea);
    }

    file.close();

    return true;
}

string getValueFile(char from, char to, bool needFrom){
    stringstream stream(linea);
    string param;

    if(needFrom) getline(stream, param, from);
    getline(stream, param, to);
    getline(stream, linea, '\n');

    return param;
}

void LoadMap(){
    MapInfo.max_tiles_x = 224;
    MapInfo.max_tiles_y = 15;
    MapInfo.typeMap = OVERWORLD;
    MapInfo.Mario_PosX = 2;
    MapInfo.Mario_PosY = 12;
    MapInfo.TOTAL_NPCs = 17; // para la creacion total del objeto NPC.
    MapInfo.TOTAL_OBJs = 47;
    MapInfo.PosCastle = 204;

    for (int y = 0; y < MapInfo.max_tiles_y; y++){
        for (int x = 0; x < MapInfo.max_tiles_x; x++){
            MapData[x][y].Layer[0].grhIndex = 0;
            MapData[x][y].Layer[1].grhIndex = 0;
            MapData[x][y].typeNPC = tNPC::NONE_CHAR;
            MapData[x][y].NPCIndex = 0;
            MapData[x][y].typeOBJ = tOBJ::NONE_OBJ;
            MapData[x][y].OBJIndex = 0;

            // NPCS - //////////////////////////////////////////////////////////
            if (x == 22 && y == 12){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 1;
            }

            if (x == 40 && y == 12){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 2;
            }

            if (x == 51 && y == 12){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 3;
            }

            if (x == 53 && y == 12){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 4;
            }

            if (x == 80 && y == 4){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 5;
            }

            if (x == 82 && y == 4){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 6;
            }

            if (x == 97 && y == 12){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 7;
            }

            if (x == 99 && y == 12){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 8;
            }

            if (x == 107 && y == 12){
                MapData[x][y].typeNPC = tNPC::TURTLE_GREEN;
                MapData[x][y].NPCIndex = 9;
            }

            if (x == 114 && y == 12){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 10;
            }

            if (x == 116 && y == 12){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 11;
            }

            if (x == 124 && y == 12){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 12;
            }

            if (x == 126 && y == 12){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 13;
            }

            if (x == 128 && y == 12){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 14;
            }

            if (x == 130 && y == 12){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 15;
            }

            if (x == 174 && y == 12){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 16;
            }

            if (x == 176 && y == 12){
                MapData[x][y].typeNPC = tNPC::HONGE;
                MapData[x][y].NPCIndex = 17;
            }

            // NUBES //////////////////////////////////////////////////////////////////////////////
            if (x == 10 && y == 3) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 11 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 12 && y == 3) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 10 && y == 4) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 11 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 12 && y == 4) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 19 && y == 2) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 20 && y == 2) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 21 && y == 2) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 19 && y == 3) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 20 && y == 3) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 21 && y == 3) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 27 && y == 3) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 28 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 29 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 30 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 31 && y == 3) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 27 && y == 4) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 28 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 29 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 30 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 31 && y == 4) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 36 && y == 2) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 37 && y == 2) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 38 && y == 2) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 39 && y == 2) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 36 && y == 3) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 37 && y == 3) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 38 && y == 3) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 39 && y == 3) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 56 && y == 3) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 57 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 58 && y == 3) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 56 && y == 4) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 57 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 58 && y == 4) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 67 && y == 2) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 68 && y == 2) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 69 && y == 2) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 67 && y == 3) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 68 && y == 3) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 69 && y == 3) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 75 && y == 3) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 76 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 77 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 78 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 79 && y == 3) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 75 && y == 4) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 76 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 77 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 78 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 79 && y == 4) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 84 && y == 2) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 85 && y == 2) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 86 && y == 2) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 87 && y == 2) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 84 && y == 3) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 85 && y == 3) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 86 && y == 3) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 87 && y == 3) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 104 && y == 3) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 105 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 106 && y == 3) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 104 && y == 4) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 105 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 106 && y == 4) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 115 && y == 2) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 116 && y == 2) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 117 && y == 2) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 115 && y == 3) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 116 && y == 3) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 117 && y == 3) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 123 && y == 3) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 124 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 125 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 126 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 127 && y == 3) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 123 && y == 4) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 124 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 125 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 126 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 127 && y == 4) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 132 && y == 2) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 133 && y == 2) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 134 && y == 2) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 135 && y == 2) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 132 && y == 3) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 133 && y == 3) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 134 && y == 3) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 135 && y == 3) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 152 && y == 3) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 153 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 154 && y == 3) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 152 && y == 4) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 153 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 154 && y == 4) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 163 && y == 2) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 164 && y == 2) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 165 && y == 2) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 163 && y == 3) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 164 && y == 3) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 165 && y == 3) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 171 && y == 3) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 172 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 173 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 174 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 175 && y == 3) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 171 && y == 4) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 172 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 173 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 174 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 175 && y == 4) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 180 && y == 2) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 181 && y == 2) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 182 && y == 2) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 183 && y == 2) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 180 && y == 3) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 181 && y == 3) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 182 && y == 3) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 183 && y == 3) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 200 && y == 3) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 201 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 202 && y == 3) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 200 && y == 4) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 201 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 202 && y == 4) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 211 && y == 2) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 212 && y == 2) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 213 && y == 2) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 211 && y == 3) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 212 && y == 3) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 213 && y == 3) MapData[x][y].Layer[0].grhIndex = 129;
            if (x == 219 && y == 3) MapData[x][y].Layer[0].grhIndex = 124;
            if (x == 220 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 221 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 222 && y == 3) MapData[x][y].Layer[0].grhIndex = 125;
            if (x == 223 && y == 3) MapData[x][y].Layer[0].grhIndex = 126;
            if (x == 219 && y == 4) MapData[x][y].Layer[0].grhIndex = 127;
            if (x == 220 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 221 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 222 && y == 4) MapData[x][y].Layer[0].grhIndex = 128;
            if (x == 223 && y == 4) MapData[x][y].Layer[0].grhIndex = 129;

            // Montañas ///////////////////////////////////////////////////////////////////////////
            if (x == 0 && y == 12) MapData[x][y].Layer[0].grhIndex = 117;
            if (x == 1 && y == 12) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 2 && y == 12) MapData[x][y].Layer[0].grhIndex = 120;
            if (x == 3 && y == 12) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 4 && y == 12) MapData[x][y].Layer[0].grhIndex = 119;
            if (x == 1 && y == 11) MapData[x][y].Layer[0].grhIndex = 117;
            if (x == 2 && y == 11) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 3 && y == 11) MapData[x][y].Layer[0].grhIndex = 119;
            if (x == 2 && y == 10) MapData[x][y].Layer[0].grhIndex = 116;
            if (x == 16 && y == 12) MapData[x][y].Layer[0].grhIndex = 117;
            if (x == 17 && y == 12) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 18 && y == 12) MapData[x][y].Layer[0].grhIndex = 119;
            if (x == 17 && y == 11) MapData[x][y].Layer[0].grhIndex = 116;
            if (x == 48 && y == 12) MapData[x][y].Layer[0].grhIndex = 117;
            if (x == 49 && y == 12) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 50 && y == 12) MapData[x][y].Layer[0].grhIndex = 120;
            if (x == 51 && y == 12) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 52 && y == 12) MapData[x][y].Layer[0].grhIndex = 119;
            if (x == 49 && y == 11) MapData[x][y].Layer[0].grhIndex = 117;
            if (x == 50 && y == 11) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 51 && y == 11) MapData[x][y].Layer[0].grhIndex = 119;
            if (x == 50 && y == 10) MapData[x][y].Layer[0].grhIndex = 116;
            if (x == 64 && y == 12) MapData[x][y].Layer[0].grhIndex = 117;
            if (x == 65 && y == 12) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 66 && y == 12) MapData[x][y].Layer[0].grhIndex = 119;
            if (x == 65 && y == 11) MapData[x][y].Layer[0].grhIndex = 116;
            if (x == 96 && y == 12) MapData[x][y].Layer[0].grhIndex = 117;
            if (x == 97 && y == 12) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 98 && y == 12) MapData[x][y].Layer[0].grhIndex = 120;
            if (x == 99 && y == 12) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 100 && y == 12) MapData[x][y].Layer[0].grhIndex = 119;
            if (x == 97 && y == 11) MapData[x][y].Layer[0].grhIndex = 117;
            if (x == 98 && y == 11) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 99 && y == 11) MapData[x][y].Layer[0].grhIndex = 119;
            if (x == 98 && y == 10) MapData[x][y].Layer[0].grhIndex = 116;
            if (x == 112 && y == 12) MapData[x][y].Layer[0].grhIndex = 117;
            if (x == 113 && y == 12) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 114 && y == 12) MapData[x][y].Layer[0].grhIndex = 119;
            if (x == 113 && y == 11) MapData[x][y].Layer[0].grhIndex = 116;
            if (x == 144 && y == 12) MapData[x][y].Layer[0].grhIndex = 117;
            if (x == 145 && y == 12) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 146 && y == 12) MapData[x][y].Layer[0].grhIndex = 120;
            if (x == 147 && y == 12) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 145 && y == 11) MapData[x][y].Layer[0].grhIndex = 117;
            if (x == 146 && y == 11) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 147 && y == 11) MapData[x][y].Layer[0].grhIndex = 119;
            if (x == 146 && y == 10) MapData[x][y].Layer[0].grhIndex = 116;
            if (x == 160 && y == 12) MapData[x][y].Layer[0].grhIndex = 117;
            if (x == 161 && y == 12) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 162 && y == 12) MapData[x][y].Layer[0].grhIndex = 119;
            if (x == 161 && y == 11) MapData[x][y].Layer[0].grhIndex = 116;
            if (x == 192 && y == 12) MapData[x][y].Layer[0].grhIndex = 117;
            if (x == 193 && y == 12) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 194 && y == 12) MapData[x][y].Layer[0].grhIndex = 120;
            if (x == 195 && y == 12) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 196 && y == 12) MapData[x][y].Layer[0].grhIndex = 119;
            if (x == 193 && y == 11) MapData[x][y].Layer[0].grhIndex = 117;
            if (x == 194 && y == 11) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 195 && y == 11) MapData[x][y].Layer[0].grhIndex = 119;
            if (x == 194 && y == 10) MapData[x][y].Layer[0].grhIndex = 116;
            if (x == 208 && y == 12) MapData[x][y].Layer[0].grhIndex = 117;
            if (x == 209 && y == 12) MapData[x][y].Layer[0].grhIndex = 118;
            if (x == 210 && y == 12) MapData[x][y].Layer[0].grhIndex = 119;
            if (x == 209 && y == 11) MapData[x][y].Layer[0].grhIndex = 116;

            // YERBAS ///////////////////////////////////////////////////////////////////////////
            if (x == 11 && y == 12) MapData[x][y].Layer[0].grhIndex = 121;
            if (x == 12 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 13 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 14 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 15 && y == 12) MapData[x][y].Layer[0].grhIndex = 123;
            if (x == 23 && y == 12) MapData[x][y].Layer[0].grhIndex = 121;
            if (x == 24 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 25 && y == 12) MapData[x][y].Layer[0].grhIndex = 123;
            if (x == 41 && y == 12) MapData[x][y].Layer[0].grhIndex = 121;
            if (x == 42 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 43 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 44 && y == 12) MapData[x][y].Layer[0].grhIndex = 123;
            if (x == 59 && y == 12) MapData[x][y].Layer[0].grhIndex = 121;
            if (x == 60 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 61 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 62 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 63 && y == 12) MapData[x][y].Layer[0].grhIndex = 123;
            if (x == 71 && y == 12) MapData[x][y].Layer[0].grhIndex = 121;
            if (x == 72 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 73 && y == 12) MapData[x][y].Layer[0].grhIndex = 123;
            if (x == 89 && y == 12) MapData[x][y].Layer[0].grhIndex = 121;
            if (x == 90 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 91 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 92 && y == 12) MapData[x][y].Layer[0].grhIndex = 123;
            if (x == 107 && y == 12) MapData[x][y].Layer[0].grhIndex = 121;
            if (x == 108 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 109 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 110 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 111 && y == 12) MapData[x][y].Layer[0].grhIndex = 123;
            if (x == 119 && y == 12) MapData[x][y].Layer[0].grhIndex = 121;
            if (x == 120 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 121 && y == 12) MapData[x][y].Layer[0].grhIndex = 123;
            if (x == 138 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 139 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 159 && y == 12) MapData[x][y].Layer[0].grhIndex = 123;
            if (x == 167 && y == 12) MapData[x][y].Layer[0].grhIndex = 121;
            if (x == 168 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 169 && y == 12) MapData[x][y].Layer[0].grhIndex = 123;
            if (x == 207 && y == 12) MapData[x][y].Layer[0].grhIndex = 123;
            if (x == 215 && y == 12) MapData[x][y].Layer[0].grhIndex = 121;
            if (x == 216 && y == 12) MapData[x][y].Layer[0].grhIndex = 122;
            if (x == 217 && y == 12) MapData[x][y].Layer[0].grhIndex = 123;

            // CONOS ///////////////////////////////////////////////////////////////////////////
            if (x == 28 && y == 11) {MapData[x][y].Layer[1].grhIndex = 142; MapData[x][y].blocked = true;}
            if (x == 29 && y == 11) {MapData[x][y].Layer[1].grhIndex = 143; MapData[x][y].blocked = true;}
            if (x == 28 && y == 12) {MapData[x][y].Layer[1].grhIndex = 144; MapData[x][y].blocked = true;}
            if (x == 29 && y == 12) {MapData[x][y].Layer[1].grhIndex = 145; MapData[x][y].blocked = true;}
            if (x == 38 && y == 10) {MapData[x][y].Layer[1].grhIndex = 142; MapData[x][y].blocked = true;}
            if (x == 39 && y == 10) {MapData[x][y].Layer[1].grhIndex = 143; MapData[x][y].blocked = true;}
            if (x == 38 && y == 11) {MapData[x][y].Layer[1].grhIndex = 144; MapData[x][y].blocked = true;}
            if (x == 39 && y == 11) {MapData[x][y].Layer[1].grhIndex = 145; MapData[x][y].blocked = true;}
            if (x == 38 && y == 12) {MapData[x][y].Layer[1].grhIndex = 144; MapData[x][y].blocked = true;}
            if (x == 39 && y == 12) {MapData[x][y].Layer[1].grhIndex = 145; MapData[x][y].blocked = true;}
            if (x == 46 && y == 9) {MapData[x][y].Layer[1].grhIndex = 142; MapData[x][y].blocked = true;}
            if (x == 47 && y == 9) {MapData[x][y].Layer[1].grhIndex = 143; MapData[x][y].blocked = true;}
            if (x == 46 && y == 10) {MapData[x][y].Layer[1].grhIndex = 144; MapData[x][y].blocked = true;}
            if (x == 47 && y == 10) {MapData[x][y].Layer[1].grhIndex = 145; MapData[x][y].blocked = true;}
            if (x == 46 && y == 11) {MapData[x][y].Layer[1].grhIndex = 144; MapData[x][y].blocked = true;}
            if (x == 47 && y == 11) {MapData[x][y].Layer[1].grhIndex = 145; MapData[x][y].blocked = true;}
            if (x == 46 && y == 12) {MapData[x][y].Layer[1].grhIndex = 144; MapData[x][y].blocked = true;}
            if (x == 47 && y == 12) {MapData[x][y].Layer[1].grhIndex = 145; MapData[x][y].blocked = true;}
            if (x == 57 && y == 9) {MapData[x][y].Layer[1].grhIndex = 142; MapData[x][y].blocked = true;}
            if (x == 58 && y == 9) {MapData[x][y].Layer[1].grhIndex = 143; MapData[x][y].blocked = true;}
            if (x == 57 && y == 10) {MapData[x][y].Layer[1].grhIndex = 144; MapData[x][y].blocked = true;}
            if (x == 58 && y == 10) {MapData[x][y].Layer[1].grhIndex = 145; MapData[x][y].blocked = true;}
            if (x == 57 && y == 11) {MapData[x][y].Layer[1].grhIndex = 144; MapData[x][y].blocked = true;}
            if (x == 58 && y == 11) {MapData[x][y].Layer[1].grhIndex = 145; MapData[x][y].blocked = true;}
            if (x == 57 && y == 12) {MapData[x][y].Layer[1].grhIndex = 144; MapData[x][y].blocked = true;}
            if (x == 58 && y == 12) {MapData[x][y].Layer[1].grhIndex = 145; MapData[x][y].blocked = true;}
            if (x == 163 && y == 11) {MapData[x][y].Layer[1].grhIndex = 142; MapData[x][y].blocked = true;}
            if (x == 164 && y == 11) {MapData[x][y].Layer[1].grhIndex = 143; MapData[x][y].blocked = true;}
            if (x == 163 && y == 12) {MapData[x][y].Layer[1].grhIndex = 144; MapData[x][y].blocked = true;}
            if (x == 164 && y == 12) {MapData[x][y].Layer[1].grhIndex = 145; MapData[x][y].blocked = true;}
            if (x == 179 && y == 11) {MapData[x][y].Layer[1].grhIndex = 142; MapData[x][y].blocked = true;}
            if (x == 180 && y == 11) {MapData[x][y].Layer[1].grhIndex = 143; MapData[x][y].blocked = true;}
            if (x == 179 && y == 12) {MapData[x][y].Layer[1].grhIndex = 144; MapData[x][y].blocked = true;}
            if (x == 180 && y == 12) {MapData[x][y].Layer[1].grhIndex = 145; MapData[x][y].blocked = true;}

            // OBJETOS ///////////////////////////////////////////////////////////////////////////

            if (x == 16 && y == 9){
                MapData[x][y].OBJIndex = 1;
                MapData[x][y].typeOBJ = BOX_COIN;
            }

            if (x == 20 && y == 9){
                MapData[x][y].OBJIndex = 2;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 21 && y == 9){
                MapData[x][y].OBJIndex = 3;
                MapData[x][y].typeOBJ = BOX_COIN;
            }

            if (x == 22 && y == 9){
                MapData[x][y].OBJIndex = 4;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 23 && y == 9){
                MapData[x][y].OBJIndex = 5;
                MapData[x][y].typeOBJ = BOX_COIN;
            }

            if (x == 24 && y == 9){
                MapData[x][y].OBJIndex = 6;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 22 && y == 5){
                MapData[x][y].OBJIndex = 7;
                MapData[x][y].typeOBJ = BOX_COIN;
            }

            if (x == 64 && y == 7){
                MapData[x][y].OBJIndex = 8;
                MapData[x][y].typeOBJ = GREEN_HONGE;
            }

            if (x == 77 && y == 9){
                MapData[x][y].OBJIndex = 9;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 78 && y == 9){
                MapData[x][y].OBJIndex = 10;
                MapData[x][y].typeOBJ = BOX_COIN; // HONGE
            }

            if (x == 79 && y == 9){
                MapData[x][y].OBJIndex = 11;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 80 && y == 5){
                MapData[x][y].OBJIndex = 12;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 81 && y == 5){
                MapData[x][y].OBJIndex = 13;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 82 && y == 5){
                MapData[x][y].OBJIndex = 14;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 83 && y == 5){
                MapData[x][y].OBJIndex = 15;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 84 && y == 5){
                MapData[x][y].OBJIndex = 16;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 85 && y == 5){
                MapData[x][y].OBJIndex = 17;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 86 && y == 5){
                MapData[x][y].OBJIndex = 18;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 87 && y == 5){
                MapData[x][y].OBJIndex = 19;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 91 && y == 5){
                MapData[x][y].OBJIndex = 20;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 92 && y == 5){
                MapData[x][y].OBJIndex = 21;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 93 && y == 5){
                MapData[x][y].OBJIndex = 22;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 94 && y == 5){
                MapData[x][y].OBJIndex = 23;
                MapData[x][y].typeOBJ = BOX_COIN;
            }

            if (x == 94 && y == 9){
                MapData[x][y].OBJIndex = 24;
                MapData[x][y].typeOBJ = BLOCK_COIN; // WITH COINS
            }

            if (x == 100 && y == 9){
                MapData[x][y].OBJIndex = 25;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 101 && y == 9){
                MapData[x][y].OBJIndex = 26;
                MapData[x][y].typeOBJ = BLOCK; // WITH STAR
            }

            if (x == 106 && y == 9){
                MapData[x][y].OBJIndex = 27;
                MapData[x][y].typeOBJ = BOX_COIN;
            }

            if (x == 109 && y == 9){
                MapData[x][y].OBJIndex = 28;
                MapData[x][y].typeOBJ = BOX_COIN;
            }

            if (x == 112 && y == 9){
                MapData[x][y].OBJIndex = 29;
                MapData[x][y].typeOBJ = BOX_COIN;
            }

            if (x == 109 && y == 5){
                MapData[x][y].OBJIndex = 30;
                MapData[x][y].typeOBJ = BOX_COIN; // HONGE
            }

            if (x == 118 && y == 9){
                MapData[x][y].OBJIndex = 31;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 121 && y == 5){
                MapData[x][y].OBJIndex = 32;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 122 && y == 5){
                MapData[x][y].OBJIndex = 33;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 123 && y == 5){
                MapData[x][y].OBJIndex = 34;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 128 && y == 5){
                MapData[x][y].OBJIndex = 35;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 129 && y == 5){
                MapData[x][y].OBJIndex = 36;
                MapData[x][y].typeOBJ = BOX_COIN;
            }

            if (x == 130 && y == 5){
                MapData[x][y].OBJIndex = 37;
                MapData[x][y].typeOBJ = BOX_COIN;
            }

            if (x == 129 && y == 9){
                MapData[x][y].OBJIndex = 38;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 130 && y == 9){
                MapData[x][y].OBJIndex = 39;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 131 && y == 5){
                MapData[x][y].OBJIndex = 40;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 168 && y == 9){
                MapData[x][y].OBJIndex = 41;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 169 && y == 9){
                MapData[x][y].OBJIndex = 42;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 170 && y == 9){
                MapData[x][y].OBJIndex = 43;
                MapData[x][y].typeOBJ = BOX_COIN;
            }

            if (x == 171 && y == 9){
                MapData[x][y].OBJIndex = 44;
                MapData[x][y].typeOBJ = BLOCK;
            }

            if (x == 198 && y == 3){
                MapData[x][y].OBJIndex = 45;
                MapData[x][y].typeOBJ = FLAG;
            }

            if (x == 64 && y == 8){
                MapData[x][y].OBJIndex = 46;
                MapData[x][y].typeOBJ = BOX_INVISIBLE_HONGE_OR_FLOWER;
            }

            if (x == 204 && y == 8){
                MapData[x][y].OBJIndex = 47;
                MapData[x][y].typeOBJ = MINI_FLAG;
            }


            // blocks ///////////////////////////////////////////////////////////////////////////

            if (x >= 134 && x <= 137 && y == 12){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if (x >= 135 && x <= 137 && y == 11){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if (x >= 136 && x <= 137 && y == 10){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if (x == 137 && y == 9){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            /////

            if (x == 140 && y == 9){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if (x >= 140 && x <= 141 && y == 10){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if (x >= 140 && x <= 142 && y == 11){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if (x >= 140 && x <= 143 && y == 12){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }


            ////

            if (x >= 148 && x <= 152 && y == 12){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if (x >= 149 && x <= 152 && y == 11){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if (x >= 150 && x <= 152 && y == 10){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if (x >= 151 && x <= 152 && y == 9){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if (x == 155 && y == 9){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if (x >= 155 && x <= 156 && y == 10){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if (x >= 155 && x <= 157 && y == 11){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if (x >= 155 && x <= 158 && y == 12){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            ////

            if(x >= 181 && x <= 189 && y == 12){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }


            if(x >= 182 && x <= 189 && y == 11){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if(x >= 183 && x <= 189 && y == 10){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if(x >= 184 && x <= 189 && y == 9){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if(x >= 185 && x <= 189 && y == 8){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if(x >= 186 && x <= 189 && y == 7){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if(x >= 187 && x <= 189 && y == 6){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if(x >= 188 && x <= 189 && y == 5){
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }

            if(x == 198 && y == 12) {
                MapData[x][y].Layer[0].grhIndex = 36;
                MapData[x][y].blocked = true;
            }


            // palo final
            if (y == 2 && x == 198) MapData[x][y].Layer[0].grhIndex = 176;
            if (y >= 3 && y <= 11 && x == 198) MapData[x][y].Layer[0].grhIndex = 178;
            if (y >= 0 && y <= 4 && x == 199) MapData[x][y].blocked = true;

            // Casita final.
            if (x == 202 && y == 12) MapData[x][y].Layer[0].grhIndex = 200;
            if (x == 203 && y == 12) MapData[x][y].Layer[0].grhIndex = 200;
            if (x == 204 && y == 12) MapData[x][y].Layer[1].grhIndex = 201;
            if (x == 205 && y == 12) MapData[x][y].Layer[0].grhIndex = 200;
            if (x == 206 && y == 12) MapData[x][y].Layer[0].grhIndex = 200;
            if (x == 202 && y == 11) MapData[x][y].Layer[0].grhIndex = 200;
            if (x == 203 && y == 11) MapData[x][y].Layer[0].grhIndex = 200;
            if (x == 204 && y == 11) MapData[x][y].Layer[1].grhIndex = 198;
            if (x == 205 && y == 11) MapData[x][y].Layer[0].grhIndex = 200;
            if (x == 206 && y == 11) MapData[x][y].Layer[0].grhIndex = 200;
            if (x == 202 && y == 10) MapData[x][y].Layer[0].grhIndex = 196;
            if (x == 203 && y == 10) MapData[x][y].Layer[0].grhIndex = 202;
            if (x == 204 && y == 10) MapData[x][y].Layer[0].grhIndex = 202;
            if (x == 205 && y == 10) MapData[x][y].Layer[0].grhIndex = 202;
            if (x == 206 && y == 10) MapData[x][y].Layer[0].grhIndex = 196;
            if (x == 203 && y == 9) MapData[x][y].Layer[1].grhIndex = 197;
            if (x == 204 && y == 9) MapData[x][y].Layer[1].grhIndex = 200;
            if (x == 205 && y == 9) MapData[x][y].Layer[1].grhIndex = 199;
            if (x == 203 && y == 8) MapData[x][y].Layer[1].grhIndex = 196;
            if (x == 204 && y == 8) MapData[x][y].Layer[1].grhIndex = 196;
            if (x == 205 && y == 8) MapData[x][y].Layer[1].grhIndex = 196;


            // piso del mapa.
            if (y == 13 || y == 14){
                if (x != 69 && x != 70 && x != 86 && x != 87 && x != 88 && x != 153 && x != 154){
                    MapData[x][y].blocked = true;
                    MapData[x][y].Layer[0].grhIndex = 14;
                } else {
                    MapData[x][y].blocked = false;
                    MapData[x][y].Layer[0].grhIndex = 0;
                }

            }
        }
    }

}
