#pragma once

#include <string>
#include <vector>
#include <filesystem>

extern bool LOADER_NORMALIZE;

class audio_select
{
private:
    std::vector<std::filesystem::path> itemies;
    int item_current_idx = 0;
    std::string dir_path = "/Users/northkillpd/temp";
    std::filesystem::path selected_path;

    std::function<void(std::filesystem::path)> callback;
    std::function<void(std::string)> ir_callback;

    void update_file_list();

    int w = 41;
    int h = 22;
    bool is_loading = false;

public:
    explicit audio_select(
            std::function<void(std::filesystem::path)> callback,
            std::function<void(std::string)> ir_callback
    );

    virtual ~audio_select();

    void on_frame();
    void render_content();
};