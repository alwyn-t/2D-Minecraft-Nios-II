#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int pixel_buffer_start;      // global variable
short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

#define screen_width 320
#define screen_height 240
#define obsidian_black 0x1925
#define cold_steel_white 0xF79D

// game assets
// 0, 150, 0 -> 100101 -> 0x04A0 green
// 117, 59, 18 -> 01110 001110 00010 -> 0x71C2
short int grass_block_texture[64] = {
    0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0,
    0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0,
    0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0, 0x04A0,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2,
    0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2, 0x71C2};

// game definitions
enum BlockType { grass_block, dirt, stone, oak_wood_log, leaves, water, lava };
// each block is 8x8
struct Block {
  int x_8;
  int y_8;
  enum BlockType block_type;
  bool overlay;
  int animation_frame_state;
};
struct Block testBlock = {0, 0, stone, false, 0};
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
struct camera global_camera = {0, 0};

// game generation
void generate_map();
void load_map();
void save_map();

// game rendering
void draw_block(struct Block *);
void draw_blocks();
void draw_entity();
void draw_entities();
void draw_block_overlay(struct Block *);
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

  /* set front pixel buffer to Buffer 1 */
  *(pixel_ctrl_ptr + 1) =
      (int)&Buffer1; // first store the address in the  back buffer
  /* now, swap the front/back buffers, to set the front buffer location */
  wait_for_vsync();
  /* initialize a pointer to the pixel buffer, used by drawing functions */
  // pixel_buffer_start = *pixel_ctrl_ptr;/////////
  // clear_screen(); // pixel_buffer_start points to the pixel buffer

  /* set back pixel buffer to Buffer 2 */
  *(pixel_ctrl_ptr + 1) = (int)&Buffer2;
  wait_for_vsync();
  pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
  clear_screen(); // pixel_buffer_start points to the pixel buffer
  wait_for_vsync();
  pixel_buffer_start = *(pixel_ctrl_ptr + 1);
  clear_screen();

  while (1) {
    draw_block(&testBlock);

    wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
  }
}

// game generation
void generate_map() {}
void load_map() {}
void save_map() {}

// game rendering
void draw_block(struct Block *block) {
  int corner_of_block_screen_x =
      (block->x_8 << 3) - 4 - global_camera.x + screen_width / 2;
  int corner_of_block_screen_y =
      (block->y_8 << 3) - 4 - global_camera.y + screen_height / 2;

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
                 grass_block_texture[block_local_x + block_local_y * 8]);
    }
  }
}
void draw_blocks() {}
void draw_entity() {}
void draw_entities() {}
void draw_block_overlay(struct Block *block) {}
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

  one_pixel_address = (short int *)(pixel_buffer_start + (y << 10) + (x << 1));

  *one_pixel_address = line_color;
}

void wait_for_vsync() {
  volatile int *pixel_ctrl_ptr = (int *)0xFF203020;
  *pixel_ctrl_ptr = 1; // swap buffers
  int status = *(pixel_ctrl_ptr + 3);
  while ((status & 0x01))
    status = *(pixel_ctrl_ptr + 3);
}
