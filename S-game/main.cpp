#define GRAPHICS_API_VULKAN
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <iostream>

const int MAP_SIZE = 30; // Размер карты 30x30 тайлов

struct WorldObject {
    Vector3 position;
    bool active;
    int type; // 0 - дерево, 1 - цветок
};

float GetHeightAt(float heights[MAP_SIZE][MAP_SIZE], float x, float z) {
    if (x < 0 || x >= MAP_SIZE - 1 || z < 0 || z >= MAP_SIZE - 1) return 0.0f;
    int xi = (int)x;
    int zi = (int)z;
    float dx = x - xi;
    float dz = z - zi;
    float h1 = heights[xi][zi] * (1 - dx) + heights[xi + 1][zi] * dx;
    float h2 = heights[xi][zi + 1] * (1 - dx) + heights[xi + 1][zi + 1] * dx;
    return h1 * (1 - dz) + h2 * dz;
}

int main() {
    InitWindow(1280, 720, "Smeshariki World: Perlin & Interactions");

    // 1. ГЕНЕРАЦИЯ ШУМА ПЕРЛИНА
    Image perlinImage = GenImagePerlinNoise(MAP_SIZE, MAP_SIZE, 0, 0, 5.0f);
    Color* pixels = LoadImageColors(perlinImage);
    float heights[MAP_SIZE][MAP_SIZE];
    for (int y = 0; y < MAP_SIZE; y++) {
        for (int x = 0; x < MAP_SIZE; x++) {
            // Берем яркость пикселя как высоту (0.0 - 5.0)
            heights[x][y] = (pixels[y * MAP_SIZE + x].r / 255.0f) * 4.0f;
        }
    }
    UnloadImageColors(pixels);
    UnloadImage(perlinImage);

    // 2. ЗАГРУЗКА ТЕКСТУР (если файлов нет, Raylib нарисует белый квадрат)
    Texture2D charTex = LoadTexture("resources/kopatych.png");
    Texture2D treeTex = LoadTexture("resources/tree-kust.png");

    // Камера
    Camera3D camera = { { 20, 20, 20 }, { 15, 0, 15 }, { 0, 1, 0 }, 15.0f, CAMERA_ORTHOGRAPHIC };

    // Игрок
    Vector2 playerPos = { 15.0f, 15.0f };

    // 3. ОБЪЕКТЫ НА КАРТЕ
    std::vector<WorldObject> objects;
    for (int i = 0; i < 15; i++) {
        float rx = (float)GetRandomValue(2, MAP_SIZE - 2);
        float rz = (float)GetRandomValue(2, MAP_SIZE - 2);
        objects.push_back({ { rx, GetHeightAt(heights, rx, rz), rz }, true, 0 });
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // УПРАВЛЕНИЕ И ПРОВЕРКА ГРАНИЦ
        Vector2 moveDir = { 0 };
        if (IsKeyDown(KEY_W)) moveDir.y -= 1;
        if (IsKeyDown(KEY_S)) moveDir.y += 1;
        if (IsKeyDown(KEY_A)) moveDir.x -= 1;
        if (IsKeyDown(KEY_D)) moveDir.x += 1;

        if (Vector2Length(moveDir) > 0) {
            moveDir = Vector2Normalize(moveDir);
            float nextX = playerPos.x + moveDir.x * 5.0f * dt;
            float nextZ = playerPos.y + moveDir.y * 5.0f * dt;
            // Ограничение, чтобы не выйти за карту
            if (nextX >= 0 && nextX < MAP_SIZE - 1) playerPos.x = nextX;
            if (nextZ >= 0 && nextZ < MAP_SIZE - 1) playerPos.y = nextZ;
        }

        float currentH = GetHeightAt(heights, playerPos.x, playerPos.y);
        camera.target = { playerPos.x, currentH, playerPos.y };
        camera.position = Vector3Add(camera.target, { 15, 15, 15 });

        // 4. ОБРАБОТКА КЛИКА (Взаимодействие)
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Ray ray = GetMouseRay(GetMousePosition(), camera);
            for (auto& obj : objects) {
                if (obj.active) {
                    // Проверка столкновения луча со сферой вокруг объекта
                    RayCollision coll = GetRayCollisionSphere(ray, { obj.position.x, obj.position.y + 1.0f, obj.position.z }, 0.8f);
                    if (coll.hit) {
                        obj.active = false;
                        std::cout << "Объект собран! Ламповость повышена." << std::endl;
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);
        BeginMode3D(camera);

        // Рисуем ландшафт
        for (int x = 0; x < MAP_SIZE - 1; x++) {
            for (int z = 0; z < MAP_SIZE - 1; z++) {
                Vector3 v1 = { (float)x, heights[x][z], (float)z };
                Vector3 v2 = { (float)x + 1, heights[x + 1][z], (float)z };
                Vector3 v3 = { (float)x + 1, heights[x + 1][z + 1], (float)z + 1 };
                Vector3 v4 = { (float)x, heights[x][z + 1], (float)z + 1 };
                DrawTriangle3D(v1, v3, v2, ColorAlpha(LIME, 0.8f));
                DrawTriangle3D(v1, v4, v3, ColorAlpha(LIME, 0.9f));
            }
        }

        // Рисуем объекты (деревья)
        for (auto& obj : objects) {
            if (obj.active) {
                DrawBillboard(camera, treeTex, { obj.position.x, obj.position.y + 1.0f, obj.position.z }, 2.0f, WHITE);
            }
        }

        // Рисуем игрока
        DrawBillboard(camera, charTex, { playerPos.x, currentH + 0.8f, playerPos.y }, 1.5f, WHITE);

        EndMode3D();
        DrawText("Кликни на дерево, чтобы собрать его", 10, 10, 20, WHITE);
        EndDrawing();
    }

    UnloadTexture(charTex);
    UnloadTexture(treeTex);
    CloseWindow();
    return 0;
}
