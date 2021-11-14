#include "gamedata.h"

Character::Character(){
    flipChar = SDL_FLIP_NONE;
    posMapX = 0;
    posMapY = 0;
    posWalk = 2;
    numBody = 0;
    PointsPixelX = 0;
    PointsPixelY = 0;
    ticks = 0;
    isDead = false;
    evolution = 0;
    PixelOffSetX = 0;
    PixelOffSetY = 0;
    VelX = 0;
    VelY = 0;
}

Character::Character(int mapX, int mapY, int pWalk){
    flipChar = SDL_FLIP_NONE;
    posMapX = mapX;
    posMapY = mapY;
    posWalk = pWalk;

    PointsPixelX = 0;
    PointsPixelY = 0;
    ticks = 0;
    isDead = false;
    evolution = 0;
    PixelOffSetX = 0;
    PixelOffSetY = 0;
    VelX = 0;
    VelY = 0;
}

Character::~Character(){

}

SDL_RendererFlip Character::GetFlip(){ return flipChar; }
int Character::GetMapX(){ return posMapX; }
int Character::GetMapY(){ return posMapY; }
int Character::GetNumBody(){ return numBody; }
int Character::GetPWalk(){ return posWalk; }
int Character::GetPPosX(){ return PointsPixelX; }
int Character::GetPPosY(){ return PointsPixelY; }
Uint32 Character::GetTicks() { return ticks; }
bool Character::GetIsDead() { return isDead; }
int Character::GetEvolution() { return evolution; }
int Character::GetPOSetX() { return PixelOffSetX; }
int Character::GetPOSetY() { return PixelOffSetY; }

Mario::Mario(int mapX, int mapY, int pWalk) : Character(mapX, mapY, pWalk){
    isMoving = false;
    isJumping = false;
    inTheAir = false;
    isDead = false;
    inEffectEvolution = false;
    isSmallJump = false;
    inEffectFlag = false;
    winPointsForTime = false;
    EndWin = false;
    pointsFlag = 0;

    numBody = BODY_MARIO_SMALL;

    // sea la posicion tile del mapa que sea siempre se inicia de 0.
    PixelOffSetX = 0;
    PixelOffSetY = 0;
    PointsPixelX = 0;
    PointsPixelY = 0;

    UPs = 3;
    evolution = Evolutions::SMALL;
    time = 400;
    points = 0;
    coins = 0;

    VelY = 0;
    VelX = 0;
    maxJumpPosY = 0;
}

Mario::~Mario(){

}

void Mario::MoveLeftOrRight(bool left){
    isMoving = true;
    if (!inTheAir) posWalk = 2;

    if (left){
        flipChar = SDL_FLIP_HORIZONTAL;
        VelX = -MAX_VEL;
    } else { // move to right
        flipChar = SDL_FLIP_NONE;
        VelX = MAX_VEL;
    }
}

void Mario::Jump(){
    if (inTheAir == false){
        isJumping = true;
        VelY = -MAX_VEL;
    }
}

void Mario::HandleEvent(SDL_Event &e, bool isPaused){
    if(inEffectFlag) return;

    if (isPaused || inEffectEvolution){
        VelX = 0;
        VelY = 0;
        isMoving = false;
        isJumping = false;
        isSmallJump = false;
        return;
    }

    if(e.type == SDL_KEYDOWN && e.key.repeat == 0){
        switch(e.key.keysym.sym){
            case SDLK_UP:
                if (!isSmallJump){
                    maxJumpPosY = posMapY - 6;  // Detectamos el pico maximo de salto.
                    if(isJumping == false && inTheAir == false){
                        if (evolution == Evolutions::SMALL){
                            Audio::PlaySound("jump");
                        } else {
                            Audio::PlaySound("jumpbig"); // mario grande.
                        }
                    }
                    Jump();
                }
                break;

            case SDLK_LEFT:
                MoveLeftOrRight(true);
                break;

            case SDLK_RIGHT:
                MoveLeftOrRight(false);
                break;
        }
    } else if(e.type == SDL_KEYUP && e.key.repeat == 0){
        // Si no preciono o deje de precionar X tecla...
        switch(e.key.keysym.sym){
            case SDLK_UP:
                if (!isSmallJump){
                    isJumping = false;
                    if (VelY == -MAX_VEL) VelY += MAX_VEL;
                }
                break;

            case SDLK_LEFT:
                if (VelX == -MAX_VEL) VelX += MAX_VEL;
                if (VelX == 0) isMoving = false;
                break;

            case SDLK_RIGHT:
                if (VelX == MAX_VEL) VelX -= MAX_VEL;
                if (VelX == 0) isMoving = false;
                break;
        }

    }
}

bool Mario::DetectCollisionButtom(){
    // ni te gastes en analizar cosas que no existen dentro de la matriz
    if (posMapX > Resources::MapInfo.max_tiles_x || posMapY < 0) return false;

    // Mario hit box
    int leftM = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int rightM = leftM + 24; // detectar con la carga de bodys.
    int topM = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomM = topM + TILE_PIXEL_SIZE;


    if(Resources::MapData[ (int)leftM / TILE_PIXEL_SIZE ][ (int) bottomM / TILE_PIXEL_SIZE ].blocked ||
       Resources::MapData[ (int)rightM / TILE_PIXEL_SIZE ][ (int) bottomM / TILE_PIXEL_SIZE ].blocked){
        return true;
    }

    // no hay colision
    return false;
}

