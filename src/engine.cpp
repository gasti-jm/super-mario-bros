#include "engine.h"

Engine::Engine(){
    // Inicio las variables en el constructor.
    Resources::LoadGrhData();
    Resources::LoadBodys();
    Resources::LoadFonts();

    // Graficos de monedas.
    grhCoin.grhIndex = 219;
    grhMiniCoin_OVERWORLD.grhIndex = 220;
    grhMiniCoin_OTHER.grhIndex = 221;

    Resources::InitGrh(grhCoin, grhCoin.grhIndex, true);
    Resources::InitGrh(grhMiniCoin_OVERWORLD, grhMiniCoin_OVERWORLD.grhIndex, true);
    Resources::InitGrh(grhMiniCoin_OTHER, grhMiniCoin_OTHER.grhIndex, true);

    mario = NULL;
    renderer = NULL;
    listTLoaded = NULL;

    sampleRenderTypes = renderType::RENDER_MENU;
}

Engine::~Engine(){

}

bool Engine::InitSDL(){
    bool status = true;

    // Iniciamos SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
        cout << "No se pudo iniciar el motor grafico SDL! SDL ERROR: " << SDL_GetError() << endl;
        status = false;
    } else {

        if (!window.InitWindow()){
            cout << "No se pudo crear la ventana! SDL ERROR: " << SDL_GetError() << endl;
            status = false;
        } else {

            // creating render....
            renderer = window.CreateRenderer();

            if (renderer == NULL){
                cout << "No se pudo crear el render! SDL ERROR: " << SDL_GetError() << endl;
                status = false;
            } else {

                // INIT RENDER COLOR.
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

                //PNG LOADING
                int imgFlags = IMG_INIT_PNG;
                if(!(IMG_Init(imgFlags) & imgFlags)){
                   cout << "No se pudo iniciar SDL_IMAGE! SDL_IMAGE ERROR: " << IMG_GetError() << endl;
                   status = false;
                }

                if (!(Audio::InitAudio())){
                    status = false;
                }

                Audio::SetVolume(Resources::Config.volume);
                Audio::PlayMusic("overworld");
            }

        }

    }

    return status;
}

