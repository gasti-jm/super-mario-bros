#include <SDL.h>
#include "structs.h"
#include "audio.h"

class Character{
    protected:
        int posMapX, posMapY, posWalk, numBody, evolution;
        bool isDead;
        float PointsPixelX, PointsPixelY; // Posicion de puntos x pixel
        float PixelOffSetX, PixelOffSetY; // Posicion x pixel
        float VelX, VelY; // Velocidad de movimiento.
        SDL_RendererFlip flipChar;
        Uint32 ticks = 0;

    public:
        Character(int, int, int);
        Character();
        ~Character();

        SDL_RendererFlip GetFlip();
        Uint32 GetTicks();
        int GetMapX();
        int GetMapY();
        int GetNumBody();
        int GetPWalk();
        int GetPPosX();
        int GetPPosY();
        bool GetIsDead();
        int GetEvolution();
        int GetPOSetX();
        int GetPOSetY();
};

class Mario : public Character{
    private:
        const float MAX_VEL = 10;
        const float VEL_CAIDA = 7;

        int UPs, time, points, coins, pointsFlag;
        int maxJumpPosY;
        bool isMoving, isJumping, inTheAir, inEffectEvolution, inEffectFlag, winPointsForTime;
        bool EndWin;
        bool isSmallJump = false;

    public:
        Mario(int, int, int);
        ~Mario();

        // Getters
        bool GetIsMoving();
        int GetUPs();
        int GetTime();
        int GetPoints();
        int GetCoins();
        bool GetEffect();
        bool GetIsJumping();
        bool GetEffectWin();
        bool GetEndWin();
        int GetPointsFlag();

        void SetDead();
        void SetEffectFlag();
        void ChangeEvolution(bool);
        void EffectChangeBody();
        void EffectDead();
        void EffectWinWorld();
        void EffectWinPointsForTime();
        void DarPuntos(int);
        void DarCoins();
        void DarUPS();

        void HandleEvent(SDL_Event&, bool);
        void UpdatePosition(float);
        void UpdateTime();
        void respawnMario();

        bool DetectCollisionTop();
        bool DetectCollisionButtom();
        bool DetectCollisionRight();
        bool DetectCollisionLeft();

        void MoveLeftOrRight(bool);
        void Jump();
        void SmallJump();
};

class NPC : public Character{
    private:
        float MAX_VEL = 2;
        bool isSpawned, inEffectDead;
        int NPCType, pointsNPC;
        body* NPCBody;
        bool hurt, GivePoints;

    public:
        NPC(int, int, int, int);
        NPC();
        ~NPC();

        void UpdatePosition(float);

        // Colisiones entre el mapa para los NPC.
        bool DetectCollisionButtom();
        bool DetectCollisionRight();
        bool DetectCollisionLeft();

        // Si mario colisiona con un personaje.
        bool MarioCollision_Left(int, int, int, int);
        bool MarioCollision_Right(int, int, int, int);
        bool MarioCollision_Top(int, int, int, int);
        bool MarioCollision_Buttom(int, int, int, int); // MATA NPC.

        bool GetSpawned();
        body* GetBodyStruct();
        int GetNPCType();
        bool GetEffectDead();
        int GetPoints();
        bool GetGivePoints();
        bool GetHurt();
        float GetMAXVEL();

        void SetGivePoints(bool);
        void SetPrincipalProperties(int);
        void SetSpawn();
        void SetDeadNPC();
        void SetEffectDead(bool);
        void SetDeadAfterEffect();
        void SetHurt();
        void ChangeEvolution();
};

class MapOBJ{
    private:
        float MAX_VEL_X = 3, MAX_VEL_Y = 5;
        grh* grafico;
        int posMapX, posMapY, typeOBJ, evolution, pointsOBJ;
        float PixelOffSetX, PixelOffSetY, oVelX, oVelY, PointsPixelX, PointsPixelY; // objetos con movimiento como el hongo, estrella, etc.
        Uint32 ticks = 0;
        bool inEffect, DaCoinsPoints, isSpawned, DioUP;

    public:
        MapOBJ(int, int, int);
        MapOBJ();
        ~MapOBJ();

        grh* GetGraphic();
        int GetPOSetX();
        int GetPOSetY();
        int GetMapX();
        int GetMapY();
        bool GetEffect();
        bool GetIsSpawned();
        string GetPointsOBJ();
        int GetPoints();
        int GetEvolution();
        int GetPPosX();
        int GetPPosY();
        bool GetDaCoinsPoints();
        int GetTypeOBJ();
        bool GetDioUP();
        Uint32 GetTicks();

        bool MarioCollision_Left(int, int, int, int);
        bool MarioCollision_Right(int, int, int, int);
        bool MarioCollision_Top(int, int, int, int);
        bool MarioCollision_Buttom(int, int, int, int);

        bool DetectCollisionButtom();
        bool DetectCollisionRight();
        bool DetectCollisionLeft();

        void SetOtherProperties();
        void MoveOBJEffect(float);
        void EffectOBJHonge(float);
        void MoveOBJFlag(float);
        void ChangeOBJ();
        void EffectOBJSquare();
        void SetGraphicBlock();
        void SetSpawn(bool);
        void SetDioUP();
};
