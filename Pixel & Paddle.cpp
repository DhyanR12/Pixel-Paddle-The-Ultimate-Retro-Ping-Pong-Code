#include <iostream>
#include <raylib.h>

using namespace std;

Color Green = {38, 185, 154, 255};
Color Deep_Green = {20, 160, 133, 255};
Color Light_Green = {129, 204, 184, 255};
Color Ball_Yellow = {243, 213, 91, 255};
int player_score = 0;
int cpu_score = 0;
int Winning_Score = 5;

enum GameState { Start, MENU, VS_CPU, PvP, Instruction_vsCPU, Instruction_Pvp, Credits, Player_Wins, CPU_Wins,InFo };
GameState current_state = Start;

class Ball {
public:
    float x, y;
    int speed_x, speed_y;
    int radius;
    float serve_Timer = 0.0f;

    void Draw() {
        DrawCircle(x, y, radius, Ball_Yellow);
    }

    void Update() {
        if (serve_Timer > 0) {
            serve_Timer -= GetFrameTime();
            return;
        }

        x += speed_x;
        y += speed_y;

        if (y + radius >= GetScreenHeight() || y - radius <= 0) {
            speed_y *= -1;
        }

        if (x + radius >= GetScreenWidth()) {
            cpu_score++;
            if (cpu_score >= Winning_Score) {
                current_state = CPU_Wins;
            } else {
                ResetBall();
            }
        }
        if (x - radius <= 0) {
            player_score++;
            if (player_score >= Winning_Score) {
                current_state = Player_Wins;
            } else {
                ResetBall();
            }
        }
    }

    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;
        int speed_Choices[2] = { 1, -1 };
        speed_x = 7 * speed_Choices[GetRandomValue(0, 1)];
        speed_y = 7 * speed_Choices[GetRandomValue(0, 1)];
        serve_Timer = 1.0f;
    }
};

class Paddle {
public:
    float X, Y;
    float height, width;
    float speed;

    void Limitedmove() {
        if (Y <= 0) Y = 0;
        if (Y + height >= GetScreenHeight()) Y = GetScreenHeight() - height;
    }

    void Draw() {
        DrawRectangleRounded(Rectangle{ X, Y, width, height }, 0.8, 0, WHITE);
    }

    void Update0() {
        if (IsKeyDown(KEY_UP)) Y = Y - speed;
        if (IsKeyDown(KEY_DOWN)) Y = Y + speed;
        Limitedmove();
    }
};

class CPU : public Paddle {
public:
    void Update1(int y_ball) {
        if (Y + height / 2 > y_ball) Y = Y - speed;
        if (Y + height / 2 < y_ball) Y = Y + speed;
        Limitedmove();
    }
};

class Player : public Paddle {
public:
    void update2() {
        if (IsKeyDown(KEY_W)) Y = Y - speed;
        if (IsKeyDown(KEY_S)) Y = Y + speed;
        Limitedmove();
    }
};

Ball ball;
Paddle player;
CPU cpu;
Player pvp;

