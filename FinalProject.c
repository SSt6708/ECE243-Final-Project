#include <stdio.h>
#include <stdbool.h>

volatile int pixel_buffer_start;

void clear_screen();
void wait_for_vsync();
//declare global variables on objects


int board[3][3] = {0};
//location of obsticals
//total of 20 obstacles
int obstacal_x[20];
int obstacleMov_x[20];
int obstacal_SDRAM_x[20];
int obstacal_ONCHIP_x[20];

int obstacal_y[20];
int obstacleMov_y[20];
int obstacal_SDRAM_y[20];
int obstacal_ONCHIP_y[20];

//location of player

int player_x, player_y;
int playerMov;
int player_SDRAM;
int player_ONCHIP;

bool gameOver = false; // initialize game status

int level = 1; //initialize game level

//initialize playrt position




int main(void) {

    volatile int *pixel_ctrl_ptr = (int *)0xFF203020;

    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the
										// back buffer
    wait_for_vsync();

    pixel_buffer_start = *pixel_ctrl_ptr;

    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	  clear_screen();


  //initialize and load sprites here


while(1){


  wait_for_vsync();
  pixel_buffer_start = *(pixel_ctrl_ptr + 1); //new back buffer
}




}

void plot_pixel(int x, int y, short int line_color)
{
	*(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}


void clear_screen() {
	int i = 0;
	for (; i < 320; i++) {
		int j = 0;
		for (; j < 240; j++) {
			plot_pixel(i, j, 0x0000);
		}
	}
}

void wait_for_vsync() {
	volatile int* pixel_ctrl_ptr = 0xFF203020;
	register int status;

	*pixel_ctrl_ptr = 1;

	status = *(pixel_ctrl_ptr + 3);
	while ((status & 0x01) != 0) {
		status = *(pixel_ctrl_ptr + 3);
	}
	return;
}