void Engine::CloseSDL(){
    DELETE_ALL_TEXTURES(listTLoaded);
    delete listTLoaded;

    Textura = NULL;
    delete Textura;

    delete [] Resources::GrhData;
    delete [] Resources::bodyData;

    if (sampleRenderTypes != RENDER_MENU){
        delete [] Resources::OBJs;
        delete [] Resources::NPCs;

        for(int i = 0; i < Resources::MapInfo.max_tiles_x; i++){
            delete[] Resources::MapData[i];
        }
        delete[] Resources::MapData;
    }

    if (mario != NULL) delete mario;

    Audio::CloseAudio();

    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    window.FreeMemory();

    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Engine::SetRenderColor(){
    // color de fondo!
    if (sampleRenderTypes == RENDER_BLACKSCREEN){
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    } else {
        if(Resources::MapInfo.typeMap == OVERWORLD ||
            Resources::MapInfo.typeMap == UNDERWATER ||
            sampleRenderTypes == RENDER_MENU){

            SDL_SetRenderDrawColor(renderer, 92, 148, 252, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        }
    }
}

void Engine::RenderScreen(){
    if (inChangeRenderer){
        SDL_DestroyRenderer(renderer);
        renderer = window.CreateRenderer();
        DELETE_ALL_TEXTURES(listTLoaded); // las texturas se almacenan en un render, por ende tengo que borrar todas.
        inChangeRenderer = false;
        return;
    }

    if (window.IsMinimized()) return; // Ni te gastes al estar minimizado.

    SetRenderColor();
    SDL_RenderClear(renderer); // Clear screen

    switch(sampleRenderTypes){
        case renderType::RENDER_MENU:
            RenderMenu();
            break;

        case renderType::RENDER_BLACKSCREEN:
            RenderBlackScreen();
            break;

        case renderType::RENDER_GAME:
            RenderGame();
            if (gPause) RenderPause();
            break;
    }

    SDL_RenderPresent(renderer); // Update screen
}

void Engine::RenderPause(){
    DrawQuad((Resources::Config.screen_width / 2) - 176,
            (Resources::Config.screen_height / 2) - 88,
            352, 176, 0, 0, 0, 150);

    DrawQuad((Resources::Config.screen_width / 2) - 171,
            (Resources::Config.screen_height / 2) - 83,
            342, 166, 255, 255, 255, 255, true);

    DrawText("PAUSA", (Resources::Config.screen_width / 2) - 38, (Resources::Config.screen_height / 2) - 8);
}

void Engine::RenderOptions(){
    string YesOrNo;
    int barra = (Resources::Config.volume * -2) + 250;
    if (Resources::Config.volume == 128) barra = 0;

    // MENU
    DrawQuad((Resources::Config.screen_width / 2) - 210,
            (Resources::Config.screen_height / 2) - 88,
            430, 176, 0, 0, 0, 150);

    DrawQuad((Resources::Config.screen_width / 2) - 205,
            (Resources::Config.screen_height / 2) - 83,
            420, 166, 255, 255, 255, 255, true);

    // BARRA DE VOLUMEN
    DrawQuad((Resources::Config.screen_width / 2) - 50,
            (Resources::Config.screen_height / 2) - 53,
            barra, 20, 128, 128, 128, 150);

    DrawQuad((Resources::Config.screen_width / 2) - 50,
            (Resources::Config.screen_height / 2) - 53,
            250, 20, 255, 255, 255, 255, true);



    switch(optionSettings){
        case 0:
            DrawText(">", (Resources::Config.screen_width / 2) - 195, (Resources::Config.screen_height / 2) - 50);
            break;

        case 1:
            Resources::Config.fullscreen ? YesOrNo = "<SI>" : YesOrNo = "<NO>";
            DrawText(YesOrNo, (Resources::Config.screen_width / 2) + 150, (Resources::Config.screen_height / 2) - 25);
            DrawText(">", (Resources::Config.screen_width / 2) - 195, (Resources::Config.screen_height / 2) - 25);
            break;

        case 2:
            Resources::Config.vsync ? YesOrNo = "<SI>" : YesOrNo = "<NO>";
            DrawText(YesOrNo, (Resources::Config.screen_width / 2) + 150, (Resources::Config.screen_height / 2) - 5);
            DrawText(">", (Resources::Config.screen_width / 2) - 195, (Resources::Config.screen_height / 2) - 5);
            break;

        case 3:
            Resources::Config.capFPS ? YesOrNo = "<SI>" : YesOrNo = "<NO>";
            DrawText(YesOrNo, (Resources::Config.screen_width / 2) + 150, (Resources::Config.screen_height / 2) + 15);
            DrawText(">", (Resources::Config.screen_width / 2) - 195, (Resources::Config.screen_height / 2) + 15);
            break;

        case 4:
            DrawText(">", (Resources::Config.screen_width / 2) - 195, (Resources::Config.screen_height / 2) + 35);
            break;
    }

    DrawText("OPCIONES", (Resources::Config.screen_width / 2) - 62,
            (Resources::Config.screen_height / 2) - 75);

    DrawText("VOLUMEN", (Resources::Config.screen_width / 2) - 180,
             (Resources::Config.screen_height / 2) - 50);

    DrawText("PANTALLA COMPLETA", (Resources::Config.screen_width / 2) - 180,
             (Resources::Config.screen_height / 2) - 25);

    DrawText("VSYNC", (Resources::Config.screen_width / 2) - 180,
             (Resources::Config.screen_height / 2) - 5);

    DrawText("LIMITAR FPS", (Resources::Config.screen_width / 2) - 180,
             (Resources::Config.screen_height / 2) + 15);

    DrawText("SALIR", (Resources::Config.screen_width / 2) - 180,
             (Resources::Config.screen_height / 2) + 35);


}

void Engine::RenderBlackScreen(){
    stringstream UPs;
    UPs << "x" << mario->GetUPs();

    if(mario->GetEndWin()){
        DrawText("GRACIAS POR JUGAR ESTA DEMO!", (Resources::Config.screen_width / 2) - 200,
                    (Resources::Config.screen_height / 2) - 8);
    } else {
        if (mario->GetUPs() >= 1){
            DrawText(UPs.str().c_str(), (Resources::Config.screen_width / 2) - 16,
                    (Resources::Config.screen_height / 2) - 8);

            DrawTextureGrhIndex(1, (Resources::Config.screen_width / 2) - 46,
                                (Resources::Config.screen_height / 2) - 16);

        } else {
            DrawText("GAME OVER", (Resources::Config.screen_width / 2) - 72,
                    (Resources::Config.screen_height / 2) - 8);
        }
    }

    RenderStats();

    if(SDL_GetTicks() >= ticksBlackScreen + 5000){
        if (mario->GetEndWin()){
            BackToMenu();
        } else {
            if (mario->GetUPs() >= 1){
                if (mario->GetIsDead()){
                    mario->respawnMario();
                }
                PlayMusicMAP();
                sampleRenderTypes = renderType::RENDER_GAME;
            } else {
                BackToMenu();
            }
        }
    }
}

void Engine::BackToMenu(){
    delete [] Resources::OBJs;
    delete [] Resources::NPCs;

    for(int i = 0; i < Resources::MapInfo.max_tiles_y; i++){
        delete[] Resources::MapData[i];
    }
	delete[] Resources::MapData;

    mario = NULL;
    delete mario;
    DELETE_ALL_TEXTURES(listTLoaded);
    Audio::PlayMusic("overworld");
    sampleRenderTypes = renderType::RENDER_MENU;
}

void Engine::RenderStats(){
    // Aca se van a renderizar los puntos, monedas, tiempo etc.
    stringstream coins, points, time;

    if (mario->GetPoints() <= 9){
        points << "00000" << mario->GetPoints();
    } else if (mario->GetPoints() <= 99){
        points << "0000" << mario->GetPoints();
    } else if (mario->GetPoints() <= 999){
        points << "000" << mario->GetPoints();
    } else if (mario->GetPoints() <= 9999){
        points << "00" << mario->GetPoints();
    } else if (mario->GetPoints() <= 99999){
        points << "0" << mario->GetPoints();
    } else{
        points << mario->GetPoints();
    }

    if (mario->GetCoins() >= 10){
        coins << "x" << mario->GetCoins();
    } else if (mario->GetCoins() < 10){
        coins << "x0" << mario->GetCoins();
    }

    if (mario->GetTime() >= 100) {
        time << mario->GetTime();
    } else if (mario->GetTime() >= 10){
        time << "0" << mario->GetTime();
    } else {
        time << "00" << mario->GetTime();
    }

    DrawText("MARIO", 50, 15);
    DrawText(points.str().c_str(), 50, 32); // puntos

    DrawText("WORLD", (Resources::Config.screen_width / 2) + 50, 15);
    DrawText("1-3", (Resources::Config.screen_width / 2) + 66, 32);

    if(Resources::MapInfo.typeMap == OVERWORLD){
        DrawTexture(grhMiniCoin_OVERWORLD,(Resources::Config.screen_width / 2) - 96, 32, false, true);
    } else {
        DrawTexture(grhMiniCoin_OTHER,(Resources::Config.screen_width / 2) - 96, 32, false, true);
    }

    DrawText(coins.str().c_str(), (Resources::Config.screen_width / 2) - 80, 32);

    DrawText("TIME", Resources::Config.screen_width - 114, 15);
    DrawText(time.str().c_str(), (Resources::Config.screen_width - 50) - 48, 32); // tiempo
}

void Engine::RenderMenu(){
    DrawTextureGrhIndex(13, (Resources::Config.screen_width / 2) - 176, (Resources::Config.screen_height / 2) - 176);

    switch (optionMenu){
        case 0:
            DrawTextureGrhIndex(115, (Resources::Config.screen_width / 2) - 125, (Resources::Config.screen_height / 2) + 50);
            break;
        case 1:
            DrawTextureGrhIndex(115, (Resources::Config.screen_width / 2) - 125, (Resources::Config.screen_height / 2) + 70);
            break;
        case 2:
            DrawTextureGrhIndex(115, (Resources::Config.screen_width / 2) - 125, (Resources::Config.screen_height / 2) + 90);
            break;
    }

    DrawText("INICIAR JUEGO", (Resources::Config.screen_width / 2) - 104,
            (Resources::Config.screen_height / 2) + 50);

    DrawText("OPCIONES",(Resources::Config.screen_width / 2) - 104,
              (Resources::Config.screen_height / 2) + 70);

    DrawText("SALIR", (Resources::Config.screen_width / 2) - 104,
             (Resources::Config.screen_height / 2) + 90);

    if(inOptions) RenderOptions();
}

void Engine::RenderGame(){
    int POffSetX = mario->GetPOSetX();
    int pMapX = mario->GetMapX();
    int minX;
    int maxX;

    // mitad de la pantalla.
    posCameraX = ((((pMapX * TILE_PIXEL_SIZE) + POffSetX) + 24 / 2) - Resources::Config.screen_width / 2);

    // rango de visibilidad.
    if(posCameraX < 0){
        posCameraX = 0;
        minX = 0;
        maxX = 20;
    } else if(posCameraX > (Resources::MapInfo.max_tiles_x * TILE_PIXEL_SIZE) - Resources::Config.screen_width){
        posCameraX = (Resources::MapInfo.max_tiles_x * TILE_PIXEL_SIZE) - Resources::Config.screen_width;
        maxX = Resources::MapInfo.max_tiles_x;
    } else {
        minX = pMapX - 10;
        if (minX < 0) minX = 0;
        maxX = pMapX + 12;
        if (maxX > Resources::MapInfo.max_tiles_x) maxX = Resources::MapInfo.max_tiles_x;
    }

    // dibujado segun rango.
    for (int y = 0; y < Resources::MapInfo.max_tiles_y; y++){
        for (int x = minX; x < maxX; x++){

            if (Resources::MapData[x][y].Layer[0].grhIndex != 0){
                DrawTexture(Resources::MapData[x][y].Layer[0],
                            x * TILE_PIXEL_SIZE - posCameraX,
                            (Resources::Config.screen_height - 480) + y * TILE_PIXEL_SIZE,
                            true, true);
            }

            // SPAWN NPC
            if(Resources::MapData[x][y].NPCIndex != 0){
                if(!Resources::NPCs[Resources::MapData[x][y].NPCIndex - 1].GetSpawned()){
                    Resources::NPCs[Resources::MapData[x][y].NPCIndex - 1].SetSpawn();
                }
            }

        }
    }

    // Render MARIO, NPC & OBJs.
    for (int i = 0; i < Resources::MapInfo.TOTAL_OBJs; i++) RenderOBJMap(i);
    for (int i = 0; i < Resources::MapInfo.TOTAL_NPCs; i++) RenderNPC(i);
    RenderMario();

    for (int y = 0; y < Resources::MapInfo.max_tiles_y; y++){
        for (int x = minX; x < maxX; x++){

            if (Resources::MapData[x][y].Layer[1].grhIndex != 0){
                DrawTexture(Resources::MapData[x][y].Layer[1],
                            x * TILE_PIXEL_SIZE - posCameraX,
                            (Resources::Config.screen_height - 480) + y * TILE_PIXEL_SIZE,
                            true, true);
            }

        }
    }

    RenderStats();

    if (showFPS){
        stringstream FPSText;
        FPSText << "FPS: " << FPS;
        DrawText(FPSText.str().c_str(), 0, 0);
    }
}

void Engine::RenderMario(){
    if (mario->GetIsDead() || mario->GetEndWin()){
        if (sampleRenderTypes != renderType::RENDER_BLACKSCREEN){
            ticksBlackScreen = SDL_GetTicks();
            sampleRenderTypes = renderType::RENDER_BLACKSCREEN;
        }
        return;
    }

    int POffSetX = mario->GetPOSetX();
    int POffSetY = mario->GetPOSetY();
    int pMapX = mario->GetMapX();
    int pMapY = mario->GetMapY();
    int numBody = mario->GetNumBody();
    int pWalkMario = mario->GetPWalk();

    if(mario->GetEffectWin()){
        if(SDL_GetTicks() <= mario->GetTicks() + 1000){
            stringstream points;
            points << mario->GetPointsFlag();
            DrawText(points.str().c_str(), mario->GetPPosX() - posCameraX, mario->GetPPosY(), 1);
        }
    }

    if (!gPause) {
        mario->UpdateTime();
        mario->UpdatePosition(timerTicksPerFrame);
    }

    if (mario->GetIsMoving()){
        if(pWalkMario == 2) Resources::bodyData[numBody].posWalk[pWalkMario].started = true;
    } else {
        // si no nos estamos moviendo dejamos el primer fotograma de la animacion
        Resources::bodyData[numBody].posWalk[pWalkMario].started = false;
        Resources::bodyData[numBody].posWalk[pWalkMario].frameCounter = 1;
    }

    if (pMapY < 15){
        DrawTexture(Resources::bodyData[numBody].posWalk[pWalkMario],
                    (pMapX * TILE_PIXEL_SIZE) + POffSetX - posCameraX,
                    (Resources::Config.screen_height - 480) + ((pMapY * TILE_PIXEL_SIZE) + POffSetY),
                     true, true, mario->GetFlip());
    }
}

void Engine::RenderNPC(int NPCIndex){
    if (!Resources::NPCs[NPCIndex].GetSpawned() || Resources::NPCs[NPCIndex].GetIsDead()) return;

    int POffSetX = Resources::NPCs[NPCIndex].GetPOSetX();
    int POffSetY = Resources::NPCs[NPCIndex].GetPOSetY();
    int pMapX = Resources::NPCs[NPCIndex].GetMapX();
    int pMapY = Resources::NPCs[NPCIndex].GetMapY();
    int pWalkNPC = Resources::NPCs[NPCIndex].GetPWalk();
    body* NPCBody = Resources::NPCs[NPCIndex].GetBodyStruct();

    if (!gPause) Resources::NPCs[NPCIndex].UpdatePosition(timerTicksPerFrame);

    // ¿Es una tortuga y esta moviendose rapido?
    if (Resources::NPCs[NPCIndex].GetNPCType() == TURTLE_GREEN
        && Resources::NPCs[NPCIndex].GetMAXVEL() == 15){

        // ¿Esta colisionando con algun otro NPC?
        for(int i = 0; i < Resources::MapInfo.TOTAL_NPCs; i++){
            int pMX = Resources::NPCs[i].GetMapX();
            int pMY = Resources::NPCs[i].GetMapY();

            if(pMapX == pMX && pMapY == pMY){
                if (i != NPCIndex){
                    // para que me cuente los puntos sin tener que acercarme a un rango de vision de spawn.
                    if (Resources::NPCs[i].GetSpawned() == false){
                        Resources::NPCs[i].SetSpawn();
                    }
                    Resources::NPCs[i].SetEffectDead(false);
                }
            }
        }
    }

    if(Resources::NPCs[NPCIndex].GetEffectDead() || pWalkNPC == 1){
        if (SDL_GetTicks() <= Resources::NPCs[NPCIndex].GetTicks() + 1000){ // TIEMPO DE MOSTRAR PUNTOS.
            stringstream sPoints;
            sPoints << Resources::NPCs[NPCIndex].GetPoints();

            DrawText(sPoints.str().c_str(),
                 Resources::NPCs[NPCIndex].GetPPosX() - posCameraX,
                 Resources::NPCs[NPCIndex].GetPPosY(), 1);
        }

        if (Resources::NPCs[NPCIndex].GetGivePoints()){
            mario->DarPuntos(Resources::NPCs[NPCIndex].GetPoints());
            Resources::NPCs[NPCIndex].SetGivePoints(false);
        }

        if(Resources::NPCs[NPCIndex].GetNPCType() != TURTLE_GREEN){
            Resources::NPCs[NPCIndex].SetDeadAfterEffect();
        }
    }

    if (Resources::NPCs[NPCIndex].MarioCollision_Buttom(mario->GetMapX(), mario->GetMapY(),
                                                        mario->GetPOSetX(), mario->GetPOSetY())){

        // Si mario esta realizando un efecto no puede matar NPCs.
        if (mario->GetEffect() == false
            && mario->GetIsJumping() == false
            && Resources::NPCs[NPCIndex].GetEffectDead() == false){

            Audio::PlaySound("stomp");
            mario->SmallJump();
            Resources::NPCs[NPCIndex].ChangeEvolution();
        }

    } else {
        if (Resources::NPCs[NPCIndex].MarioCollision_Left(mario->GetMapX(), mario->GetMapY(),
                                                          mario->GetPOSetX(), mario->GetPOSetY())

            || Resources::NPCs[NPCIndex].MarioCollision_Right(mario->GetMapX(), mario->GetMapY(),
                                                        mario->GetPOSetX(), mario->GetPOSetY())

            || Resources::NPCs[NPCIndex].MarioCollision_Top(mario->GetMapX(), mario->GetMapY(),
                                                        mario->GetPOSetX(), mario->GetPOSetY())){

            if (Resources::NPCs[NPCIndex].GetHurt()){
                if(mario->GetEffect() == false && Resources::NPCs[NPCIndex].GetEffectDead() == false){
                    mario->ChangeEvolution(false); // decrece la evolucion.
                }
            }

        } else { // Si no estoy colisionando y es una tortuga en movimiento, lo registro como un NPC que daña.
            if(Resources::NPCs[NPCIndex].GetMAXVEL() == 15 && Resources::NPCs[NPCIndex].GetHurt() == false){
                if(pMapX > mario->GetMapX() + 2){ // sino apenas lo toco muere...
                    Resources::NPCs[NPCIndex].SetHurt();
                }
            }
        }
    }

    if(pWalkNPC == 2) NPCBody->posWalk[pWalkNPC].started = true;

    DrawTexture(NPCBody->posWalk[pWalkNPC],
                (pMapX * TILE_PIXEL_SIZE) + POffSetX - posCameraX,
                (Resources::Config.screen_height - 480) + ((pMapY * TILE_PIXEL_SIZE) + POffSetY),
                true, true, Resources::NPCs[NPCIndex].GetFlip());
}

void Engine::RenderOBJMap(int OBJIndex){
    //if(!Resources::OBJs[OBJIndex].GetIsSpawned()) return;

    int POffSetX = Resources::OBJs[OBJIndex].GetPOSetX();
    int POffSetY = Resources::OBJs[OBJIndex].GetPOSetY();
    int pMapX = Resources::OBJs[OBJIndex].GetMapX();
    int pMapY = Resources::OBJs[OBJIndex].GetMapY();

    if (!gPause) Resources::OBJs[OBJIndex].MoveOBJEffect(timerTicksPerFrame);

    if (Resources::OBJs[OBJIndex].GetTypeOBJ() == FLAG &&
        mario->GetEffectWin()){
        Resources::OBJs[OBJIndex].MoveOBJFlag(timerTicksPerFrame);
    }

    if(mario->GetMapX() == Resources::MapInfo.PosCastle){
        if (Resources::OBJs[OBJIndex].GetTypeOBJ() == MINI_FLAG){
            Resources::OBJs[OBJIndex].MoveOBJFlag(timerTicksPerFrame);
        }
    }

    if (Resources::OBJs[OBJIndex].GetTypeOBJ() == GREEN_HONGE
        && Resources::OBJs[OBJIndex].GetDioUP() == true){

        if(SDL_GetTicks() <= Resources::OBJs[OBJIndex].GetTicks() + 1000){
            DrawText("1UP", Resources::OBJs[OBJIndex].GetPPosX() - posCameraX,
                 Resources::OBJs[OBJIndex].GetPPosY(), 1);
        }
    }


    if (Resources::OBJs[OBJIndex].GetTypeOBJ() == GREEN_HONGE
        && Resources::OBJs[OBJIndex].GetIsSpawned()){
        if (Resources::OBJs[OBJIndex].MarioCollision_Buttom(mario->GetMapX(), mario->GetMapY(),
                                                          mario->GetPOSetX(), mario->GetPOSetY())

            || Resources::OBJs[OBJIndex].MarioCollision_Left(mario->GetMapX(), mario->GetMapY(),
                                                          mario->GetPOSetX(), mario->GetPOSetY())

            || Resources::OBJs[OBJIndex].MarioCollision_Right(mario->GetMapX(), mario->GetMapY(),
                                                        mario->GetPOSetX(), mario->GetPOSetY())

            || Resources::OBJs[OBJIndex].MarioCollision_Top(mario->GetMapX(), mario->GetMapY(),
                                                        mario->GetPOSetX(), mario->GetPOSetY())){

            if (mario->GetEffect() == false){
                mario->DarUPS();
                Audio::PlaySound("oneup");
                Resources::OBJs[OBJIndex].SetSpawn(false);
                Resources::OBJs[OBJIndex].SetDioUP();
            }
        }
    }

    if(Resources::OBJs[OBJIndex].GetEffect() && Resources::OBJs[OBJIndex].GetDaCoinsPoints()){
        DrawTexture(grhCoin,
                Resources::OBJs[OBJIndex].GetPPosX() + 3 - posCameraX,
                Resources::OBJs[OBJIndex].GetPPosY(),
                true, true);

        DrawText(Resources::OBJs[OBJIndex].GetPointsOBJ(),
                 Resources::OBJs[OBJIndex].GetPPosX() - posCameraX,
                 Resources::OBJs[OBJIndex].GetPPosY(), 1);
    }

    if(Resources::OBJs[OBJIndex].GetIsSpawned()){
        DrawTexture(*Resources::OBJs[OBJIndex].GetGraphic(),
                (pMapX * TILE_PIXEL_SIZE) + POffSetX - posCameraX,
                (Resources::Config.screen_height - 480) + ((pMapY * TILE_PIXEL_SIZE) + POffSetY),
                true, true);
    }
}

void Engine::CheckInput(){
    while(SDL_PollEvent(&e) != 0){
        if(e.type == SDL_QUIT){
            prgRun = false;
        } else if (e.type == SDL_KEYDOWN && e.key.repeat == 0){
            switch(e.key.keysym.sym){
                case SDLK_ESCAPE:
                    prgRun = false;
                    break;

                case SDLK_KP_MULTIPLY:
                    showFPS = !showFPS;
                    break;

                case SDLK_RETURN:
                    if (sampleRenderTypes == renderType::RENDER_GAME
                        && mario->GetEvolution() != 0
                        && mario->GetEffectWin() == false){

                        if (!gPause) Audio::PlaySound("pause");
                        Audio::UnPause_Music();
                        gPause = !gPause;
                    }
                    break;
            }
        }

        switch(sampleRenderTypes){
            case renderType::RENDER_MENU:
                if(inOptions){
                    InputOptions();
                } else {
                    InputMenu();
                }
                break;

            case renderType::RENDER_GAME:
                mario->HandleEvent(e, gPause);
                break;

        }

        window.HandleEvent(renderer, e);
    }
}

void Engine::InputOptions(){
    if(e.type == SDL_KEYDOWN){
        switch(e.key.keysym.sym){
            case SDLK_UP:
                if (optionSettings > VOLUME_AUDIO) optionSettings--;
                break;

            case SDLK_DOWN:
                if (optionSettings < QUIT) optionSettings++;
                break;

            case SDLK_LEFT:
                switch(optionSettings){
                    case VOLUME_AUDIO:
                        if (Resources::Config.volume != 128){
                            Resources::Config.volume += 8;
                            Audio::SetVolume(Resources::Config.volume);
                        }
                        break;

                    case FULLSCREEN:
                        if(Resources::Config.fullscreen){
                            Resources::Config.fullscreen = false;
                            window.ChangeScreen(false);
                        }
                        break;

                    case VSYNC:
                        if(Resources::Config.vsync){
                            Resources::Config.vsync = false;
                            inChangeRenderer = true;
                        }
                        break;

                    case FPSLIMIT:
                        if(Resources::Config.capFPS){
                            Resources::Config.capFPS = false;
                        }
                        break;
                }
                break;

            case SDLK_RIGHT:
                switch(optionSettings){
                    case VOLUME_AUDIO:
                        if (Resources::Config.volume != 0){
                            Resources::Config.volume -= 8;
                            Audio::SetVolume(Resources::Config.volume);
                        }
                        break;

                    case FULLSCREEN:
                        if(!Resources::Config.fullscreen){
                            Resources::Config.fullscreen = true;
                            window.ChangeScreen(true);
                        }
                        break;

                    case VSYNC:
                        if(!Resources::Config.vsync){
                            Resources::Config.vsync = true;
                            inChangeRenderer = true;
                        }

                        break;

                    case FPSLIMIT:
                        if(!Resources::Config.capFPS){
                            Resources::Config.capFPS = true;
                        }
                        break;
                }
                break;

            case SDLK_RETURN:
                if (optionSettings == OPTION_SETTINGS::QUIT){
                    Resources::ToRewriteConfig(); // Guardamos los cambios.
                    inOptions = false;
                    optionSettings = OPTION_SETTINGS::VOLUME_AUDIO;
                }
        }
    }
}

void Engine::InputMenu(){
    if(e.type == SDL_KEYDOWN){
        switch(e.key.keysym.sym){
            case SDLK_UP:
                if (optionMenu > OPTION_MENU::INICIAR_JUEGO) optionMenu--;
                break;

            case SDLK_DOWN:
                if (optionMenu < OPTION_MENU::SALIR) optionMenu++;
                break;

            case SDLK_RETURN:
                if (optionMenu == OPTION_MENU::INICIAR_JUEGO){
                    Audio::StopMusic();
                    Resources::LoadMap();
                    mario = new Mario(Resources::MapInfo.Mario_PosX, Resources::MapInfo.Mario_PosY, 2);
                    DELETE_ALL_TEXTURES(listTLoaded); // elimino las texutras cargadas en el menu.

                    ticksBlackScreen = SDL_GetTicks();
                    sampleRenderTypes = renderType::RENDER_BLACKSCREEN;
                }

                if (optionMenu == OPTION_MENU::OPCIONES) inOptions = true;
                if (optionMenu == OPTION_MENU::SALIR) prgRun = false;
                break;
        }
    }
}

void Engine::UpdateFramesPerSecond(){
    if (window.IsMinimized()) return; // Ni te gastes al estar minimizado.

    if (SDL_GetTicks() >= lFrameTimer + 1000){ // Actualizamos al pasar 1 seg.
        lFrameTimer = SDL_GetTicks();
        FPS = countedFrames;
        countedFrames = 0;
    }

    countedFrames++;
    timerElapsedTime = GetElapsedTime();
    timerTicksPerFrame = (timerElapsedTime * engineBaseSpeed);

    if (Resources::Config.capFPS){ // Tenemos limitados los FPS?
        Uint32 frameTick = SDL_GetTicks();
        if((SDL_GetTicks() - frameTick) < SCREEN_TICKS_PER_FRAME){
            SDL_Delay(SCREEN_TICKS_PER_FRAME - (SDL_GetTicks() - frameTick));
        }
    }
}

Uint32 Engine::GetElapsedTime(){
    Uint32 start_time = SDL_GetTicks();
    Uint32 ms = (start_time - end_time);
    end_time = SDL_GetTicks();

    return ms;
}

void Engine::DrawTextureGrhIndex(int grhIndex, int x, int y, double angle,
                                  SDL_RendererFlip flip, bool blend, Uint8 alpha){

    grh graf;
    graf.grhIndex = grhIndex;
    Resources::InitGrh(graf, graf.grhIndex, false);

    if (graf.grhIndex == 0) return;

    SDL_Rect RECT; // creamos un recorte

    CheckTextureExist(Resources::GrhData[graf.grhIndex].filenum); // obtenemos una posicion indicada.

    if (blend){
        Textura->textureLoaded.SetBlend(SDL_BLENDMODE_ADD);
    }
    Textura->textureLoaded.SetAlpha(alpha);

    RECT.x = Resources::GrhData[graf.grhIndex].sX;
    RECT.y = Resources::GrhData[graf.grhIndex].sY;
    RECT.w = Resources::GrhData[graf.grhIndex].pixelWidth;
    RECT.h = Resources::GrhData[graf.grhIndex].pixelHeight;

    Textura->textureLoaded.Render(renderer, x, y, &RECT, angle, NULL, flip);
}

void Engine::DrawTexture(grh &Grh, int x, int y, bool center, bool animate,
                         SDL_RendererFlip flip, double angle,
                         bool blend, Uint8 alpha){

    if (Grh.grhIndex == 0) return;
    if (Resources::GrhData[Grh.grhIndex].numFrames == 0) return;

    if (animate && gPause == false){
        if (Grh.started){
            Grh.frameCounter = Grh.frameCounter + (timerElapsedTime * Resources::GrhData[Grh.grhIndex].numFrames / Grh.speed);
            if (Grh.frameCounter > Resources::GrhData[Grh.grhIndex].numFrames){
                Grh.frameCounter = 1;
                if (Grh.loops != -1){
                    if (Grh.loops > 0){
                        Grh.loops--;
                    } else {
                        Grh.started = false;
                    }
                }
            }
        }
    }

    // Averigua que fotograma dibujar (segun cierto frame de animacion)
    int currentGrhIndex = Resources::GrhData[Grh.grhIndex].frames[(int)Grh.frameCounter];

    if (center){
        /*
        if (Resources::GrhData[currentGrhIndex].tileWidth != 1){
            x = x - (int)(Resources::GrhData[currentGrhIndex].tileWidth * TILE_PIXEL_SIZE / 2) + TILE_PIXEL_SIZE / 2;
        }
        */

        if (Resources::GrhData[currentGrhIndex].tileHeight != 1){
            y = y - (int)(Resources::GrhData[currentGrhIndex].tileHeight * TILE_PIXEL_SIZE) + TILE_PIXEL_SIZE;
        }
    }

    if (currentGrhIndex == 0) return;
    if (Resources::GrhData[currentGrhIndex].filenum == 0) return;

    SDL_Rect RECT; // creamos un recorte

    CheckTextureExist(Resources::GrhData[currentGrhIndex].filenum);

    if (blend){
        Textura->textureLoaded.SetBlend(SDL_BLENDMODE_ADD);
    }

    Textura->textureLoaded.SetAlpha(alpha);

    RECT.x = Resources::GrhData[currentGrhIndex].sX;
    RECT.y = Resources::GrhData[currentGrhIndex].sY;
    RECT.w = Resources::GrhData[currentGrhIndex].pixelWidth;
    RECT.h = Resources::GrhData[currentGrhIndex].pixelHeight;

    Textura->textureLoaded.Render(renderer, x, y, &RECT, angle, NULL, flip);
}

void Engine::DrawText(string text, int x, int y, int nType){
    int ascii = 0, d = 0;
    if (text == "") return;

    // lo uso para conseguir el ascii y el len.
    const char* cText = text.c_str();

    for (int a = 0; a < (int)(strlen(cText)); a++){
        ascii = (int)cText[a]; // consigo el ascii.
        if (ascii > 255) ascii = 0;

        if (ascii != 32){ // espacio
            if (Resources::font_type[nType].ascii_code[ascii] != 0){
                DrawTextureGrhIndex(Resources::font_type[nType].ascii_code[ascii], (x + d), y);
                d = d + Resources::GrhData[Resources::font_type[nType].ascii_code[ascii]].pixelWidth + 2; // separo 2 pixeles x letra.
            }
        } else {
            d = d + 4;
        }
    }
}

void Engine::DrawQuad(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool line){
    SDL_Rect RECT;

    RECT.h = h;
    RECT.w = w;
    RECT.x = x;
    RECT.y = y;

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    if (line){
        SDL_RenderDrawRect(renderer, &RECT);
    } else {
       SDL_RenderFillRect(renderer, &RECT);
    }
}

void Engine::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a){
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void Engine::CheckTextureExist(int n){
    // primero busca
    SEARCH_TEXTURE(listTLoaded, n);

    // no existe?
    if (Textura == NULL){
        ADD_NEW_TEXTURE(listTLoaded, n); // agregamos
        SEARCH_TEXTURE(listTLoaded, n); // volvemos a buscar su posicion
    }
}

void Engine::SEARCH_TEXTURE(nodoTextures *lista, int n){
    while((lista != NULL) && (lista->fileNumLoaded <= n)){
        if (lista->fileNumLoaded == n){
            Textura = lista;
            return;
        }
        lista = lista->siguiente; // avanza una posicion de la lista
    }

    Textura = NULL;
}

void Engine::ADD_NEW_TEXTURE(nodoTextures *&lista, int n){
    stringstream path;
    path << "Resources/graphics/" << n << ".bmp";

    nodoTextures *nuevo_nodo = new nodoTextures();
    nuevo_nodo->fileNumLoaded = n;
    nuevo_nodo->textureLoaded.LoadFromFile(renderer, path.str().c_str());

    nodoTextures *aux1 = lista;
    nodoTextures *aux2;

    while((aux1 != NULL) && (aux1->fileNumLoaded < n)){
        aux2 = aux1;
        aux1 = aux1->siguiente;
    }

    if(lista == aux1){
        lista = nuevo_nodo;
    } else {
        aux2->siguiente = nuevo_nodo;
    }

    nuevo_nodo->siguiente = aux1;

    // Eliminamos la reserva de memoria.
    aux1 = NULL;
    aux2 = NULL;
    nuevo_nodo = NULL;
    delete aux1;
    delete aux2;
    delete nuevo_nodo;
}

void Engine::DELETE_ALL_TEXTURES(nodoTextures *&lista){
    while(lista != NULL){ // mientras no sea el final de la lista
        nodoTextures *aux = lista;
        aux->textureLoaded.FreeMemory();
        lista = aux->siguiente;
        delete aux;
    }
}

void Engine::PlayMusicMAP(){
    switch(Resources::MapInfo.typeMap){
        case OVERWORLD:
            Audio::PlayMusic("overworld");
            break;
        case UNDERWATER:
            Audio::PlayMusic("underwater");
            break;
        case UNDERGROUND:
        case CASTLE:
            Audio::PlayMusic("underground");
            break;
    }
}

bool Engine::IsRunning(){
    return prgRun;
}

Texture::Texture(){
    Textura = NULL;
    tWidth = 0;
    tHeight = 0;
}

Texture::~Texture(){
    this->FreeMemory();
}

bool Texture::LoadFromFile(SDL_Renderer *newRenderer, string path){
    this->FreeMemory();

    SDL_Texture *newTexture = NULL;

    // Cargamos la imagen en cierta ruta.
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL){
        cout << "No se pudo cargar la imagen en la ruta " << path.c_str()
             << " SDL ERROR: " << SDL_GetError() << endl;
    } else {
        // Quitamos el fondo violeta de la imagen cargada!
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 255, 0, 255));
        newTexture = SDL_CreateTextureFromSurface(newRenderer, loadedSurface);

        if (newTexture == NULL){
            cout << "No pudo crearse la nueva textura en la ruta " << path.c_str()
                 << " SDL ERROR: " << SDL_GetError() << endl;
        } else {
            tWidth = loadedSurface->w;
            tHeight = loadedSurface->h;
        }

        SDL_FreeSurface(loadedSurface); // liberamos memoria de la imagen cargada anteriormente antes de editar.
    }

    Textura = newTexture;
    return Textura != NULL;
}

