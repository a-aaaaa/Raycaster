#include <iostream>
#include <Windows.h>
#include <chrono>
#include <utility>

using namespace std;

int screenWidth = 180;
int screenHeight = 60;

double playerX = 8;
double playerY = 12;
double playerDirX = -1;
double playerDirY = -1;
double playerMoveSpeed = 0.05;
double playerRotSpeed = 0.02;
double planeX = 0;
double planeY = 0.66;

int mapWidth = 16;
int mapHeight = 16;
int map[] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

int main()
{
    wchar_t *screen = new wchar_t[screenWidth * screenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    bool escPressed = false;
    double drawInterval = 1000000000 / 60.0;
    double delta = 0;
    auto previousTime = chrono::high_resolution_clock::now();
    auto currentTime = chrono::high_resolution_clock::now();
        
    while (!escPressed)
    {
        while (delta < 1) {
            currentTime = chrono::high_resolution_clock::now();
            delta += chrono::duration_cast<chrono::nanoseconds>(currentTime - previousTime).count() / drawInterval;
            previousTime = currentTime;
        }

        delta = 0;

        if (GetAsyncKeyState(VK_ESCAPE))
            escPressed = true;

        if (GetAsyncKeyState('W'))
        {
            if (map[(int)(playerX + playerDirX * playerMoveSpeed) + (int)playerY * mapWidth] == 0)
                playerX += playerDirX * playerMoveSpeed;
            if (map[(int)playerX + (int)(playerY + playerDirY * playerMoveSpeed) * mapWidth] == 0)
                playerY += playerDirY * playerMoveSpeed;
        }
        if (GetAsyncKeyState('S'))
        {
            if (map[(int)(playerX - playerDirX * playerMoveSpeed) + (int)playerY * mapWidth] == 0)
                playerX -= playerDirX * playerMoveSpeed;
            if (map[(int)playerX + (int)(playerY - playerDirY * playerMoveSpeed) * mapWidth] == 0)
                playerY -= playerDirY * playerMoveSpeed;
        }
        if (GetAsyncKeyState('A'))
        {
            double oldDirX = playerDirX;
            playerDirX = playerDirX * cos(playerRotSpeed) - playerDirY * sin(playerRotSpeed);
            playerDirY = oldDirX * sin(playerRotSpeed) + playerDirY * cos(playerRotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(playerRotSpeed) - planeY * sin(playerRotSpeed);
            planeY = oldPlaneX * sin(playerRotSpeed) + planeY * cos(playerRotSpeed);
        }
        if (GetAsyncKeyState('D'))
        {
            double oldDirX = playerDirX;
            playerDirX = playerDirX * cos(-playerRotSpeed) - playerDirY * sin(-playerRotSpeed);
            playerDirY = oldDirX * sin(-playerRotSpeed) + playerDirY * cos(-playerRotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(-playerRotSpeed) - planeY * sin(-playerRotSpeed);
            planeY = oldPlaneX * sin(-playerRotSpeed) + planeY * cos(-playerRotSpeed);
        }


        for (int x = 0; x < screenWidth; x++)
        {
            double cameraX = 2.0 * x / screenWidth - 1;
            double rayDirX = playerDirX + planeX * cameraX;
            double rayDirY = playerDirY + planeY * cameraX;

            int mapX = int(playerX);
            int mapY = int(playerY);

            double sideDistX, sideDistY;

            double deltaDistX = abs(1 / rayDirX);
            double deltaDistY = abs(1 / rayDirY);
            double wallDist;

            int stepX, stepY;
            bool hit = false;
            int side = 0;

            if (rayDirX < 0)
            {
                stepX = -1;
                sideDistX = (playerX - mapX) * deltaDistX;
            }
            else
            {
                stepX = 1;
                sideDistX = (mapX + 1.0 - playerX) * deltaDistX;
            }
            if (rayDirY < 0)
            {
                stepY = -1;
                sideDistY = (playerY - mapY) * deltaDistY;
            }
            else
            {
                stepY = 1;
                sideDistY = (mapY + 1.0 - playerY) * deltaDistY;
            }

            while (!hit)
            {
                if (sideDistX < sideDistY)
                {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                }
                else
                {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }

                if (map[mapX + mapY * mapWidth])
                    hit = true;
            }

            if (side == 0)
                wallDist = sideDistX - deltaDistX;
            else
                wallDist = sideDistY - deltaDistY;

            int lineHeight = (int)(screenHeight / wallDist);

            int drawStart = -lineHeight / 2 + screenHeight / 2;
            int drawEnd = lineHeight / 2 + screenHeight / 2;

            if (drawStart < 0)
                drawStart = 0;           
            if (drawEnd >= screenHeight)
                drawEnd = screenHeight - 1;

            for (int y = 0; y < screenHeight; y++)
                if (y <= drawStart)
                    screen[x + y * screenWidth] = ' ';
                else if (y > drawStart && y <= drawEnd)
                {
                    if (wallDist < 1)
                        screen[x + y * screenWidth] = '@';
                    else if (wallDist < 2)
                        screen[x + y * screenWidth] = '%';
                    else if (wallDist < 3)
                        screen[x + y * screenWidth] = '#';
                    else if (wallDist < 4)
                        screen[x + y * screenWidth] = '*';
                    else if (wallDist < 5)
                        screen[x + y * screenWidth] = '+';
                    else if (wallDist < 6)
                        screen[x + y * screenWidth] = '=';
                    else if (wallDist < 7)
                        screen[x + y * screenWidth] = ',';
                    else if (wallDist < 8)
                        screen[x + y * screenWidth] = '-';
                    else
                        screen[x + y * screenWidth] = '.';
                }
                else
                    screen[x + y * screenWidth] = ' ';
        }
        screen[screenWidth * screenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
    }
    return 0;
}