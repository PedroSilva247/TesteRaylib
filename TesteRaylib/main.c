#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#define GRAVITY 0.08f
#define WINDOW_HEIGHT 800.0f
#define WINDOW_WIDTH 1600.0f
#define MAX_PLATFORMS 20
#define DURATION_ON_GROUND_TO_JUMP 0.1

typedef struct {
    Rectangle top, left, bottom, right;
} HitBox;

typedef struct {
    Rectangle body;
    HitBox hitBox;
    int health;
    //double durationOnGround;
    float speedX;
    float speedY;
    float maxSpeedY;
    float defaultSpeed;
    float jumpInitialSpeed;
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
    // top, left, bottom, right
    // x, y, width, height
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
                rec->y + 2.0f,
                2.0f,
                rec->height - 4.0f
            },
            {
                rec->x,
                rec->y + rec->height - 2.0f,
                rec->width,
                2.0f
            },
            {
                rec->x + rec->width - 2.0f,
                rec->y + 2.0f,
                2.0f,
                rec->height - 4.0f
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
    hitBox->left.y = rec->y + 2.0f;

    hitBox->bottom.x = rec->x;
    hitBox->bottom.y = rec->y + rec->height - 2.0f;

    hitBox->right.x = rec->x + rec->width - 2.0f;
    hitBox->right.y = rec->y + 2.0f;
}
void updatePlayer(Player* player) {
    if (player->speedX != 0) {
        player->body.x += player->speedX;
    }

    player->body.y += player->speedY;
    if (!player->bottomTouch  && player->speedY < player->maxSpeedY) {
           player->speedY += GRAVITY;
    }
    /*
    if (player->bottomTouch && player->durationOnGround <= DURATION_ON_GROUND_TO_JUMP) {
        player->durationOnGround += GetFrameTime();
    } else if(player->durationOnGround > 0) {
        player->durationOnGround -= GetFrameTime();
    }
    //printf("%lf\n", player->durationOnGround);
    //printf("%df\n", player->bottomTouch);
    */
    updateHitBox(&(player->body), &(player->hitBox));

}
void updateCollisions(Player* player, Rectangle* ground, int countPlatforms, Platform** platforms) {
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

     for (int i = 0; i < countPlatforms; i++) {
         if (CheckCollisionRecs(player->hitBox.bottom, platforms[i]->hitBox.top)) {
             player->bottomTouch = true;
             player->body.y = platforms[i]->rec.y - player->body.height;
         }
         if (CheckCollisionRecs(player->hitBox.top, platforms[i]->hitBox.bottom)) {
             player->topTouch = true;
         }
         if (CheckCollisionRecs(player->hitBox.right, platforms[i]->hitBox.left)) {
             player->rightTouch = true;
             player->body.x = platforms[i]->rec.x - player->body.width;
         }
         if (CheckCollisionRecs(player->hitBox.left, platforms[i]->hitBox.right)) {
             player->leftTouch = true;
             player->body.x = platforms[i]->rec.x + platforms[i]->rec.width;
         }
     }
     

     if (player->bottomTouch) {
         if (player->speedY > 0) {
            player->speedY = 0;
         }
     }
     if (player->topTouch) {
         if (player->speedY < 0) {
            player->speedY = GRAVITY;
         }
     }
     if (player->leftTouch) {
         player->speedX = 0; 
     }
     if (player->rightTouch) {
         player->speedX = 0;
     }
}

void UpdatePhysics(float fixedTimestep, Player* player, Rectangle* ground, int countPlatforms, Platform** platforms) {
    // UPDATES
    updatePlayer(player);

    // COLLISIONS
    updateCollisions(player, ground, countPlatforms, platforms);
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

// TIMESTEP
    
float accumulatedTime = 0.0f;
float fixedTimestep = 1.0 / 240.0;

// MAIN


int main() {
    InitWindow((int) WINDOW_WIDTH, (int) WINDOW_HEIGHT, "Jogo 01");

    SetTargetFPS(200);

    int countPlatforms = 0;
    Platform** platforms = (Platform**)malloc(MAX_PLATFORMS * sizeof(Platform*));

    Player player;
    player.body.x = 50.0f;
    player.body.y = 100.0f;
    player.body.height = 20.0f;
    player.body.width = 20.0f;

    player.health = 100;
    player.speedX = 0;
    player.speedY = 0;
    player.defaultSpeed = 2;
    player.jumpInitialSpeed = 5.0f;
    player.maxSpeedY = 6;
    //player.durationOnGround = 0.0;

    player.hitBox = *createRecHitBox(&(player.body));

	Rectangle ground;
	ground.height = 100.0f;
	ground.width = WINDOW_WIDTH;
	ground.x = 0.0f;
	ground.y = WINDOW_HEIGHT - ground.height;

    // Platforms
    //                                   x       y       width   height
    Platform platform1 = *createPlatform(100.0f, 600.0f, 150.0f, 40.0f, RED);
    Platform platform2 = *createPlatform(500.0f, 450.0f, 150.0f, 300.0f, BLUE);
    Platform platform3 = *createPlatform(900.0f, 500.0f, 150.0f, 40.0f, DARKPURPLE);

    addPlatform(&countPlatforms, platforms, &platform1);
    addPlatform(&countPlatforms, platforms, &platform2);
    addPlatform(&countPlatforms, platforms, &platform3);

    

    while (!WindowShouldClose()) {
        // KEYBOARD
        if (IsKeyDown(KEY_W) && player.bottomTouch /* && player.durationOnGround >= DURATION_ON_GROUND_TO_JUMP */) {
            jumpPlayer(&player, -player.jumpInitialSpeed);
        }
        if (IsKeyDown(KEY_A)) {
            if (!player.leftTouch) {
                if (player.bottomTouch) {
                    player.speedX = -(player.defaultSpeed);
                } else {
                    if (-player.speedX < player.defaultSpeed) {
                        player.speedX += -(player.defaultSpeed) / 15;
                    } else {
                        player.speedX = -(player.defaultSpeed);
                    }
                }
            }
        } else if (IsKeyDown(KEY_D)) {
            if (!player.rightTouch) {
                if (player.bottomTouch) {
                    player.speedX = player.defaultSpeed;
                } else {
                    if (player.speedX < player.defaultSpeed) {
                        player.speedX += player.defaultSpeed / 15;
                    } else {
                        player.speedX = player.defaultSpeed;
                    }
                }
            }
        } else if (player.bottomTouch) {
            player.speedX = 0;
        }

        if (IsKeyUp(KEY_A) && !player.bottomTouch && player.speedX < 0) {
            player.speedX *= 0.95;
        }
        if (IsKeyUp(KEY_D) && !player.bottomTouch && player.speedX > 0) {
            player.speedX *= 0.95;
        }
        
        // UPDATE 

        float delta = GetFrameTime();
        accumulatedTime += delta;

        while (accumulatedTime >= fixedTimestep) {
            UpdatePhysics(fixedTimestep, &player, &ground, countPlatforms, platforms);
            accumulatedTime -= fixedTimestep;
        }

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

