#include "httplib.h"

#include "path.h"
#include "player.h"

#include <chrono>
#include <string>
#include <vector>
#include <thread>
#include <codecvt>
#include <fstream>
#include <sstream>

enum class Stage
{
    Waiting, // waiting for players to join
    Ready,   // ready to start
    Racing   // racing
};

int val_countdown = 4;        // countdown value
Stage stage = Stage::Waiting; // current stage

int id_player = 0;                // player id
std::atomic<int> progress_1 = -1; // player 1 progress
std::atomic<int> progress_2 = -1; // player 2 progress
int num_total_char = 0;           // total number of characters
bool cursro_blink = false;
std::atomic<bool> running(true);

// Path
Path path({
    {842, 842},
    {1322, 842},
    {1322, 442},
    {2762, 442},
    {2762, 842},
    {3162, 842},
    {3162, 1722},
    {2122, 1722},
    {2122, 1562},
    {842, 1562},
    {842, 842},
});

// Player atlas
Atlas atlas_1p_idle_up;    // player 1 idle up
Atlas atlas_1p_idle_down;  // player 1 idle down
Atlas atlas_1p_idle_left;  // player 1 idle left
Atlas atlas_1p_idle_right; // player 1 idle right
Atlas atlas_1p_run_up;     // player 1 run up
Atlas atlas_1p_run_down;   // player 1 run down
Atlas atlas_1p_run_left;   // player 1 run left
Atlas atlas_1p_run_right;  // player 1 run right
Atlas atlas_2p_idle_up;    // player 2 idle up
Atlas atlas_2p_idle_down;  // player 2 idle down
Atlas atlas_2p_idle_left;  // player 2 idle left
Atlas atlas_2p_idle_right; // player 2 idle right
Atlas atlas_2p_run_up;     // player 2 run up
Atlas atlas_2p_run_down;   // player 2 run down
Atlas atlas_2p_run_left;   //  player 2 run left
Atlas atlas_2p_run_right;  // player 2 run right

// ui
IMAGE img_ui_1;       // ui image 1
IMAGE img_ui_2;       // ui image 2
IMAGE img_ui_3;       // ui image 3
IMAGE img_ui_fight;   // ui image fight
IMAGE img_ui_textbox; // ui image textbox
IMAGE img_background; // background image

// text
int idx_line = 0;                       // current line index
int idx_char = 0;                       // current character index
std::string str_text;                   // text
std::vector<std::string> str_line_text; // line text

// server
std::string str_address;
httplib::Client *client = nullptr;

// coverter
std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

