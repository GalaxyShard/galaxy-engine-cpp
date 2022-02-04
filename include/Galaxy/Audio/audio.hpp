#pragma once
#include <string>
#include <Galaxy/Assets/assets.hpp>
#include <vector>

class AudioData
{
private:
    int sampleRate;
    char channels;
    char bitsPerSample;
    std::vector<char> rawAudio;
    friend class AudioPlayer;
public:
    static AssetRef<AudioData> load_wav(const std::string &path);
};

class AudioPlayer
{
private:
    unsigned int soundBuffer;
    unsigned int audioSource;

public:
    AudioPlayer(AudioData *data);
    ~AudioPlayer();
    void play();
    void pause();
    void stop();
    void loop(bool loopAudio);

    AudioPlayer(const AudioPlayer &) = delete;
    AudioPlayer& operator=(const AudioPlayer &) = delete;
    AudioPlayer(AudioPlayer &&) = delete;
    AudioPlayer& operator=(AudioPlayer &&) = delete;
};