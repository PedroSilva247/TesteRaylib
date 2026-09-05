#include <stdio.h>
#include <raylib.h>

#define GRAVITY 0.3f
#define WINDOW_HEIGHT 800.0f
#define WINDOW_WIDTH 1600.0f



typedef struct {
    Rectangle body, bottom, top, right, left;
    int health;
    float speedX;
    float speedY;
    float defaultSpeed;
    bool onGround;
    bool bottomTouch;
    bool topTouch;
    bool rightTouch;
    bool leftTouch;
} Player;

Rectangle platforms[] = {
	{ 0.0f, 700.0f, 400.0f, 100.0f },
	{ 500.0f, 600.0f, 300.0f, 100.0f },
	{ 900.0f, 500.0f, 200.0f, 100.0f },
	{ 1200.0f, 400.0f, 300.0f, 100.0f }
};
int platformCount = sizeof(platforms) / sizeof(platforms[0]);

void movePlayer(Player* player, float speed, char axis) {
    if (axis == 'x') {
        player->body.x += speed;
    }
    else if (axis == 'y') {
        player->body.y += speed;
    }
}

void updatePlayer(Player* player) {
    if (player->speedX != 0) {
        player->body.x += player->speedX;
    }

    player->body.y += player->speedY;
    player->speedY += GRAVITY;

    player->bottom.x = player->body.x + (player->body.width - player->bottom.width) / 2;
    player->bottom.y = player->body.y + player->body.height;

    player->top.x = player->body.x + (player->body.width - player->top.width) / 2;
    player->top.y = player->body.y - player->top.height;

    player->right.x = player->body.x + player->body.width;
    player->right.y = player->body.y + (player->body.height - player->right.height) / 2;

    player->left.x = player->body.x - player->left.width;
    player->left.y = player->body.y + (player->body.height - player->left.height)/2;

}

void updateCollisions(Player* player, Rectangle* ground) {
     player->onGround = false;
     player->bottomTouch = false;
     player->topTouch = false;
     player->rightTouch = false;
     player->leftTouch = false;

     if (CheckCollisionRecs(player->bottom, *ground)) {
         player->onGround = true;
         player->bottomTouch = true;
         player->body.y = ground->y - player->body.height;
     }

     for (int i = 0; i < platformCount; i++) {
         if (CheckCollisionRecs(player->bottom, platforms[i])) {
             player->bottomTouch = true;
             player->onGround = true;
             player->body.y = platforms[i].y - player->body.height;
         }
         if (CheckCollisionRecs(player->top, platforms[i])) {
             player->topTouch = true;
             //player->body.y = platforms[i].y + platforms[i].height;
         }
         if (CheckCollisionRecs(player->right, platforms[i])) {
             player->rightTouch = true;
             player->body.x = platforms[i].x - player->body.width;
         }
         if (CheckCollisionRecs(player->left, platforms[i])) {
             player->leftTouch = true;
             player->body.x = platforms[i].x + platforms[i].width;
         }
     }

     if (player->bottomTouch) {
         player->speedY = 0;
     }
     if (player->topTouch) {
         player->speedY = -GRAVITY;
     }
     if (player->leftTouch) {
         player->speedX = 0; 
     }
     if (player->rightTouch) {
         player->speedX = 0;
     }
}

void jumpPlayer(Player* player, float initialSpeed) {
    player->speedY = initialSpeed;
}

int main() {
    InitWindow((int) WINDOW_WIDTH, (int) WINDOW_HEIGHT, "Jogo 01");

    SetTargetFPS(60);

    Player player;
    player.body.x = 50.0f;
    player.body.y = 100.0f;
    player.body.height = 20.0f;
    player.body.width = 20.0f;

    player.health = 100;
    player.speedX = 0;
    player.speedY = 0;
    player.defaultSpeed = 4;

    player.bottom.x = player.body.x + 1.0f;
    player.bottom.y = player.body.y + player.body.height;
    player.bottom.width = player.body.width - 2.0f;
    player.bottom.height = 3.0f;

    player.top.x = player.body.x + 1.0f;
    player.top.y = player.body.y;
    player.top.width = player.body.width - 2.0f;
    player.top.height = 3.0f;

    player.right.x = player.body.x + player.body.width;
    player.right.y = player.body.y + 1.0f;
    player.right.width = 3.0f;
    player.right.height = player.body.height - 2.0f;

    player.left.x = player.body.x;
    player.left.y = player.body.y + 1.0f;
    player.left.width = 3.0f;
    player.left.height = player.body.height - 2.0f;

    // RESOLVER O PROBLEMA DE TELEPORTAR EM COLISAO LATERAL / ENTRAR NO CHAO EM COLISAO VERTIVAL DE ALTA VELOCIADE

	Rectangle ground;
	ground.height = 100.0f;
	ground.width = WINDOW_WIDTH;
	ground.x = 0.0f;
	ground.y = WINDOW_HEIGHT - ground.height;


    

    while (!WindowShouldClose()) {
        // KEYBOARD
        if (IsKeyDown(KEY_W) && player.onGround) {
            jumpPlayer(&player, -8.0f);
        }
        if (IsKeyDown(KEY_A)) {
            if (!player.leftTouch) {
                player.speedX = -(player.defaultSpeed);
            }
        } else if (IsKeyDown(KEY_D)) {
            if (!player.rightTouch) {
                player.speedX = player.defaultSpeed;
            }
        }
        else {
            player.speedX = 0;
        }
        

        // UPDATES
        updatePlayer(&player);

        // COLLISIONS
        updateCollisions(&player, &ground);
        
        

        // DRAWING
        BeginDrawing();
            ClearBackground(WHITE);

            DrawRectangleRec(ground, GREEN);
            for(int i = 0; i < platformCount; i++) {
                DrawRectangleRec(platforms[i], BROWN);
            }
            DrawRectangleRec(player.body, RED);

            DrawRectangleRec(player.bottom, BLACK);
            DrawRectangleRec(player.top, BLUE);
            DrawRectangleRec(player.right, PURPLE);
            DrawRectangleRec(player.left, PINK);

        EndDrawing();

    }
    CloseWindow();
    return 0;


}