// load assets
void load_resources(HWND hwnd)
{
    AddFontResourceEx(L"asset/IPix.ttf", FR_PRIVATE, 0);

    atlas_1p_idle_up.load(L"asset/img/hajimi_idle_back_%d.png", 4);
    atlas_1p_idle_down.load(L"asset/img/hajimi_idle_front_%d.png", 4);
    atlas_1p_idle_left.load(L"asset/img/hajimi_idle_left_%d.png", 4);
    atlas_1p_idle_right.load(L"asset/img/hajimi_idle_right_%d.png", 4);
    atlas_1p_run_up.load(L"asset/img/hajimi_run_back_%d.png", 4);
    atlas_1p_run_down.load(L"asset/img/hajimi_run_front_%d.png", 4);
    atlas_1p_run_left.load(L"asset/img/hajimi_run_left_%d.png", 4);
    atlas_1p_run_right.load(L"asset/img/hajimi_run_right_%d.png", 4);
    atlas_2p_idle_up.load(L"asset/img/manbo_idle_back_%d.png", 4);
    atlas_2p_idle_down.load(L"asset/img/manbo_idle_front_%d.png", 4);
    atlas_2p_idle_left.load(L"asset/img/manbo_idle_left_%d.png", 4);
    atlas_2p_idle_right.load(L"asset/img/manbo_idle_right_%d.png", 4);
    atlas_2p_run_up.load(L"asset/img/manbo_run_back_%d.png", 4);
    atlas_2p_run_down.load(L"asset/img/manbo_run_front_%d.png", 4);
    atlas_2p_run_left.load(L"asset/img/manbo_run_left_%d.png", 4);
    atlas_2p_run_right.load(L"asset/img/manbo_run_right_%d.png", 4);

    loadimage(&img_ui_1, L"asset/img/ui_1.png");
    loadimage(&img_ui_2, L"asset/img/ui_2.png");
    loadimage(&img_ui_3, L"asset/img/ui_3.png");
    loadimage(&img_ui_fight, L"asset/img/ui_fight.png");
    loadimage(&img_ui_textbox, L"asset/img/ui_textbox.png");
    loadimage(&img_background, L"asset/img/background.png");

    load_audio(L"asset/audio/bgm.mp3", L"bgm");
    load_audio(L"asset/audio/1p_win.mp3", L"1p_win");
    load_audio(L"asset/audio/2p_win.mp3", L"2p_win");
    load_audio(L"asset/audio/click_1.mp3", L"click_1");
    load_audio(L"asset/audio/click_2.mp3", L"click_2");
    load_audio(L"asset/audio/click_3.mp3", L"click_3");
    load_audio(L"asset/audio/click_4.mp3", L"click_4");
    load_audio(L"asset/audio/ui_1.mp3", L"ui_1");
    load_audio(L"asset/audio/ui_2.mp3", L"ui_2");
    load_audio(L"asset/audio/ui_3.mp3", L"ui_3");
    load_audio(L"asset/audio/ui_fight.mp3", L"ui_fight");

    std::ifstream file("config.cfg");
    if (!file.is_open())
    {
        MessageBox(hwnd, L"config.cfg not found!", L"Error", MB_OK | MB_ICONERROR);
        exit(-1);
    }
    std::stringstream ss;
    ss << file.rdbuf();
    str_address = ss.str();
    file.close();
}

// login to server
void login_to_server(HWND hwnd)
{
    client = new httplib::Client(str_address);
    client->set_keep_alive(true);
    httplib::Result res = client->Post("/login");
    if (!res || res->status != 200)
    {
        MessageBox(hwnd, L"Failed to login to server!", L"Error", MB_OK | MB_ICONERROR);
        exit(-1);
    }

    id_player = std::stoi(res->body);

    if (id_player <= 0)
    {
        MessageBox(hwnd, L"Server is Full!", L"Reject", MB_OK | MB_ICONERROR);
        exit(-1);
    }

    (id_player == 1) ? progress_1 = 0 : progress_2 = 0;

    str_text = client->Post("/query_text")->body;

    std::stringstream ss(str_text);
    std::string str_line;
    while (std::getline(ss, str_line))
    {
        str_line_text.push_back(str_line);
        num_total_char += (int)str_line.size();
    }

    std::thread([&]()
                { while(running){
                    using namespace std::chrono;
                    std::string route = id_player == 1 ? "/update_1" : "/update_2";
                    std::string body = std::to_string((id_player == 1 ? progress_1 : progress_2).load());
                    httplib::Result res = client->Post(route, body, "text/plain");
                    if(res && res->status == 200){
                        int progress = std::stoi(res->body);
                        (id_player == 1 ? progress_2 : progress_1).store(progress);
                    }
                    std::this_thread::sleep_for(milliseconds(100));
                } })
        .detach();
}