void Texture::FreeMemory(){
    if (Textura != NULL){ // Destruir en caso de existir.
        SDL_DestroyTexture(Textura);
        Textura = NULL;
        tWidth = 0;
        tHeight = 0;
    }
}

void Texture::SetColor(Uint8 R, Uint8 G, Uint8 B){
    SDL_SetTextureColorMod(Textura, R, G, B);
}

void Texture::SetBlend(SDL_BlendMode blend){
    SDL_SetTextureBlendMode(Textura, blend);
}

void Texture::SetAlpha(Uint8 A){
    SDL_SetTextureAlphaMod(Textura, A);
}

void Texture::Render(SDL_Renderer *newRenderer, int x, int y, SDL_Rect* clip,
                     double angle, SDL_Point* center, SDL_RendererFlip flip){

    SDL_Rect RECT = {x, y, tWidth, tHeight};

    if (clip != NULL){
        RECT.w = clip->w;
        RECT.h = clip->h;
    }

    SDL_RenderCopyEx(newRenderer, Textura, clip, &RECT, angle, center, flip);
}

int Texture::GetWidth(){
    return tWidth;
}

int Texture::GetHeight(){
    return tHeight;
}

Window::Window(){
    Resources::LoadConfig();

    window = NULL;
    wKeyBoardFocus = false;
    wMouseFocus = false;
    wMinimized = false;
    wWidth = Resources::Config.screen_width;
    wHeight = Resources::Config.screen_height;
}