bool Mario::DetectCollisionTop(){
    // ni te gastes en analizar cosas que no existen dentro de la matriz
    if (posMapX > Resources::MapInfo.max_tiles_x || posMapY < 0) return false;

    // Mario hit box
    int leftM = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int rightM = leftM + 24; // detectar con la carga de bodys.
    int topM = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int centerTop = leftM + 12;

    if(Resources::MapData[(int)centerTop / TILE_PIXEL_SIZE][ (int) topM / TILE_PIXEL_SIZE ].typeOBJ != NONE_OBJ){
        if(inEffectEvolution == false && isJumping == true){
            if(Resources::OBJs[Resources::MapData[(int)centerTop / TILE_PIXEL_SIZE][(int)topM / TILE_PIXEL_SIZE].OBJIndex - 1].GetEvolution() != 0){
                DarCoins();
                DarPuntos(Resources::OBJs[Resources::MapData[(int)centerTop / TILE_PIXEL_SIZE][(int)topM / TILE_PIXEL_SIZE].OBJIndex - 1].GetPoints());
            }

            Resources::OBJs[Resources::MapData[(int)centerTop / TILE_PIXEL_SIZE][(int)topM / TILE_PIXEL_SIZE].OBJIndex - 1].ChangeOBJ();
        }
    }

    if(Resources::MapData[ (int)leftM / TILE_PIXEL_SIZE ][ (int) topM / TILE_PIXEL_SIZE ].blocked ||
       Resources::MapData[ (int)rightM / TILE_PIXEL_SIZE ][ (int) topM / TILE_PIXEL_SIZE ].blocked){
        return true;
    }

    // no hay colision
    return false;
}

bool Mario::DetectCollisionRight(){
    // ni te gastes en analizar cosas que no existen dentro de la matriz
    if (posMapX > Resources::MapInfo.max_tiles_x || posMapY < 0) return false;

    // Mario hit box
    int leftM = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int rightM = leftM + 24; // detectar con la carga de bodys.
    int topM = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomM = topM + 31; // si pongo 32 no me va a dejar caminar xD

    if(Resources::MapData[ (int)rightM / TILE_PIXEL_SIZE ][(int) bottomM / TILE_PIXEL_SIZE].Layer[0].grhIndex == 178){
        if(!inEffectFlag){
            posMapY = (int)bottomM / TILE_PIXEL_SIZE;
            posMapX = (int)rightM / TILE_PIXEL_SIZE;
            if (posMapY >= 12) posMapY = 11;
            PixelOffSetY = 0;

            switch(posMapY){
                case 11: pointsFlag = 100; break;
                case 10:
                case 9: pointsFlag = 400; break;
                case 8:
                case 7: pointsFlag = 800; break;
                case 6:
                case 5: pointsFlag = 2000; break;
                case 4:
                case 3: pointsFlag = 5000; break;
            }

            PointsPixelX = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX - 48;
            PointsPixelY = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
            DarPuntos(pointsFlag);
            SetEffectFlag();
        }
    }

    if(Resources::MapData[ (int)rightM / TILE_PIXEL_SIZE ][posMapY].blocked ||
       Resources::MapData[ (int)rightM / TILE_PIXEL_SIZE ][ (int) bottomM / TILE_PIXEL_SIZE ].blocked){
        return true;
    }

    // no hay colision
    return false;
}

bool Mario::DetectCollisionLeft(){
    // ni te gastes en analizar cosas que no existen dentro de la matriz
    if (posMapX > Resources::MapInfo.max_tiles_x || posMapY < 0) return false;

    // Mario hit box
    int leftM = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int topM = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomM = topM + 31;

    if(Resources::MapData[ (int)leftM / TILE_PIXEL_SIZE ][posMapY].blocked ||
       Resources::MapData[ (int)leftM / TILE_PIXEL_SIZE ][ (int) bottomM / TILE_PIXEL_SIZE ].blocked){
        return true;
    }

    // no hay colision
    return false;
}

void Mario::UpdateTime(){
    if (time == 0 || evolution == Evolutions::DEAD || inEffectFlag == true) return; // no te gastes mas.

    if (SDL_GetTicks() >= ticks + 1000){
        ticks = SDL_GetTicks();
        if (time != 0){
            time--;
            if (time == 150) Audio::PlaySound("lowtime");
        }
    }
}

void Mario::respawnMario(){
    delete [] Resources::OBJs;
    delete [] Resources::NPCs;

    for(int i = 0; i < Resources::MapInfo.max_tiles_x; i++){
        delete[] Resources::MapData[i];
    }
	delete[] Resources::MapData;

    Resources::LoadMap();

    isMoving = false;
    isJumping = false;
    inTheAir = false;
    isDead = false;
    inEffectEvolution = false;
    inEffectFlag = false;
    isSmallJump = false;
    winPointsForTime = false;
    flipChar = SDL_FLIP_NONE;

    numBody = 0;
    PixelOffSetX = 0;
    PixelOffSetY = 0;
    evolution = Evolutions::SMALL;
    time = 400;
    points = 0;
    coins = 0;
    VelY = 0;
    VelX = 0;
    maxJumpPosY = 0;
    posMapX = Resources::MapInfo.Mario_PosX;
    posMapY = Resources::MapInfo.Mario_PosY;

    ticks = SDL_GetTicks();
}

