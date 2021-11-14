#include "audio.h"

Mix_Music *Audio::loadedMusic = NULL;
Mix_Chunk *Audio::loadedSound = NULL;

Audio::Audio(){

}

Audio::~Audio(){
    CloseAudio();
}

bool Audio::InitAudio(){

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
        cout << "No se pudo inicializar el sistema de audio! SDL ERROR: " << Mix_GetError() << endl;
        return false;
    }

    return true;
}

void Audio::CloseAudio(){
    Mix_FreeChunk(loadedSound);
    loadedSound = NULL;

    Mix_FreeMusic(loadedMusic);
    loadedMusic = NULL;
}

void Audio::PlayMusic(string nameMusic){
    stringstream path;
    path << "Resources/sounds/" << nameMusic << ".wav";

    // ¿Hay musica reproduciendose?
    if(loadedMusic != NULL){
        StopMusic();
    }

    //Load music
    loadedMusic = Mix_LoadMUS(path.str().c_str());
    if(loadedMusic == NULL){
        cout << "Error al cargar la musica! SDL_Mixer ERROR: " << Mix_GetError() << endl;
    } else {
        Mix_PlayMusic(loadedMusic, -1); // Reproducimos.
    }
}

void Audio::UnPause_Music(){
    if(Mix_PausedMusic() == 1){
        Mix_ResumeMusic(); // Resume the music
    } else {
        Mix_PauseMusic(); // Pause the music
    }
}

void Audio::StopMusic(){
    Mix_HaltMusic(); // Stop music
    Mix_FreeMusic(loadedMusic); // Free the music.
    loadedMusic = NULL;
}

void Audio::PlaySound(string nameSound){
    stringstream path;
    path << "Resources/sounds/" << nameSound << ".wav";

    if(loadedSound != NULL){
        Mix_FreeChunk(loadedSound);
        loadedSound = NULL;
    }

    //Load sound effects
    loadedSound = Mix_LoadWAV(path.str().c_str());
    if(loadedSound == NULL){
        cout << "Error al cargar los efectos de sonido! SDL_mixer Error: " << Mix_GetError() << endl;
    } else {
        Mix_PlayChannel(-1, loadedSound, 0); // Suena una sola vez.
    }
}

void Audio::SetVolume(int volume){
    Mix_Volume(-1, MIX_MAX_VOLUME - volume);
    Mix_VolumeMusic(MIX_MAX_VOLUME - volume);
}
