#pragma once

#include <deque>
#include <string>
#include <filesystem>
#include <algorithm>
#include <random>
#include <fstream>

class queue
{
private:
    std::deque<std::filesystem::path> q, q_backup;
    std::deque<std::filesystem::path> hist, hist_backup;
    std::function<void(std::filesystem::path)> loader;
    std::function<void(void)> unloader;
    std::function<void(void)> play;

    std::random_device rd;

    bool shuffle;
    bool repeat1;
    bool repeat_all;

    int w = 41;
    int h = 13;

public:
    explicit queue(
            std::function<void(std::filesystem::path)> loader,
            std::function<void(void)> unloader,
            std::function<void(void)> play
    );

    void on_track_end();
    void enqueue_track_or_playlist(std::filesystem::path path);
    void shuffle_queue(bool yes);
    void on_frame();
    void render_content();
};