void Mario::UpdatePosition(float timer){
    if (inEffectFlag){
        if (!winPointsForTime){
            EffectWinWorld();
        } else {
            EffectWinPointsForTime();
        }
        PointsPixelY -= 3 * timer;
        PixelOffSetY += VelY * timer;
        PixelOffSetX += VelX * timer;
        return;
    }


    if (evolution == Evolutions::DEAD){
        EffectDead();
        PixelOffSetY += VelY * timer; // para el efecto muerto.
        return; // no te muevas mas, estas muerto..
    }

    // se cayo del mapa.. xD
    if(PixelOffSetY >= 0 && posMapY >= 15){
        SetDead();
        return;
    }

    PixelOffSetY += VelY * timer;

    if (DetectCollisionTop()){
        PixelOffSetY -= VelY * timer;
        if (maxJumpPosY != posMapY) maxJumpPosY = posMapY;
    }

    // Update posMapY
    if (PixelOffSetY >= 32){
        PixelOffSetY = 0;
        posMapY++;
    } else if (PixelOffSetY <= -1){
        PixelOffSetY = 31;
        posMapY--;
    }

    // fuerza siempre hacia abajo.
    if(!DetectCollisionButtom() && !isJumping){
        inTheAir = true;
        posWalk = 0;
        PixelOffSetY += VEL_CAIDA * timer;
    } else {
        inTheAir = false;
        isJumping ? posWalk = 0 : posWalk = 2;

        if (isSmallJump) Jump();

        if (maxJumpPosY == posMapY){
            if (isSmallJump) isSmallJump = false;
            maxJumpPosY = 0;
            isJumping = false;
            if (VelY == -MAX_VEL) VelY += MAX_VEL;
        }
    }

    PixelOffSetX += VelX * timer;

    if( ((posMapX * 32) + PixelOffSetX <= 0) ||
        ((posMapX * 32) + PixelOffSetX >= (Resources::MapInfo.max_tiles_x * 32) - 24) ||
        DetectCollisionLeft() || DetectCollisionRight()){

        PixelOffSetX -= VelX * timer;
    }

    // Update posMapX
    if (PixelOffSetX >= 32){
        PixelOffSetX = 0;
        posMapX++;
    } else if (PixelOffSetX <= -1){
        PixelOffSetX = 31;
        posMapX--;
    }
}

void Mario::SetDead(){
    ticks = SDL_GetTicks();
    Audio::StopMusic();
    Audio::PlaySound("death"); // dura 2 segs.
    posWalk = 1;
    evolution = Evolutions::DEAD;
}

void Mario::SetEffectFlag(){
    ticks = SDL_GetTicks();
    posWalk = 3;
    inTheAir = false;
    isMoving = false;
    isJumping = false;
    flipChar = SDL_FLIP_NONE;
    VelX = 0;
    VelY = 0;
    PixelOffSetX = 0;
    inEffectFlag = true;
    Audio::StopMusic();
    Audio::PlaySound("levelend");
}

void Mario::ChangeEvolution(bool crecer){
    inEffectEvolution = true; // sea cual sea...

    if (crecer){
        if(evolution < Evolutions::POWER_STAR){
            evolution++;
        }
    } else {
        if (evolution > Evolutions::SMALL){
            evolution--;
        } else {
            SetDead();
        }
    }
}

void Mario::EffectChangeBody(){
    // Al transformarse pierde su velocidad de cualquier eje.
    VelX = 0;
    VelY = 0;

    switch(evolution){
        case Evolutions::SMALL: // de grande a pequeño.
            break;
        case Evolutions::BIG:
            break;
        case Evolutions::POWER_FLOWER:
            break;
        case Evolutions::POWER_STAR:
            break;
    }
}

void Mario::EffectDead(){
    // 800 ticks de tiempo muerto sin subir..
    if (SDL_GetTicks() >= ticks + 600){ // subimos
        VelY = -VEL_CAIDA + 2;
    }

    if (SDL_GetTicks() >= ticks + 1300){ // bajamos
        VelY = VEL_CAIDA + 8;
    }

    if (SDL_GetTicks() >= ticks + 3000){ // tiempo total del sonido.
        isDead = true; // para cambiar de pantalla.
        if (UPs >= 1){
            UPs--;
            if (UPs == 0) Audio::PlaySound("gameover");
        }
    }
}

void Mario::EffectWinWorld(){

    // Update posMapY
    if (PixelOffSetY >= 32){
        PixelOffSetY = 0;
        posMapY++;
    } else if (PixelOffSetY <= -1){
        PixelOffSetY = 31;
        posMapY--;
    }

    // Update posMapX
    if (PixelOffSetX >= 32){
        PixelOffSetX = 0;
        posMapX++;
    } else if (PixelOffSetX <= -1){
        PixelOffSetX = 31;
        posMapX--;
    }

    // bajada de bandera.
    if(posMapY != 11){
        VelY = MAX_VEL;
    } else {
        VelY = 0;
    }

    if (SDL_GetTicks() >= ticks + 1900){
        flipChar = SDL_FLIP_HORIZONTAL;
        if (VelX == 0 && posMapX != Resources::MapInfo.PosCastle) PixelOffSetX = 16;
    }

    if (SDL_GetTicks() >= ticks + 2100){
        flipChar = SDL_FLIP_NONE;

        if(!DetectCollisionButtom()){
            inTheAir = true;
            posWalk = 0;
            VelY = VEL_CAIDA;
        } else{
            inTheAir = false;
            VelY = 0;
            posWalk = 2;
        }

        if (posMapX != Resources::MapInfo.PosCastle){
            inTheAir ? posWalk = 0 : posWalk = 2;
            isMoving = true;
            VelX = MAX_VEL;
        } else {
            isMoving = false;
            VelX = 0;
        }
    }

    if (SDL_GetTicks() >= ticks + 7500){ // fin de tiempo de la musica.
        if (!winPointsForTime){
            ticks = SDL_GetTicks();
            winPointsForTime = true;
        }
    }
}