// logout from server
void logout_from_server()
{
    running.store(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(101));
    client->Post("/logout", (id_player == 1) ? "1" : "2", "text/plain");
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
// int main()
{
    // FreeConsole();
    ////////////处理游戏初始化///////////
    using namespace std::chrono;

    HWND hwnd = initgraph(1280, 720);
    SetWindowText(hwnd, _T("This is a game"));
    settextstyle(28, 0, L"IPix");
    setbkmode(TRANSPARENT);
    SetForegroundWindow(hwnd);
    load_resources(hwnd);
    login_to_server(hwnd);

    ExMessage msg;
    Timer timer_countdown;
    Timer timer_cursor;
    Camera camera_ui, camera_scene;

    Player player1(atlas_1p_idle_up, atlas_1p_idle_down, atlas_1p_idle_left, atlas_1p_idle_right, atlas_1p_run_up, atlas_1p_run_down, atlas_1p_run_left, atlas_1p_run_right);
    Player player2(atlas_2p_idle_up, atlas_2p_idle_down, atlas_2p_idle_left, atlas_2p_idle_right, atlas_2p_run_up, atlas_2p_run_down, atlas_2p_run_left, atlas_2p_run_right);

    camera_ui.set_size({1280, 720});
    camera_scene.set_size({1280, 720});

    player1.set_position({842, 842});
    player2.set_position({842, 842});

    timer_cursor.set_one_shot(false);
    timer_cursor.set_wait_time(0.5f);
    timer_cursor.set_time_out([&]()
                              { cursro_blink = !cursro_blink; });
    timer_countdown.set_one_shot(false);
    timer_countdown.set_wait_time(1.0f);
    timer_countdown.set_time_out([&]()
                                 { 
                                    val_countdown--;
                                    switch(val_countdown){
                                        case 3:
                                            play_audio(L"ui_3");
                                            break;
                                        case 2:
                                            play_audio(L"ui_2");
                                            break;
                                        case 1:
                                            play_audio(L"ui_1");
                                            break;
                                        case 0:
                                            play_audio(L"ui_fight");
                                            break;
                                        case -1:
                                            stage = Stage::Racing;
                                            play_audio(L"bgm", true);
                                            break;
                                    } });
    const milliseconds frame_duration = milliseconds(1000 / 144);
    steady_clock::time_point last_frame_time = steady_clock::now();
    BeginBatchDraw();
    while (true)
    {
        ////////////处理玩家输入///////////
        while (peekmessage(&msg))
        {
            // std::cout << msg.message << " " << isClose(hwnd) << std::endl;
            if (msg.message == WM_KEYUP && msg.vkcode == VK_ESCAPE)
            {
                logout_from_server();
                exit(0);
            }
            if (stage != Stage::Racing)
            {
                continue;
            }
            if (msg.message == WM_CHAR && idx_line < str_line_text.size())
            {
                const std::string &str_line = str_line_text[idx_line];
                if (str_line[idx_char] == msg.ch)
                {
                    switch (rand() % 4)
                    {
                    case 0:
                        play_audio(L"click_1");
                        break;
                    case 1:
                        play_audio(L"click_2");
                        break;
                    case 2:
                        play_audio(L"click_3");
                        break;
                    case 3:
                        play_audio(L"click_4");
                        break;
                    }

                    (id_player == 1 ? progress_1 : progress_2)++;
                    idx_char++;
                    if (idx_char >= str_line.size())
                    {
                        idx_char = 0;
                        idx_line++;
                    }
                }
            }
        }
        ////////////处理游戏更新///////////
        steady_clock::time_point frame_start = steady_clock::now();
        duration<float> delta = duration<float>(frame_start - last_frame_time);

        if (stage == Stage::Waiting)
        {
            if (progress_1 >= 0 && progress_2 >= 0)
            {
                stage = Stage::Ready;
            }
        }
        else
        {
            if (stage == Stage::Ready)
            {
                timer_countdown.on_update(delta.count());
            }
            timer_cursor.on_update(delta.count());
            if ((id_player == 1 && progress_1 >= num_total_char) || (id_player == 2 && progress_2 >= num_total_char))
            {
                stop_audio(L"bgm");
                play_audio(id_player == 1 ? L"1p_win" : L"2p_win");
                MessageBox(hwnd, L"You Win!", L"Game Over", MB_OK | MB_ICONINFORMATION);
                logout_from_server();
                exit(0);
            }
            else if ((id_player == 1 && progress_2 >= num_total_char) || (id_player == 2 && progress_1 >= num_total_char))
            {
                stop_audio(L"bgm");
                MessageBox(hwnd, L"You Lost!", L"Game Over", MB_OK | MB_ICONINFORMATION);
                logout_from_server();
                exit(0);
            }

            player1.set_target(path.get_postion_at_progress((float)progress_1 / num_total_char));
            player2.set_target(path.get_postion_at_progress((float)progress_2 / num_total_char));

            player1.on_update(delta.count());
            player2.on_update(delta.count());

            camera_scene.look_at((id_player == 1 ? player1.get_position() : player2.get_position()));
        }

        ////////////处理画面绘制///////////
        setbkcolor(RGB(0, 0, 0));
        cleardevice();

        if (stage == Stage::Waiting)
        {
            outtextxy(15, 675, L"Waiting for players to join...");
        }
        else
        {
            // draw background
            static const Rect rect_bg = {0, 0, img_background.getwidth(), img_background.getheight()};
            putimage_ex(camera_scene, &img_background, &rect_bg);

            // draw player
            if (player1.get_position().y > player2.get_position().y)
            {
                player2.on_render(camera_scene);
                player1.on_render(camera_scene);
            }
            else
            {
                player1.on_render(camera_scene);
                player2.on_render(camera_scene);
            }

            // draw countdown
            switch (val_countdown)
            {
            case 3:
            {
                static const Rect rect_ui_3 = {1280 / 2 - img_ui_3.getwidth() / 2, 720 / 2 - img_ui_3.getheight() / 2, img_ui_3.getwidth(), img_ui_3.getheight()};
                putimage_ex(camera_ui, &img_ui_3, &rect_ui_3);
            }
            break;
            case 2:
            {
                static const Rect rect_ui_2 = {1280 / 2 - img_ui_2.getwidth() / 2, 720 / 2 - img_ui_2.getheight() / 2, img_ui_2.getwidth(), img_ui_2.getheight()};
                putimage_ex(camera_ui, &img_ui_2, &rect_ui_2);
            }
            break;
            case 1:
            {
                static const Rect rect_ui_1 = {1280 / 2 - img_ui_1.getwidth() / 2, 720 / 2 - img_ui_1.getheight() / 2, img_ui_1.getwidth(), img_ui_1.getheight()};
                putimage_ex(camera_ui, &img_ui_1, &rect_ui_1);
            }
            break;
            case 0:
            {
                static const Rect rect_ui_fight = {1280 / 2 - img_ui_fight.getwidth() / 2, 720 / 2 - img_ui_fight.getheight() / 2, img_ui_fight.getwidth(), img_ui_fight.getheight()};
                putimage_ex(camera_ui, &img_ui_fight, &rect_ui_fight);
            }
            break;
            default:
                break;
            }
            // draw ui
            if (stage == Stage::Racing)
            {
                static const Rect rect_ui_textbox = {0, 720 - img_ui_textbox.getheight(), img_ui_textbox.getwidth(), img_ui_textbox.getheight()};
                std::wstring wstr_line = converter.from_bytes(str_line_text[idx_line]);
                std::wstring wstr_completed = converter.from_bytes(str_line_text[idx_line].substr(0, idx_char));
                putimage_ex(camera_ui, &img_ui_textbox, &rect_ui_textbox);
                settextcolor(RGB(125, 125, 125));
                outtextxy(185 + 2, rect_ui_textbox.y + 65 + 2, wstr_line.c_str());
                settextcolor(RGB(25, 25, 25));
                outtextxy(185, rect_ui_textbox.y + 65, wstr_line.c_str());
                settextcolor(RGB(0, 149, 217));
                outtextxy(185, rect_ui_textbox.y + 65, wstr_completed.c_str());
                setlinecolor(RGB(0, 149, 217));
                setlinestyle(PS_SOLID, 2);
                if (cursro_blink)
                {
                    line(185 + textwidth(wstr_completed.c_str()), rect_ui_textbox.y + 65, 185 + textwidth(wstr_completed.c_str()), rect_ui_textbox.y + 65 + 28);
                }
            }
        }
        FlushBatchDraw();

        last_frame_time = frame_start;
        milliseconds sleep_time = frame_duration - duration_cast<milliseconds>(steady_clock::now() - frame_start);
        if (sleep_time.count() > 0)
        {
            std::this_thread::sleep_for(sleep_time);
        }
    }
    EndBatchDraw();
    return 0;
}