#include "include/raylib.h"
#include "include/dirent.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"
#include "stdint.h"
#include "main.h"

// Starting with the random avatar generator I already created
// initial thoughts:
// 1. I won't need any of the randomness or rarity stuff
// 2. That is about as far as i've thought about things
// So I guess the first step will be removing all the randomness.

// Count the number of assets within a given directory.
internal int
Directory_Count(char *filepath)
{
    DIR *d;
    struct dirent *dir;
    
    int file_count = 0;
    d = opendir(filepath);
    if(d)
    {
        while((dir = readdir(d)) != NULL)
        {
            file_count++;
        }
        closedir(d);
        file_count -= 1;
        return file_count;
    }
    else
    {
        return file_count;
    }
}

internal char 
*Randomised_Asset(int *asset_id, int rarity_group, char *filepath,
                  Asset *asset, pcg32_random_t *rng)
{
    DIR *d;
    struct dirent *dir;

    int num_count  = 0;
    int max_assets = Directory_Count(filepath);

    uint32_t rand_num  = (pcg32_random_r(rng) % (max_assets - 2 + 1)) + 2;
    *asset_id = rand_num + rarity_group;
    asset->counter[*asset_id] += 1;

    char *random_asset = (char *)malloc(MAX_BUFFER);
    memset(random_asset, 0, MAX_BUFFER);

    d = opendir(filepath);
    if(d)
    {
        while((dir = readdir(d)) != NULL)
        {
            if(num_count == rand_num)
            {
               strcpy(random_asset, dir->d_name);
            }
            num_count++;
        }
        closedir(d);
        return random_asset;
    }
}

int main()
{
    int window_width  = 1920;
    int window_height = 1080;

    InitWindow(window_width, window_height, "MyGame");
    SetTargetFPS(60);

    bool first_time_load_texture = true;
    Texture2D face = {};
    Texture2D eyes = {};
    Texture2D hat  = {};

    int export_count = 1;

    Asset face_asset = {};
    Asset eyes_asset = {};
    Asset hat_asset  = {};

    Combination combination = {};

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if(first_time_load_texture)
        {
            // The first time we load a texture, face, eyes and hat haven't
            // been set yet so no need to unload any textures.
            first_time_load_texture = false;
        }
        else
        {
            // Make sure to clean up the old texture since we're going
            // to load a new one in.
            UnloadTexture(face);
            UnloadTexture(eyes);
            UnloadTexture(hat);
        }

        // Prepare the textures for drawing to the screen.
        {
            char face_filepath[MAX_BUFFER] = "../assets/face/";
            char eyes_filepath[MAX_BUFFER] = "../assets/eyes/";
            char hat_filepath[MAX_BUFFER]  = "../assets/hat/";

            char *face_asset_path = Randomised_Asset(&combination.face, face_rarity,
                                                     face_filepath, &face_asset, &rng);
            char *eyes_asset_path = Randomised_Asset(&combination.eyes, eyes_rarity, 
                                                     eyes_filepath, &eyes_asset, &rng);
            char *hat_asset_path  = Randomised_Asset(&combination.hat, hat_rarity,
                                                     hat_filepath, &hat_asset, &rng);

            strcat(face_filepath, face_asset_path);
            strcat(eyes_filepath, eyes_asset_path);
            strcat(hat_filepath,  hat_asset_path);

            free(face_asset_path);
            free(eyes_asset_path);
            free(hat_asset_path);

            face = LoadTexture(face_filepath);
            eyes = LoadTexture(eyes_filepath);
            hat  = LoadTexture(hat_filepath);
        }

        {
            DrawTexture(face, 0, 0, RAYWHITE);
            DrawTexture(eyes, 0, 0, RAYWHITE);
            DrawTexture(hat,  0, 0, RAYWHITE);
            EndDrawing();
        }
    }
}
