#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#define GRAVITY 0.3f
#define WINDOW_HEIGHT 800.0f
#define WINDOW_WIDTH 1600.0f
#define MAX_PLATFORMS 20

typedef struct {
    Rectangle top, left, bottom, right;
} HitBox;

typedef struct {
    Rectangle body;
    HitBox hitBox;
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

typedef struct {
    Rectangle rec;
    HitBox hitBox;
    Color color;
} Platform;



HitBox* createRecHitBox(Rectangle* rec) {
    HitBox *p = (HitBox*) malloc(sizeof(HitBox));
    if (p != NULL) {
        *p = (HitBox){
            {
                rec->x,
                rec->y,
                rec->width,
                2.0f
            },
            {
                rec->x,
                rec->y,
                2.0f,
                rec->height
            },
            {
                rec->x,
                rec->y + rec->height - 2.0f,
                rec->width,
                2.0f
            },
            {
                rec->x + rec->width - 2.0f,
                rec->y,
                2.0f,
                rec->height,
            }
        };
    }

    return p;
}

Platform* createPlatform(float x, float y, float width, float height, Color color) {
    Platform* p = (Platform*) malloc(sizeof(Platform));
    if (p != NULL) {
        Rectangle rec = { x, y, width, height };
        *p = (Platform){
            rec,
            *createRecHitBox(&rec),
            color
        };
    }

    return p;
}

void addPlatform(int* countPlatforms, Platform** platformsArray, Platform* platform) {
    platformsArray[*countPlatforms] = platform;
    (*countPlatforms)++;
}

// UPDATES

void updateHitBox(Rectangle* rec, HitBox* hitBox) {
    hitBox->top.x = rec->x;
    hitBox->top.y = rec->y;

    hitBox->left.x = rec->x;
    hitBox->left.y = rec->y;

    hitBox->bottom.x = rec->x;
    hitBox->bottom.y = rec->y + rec->height - 2.0f;

    hitBox->right.x = rec->x + rec->width - 2.0f;
    hitBox->right.y = rec->y;
}
void updatePlayer(Player* player) {
    if (player->speedX != 0) {
        player->body.x += player->speedX;
    }

    player->body.y += player->speedY;
    player->speedY += GRAVITY;

    updateHitBox(&(player->body), &(player->hitBox));

}
void updateCollisions(Player* player, Rectangle* ground) {
     player->onGround = false;
     player->bottomTouch = false;
     player->topTouch = false;
     player->rightTouch = false;
     player->leftTouch = false;

     if (CheckCollisionRecs(player->hitBox.bottom, *ground)) {
         player->onGround = true;
         player->bottomTouch = true;
         player->body.y = ground->y - player->body.height;
     }

     //for (int i = 0; i < platformCount; i++) {
     //    if (CheckCollisionRecs(player->hitBox.bottom, platforms[i])) {
     //        player->bottomTouch = true;
     //        player->onGround = true;
     //        player->body.y = platforms[i].y - player->body.height;
     //    }
     //    if (CheckCollisionRecs(player->hitBox.top, platforms[i])) {
     //        player->topTouch = true;
     //        //player->body.y = platforms[i].y + platforms[i].height;
     //    }
     //    if (CheckCollisionRecs(player->hitBox.right, platforms[i])) {
     //        player->rightTouch = true;
     //        player->body.x = platforms[i].x - player->body.width;
     //    }
     //    if (CheckCollisionRecs(player->hitBox.left, platforms[i])) {
     //        player->leftTouch = true;
     //        player->body.x = platforms[i].x + platforms[i].width;
     //    }
     //}

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

// ACTIONS

void movePlayer(Player* player, float speed, char axis) {
    if (axis == 'x') {
        player->body.x += speed;
    }
    else if (axis == 'y') {
        player->body.y += speed;
    }
}
void jumpPlayer(Player* player, float initialSpeed) {
    player->speedY = initialSpeed;
}

// MAIN

int main() {
    int countPlatforms = 0;
    Platform** platforms = (Platform**)malloc(MAX_PLATFORMS * sizeof(Platform*));

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

    player.hitBox = *createRecHitBox(&(player.body));

    // RESOLVER O PROBLEMA DE TELEPORTAR EM COLISAO LATERAL / ENTRAR NO CHAO EM COLISAO VERTICAL DE ALTA VELOCIADE

	Rectangle ground;
	ground.height = 100.0f;
	ground.width = WINDOW_WIDTH;
	ground.x = 0.0f;
	ground.y = WINDOW_HEIGHT - ground.height;

    // Platforms

    Platform platform1 = *createPlatform(100.0f, 650.0f, 200.0f, 40.0f, RED);
    Platform platform2 = *createPlatform(500.0f, 650.0f, 200.0f, 40.0f, BLUE);
    Platform platform3 = *createPlatform(900.0f, 650.0f, 200.0f, 40.0f, DARKPURPLE);

    addPlatform(&countPlatforms, platforms, &platform1);
    addPlatform(&countPlatforms, platforms, &platform2);
    addPlatform(&countPlatforms, platforms, &platform3);

    

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
            
            for (int i = 0; i < countPlatforms; i++) {
                DrawRectangleRec(platforms[i]->rec, platforms[i]->color);
                DrawRectangleRec(platforms[i]->hitBox.bottom, BLACK);
                DrawRectangleRec(platforms[i]->hitBox.top, BLUE);
                DrawRectangleRec(platforms[i]->hitBox.right, PURPLE);
                DrawRectangleRec(platforms[i]->hitBox.left, PINK);
            }

            DrawRectangleRec(player.body, RED);

            DrawRectangleRec(player.hitBox.bottom, BLACK);
            DrawRectangleRec(player.hitBox.top, BLUE);
            DrawRectangleRec(player.hitBox.right, PURPLE);
            DrawRectangleRec(player.hitBox.left, PINK);

        EndDrawing();

    }
    CloseWindow();
    return 0;


}