void Window::FreeMemory(){
    if(window != NULL){
		SDL_DestroyWindow(window);
	}

	wMouseFocus = false;
	wKeyBoardFocus = false;
	wWidth = 0;
	wHeight = 0;
}

bool Window::InitWindow(){
    if (Resources::Config.fullscreen){
        window = SDL_CreateWindow("MARIO BROS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  wWidth, wHeight, SDL_WINDOW_FULLSCREEN);
        SDL_ShowCursor(false);
    } else {
        window = SDL_CreateWindow("MARIO BROS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  wWidth, wHeight, SDL_WINDOW_SHOWN);
    }

    if (window != NULL){
        wMouseFocus = true;
        wKeyBoardFocus = true;
    }

    return window != NULL;
}

SDL_Renderer* Window::CreateRenderer(){
    if (Resources::Config.vsync){
        return SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    } else {
        return SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }
}

void Window::HandleEvent(SDL_Renderer* newRenderer, SDL_Event &e){
    if(e.type == SDL_WINDOWEVENT){
        switch(e.window.event){
            case SDL_WINDOWEVENT_EXPOSED:
                SDL_RenderPresent(newRenderer);
                break;

            case SDL_WINDOWEVENT_ENTER:
                wMouseFocus = true;
                break;

            case SDL_WINDOWEVENT_LEAVE:
                wMouseFocus = false;
                break;

            case SDL_WINDOWEVENT_FOCUS_GAINED:
                wKeyBoardFocus = true;
                break;

            case SDL_WINDOWEVENT_FOCUS_LOST:
                wKeyBoardFocus = false;
                break;

            case SDL_WINDOWEVENT_MINIMIZED:
                wMinimized = true;
                break;

            case SDL_WINDOWEVENT_MAXIMIZED:
                wMinimized = false;
                break;

            case SDL_WINDOWEVENT_RESTORED:
                wMinimized = false;
                break;

        }
    }
}

void Window::ChangeScreen(bool fullscreen){
    if (fullscreen){
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        SDL_ShowCursor(false);
    } else {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_SHOWN);
        SDL_ShowCursor(true);
    }
}

bool Window::Has_MouseFocus(){
    return wMouseFocus;
}

bool Window::Has_KeyBoardFocus(){
    return wKeyBoardFocus;
}

bool Window::IsMinimized(){
    return wMinimized;
}