void Mario::EffectWinPointsForTime(){
    if (SDL_GetTicks() >= ticks + 30){
        if(time != 0){
            Audio::PlaySound("scorering");
            time -= 2;
            points += 100;

            if (time == 1){
                time -= 1;
                points += 50;
            }
        }
        ticks += 30;
    }

    if (time == 0){
        winPointsForTime = false;
        EndWin = true;
    }
}

void Mario::SmallJump(){
    // posicion de los pies de mario.
    int topM = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomM = topM + TILE_PIXEL_SIZE;

    isJumping = true;
    isSmallJump = true; // Para que automaticamente suba
    maxJumpPosY = (int)(bottomM / TILE_PIXEL_SIZE) - 3; // Pequeño salto
}

void Mario::DarUPS() {UPs++;}
void Mario::DarCoins() { coins++; }
void Mario::DarPuntos(int Cantidad){ points += Cantidad; }
bool Mario::GetEffect() { return inEffectEvolution; }
bool Mario::GetIsMoving() { return isMoving; }
int Mario::GetUPs() { return UPs; }
int Mario::GetTime() { return time; }
int Mario::GetPoints() { return points; }
int Mario::GetCoins() { return coins; }
bool Mario::GetIsJumping() { return isJumping; }
bool Mario::GetEffectWin() { return inEffectFlag; }
bool Mario::GetEndWin() { return EndWin; }
int Mario::GetPointsFlag() { return pointsFlag; }

NPC::NPC(){
    pointsNPC = 0;
    ticks = 0;
    evolution = 0;
    MAX_VEL = 2;
    NPCType = 0;
    NPCBody = NULL;
    isSpawned = false;
    isDead = false;
    inEffectDead = false;
    GivePoints = true;
    PixelOffSetX = 0;
    PixelOffSetY = 0;
    PointsPixelX = 0;
    PointsPixelY = 0;
    VelY = 0;
    VelX = -MAX_VEL;
    hurt = true;
}

NPC::NPC(int mapX, int mapY, int pWalk, int NPCType) : Character(mapX, mapY, pWalk){
    isSpawned = false;
    isDead = false;
    inEffectDead = false;
    hurt = true;
    GivePoints = true;

    ticks = 0;
    this->NPCType = NPCType;

    SetPrincipalProperties(NPCType);

    PixelOffSetX = 0;
    PixelOffSetY = 0;

    PointsPixelX = 0;
    PointsPixelY = 0;

    VelY = 0;
    VelX = -MAX_VEL;
}

NPC::~NPC(){

}

void NPC::SetPrincipalProperties(int t_NPC){
     switch(t_NPC){
        case tNPC::HONGE:
            evolution = 1;
            pointsNPC = 100;
            numBody = bChar::BODY_HONGE;
            break;

        case tNPC::TURTLE_GREEN:
            evolution = 2;
            pointsNPC = 200;
            numBody = bChar::BODY_TURTLE_GREEN;
            break;

        case tNPC::TURTLE_WINGS_GREEN:
            pointsNPC = 200;
            evolution = 3; // si pierde 1, se transforma en una tortuga sin alas.
            numBody = bChar::BODY_TURTLE_WINGS_GREEN;
            break;
     }

     NPCBody = new body;
     *NPCBody = Resources::bodyData[numBody];
     NPCBody->posWalk[posWalk].frameCounter = 1;
     NPCBody->posWalk[posWalk].started = false;
}

void NPC::UpdatePosition(float timer){
    if (inEffectDead){
        PointsPixelY -= 2 * timer;
        VelX = 0;
        NPCBody->posWalk[posWalk].frameCounter = 1;
        NPCBody->posWalk[posWalk].started = false;
        PixelOffSetY += VelY * timer;
        return;
    } else {
        if (NPCType == TURTLE_GREEN) PointsPixelY -= 2 * timer;
    }

    if(PixelOffSetY >= 32 && posMapY > 14) isDead = true;

    PixelOffSetY += VelY * timer;

    // Update posMapY
    if (PixelOffSetY >= 32){
        PixelOffSetY = 0;
        posMapY++;
    } else if (PixelOffSetY <= -1){
        PixelOffSetY = 31;
        posMapY--;
    }

    // fuerza siempre hacia abajo.
    if(!DetectCollisionButtom()){
        PixelOffSetY += 7 * timer;
    }

    PixelOffSetX += VelX * timer; // Move left or right.

    // Update posMapX
    if (PixelOffSetX >= 32){
        PixelOffSetX = 0;
        posMapX++;
    } else if (PixelOffSetX <= -1){
        PixelOffSetX = 31;
        posMapX--;
    }

    // si detecta fin de mapa o colossion de mapa cambiamos posicion de caminata
    if(((posMapX * TILE_PIXEL_SIZE) + PixelOffSetX <= 0) || DetectCollisionLeft()){
        flipChar = SDL_FLIP_HORIZONTAL;
        VelX = MAX_VEL;
    }

    if(((posMapX * TILE_PIXEL_SIZE) + PixelOffSetX >= (Resources::MapInfo.max_tiles_x * TILE_PIXEL_SIZE) - TILE_PIXEL_SIZE) || DetectCollisionRight()){
        flipChar = SDL_FLIP_NONE;
        VelX = -MAX_VEL;
    }
}