int main() {
    int screen_width = 1200;
    int screen_height = 800;
    InitWindow(screen_width, screen_height, "Pong");
    InitAudioDevice();

    Texture2D background = LoadTexture("Graphics/PongBG.png");
    Texture2D MenuBG = LoadTexture("Graphics/Menu.png");
    Texture2D InstructionBG = LoadTexture("Graphics/Instruction_AI.png");
    Texture2D InstructionPVP = LoadTexture("Graphics/Instruction_PvP.png");
    Texture2D CreditsBG = LoadTexture("Graphics/Credits.png");
    Texture2D WinBG = LoadTexture("Graphics/WinBG.png");
    Texture2D LoseBG = LoadTexture("Graphics/LoseBG.png");
    Texture2D InfoBG = LoadTexture("Graphics/Info.png");

    Sound Bounce_Sound = LoadSound("Graphics/Jump.mp3");
    Music Menu_music = LoadMusicStream("Graphics/Main song.mp3");
    Menu_music.looping = true;
    PlayMusicStream(Menu_music);

    SetTargetFPS(60);

    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = 7;
    ball.speed_y = 7;
    ball.serve_Timer = 1.5f;

    player.width = 25;
    player.height = 120;
    player.X = screen_width - player.width - 10;
    player.Y = screen_height / 2 - player.height / 2;
    player.speed = 6;

    cpu.width = 25;
    cpu.height = 120;
    cpu.speed = 6;
    cpu.X = 10;
    cpu.Y = screen_height / 2 - cpu.height / 2;

    pvp.width = 25;
    pvp.height = 120;
    pvp.speed = 6;
    pvp.X = 10;
    pvp.Y = screen_height / 2 - pvp.height / 2;

    while (!WindowShouldClose()) {
        if (current_state != VS_CPU && current_state != PvP) {
            UpdateMusicStream(Menu_music);
            if (!IsMusicStreamPlaying(Menu_music)) PlayMusicStream(Menu_music);
        }
        else {
            StopMusicStream(Menu_music);
        }

        switch (current_state) {
        case Start:
            if (GetKeyPressed() != 0 || IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                current_state = MENU;
            }
            break;

        case MENU:
            player_score = 0;
            cpu_score = 0;
            if (IsKeyPressed(KEY_ONE)) current_state = Instruction_vsCPU;
            if (IsKeyPressed(KEY_TWO)) current_state = Instruction_Pvp;
            if (IsKeyPressed(KEY_THREE)) current_state = Credits;
            if (IsKeyPressed(KEY_FOUR)) current_state = InFo;
            break;

        case Credits:
            if (IsKeyPressed(KEY_B)) current_state = MENU;
            break;
         case InFo:
            if (IsKeyPressed(KEY_B)) current_state = MENU;
            break;   

        case Instruction_vsCPU:
            if (GetKeyPressed() != 0 || IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                current_state = VS_CPU;
                ball.ResetBall();
            }
            break;

        case Instruction_Pvp:
            if (GetKeyPressed() != 0 || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                current_state = PvP;
                ball.ResetBall();
            }
            break;

        case VS_CPU:
            if (IsKeyPressed(KEY_B)) current_state = MENU;
            ball.Update();
            player.Update0();
            cpu.Update1(ball.y);

            if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, (float)ball.radius, Rectangle{ player.X, player.Y, player.width, player.height })) {
                ball.speed_x *= -1;
                PlaySound(Bounce_Sound);
            }
            if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, (float)ball.radius, Rectangle{ cpu.X, cpu.Y, cpu.width, cpu.height })) {
                ball.speed_x *= -1;
                PlaySound(Bounce_Sound);
            }
            break;

        case PvP:
            if (IsKeyPressed(KEY_B)) current_state = MENU;
            ball.Update();
            player.Update0();
            pvp.update2();

            if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, (float)ball.radius, Rectangle{ player.X, player.Y, player.width, player.height })) {
                ball.speed_x *= -1;
                PlaySound(Bounce_Sound);
            }
            if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, (float)ball.radius, Rectangle{ pvp.X, pvp.Y, pvp.width, pvp.height })) {
                ball.speed_x *= -1;
                PlaySound(Bounce_Sound);
            }
            break;

        case Player_Wins:
        if (IsKeyPressed(KEY_B)) current_state = MENU;
            break;
        case CPU_Wins:
            if (IsKeyPressed(KEY_B)) current_state = MENU;
            break;
        }

        BeginDrawing();
        switch (current_state) {
        case Start:
            DrawTexture(background, 0, 0, WHITE);
            DrawText("Press Any Key To Start", 360, 600, 40, Ball_Yellow);
            break;
        case MENU:
            DrawTexture(MenuBG, 0, 0, WHITE);
            break;
        case Credits:
            DrawTexture(CreditsBG, 0, 0, WHITE);
            break;
            case InFo:
                DrawTexture(InfoBG, 0, 0, WHITE);
                break;
        case Instruction_vsCPU:
            DrawTexture(InstructionBG, 0, 0, WHITE);
            break;
        case Instruction_Pvp:
            DrawTexture(InstructionPVP, 0, 0, WHITE);
            break;
        case VS_CPU:
            ClearBackground(Deep_Green);
            DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, Green);
            DrawCircle(screen_width / 2, screen_height / 2, 150, Light_Green);
            DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
            ball.Draw();
            player.Draw();
            cpu.Draw();
            DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
            DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);
            break;
        case PvP:
            ClearBackground(Deep_Green);
            DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, Green);
            DrawCircle(screen_width / 2, screen_height / 2, 150, Light_Green);
            DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
            ball.Draw();
            player.Draw();
            pvp.Draw();
            DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
            DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);
            break;
        case Player_Wins:
            DrawTexture(WinBG, 0, 0, WHITE);
            break;
        case CPU_Wins:
            DrawTexture(LoseBG, 0, 0, WHITE);
            break;
        }
        EndDrawing();
    }

    UnloadTexture(background);
    UnloadTexture(MenuBG);
    UnloadTexture(InstructionBG);
    UnloadTexture(InstructionPVP);
    UnloadTexture(CreditsBG);
    UnloadTexture(WinBG);
    UnloadTexture(LoseBG);
    UnloadSound(Bounce_Sound);
    UnloadMusicStream(Menu_music);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}              
           
