#include <Galaxy/Audio/audio.hpp>
#include <Galaxy/Math/binary.hpp>
#include <Galaxy/OS/defines.hpp>

#if OS_MAC || OS_IOS
    #include <OpenAL/OpenAL.h>
#endif
#if OS_WEB
    #include <AL/al.h>
    #include <AL/alc.h>
#endif
#include <internalinit.hpp>
//#include <emscripten.h>

static ALCdevice *alcDevice;

void read_until_found(std::ifstream &stream, const char *dataStr, char *buffer)
{
    int foundLetters = 0;
    while(1)
    {
        stream.read(buffer, 1);
        if (!stream)
            throw("Failed to find data chunk");
        if (foundLetters)
        {
            if (buffer[0] == dataStr[foundLetters])
            {
                ++foundLetters;
                if (foundLetters == 4)
                    break;
            }
            else foundLetters = 0;
        }
        if (buffer[0] == dataStr[0]) foundLetters = 1;
    }
}
std::vector<char> load_wav(const std::string &path, char *channels, int *sampleRate, char *bitsPerSample)
{
    
    std::ifstream stream = std::ifstream(path, std::ios::binary);
    if (!stream.good())
        throw("Invalid stream");

    char buffer[4];

    // Header
    if (!stream.read(buffer, 4))
        throw("Failed to read (RIFF)");
    if (strncmp(buffer, "RIFF", 4))
        throw("Invalid file (RIFF)");
    
    // file size (minus 8 bytes for the start)
    if (!stream.read(buffer, 4))
        throw("Failed to read (file size)");
    
    if (!stream.read(buffer, 4))
        throw("Failed to read (WAVE)");
    if (strncmp(buffer, "WAVE", 4))
        throw("Invalid file (WAVE)");



    // Format chunk
    read_until_found(stream, "fmt ", buffer);
    
    // Format chunk size (16 for PCM)
    stream.ignore(4);

    if (!stream.read(buffer, 2))
        throw("Failed to read format");
    short audioFormat = to_native_endian16(buffer, Endian::LITTLE);
    if (audioFormat != 1)
        throw("Invalid audio format");


    if (!stream.read(buffer, 2))
        throw("Failed to read (channels)");
    *channels = to_native_endian16(buffer, Endian::LITTLE);
    
    
    if (!stream.read(buffer, 4))
        throw("Failed to read (sample rate)");
    *sampleRate = to_native_endian32(buffer, Endian::LITTLE);

    // byte rate(4), block align(2)
    stream.ignore(6);


    if (!stream.read(buffer, 2))
        throw("Failed to read (bps)");
    memcpy(bitsPerSample, buffer, 1);

    // "data"
    read_until_found(stream, "data", buffer);
    
    if (!stream.read(buffer, 4))
        throw("Failed to read (size)");
    int dataSize = to_native_endian32(buffer, Endian::LITTLE);

    std::vector<char> data = std::vector<char>(dataSize);
    if (!stream.read(data.data(), dataSize))
    {
        throw("Failed to read audio data");
    }
    return data;
}


AssetRef<AudioData> AudioData::load_wav(const std::string &path)
{
    if (AssetRef<AudioData>::is_loaded(path))
        return AssetRef<AudioData>::get_loaded(path);
    
    AudioData *audio = new AudioData();
    audio->rawAudio = ::load_wav(path, &audio->channels, &audio->sampleRate, &audio->bitsPerSample);
    return AssetRef<AudioData>(audio, path);
}

AudioPlayer::AudioPlayer(AudioData *data)
{
    alGenBuffers(1, &soundBuffer);
    int format;
    if (data->channels == 1 && data->bitsPerSample == 8) format = AL_FORMAT_MONO8;
    else if (data->channels == 1 && data->bitsPerSample == 16) format = AL_FORMAT_MONO16;
    else if (data->channels == 2 && data->bitsPerSample == 8) format = AL_FORMAT_STEREO8;
    else if (data->channels == 2 && data->bitsPerSample == 16) format = AL_FORMAT_STEREO16;
    else
    {
        logerr("Unrecognized bits per sample (%o) and channels (%o)\n", data->bitsPerSample, data->channels);
        assert(false);
    }

    alBufferData(soundBuffer, format, data->rawAudio.data(), data->rawAudio.size(), data->sampleRate);

    alGenSources(1, &audioSource);
    alSourcef(audioSource, AL_PITCH, 1);
    alSourcef(audioSource, AL_GAIN, 1);
    alSource3f(audioSource, AL_POSITION, 0,0,0);
    alSource3f(audioSource, AL_VELOCITY, 0,0,0);
    alSourcei(audioSource, AL_LOOPING, 0);
    alSourcei(audioSource, AL_BUFFER, soundBuffer);
}
AudioPlayer::~AudioPlayer()
{
    alDeleteBuffers(1, &soundBuffer);
    alDeleteSources(1, &audioSource);
}
void AudioPlayer::play()
{
    alSourcePlay(audioSource);
}
void AudioPlayer::loop(bool loopAudio)
{
    alSourcei(audioSource, AL_LOOPING, loopAudio);
}
void AudioPlayer::pause()
{
    alSourcePause(audioSource);
}
void AudioPlayer::stop()
{
    alSourceStop(audioSource);
}
static void init()
{
    alcDevice = alcOpenDevice(nullptr);
    if (!alcDevice)
        throw("Failed to open audio device");
    ALCcontext *context = alcCreateContext(alcDevice, nullptr);
    alcMakeContextCurrent(context);
}
static void clean()
{
    alcDestroyContext(alcGetCurrentContext());
    alcMakeContextCurrent(nullptr);
    alcCloseDevice(alcDevice);
}
INTERNAL_INIT_FUNC(init);
CLEANUP_FUNC(clean);