bool NPC::DetectCollisionButtom(){
    // NPC hit box
    int leftN = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int rightN = leftN + TILE_PIXEL_SIZE;
    int topN = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomN = topN + TILE_PIXEL_SIZE;
    int centerN = leftN + 16;

    if(Resources::MapData[(int)centerN / TILE_PIXEL_SIZE][ (int) bottomN / TILE_PIXEL_SIZE ].typeOBJ != NONE_OBJ){
        if (Resources::OBJs[Resources::MapData[(int)centerN / TILE_PIXEL_SIZE][(int)bottomN / TILE_PIXEL_SIZE].OBJIndex - 1].GetEffect()){
            SetEffectDead(false);
        }
    }

    if(Resources::MapData[ (int)leftN / TILE_PIXEL_SIZE ][ (int) bottomN / TILE_PIXEL_SIZE ].blocked ||
       Resources::MapData[ (int)rightN / TILE_PIXEL_SIZE ][ (int) bottomN / TILE_PIXEL_SIZE ].blocked){
        return true;
    }

    // no hay colision
    return false;
}

bool NPC::DetectCollisionRight(){
    // NPC hit box
    int leftN = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int rightN = leftN + TILE_PIXEL_SIZE;
    int topN = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomN = topN + 31; // si pongo 32 no me va a dejar caminar xD

    if(Resources::MapData[ (int)rightN / TILE_PIXEL_SIZE ][posMapY].blocked ||
       Resources::MapData[ (int)rightN / TILE_PIXEL_SIZE ][ (int) bottomN / TILE_PIXEL_SIZE ].blocked){
        return true;
    }

    // no hay colision
    return false;
}

bool NPC::DetectCollisionLeft(){
    // NPC hit box
    int leftN = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int topN = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomN = topN + 31;

    if(Resources::MapData[ (int)leftN / TILE_PIXEL_SIZE ][posMapY].blocked ||
       Resources::MapData[ (int)leftN / TILE_PIXEL_SIZE ][ (int) bottomN / TILE_PIXEL_SIZE ].blocked){
        return true;
    }

    // no hay colision
    return false;
}

bool NPC::MarioCollision_Left(int mPosX, int mPosY, int mPOSetX, int mPOSetY){
    // Mario hit box
    int leftM = (mPosX * TILE_PIXEL_SIZE) + mPOSetX;
    int topM = ((mPosY * TILE_PIXEL_SIZE) + mPOSetY);
    int bottomM = topM + 31;

    // NPC hit box
    int leftN = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int rightN = leftN + 32;
    int topN = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomN = topN + 31;

    if (rightN >= leftM && bottomM > topN && bottomN > topM){
        if (rightN - leftM <= 16){

            // Efecto para la tortuga.
            if(NPCType == TURTLE_GREEN && evolution == 1){
                if(VelX == 0){
                    MAX_VEL = 15;
                    VelX = -MAX_VEL;
                }
            }

            return true;
        }
    }

    // no hay colision
    return false;
}

bool NPC::MarioCollision_Right(int mPosX, int mPosY, int mPOSetX, int mPOSetY){
    // Mario hit box
    int leftM = (mPosX * TILE_PIXEL_SIZE) + mPOSetX;
    int rightM = leftM + 24; // detectar con la carga de bodys.
    int topM = ((mPosY * TILE_PIXEL_SIZE) + mPOSetY);
    int bottomM = topM + 31;

    // NPC hit box
    int leftN = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int topN = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomN = topN + 31;

    if (leftN <= rightM && bottomM > topN && bottomN > topM){
        if (rightM - leftN <= 16){

            // Efecto para la tortuga.
            if(NPCType == TURTLE_GREEN && evolution == 1){
                if(VelX == 0){
                    MAX_VEL = 15;
                    VelX = MAX_VEL;
                }
            }

            return true;
        }
    }

    // no hay colision
    return false;
}

bool NPC::MarioCollision_Top(int mPosX, int mPosY, int mPOSetX, int mPOSetY){
    // Mario hit box
    int leftM = (mPosX * TILE_PIXEL_SIZE) + mPOSetX;
    int rightM = leftM + 24; // detectar con la carga de bodys.
    int topM = ((mPosY * TILE_PIXEL_SIZE) + mPOSetY);

    // NPC hit box
    int leftN = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int rightN = leftN + 32;
    int topN = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomN = topN + 31;

    if (bottomN >= topM && rightM >= leftN && leftM <= rightN){
        if (bottomN - topM <= 16){
            return true;
        }
    }

    // no hay colision
    return false;
}

bool NPC::MarioCollision_Buttom(int mPosX, int mPosY, int mPOSetX, int mPOSetY){
     // Mario hit box
    int leftM = (mPosX * TILE_PIXEL_SIZE) + mPOSetX;
    int rightM = leftM + 24; // detectar con la carga de bodys.
    int topM = ((mPosY * TILE_PIXEL_SIZE) + mPOSetY);
    int bottomM = topM + 31;

    // NPC hit box
    int leftN = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int rightN = leftN + 32;
    int topN = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomN = topN + 31;

    if (bottomM >= topN && topM < bottomN && rightM >= leftN && leftM <= rightN){
        if (bottomM - topN <= 8){
            return true;
        }
    }

    // no hay colision
    return false;
}

