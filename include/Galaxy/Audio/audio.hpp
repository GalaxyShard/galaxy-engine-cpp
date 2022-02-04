#pragma once
#include <string>
#include <Galaxy/Assets/assets.hpp>

class AudioData
{
public:
    static AssetRef<AudioData> load(const std::string &path);
};

class AudioPlayer
{
public:
    //enum State { OFF, PLAYING, PAUSED };
private:

public:
    //AudioPlayer();
    AudioPlayer(AudioData *data);
    ~AudioPlayer();
    void play();
    void pause();
    void stop();

    AudioPlayer(const AudioPlayer &) = delete;
    AudioPlayer& operator=(const AudioPlayer &) = delete;
    AudioPlayer(AudioPlayer &&) = delete;
    AudioPlayer& operator=(AudioPlayer &&) = delete;
};