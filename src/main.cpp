// Xonix Game
// Entry point - initializes audio and starts authentication

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "../header/Authentication.h"

using namespace std;
using namespace sf;

SoundBuffer buffer;
Sound background_sound;
bool musicEnabled = true;

void toggleBackgroundMusic()
{
    if (musicEnabled)
    {
        background_sound.pause();
        musicEnabled = false;
    }
    else
    {
        background_sound.play();
        musicEnabled = true;
    }
}

bool isMusicEnabled()
{
    return musicEnabled;
}

void setMusicEnabled(bool enabled)
{
    musicEnabled = enabled;
    if (enabled)
        background_sound.play();
    else
        background_sound.pause();
}

int main()
{
    if (!buffer.loadFromFile("assets/Sounds/Background_Audio.mp3"))
    {
        cout << "Error Loading sound!" << endl;
        return -1;
    }

    background_sound.setBuffer(buffer);
    background_sound.setLoop(true);
    background_sound.setVolume(50);
    background_sound.play();

    RenderWindow window(VideoMode(800, 600), "Xonix", Style::Close);
    window.setFramerateLimit(60);

    AuthenticationSystem authSystem(&window);
    authSystem.authenticationLoop();

    return 0;
}
