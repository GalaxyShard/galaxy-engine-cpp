#include <Galaxy/Audio/audio.hpp>
#include <OpenAL/OpenAL.h>

AudioPlayer::AudioPlayer(AudioData *data)
{

}
AudioPlayer::~AudioPlayer()
{

}
void AudioPlayer::play()
{
    
}
void AudioPlayer::pause()
{

}
void AudioPlayer::stop()
{

}

static void init()
{

    ALCdevice *device = alcOpenDevice(nullptr);
    if (!device)
        throw("Failed to open device");
    ALCcontext *context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);
}