void NPC::ChangeEvolution(){
    switch(NPCType){
        case HONGE:
            SetEffectDead(true);
            break;

        case TURTLE_GREEN: // solo muere si se cae del mapa xD.
            if (evolution == 2){
                ticks = SDL_GetTicks(); // para el tiempo de puntos.
                PointsPixelX = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX + 4;
                PointsPixelY = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY) - 16;
                hurt = false;
                VelX = 0;
                posWalk = 1;
                evolution = 1;
            } else {
                if(VelX == 0){
                    hurt = true;
                    MAX_VEL = 15;
                    VelX = MAX_VEL;
                } else {
                    hurt = false;
                    VelX = 0;
                    MAX_VEL = 2;
                }
            }
            break;
    }
}

void NPC::SetEffectDead(bool typeEffect){
    ticks = SDL_GetTicks();
    inEffectDead = true;
    PointsPixelX = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX + 4;
    PointsPixelY = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY) - 16;

    if (typeEffect){ // Efecto normal desde el piso
        posWalk = 1;
    } else {
        flipChar = SDL_FLIP_VERTICAL;
        VelY = 15;
    }
}

void NPC::SetDeadAfterEffect(){
    if(SDL_GetTicks() >= ticks + 1000){
        SetDeadNPC();
    }
}

void NPC::SetHurt(){ hurt = true; }
float NPC::GetMAXVEL(){ return MAX_VEL; }
bool NPC::GetGivePoints(){ return GivePoints; }
void NPC::SetGivePoints(bool TorF){ GivePoints = TorF; }
int NPC::GetPoints(){ return pointsNPC; }
void NPC::SetDeadNPC(){ isDead = true; }
bool NPC::GetSpawned() { return isSpawned; }
void NPC::SetSpawn() { isSpawned = true; }
body* NPC::GetBodyStruct() { return NPCBody; }
int NPC::GetNPCType(){ return NPCType; }
bool NPC::GetEffectDead() { return inEffectDead; }
bool NPC::GetHurt() { return hurt; }

MapOBJ::MapOBJ(){
    posMapX = 0;
    posMapY = 0;
    PixelOffSetX = 0;
    PixelOffSetY = 0;
    oVelX = 0;
    oVelY = 0;
    typeOBJ = 0;
    grafico = NULL;
    inEffect = false;
    evolution = 0;
    pointsOBJ = 0;
    PointsPixelX = 0;
    PointsPixelY = 0;
    DaCoinsPoints = false;
    isSpawned = true;
    DioUP = false;
}

MapOBJ::MapOBJ(int posMapX, int posMapY, int typeOBJ){
    this->posMapX = posMapX;
    this->posMapY = posMapY;
    this->typeOBJ = typeOBJ;

    evolution = 0;
    oVelX = 0;
    oVelY = 0;
    PixelOffSetX = 0;
    PixelOffSetY = 0;
    pointsOBJ = 0;
    PointsPixelX = 0;
    PointsPixelY = 0;
    inEffect = false;
    DaCoinsPoints = false;
    isSpawned = true;
    DioUP = false;

    SetOtherProperties();
}

MapOBJ::~MapOBJ(){

}

void MapOBJ::SetOtherProperties(){
    grafico = new grh;

    switch(typeOBJ){
        case BLOCK:
            if(Resources::MapInfo.typeMap == OVERWORLD){
                grafico->grhIndex = 33;
            } else if(Resources::MapInfo.typeMap == UNDERGROUND) {
                grafico->grhIndex = 34;
            } else {
                grafico->grhIndex = 35;
            }

            Resources::MapData[posMapX][posMapY].blocked = true;
            break;

        case BLOCK_COIN:
            if(Resources::MapInfo.typeMap == OVERWORLD){
                grafico->grhIndex = 33;
            } else if(Resources::MapInfo.typeMap == UNDERGROUND) {
                grafico->grhIndex = 34;
            } else {
                grafico->grhIndex = 35;
            }

            Resources::MapData[posMapX][posMapY].blocked = true;
            DaCoinsPoints = true;
            pointsOBJ = 200;
            evolution = 10;
            break;

        case BOX_COIN:
            if(Resources::MapInfo.typeMap == OVERWORLD){
                grafico->grhIndex = 213;
            } else {
                grafico->grhIndex = 214;
            }

            evolution = 1;
            pointsOBJ = 200;
            DaCoinsPoints = true;
            Resources::MapData[posMapX][posMapY].blocked = true;
            break;

        case BOX_INVISIBLE_HONGE_OR_FLOWER:
            Resources::MapData[posMapX][posMapY].blocked = false;
            grafico->grhIndex = 0;
            break;

        case GREEN_HONGE:
            oVelX = MAX_VEL_X;
            grafico->grhIndex = 223;
            isSpawned = false;
            PixelOffSetY = 32;
            inEffect = true;
            break;

        case RED_HONGE:
            oVelX = MAX_VEL_X;
            grafico->grhIndex = 222;
            isSpawned = false;
            PixelOffSetY = 32;
            inEffect = true;
            break;

        case FLAG:
            grafico->grhIndex = 177;
            PixelOffSetX = -16;
            pointsOBJ = 5000;
            break;

        case MINI_FLAG:
            grafico->grhIndex = 227;
            PixelOffSetY = 5;
            break;
    }

    Resources::InitGrh(*grafico, grafico->grhIndex, true);
}

