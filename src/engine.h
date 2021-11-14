// C++ INCLUDES
#include <iostream>
#include <string>
#include <sstream>

// SDL INLCUDES
#include <SDL.h>
#include <SDL_image.h>

// Headers
#include "gamedata.h"

using namespace std;

class Texture{
    private:
        SDL_Texture *Textura;
        int tWidth, tHeight;

    public:
        Texture();
        ~Texture();

        bool LoadFromFile(SDL_Renderer *, string);
        void FreeMemory();
        void SetColor(Uint8, Uint8, Uint8);
        void SetBlend(SDL_BlendMode);
        void SetAlpha(Uint8);
        void Render(SDL_Renderer *, int, int,
                    SDL_Rect* = NULL, double = 0.0, SDL_Point* = NULL, SDL_RendererFlip = SDL_FLIP_NONE);

        // Getters
        int GetWidth();
        int GetHeight();
};

struct nodoTextures{
    int fileNumLoaded;
    Texture textureLoaded;
    nodoTextures *siguiente;
};

class Window{
    private:
        SDL_Window *window;
        int wWidth, wHeight;
        bool wMouseFocus, wKeyBoardFocus, wMinimized;

    public:
        Window();

        bool InitWindow();
        SDL_Renderer* CreateRenderer();

        void HandleEvent(SDL_Renderer*, SDL_Event&);
        void FreeMemory();
        void ChangeScreen(bool);

        bool Has_MouseFocus();
        bool Has_KeyBoardFocus();
        bool IsMinimized();
};

class Engine{
    private:
        bool prgRun = true; // Main loop
        Window window; // Ventana
        bool inChangeRenderer = false;
        SDL_Renderer *renderer; // Render
        SDL_Event e; // Eventos
        int sampleRenderTypes; // menu, juego, etc...
        int optionMenu = OPTION_MENU::INICIAR_JUEGO;
        int optionSettings = OPTION_SETTINGS::VOLUME_AUDIO;

        int posCameraX;
        bool inOptions = false;
        bool gPause = false;

        // FPS & TIMERS
        bool showFPS = false;
        Uint32 FPS = 0;
        Uint32 lFrameTimer = 0;
        int countedFrames = 0;

        // Limit Frames
        const Uint32 SCREEN_FPS = 60;
        const Uint32 SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

        Uint32 ticksBlackScreen = 0;
        Uint32 end_time = 0;
        Uint32 timerElapsedTime = 0;
        float timerTicksPerFrame = 0;
        const float engineBaseSpeed = 0.02;

        grh grhCoin, grhMiniCoin_OVERWORLD, grhMiniCoin_OTHER;

        Mario *mario;
        nodoTextures *listTLoaded; // Lista
        nodoTextures *Textura = new nodoTextures(); // toma un nodo de la lista y lo renderiza.

    public:
        Engine();
        ~Engine();

        bool InitSDL();
        void CloseSDL();

        void SetRenderColor();
        void RenderPause();
        void RenderOptions();
        void RenderScreen();
        void RenderMenu();
        void RenderBlackScreen();
        void RenderGame();
        void RenderStats();
        void RenderMario();
        void RenderNPC(int);
        void RenderOBJMap(int);
        void PlayMusicMAP();
        void BackToMenu();

        void DrawTextureGrhIndex(int, int, int, double = 0.0,
                        SDL_RendererFlip = SDL_FLIP_NONE, bool = false, Uint8 = 255);

        void DrawTexture(grh&, int, int, bool = false, bool = false,
                         SDL_RendererFlip = SDL_FLIP_NONE, double = 0.0,
                         bool = false, Uint8 = 255);

        void DrawText(string, int, int, int = 0);

        void DrawQuad(int, int, int, int, Uint8, Uint8, Uint8, Uint8, bool = false);
        void DrawLine(int, int, int, int, Uint8, Uint8, Uint8, Uint8);

        void ADD_NEW_TEXTURE(nodoTextures *&, int);
        void SEARCH_TEXTURE(nodoTextures *, int);
        void CheckTextureExist(int);
        void DELETE_ALL_TEXTURES(nodoTextures *&);

        void CheckInput();
        void InputMenu();
        void InputOptions();

        void UpdateFramesPerSecond();
        Uint32 GetElapsedTime();

        // Getters
        bool IsRunning();
};
