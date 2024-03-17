#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int pixel_buffer_start;      // global variable
short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

#define screen_width 320
#define screen_height 240
#define obsidian_black 0x0882   // 00001 000100 00010 -> 0x0882
#define cold_steel_white 0xFFBE // 11111 111101 11110 -> 0xFFBE

#define nullptr ((void *)0)

// game assets
// 0, 150, 0 -> 100101 -> 0x04A0 green
// 117, 59, 18 -> 01110 001110 00010 -> 0x71C2
// 173, 40, 14 -> 0xA941
short int air_block_texture[64] = {
    0xA941, 0xA941, 0xA941, 0xA941, 0xA941, 0xA941, 0xA941, 0xA941,
    0xA941, 0x0882, 0x0882, 0x0882, 0x0882, 0x0882, 0xA941, 0xA941,
    0xA941, 0x0882, 0x0882, 0x0882, 0x0882, 0xA941, 0x0882, 0xA941,
    0xA941, 0x0882, 0x0882, 0x0882, 0xA941, 0x0882, 0x0882, 0xA941,
    0xA941, 0x0882, 0x0882, 0xA941, 0x0882, 0x0882, 0x0882, 0xA941,
    0xA941, 0x0882, 0xA941, 0x0882, 0x0882, 0x0882, 0x0882, 0xA941,
    0xA941, 0xA941, 0x0882, 0x0882, 0x0882, 0x0882, 0x0882, 0xA941,
    0xA941, 0xA941, 0xA941, 0xA941, 0xA941, 0xA941, 0xA941, 0xA941};
short int grass_block_texture[64] = {
    0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0,
    0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0,
    0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2};
short int dirt_block_texture[64] = {
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2};

// game definitions
enum BlockType {
  air,
  grass_block,
  dirt,
  stone,
  oak_wood_log,
  leaves,
  water,
  lava
};
// each block is 8x8
struct Block {
  enum BlockType block_type;
  bool overlay;
  int animation_frame_state;
};
struct Block testBlock = {stone, false, 0};
enum EntityType { player, chicken, pig, cow };
struct Entity {
  int x;
  int y;
  enum EntityType entity_type;
  int animation_frame_state;
};
// centre of the camera position
struct camera {
  int x;
  int y;
};
struct camera global_camera = {0, 24 * 8};
struct Chunk {
  struct Block block_array[128 * 64]; // 128 height, 64 width
};
struct Block *getBlockFromChunk(struct Chunk *chunk, int x_8, int y_8);
void setBlockInChunk(struct Chunk *chunk, int x_8, int y_8, struct Block block);
#define chunk_width 16
struct World {
  struct Chunk chunk_array[chunk_width];
};
struct Chunk *getChunkFromPosition(int x_8);
struct World global_world;

// game generation
void generate_map();
void load_map();
void save_map();

// game rendering
void draw_block(struct Block *, int x_8, int y_8);
void draw_blocks();
void draw_entity();
void draw_entities();
void draw_block_overlay(struct Block *, int x_8, int y_8);
void draw_block_overlays();
void draw_map();

// general rendering
void clear_screen();
void draw_line(int x0, int y0, int x1, int y1, short int line_color);
void swap(int *a, int *b);
void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();

int main(void) {
  volatile int *pixel_ctrl_ptr = (int *)0xFF203020;
  // declare other variables(not shown)

  generate_map();

  /* set front pixel buffer to Buffer 1 */
  *(pixel_ctrl_ptr + 1) =
      (int)&Buffer1; // first store the address in the  back buffer
  /* now, swap the front/back buffers, to set the front buffer location */
  wait_for_vsync();

  /* set back pixel buffer to Buffer 2 */
  *(pixel_ctrl_ptr + 1) = (int)&Buffer2;
  wait_for_vsync();
  pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
  clear_screen(); // pixel_buffer_start points to the pixel buffer
  wait_for_vsync();
  pixel_buffer_start = *(pixel_ctrl_ptr + 1);
  clear_screen();

  while (1) {
    printf("draw\n");
    // draw_block(&testBlock, 20, 1);
    draw_blocks();

    wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
  }
}

// game definitions
struct Block *getBlockFromChunk(struct Chunk *chunk, int x_8, int y_8) {
  return &chunk->block_array[x_8 + y_8 * 128];
}
void setBlockInChunk(struct Chunk *chunk, int x_8, int y_8,
                     struct Block block) {
  struct Block *target_block = getBlockFromChunk(chunk, x_8, y_8);
  target_block->block_type = block.block_type;
  target_block->overlay = block.overlay;
  target_block->animation_frame_state = block.animation_frame_state;
}
struct Chunk *getChunkFromPosition(int x_8) {
  int chunk_index = (x_8 >> 6);
  if (chunk_index < 0 || chunk_index >= chunk_width)
    return nullptr;
  else
    return &global_world.chunk_array[chunk_index];
}

