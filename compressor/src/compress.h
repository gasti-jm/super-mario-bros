#ifndef COMPRESS_H_INCLUDED
#define COMPRESS_H_INCLUDED

#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include "structs.h"
using namespace std;

void LoadFonts();
void LoadBodys();
void LoadMap();
bool LoadGraphics();

void comprimirCFG();
void comprimirFonts();
void comprimirBodys();
void comprimirMapa();
void comprimirGraficos();

string getValueFile(char, char, bool = true);

#endif