void MapOBJ::ChangeOBJ(){
    if(inEffect) return;

    PointsPixelX = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX + 4;
    PointsPixelY = (posMapY * TILE_PIXEL_SIZE) + PixelOffSetY - 24;

    switch(typeOBJ){
        case BLOCK:
            ticks = SDL_GetTicks();
            Audio::PlaySound("blockhit");
            inEffect = true;
            break;

        case BOX_COIN:
            ticks = SDL_GetTicks();
            if (evolution != 0){
                Audio::PlaySound("coin");
                evolution--;
                inEffect = true;
            }
            break;

        case BLOCK_COIN:
            ticks = SDL_GetTicks();
            if (evolution != 0){
                Audio::PlaySound("coin");
                evolution--;
                inEffect = true;
            }
            break;

        case BOX_INVISIBLE_HONGE_OR_FLOWER:
            if (grafico->grhIndex == 0){
                Audio::PlaySound("mushroomappear");
                SetGraphicBlock();
                Resources::MapData[posMapX][posMapY].blocked = true;
                Resources::OBJs[Resources::MapData[posMapX][posMapY - 1].OBJIndex - 1].SetSpawn(true);
            }
            break;

    }
}

void MapOBJ::MoveOBJEffect(float timer){
    if (typeOBJ == GREEN_HONGE || typeOBJ == RED_HONGE) EffectOBJHonge(timer);

    if(inEffect){
        PixelOffSetY += oVelY * timer;
        PointsPixelY -= 3 * timer;

        switch(typeOBJ){
            case BLOCK:
                EffectOBJSquare();
                break;

            case BLOCK_COIN:
                EffectOBJSquare();
                break;

            case BLOCK_STAR:
                EffectOBJSquare();
                break;

            case BOX_COIN:
                EffectOBJSquare();
                break;
        }
    }
}

void MapOBJ::MoveOBJFlag(float timer){
    PixelOffSetY += oVelY * timer;

    if(typeOBJ == FLAG){
        if(PixelOffSetY < 256){ // 256 pixeles en tiles desde la punta de la bandera hasta abajo.
            oVelY = 10;
        } else {
            PixelOffSetY = 256;
            oVelY = 0;
        }
    } else { // mini flag.
        if(PixelOffSetY > -32){
            oVelY = -2;
        } else {
            PixelOffSetY = -32;
            oVelY = 0;
        }
    }

}

void MapOBJ::EffectOBJHonge(float timer){
    if (DioUP){
        PointsPixelY += -2 * timer;
    }

    if (!isSpawned) return;

    // efecto de spawn.
    if (PixelOffSetY <= 32 && inEffect){
        oVelY = -1;
        PixelOffSetY += oVelY * timer;

        if (PixelOffSetY <= 0){
            PixelOffSetY = 0;
            inEffect = false;
        }
        return;
    }

    if(PixelOffSetY >= 32 && posMapY > 14) isSpawned = false;

    PixelOffSetY += oVelY * timer;

    // Update posMapY
    if (PixelOffSetY >= 32){
        PixelOffSetY = 0;
        posMapY++;
    } else if (PixelOffSetY <= -1){
        PixelOffSetY = 31;
        posMapY--;
    }

    // fuerza siempre hacia abajo.
    if(!DetectCollisionButtom()){
        PixelOffSetY += 7 * timer;
    }

    PixelOffSetX += oVelX * timer; // Move left or right.

    // Update posMapX
    if (PixelOffSetX >= 32){
        PixelOffSetX = 0;
        posMapX++;
    } else if (PixelOffSetX <= -1){
        PixelOffSetX = 31;
        posMapX--;
    }

    // si detecta fin de mapa o colossion de mapa cambiamos posicion de caminata
    if(((posMapX * TILE_PIXEL_SIZE) + PixelOffSetX <= 0) || DetectCollisionLeft()){
        oVelX = MAX_VEL_X;
    }

    if(((posMapX * TILE_PIXEL_SIZE) + PixelOffSetX >= (Resources::MapInfo.max_tiles_x * TILE_PIXEL_SIZE) - TILE_PIXEL_SIZE) || DetectCollisionRight()){
        oVelX = -MAX_VEL_X;
    }
}

void MapOBJ::SetDioUP() {
    DioUP = true;
    PointsPixelX = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX + 4;
    PointsPixelY = (posMapY * TILE_PIXEL_SIZE) + PixelOffSetY - 24;
    ticks = SDL_GetTicks();
}

void MapOBJ::EffectOBJSquare(){
    if (SDL_GetTicks() <= ticks + 100){ // subida.
        oVelY = -MAX_VEL_Y - 6;
    } else {
        oVelY = MAX_VEL_Y;
        if (PixelOffSetY >= 0) PixelOffSetY = 0;
    }

    if (SDL_GetTicks() >= ticks + 300){
        if ((typeOBJ == BOX_COIN || typeOBJ == BLOCK_COIN) && evolution == 0){
            SetGraphicBlock();
            DaCoinsPoints = false;
        }

        PixelOffSetY = 0;
        inEffect = false;
    }
}

void MapOBJ::SetGraphicBlock(){
    switch(Resources::MapInfo.typeMap){
        case UNDERGROUND:
            grafico->grhIndex = 31;
            break;

        case CASTLE:
            grafico->grhIndex = 32;
            break;

        default:
            grafico->grhIndex = 30;
    }
}

bool MapOBJ::DetectCollisionButtom(){
    // NPC hit box
    int leftO = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int rightO = leftO + TILE_PIXEL_SIZE;
    int topO = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomO = topO + TILE_PIXEL_SIZE;

    if(Resources::MapData[ (int)leftO / TILE_PIXEL_SIZE ][ (int) bottomO / TILE_PIXEL_SIZE ].blocked ||
       Resources::MapData[ (int)rightO / TILE_PIXEL_SIZE ][ (int) bottomO / TILE_PIXEL_SIZE ].blocked){
        return true;
    }

    // no hay colision
    return false;
}