// game generation
void generate_map() {
  // erase old map
  struct Block temp_air_block = {air, false, 0};
  for (int i = 0; i < chunk_width; i++) {
    struct Chunk *temp_chunk = &global_world.chunk_array[i];
    for (int x_8 = 0; x_8 < 64; x_8++) {
      for (int y_8 = 0; y_8 < 128; y_8++) {
        setBlockInChunk(temp_chunk, x_8, y_8, temp_air_block);
      }
    }
  }
  struct Block temp_dirt_block = {dirt, false, 0};
  struct Block temp_grass_block = {grass_block, false, 0};
  for (int x_8 = 0; x_8 < (chunk_width << 3); x_8++) {
    struct Chunk *temp_chunk = getChunkFromPosition(x_8);
    for (int y_8 = 0; y_8 < 32; y_8++) {
      setBlockInChunk(temp_chunk, x_8, y_8,
                      (y_8 != 31) ? temp_dirt_block : temp_grass_block);
    }
  }
  setBlockInChunk(&global_world.chunk_array[0], 32, 32, temp_dirt_block);
}
void load_map() {}
void save_map() {}

// game rendering
void draw_block(struct Block *block, int x_8, int y_8) {
  int corner_of_block_screen_x = (x_8 << 3) - global_camera.x;
  int corner_of_block_screen_y = (y_8 << 3) - global_camera.y;

  short int *texture_array;
  switch (block->block_type) {
  case grass_block:
    texture_array = grass_block_texture;
    break;
  case dirt:
    texture_array = dirt_block_texture;
    break;
  case air:
    texture_array = air_block_texture;
    break;
  default:
    texture_array = dirt_block_texture;
    break;
  }
  // screen refers to screen position, local refers to pixel within the block
  int block_screen_x, block_screen_y, block_local_x, block_local_y;
  for (block_local_x = 0; block_local_x < 8; block_local_x++) {
    block_screen_x = corner_of_block_screen_x + block_local_x;
    if (block_screen_x < 0 || block_screen_x >= screen_width)
      continue;
    for (block_local_y = 0; block_local_y < 8; block_local_y++) {
      block_screen_y = corner_of_block_screen_y + block_local_y;
      if (block_screen_y < 0 || block_screen_y >= screen_height)
        continue;
      plot_pixel(block_screen_x, block_screen_y,
                 texture_array[block_local_x + (8 - 1 - block_local_y) * 8]);
    }
  }
}
void draw_blocks() {
  int camera_x_8 = global_camera.x >> 3;
  int camera_y_8 = global_camera.y >> 3;
  for (int local_x_8 = 0; local_x_8 < (screen_width >> 3) + 1; local_x_8++) {
    int x_8 = camera_x_8 + local_x_8;
    struct Chunk *temp_chunk = getChunkFromPosition(x_8);
    if (temp_chunk != nullptr) {
      for (int local_y_8 = 0; local_y_8 < (screen_height >> 3) + 1;
           local_y_8++) {
        int y_8 = camera_y_8 + local_y_8;
        draw_block(getBlockFromChunk(temp_chunk, x_8 % 64, y_8), x_8, y_8);
      }
    }
  }
}
void draw_entity() {}
void draw_entities() {}
void draw_block_overlay(struct Block *block, int x_8, int y_8) {}
void draw_block_overlays() {}
void draw_map() {}

// general rendering

void clear_screen() {
  for (int y = 0; y < screen_height; y++)
    for (int x = 0; x < screen_width; x++)
      plot_pixel(x, y, obsidian_black);
}

void draw_line(int x0, int y0, int x1, int y1, short int line_color) {
  bool steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(&x0, &y0);
    swap(&x1, &y1);
  }
  if (x0 > x1) {
    swap(&x0, &x1);
    swap(&y0, &y1);
  }

  int deltax = x1 - x0;
  int deltay = abs(y1 - y0);
  int error = -(deltax / 2);
  int y = y0;
  int y_step;

  if (y0 < y1)
    y_step = 1;
  else
    y_step = -1;

  for (int x = x0; x <= x1; x++) {
    if (steep)
      plot_pixel(y, x, line_color);
    else
      plot_pixel(x, y, line_color);
    error += deltay;
    if (error > 0) {
      y += y_step;
      error -= deltax;
    }
  }
}

void swap(int *a, int *b) {
  int c = *a;
  *a = *b;
  *b = c;
}

void plot_pixel(int x, int y, short int line_color) {
  short int *one_pixel_address;

  one_pixel_address = (short *)(pixel_buffer_start +
                                ((screen_height - 1 - y) << 10) + ((x) << 1));

  *one_pixel_address = line_color;
}

void wait_for_vsync() {
  volatile int *pixel_ctrl_ptr = (int *)0xFF203020;
  *pixel_ctrl_ptr = 1; // swap buffers
  int status = *(pixel_ctrl_ptr + 3);
  while ((status & 0x01))
    status = *(pixel_ctrl_ptr + 3);
}
