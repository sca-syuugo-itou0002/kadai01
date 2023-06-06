#include <DxLib.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int PLAYER_WIDTH = 64;
const int PLAYER_HEIGHT = 64;
const float PLAYER_SPEED = 200.0f;

const int ENEMY_WIDTH = 32;
const int ENEMY_HEIGHT = 32;
const float ENEMY_SPEED = 100.0f;
const int ENEMY_COUNT = 5;

const int BULLET_WIDTH = 8;
const int BULLET_HEIGHT = 16;
const float BULLET_SPEED = 300.0f;

int playerX, playerY;
int enemyX[ENEMY_COUNT], enemyY[ENEMY_COUNT];
bool enemyActive[ENEMY_COUNT];
int bulletX, bulletY;
bool bulletActive = false;
int score = 0;

float deltaTime = 0.0f;
int prevTime = 0;

int backBuffer;

bool isGameOver = false;

void Initialize()
{
    SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
    ChangeWindowMode(TRUE);
    SetMainWindowText("Invaders");
    if (DxLib_Init() == -1)
    {
        exit(-1);
    }

    backBuffer = MakeScreen(SCREEN_WIDTH, SCREEN_HEIGHT, TRUE);

    playerX = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2;
    playerY = SCREEN_HEIGHT - PLAYER_HEIGHT;

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        enemyX[i] = (SCREEN_WIDTH / (ENEMY_COUNT + 1)) * (i + 1) - ENEMY_WIDTH / 2;
        enemyY[i] = ENEMY_HEIGHT;
        enemyActive[i] = true;
    }
}

bool AllEnemiesDefeated()
{
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        if (enemyActive[i])
        {
            return false;
        }
    }
    return true;
}

void Update()
{
    int currentTime = GetNowCount();
    deltaTime = (currentTime - prevTime) / 1000.0f;
    prevTime = currentTime;

    if (!isGameOver)
    {
        if (CheckHitKey(KEY_INPUT_LEFT))
        {
            playerX -= PLAYER_SPEED * deltaTime;
        }
        if (CheckHitKey(KEY_INPUT_RIGHT))
        {
            playerX += PLAYER_SPEED * deltaTime;
        }

        if (playerX < 0)
        {
            playerX = 0;
        }
        if (playerX > SCREEN_WIDTH - PLAYER_WIDTH)
        {
            playerX = SCREEN_WIDTH - PLAYER_WIDTH;
        }

        if (!bulletActive && CheckHitKey(KEY_INPUT_SPACE))
        {
            bulletActive = true;
            bulletX = playerX + PLAYER_WIDTH / 2 - BULLET_WIDTH / 2;
            bulletY = playerY - BULLET_HEIGHT;
        }

        if (bulletActive)
        {
            bulletY -= BULLET_SPEED * deltaTime;
            if (bulletY < -BULLET_HEIGHT)
            {
                bulletActive = false;
            }
        }

        for (int i = 0; i < ENEMY_COUNT; i++)
        {
            if (enemyActive[i])
            {
                enemyY[i] += ENEMY_SPEED * deltaTime;
                if (enemyY[i] > SCREEN_HEIGHT)
                {
                    enemyActive[i] = false;
                }

                if (bulletActive && bulletX > enemyX[i] - BULLET_WIDTH && bulletX < enemyX[i] + ENEMY_WIDTH && bulletY > enemyY[i] - BULLET_HEIGHT && bulletY < enemyY[i] + ENEMY_HEIGHT)
                {
                    score++;
                    bulletActive = false;
                    enemyActive[i] = false;
                }
            }
        }

        if (AllEnemiesDefeated())
        {
            for (int i = 0; i < ENEMY_COUNT; i++)
            {
                enemyY[i] = ENEMY_HEIGHT;
                enemyActive[i] = true;
            }
        }

        for (int i = 0; i < ENEMY_COUNT; i++)
        {
            if (enemyActive[i] && enemyY[i] + ENEMY_HEIGHT >= playerY)
            {
                isGameOver = true;
                break;
            }
        }
    }
    else
    {
        if (CheckHitKey(KEY_INPUT_ESCAPE))
        {
            isGameOver = false;
            score = 0;

            for (int i = 0; i < ENEMY_COUNT; i++)
            {
                enemyY[i] = ENEMY_HEIGHT;
                enemyActive[i] = true;
            }
        }
    }
}

void Draw()
{
    SetDrawScreen(backBuffer);
    ClearDrawScreen();

    DrawBox(playerX, playerY, playerX + PLAYER_WIDTH, playerY + PLAYER_HEIGHT, GetColor(255, 255, 255), TRUE);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        if (enemyActive[i])
        {
            DrawBox(enemyX[i], enemyY[i], enemyX[i] + ENEMY_WIDTH, enemyY[i] + ENEMY_HEIGHT, GetColor(255, 0, 0), TRUE);
        }
    }

    if (bulletActive)
    {
        DrawBox(bulletX, bulletY, bulletX + BULLET_WIDTH, bulletY + BULLET_HEIGHT, GetColor(0, 255, 0), TRUE);
    }

    if (isGameOver)
    {
        DrawString(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 20, "Game Over", GetColor(255, 255, 255));
        DrawString(SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 20, "Press ESC to restart", GetColor(255, 255, 255));
    }
    else
    {
        DrawFormatString(10, 10, GetColor(255, 255, 255), "Score: %d", score);
    }

    SetDrawScreen(DX_SCREEN_BACK);
    DrawGraph(0, 0, backBuffer, FALSE);

    ScreenFlip();
}

void Finalize()
{
    DeleteGraph(backBuffer);
    DxLib_End();
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    Initialize();

    while (ProcessMessage() == 0 && !CheckHitKey(KEY_INPUT_ESCAPE))
    {
        Update();
        Draw();
    }

    Finalize();

    return 0;
}