bool MapOBJ::DetectCollisionRight(){
    // MapOBJ hit box
    int leftO = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int rightO = leftO + TILE_PIXEL_SIZE;
    int topO = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomO = topO + 31; // si pongo 32 no me va a dejar caminar xD

    if(Resources::MapData[ (int)rightO / TILE_PIXEL_SIZE ][posMapY].blocked ||
       Resources::MapData[ (int)rightO / TILE_PIXEL_SIZE ][ (int) bottomO / TILE_PIXEL_SIZE ].blocked){
        return true;
    }

    // no hay colision
    return false;
}

bool MapOBJ::DetectCollisionLeft(){
    // MapOBJ hit box
    int leftO = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int topO = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomO = topO + 31;

    if(Resources::MapData[ (int)leftO / TILE_PIXEL_SIZE ][posMapY].blocked ||
       Resources::MapData[ (int)leftO / TILE_PIXEL_SIZE ][ (int) bottomO / TILE_PIXEL_SIZE ].blocked){
        return true;
    }

    // no hay colision
    return false;
}

bool MapOBJ::MarioCollision_Left(int mPosX, int mPosY, int mPOSetX, int mPOSetY){
    // Mario hit box
    int leftM = (mPosX * TILE_PIXEL_SIZE) + mPOSetX;
    int topM = ((mPosY * TILE_PIXEL_SIZE) + mPOSetY);
    int bottomM = topM + 31;

    // OBJ hit box
    int leftO = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int rightO = leftO + 32;
    int topO = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomO = topO + 31;

    if (rightO >= leftM && bottomM > topO && bottomO > topM){
        if (rightO - leftM <= 16){
            return true;
        }
    }

    // no hay colision
    return false;
}

bool MapOBJ::MarioCollision_Right(int mPosX, int mPosY, int mPOSetX, int mPOSetY){
    // Mario hit box
    int leftM = (mPosX * TILE_PIXEL_SIZE) + mPOSetX;
    int rightM = leftM + 24; // detectar con la carga de bodys.
    int topM = ((mPosY * TILE_PIXEL_SIZE) + mPOSetY);
    int bottomM = topM + 31;

    // OBJ hit box
    int leftO = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int topO = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomO = topO + 31;

    if (leftO <= rightM && bottomM > topO && bottomO > topM){
        if (rightM - leftO <= 16){
            return true;
        }
    }

    // no hay colision
    return false;
}

bool MapOBJ::MarioCollision_Top(int mPosX, int mPosY, int mPOSetX, int mPOSetY){
    // Mario hit box
    int leftM = (mPosX * TILE_PIXEL_SIZE) + mPOSetX;
    int rightM = leftM + 24; // detectar con la carga de bodys.
    int topM = ((mPosY * TILE_PIXEL_SIZE) + mPOSetY);

    // OBJ hit box
    int leftO = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int rightO = leftO + 32;
    int topO = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomO = topO + 31;

    if (bottomO >= topM && rightM >= leftO && leftM <= rightO){
        if (bottomO - topM <= 16){
            return true;
        }
    }

    // no hay colision
    return false;
}

bool MapOBJ::MarioCollision_Buttom(int mPosX, int mPosY, int mPOSetX, int mPOSetY){
     // Mario hit box
    int leftM = (mPosX * TILE_PIXEL_SIZE) + mPOSetX;
    int rightM = leftM + 24; // detectar con la carga de bodys.
    int topM = ((mPosY * TILE_PIXEL_SIZE) + mPOSetY);
    int bottomM = topM + 31;

    // OBJ hit box
    int leftO = (posMapX * TILE_PIXEL_SIZE) + PixelOffSetX;
    int rightO = leftO + 32;
    int topO = ((posMapY * TILE_PIXEL_SIZE) + PixelOffSetY);
    int bottomO = topO + 31;

    if (bottomM >= topO && topM < bottomO && rightM >= leftO && leftM <= rightO){
        if (bottomM - topO <= 8){
            return true;
        }
    }

    // no hay colision
    return false;
}

string MapOBJ::GetPointsOBJ(){
    stringstream sPoints;
    sPoints << pointsOBJ;
    return sPoints.str().c_str();
}

grh* MapOBJ::GetGraphic(){ return grafico; }
int MapOBJ::GetPoints(){ return pointsOBJ; }
int MapOBJ::GetPOSetX(){ return PixelOffSetX; }
int MapOBJ::GetPOSetY(){ return PixelOffSetY; }
int MapOBJ::GetMapX(){ return posMapX; }
int MapOBJ::GetMapY(){ return posMapY; }
bool MapOBJ::GetEffect() { return inEffect; }
int MapOBJ::GetEvolution(){ return evolution; }
int MapOBJ::GetPPosX(){ return PointsPixelX; }
int MapOBJ::GetPPosY(){ return PointsPixelY; }
bool MapOBJ::GetDaCoinsPoints() { return DaCoinsPoints; }
bool MapOBJ::GetIsSpawned() { return isSpawned; }
void MapOBJ::SetSpawn(bool spawn) { isSpawned = spawn; }
int MapOBJ::GetTypeOBJ() { return typeOBJ; }
bool MapOBJ::GetDioUP() { return DioUP; }
Uint32 MapOBJ::GetTicks() { return ticks; }
