// C++ INCLUDES
#include <iostream>
#include <string>
#include <sstream>

// SDL INCLUDES
#include <SDL_mixer.h>

using namespace std;

class Audio{
    private:
        static Mix_Music *loadedMusic;
        static Mix_Chunk *loadedSound;

    public:
        Audio();
        ~Audio();

        static bool InitAudio();
        static void CloseAudio();
        static void PlayMusic(string);
        static void UnPause_Music();
        static void StopMusic();
        static void PlaySound(string);
        static void SetVolume(int);
};
