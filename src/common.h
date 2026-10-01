#pragma once

#include <cstddef>

#define KILOBYTES(n) ((size_t)n * 1024)
#define MEGABYTES(n) (KILOBYTES(n) * 1024)
#define GIGABYTES(n) (MEGABYTES(n) * 1024)

constexpr size_t GAME_MEMORY_ALLOWANCE = MEGABYTES(20);
constexpr size_t AUDIO_MEMORY_ALLOWANCE = MEGABYTES(5);

constexpr int FPS = 60;
const double FRAME_TIME_MS = 1000.0 / FPS;

const float MOVE_SPEED = 6.0;
const float UNDO_REPEAT_TIME = 0.15;
//Display
const int SCREEN_WIDTH = 1400;
const int SCREEN_HEIGHT = 1000;
const int UPSCALE_FACTOR = 4;
//const int CELL_SIZE_PX = 16 * UPSCALE_FACTOR;

const int TILE_SIZE_PX_RAW = 16;
const int TILE_SIZE_PX_SCALED = TILE_SIZE_PX_RAW * UPSCALE_FACTOR;

inline void Expand1DTo2D(int flatIndex, int width, int* x, int* y) {
    *x = flatIndex % width;
    *y = flatIndex / width;
}

inline void Expand1DTo2D(int flatIndex, int width, float* x, float* y) {
    *x = (float)(flatIndex % width);
    *y = (float)(flatIndex / width);
}
