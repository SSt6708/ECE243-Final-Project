#include <stdio.h>
#include <stdbool.h>
#include "board.h"
#include "obstacle.h"
#include "player.h"
#include "address_map_arm.h"
#include "gameover.h"


volatile int pixel_buffer_start;

volatile int* key = 0xFF200050;
volatile int* timer = 0xFFFEC600;
volatile int* HEX = 0xFF200020;

void drawBackground();
void drawGameOver();
void drawBoard(int xPos, int yPos);
void drawPlayer(int x, int y);
void drawObstacle(int x, int y);
void clear_screen();
void wait_for_vsync();
void changePlayerPos();
void update();
void clearObstacle();
void plot_pixel(int x, int y, short int line_color);
void changePlayerPos();
void draw();
void check_timer();

//declare global variables on objects

int seg7[] = { 0x3F, 0x06,0x5B,0x4F,0x66, 0x6D,0x7D,0x07,0x7F,0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79,0x71 };
int boards[3][3];
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
int playerMov_x, playerMov_y;
int player_SDRAM_x, player_SDRAM_y;
int player_ONCHIP_x, player_ONCHIP_y;

bool over = false; // initialize game status

int level = 1; //initialize game level

//initialize playrt position

//5F5E100


int main(void) {
	boards[1][2] = 1; // when theres a 1 on the board, that means theres a player on it


	volatile int *pixel_ctrl_ptr = (int *)0xFF203020;

	*(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the
										// back buffer
	wait_for_vsync();
	*(timer_ptr + 2) = 0b011;
	*(timer_ptr + 3) = 0xE100;
	*(timer_ptr + 4) = 0x5F5;
	pixel_buffer_start = *pixel_ctrl_ptr;

	*(pixel_ctrl_ptr + 1) = 0xC0000000;
	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	clear_screen();



	//initialize and load sprites here
	drawBackground();




	//initialize all the obstacles
	int i;
	for (i = 0; i < 12; i++) {

		//try to do horizontal movement first
		if (i % 4 == 0) {
			obstacal_x[i] = 0 - rand() % 300;
			obstacal_y[i] = 74 + (rand() % 3) * 30;
			obstacleMov_x[i] = 1;
			obstacleMov_y[i] = 0;

			obstacal_ONCHIP_x[i] = obstacal_x[i];
			obstacal_ONCHIP_y[i] = obstacal_y[i];
			obstacal_SDRAM_x[i] = obstacal_x[i];
			obstacal_SDRAM_y[i] = obstacal_y[i];




		}
		else if (i % 4 == 1) {
			obstacal_x[i] = 310 + rand() % 300;
			obstacal_y[i] = 74 + (rand() % 3) * 30;
			obstacleMov_x[i] = -1;
			obstacleMov_y[i] = 0;

			obstacal_ONCHIP_x[i] = obstacal_x[i];
			obstacal_ONCHIP_y[i] = obstacal_y[i];
			obstacal_SDRAM_x[i] = obstacal_x[i];
			obstacal_SDRAM_y[i] = obstacal_y[i];



		}
		else if (i % 4 == 2) {

			obstacal_x[i] = 114 + (rand() % 3) * 30; //+ rand()%70;
			obstacal_y[i] = 0 - rand() % 300;
			obstacleMov_x[i] = 0;
			obstacleMov_y[i] = 1;

			obstacal_ONCHIP_x[i] = obstacal_x[i];
			obstacal_ONCHIP_y[i] = obstacal_y[i];
			obstacal_SDRAM_x[i] = obstacal_x[i];
			obstacal_SDRAM_y[i] = obstacal_y[i];


		}
		else {
			obstacal_x[i] = 114 + (rand() % 3) * 30; //+ rand()%70;
			obstacal_y[i] = 239 + rand() % 300;
			obstacleMov_x[i] = 0;
			obstacleMov_y[i] = -1;

			obstacal_ONCHIP_x[i] = obstacal_x[i];
			obstacal_ONCHIP_y[i] = obstacal_y[i];
			obstacal_SDRAM_x[i] = obstacal_x[i];
			obstacal_SDRAM_y[i] = obstacal_y[i];




		}






	}





	while (1) {

	  //clear_screen();
		check_timer();
		int t1 = time / 10;
		int t2 = time % 10
		*HEX3_0_ptr = seg7[t1] | seg7[t2] << 8;
		
      clearObstacle();
      drawBoard(114, 74);



      changePlayerPos();


      draw();
      update();
  

    //drawGameOver();



		wait_for_vsync();
		pixel_buffer_start = *(pixel_ctrl_ptr + 1); //new back buffer
	}


return 0;


}

void drawGameOver(){

  int i = 0;
  int j = 0;
  for (; i < 320; i++) {

    for (; j < 240; j++) {
      plot_pixel(i, j, gameOver[ j*320 + i]);
    }
  }


}

void drawBackground() {

	int i = 0;
	for (; i < 320; i++) {
		int j = 0;
		for (; j < 240; j++) {
			plot_pixel(i, j, 0x0000);
		}
	}



}



void changePlayerPos() {

	int i, j;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			if (boards[i][j] == 1) {
				player_x = 114 + (i * 30);
				player_y = 74 + (j * 30);

				player_ONCHIP_x = player_x;
				player_ONCHIP_y = player_y;
				player_SDRAM_x = player_x;
				player_SDRAM_y = player_y;
			}


		}
	}

}
// 0 is on chip 1 is SDRam
void clearObstacle() {

	int i;

	int type;

	if (pixel_buffer_start == 0xC0000000) {
		type = 1;
	}
	else if (pixel_buffer_start == 0xC8000000) {
		type = 0;
	}





	if (type == 0) {
		for (i = 0; i < 12; i++) {
			int k, j;
			for (k = 0; k < 30; k++) {
				for (j = 0; j < 30; j++) {

					if (obstacal_ONCHIP_x[i] + j < 320 && obstacal_ONCHIP_x[i] + j >= 0
							&& obstacal_ONCHIP_y[i] + k < 240 && obstacal_ONCHIP_y[i] + k >= 0) {
						plot_pixel(obstacal_ONCHIP_x[i] + j, obstacal_ONCHIP_y[i] + k, 0x0000);
					}


				}
			}
		}
	}
	else if (type == 1) {
		for (i = 0; i < 12; i++) {
			int k, j;
			for (k = 0; k < 30; k++) {
				for (j = 0; j < 30; j++) {

					if (obstacal_SDRAM_x[i] + j < 320 && obstacal_SDRAM_x[i] + j >= 0
						&& obstacal_SDRAM_y[i] + k < 240 && obstacal_SDRAM_y[i] + k >= 0) {
						plot_pixel(obstacal_SDRAM_x[i] + j, obstacal_SDRAM_y[i] + k, 0x0000);
					}

				}
			}
		}
	}

}
//hi

void drawObstacle(int x, int y) {

	int i, j;

	for (i = 0; i < 30; i++) {
		for (j = 0; j < 30; j++) {

			if (obstacle[i * 30 + j] != 0xFFFF) {
				plot_pixel(x + j, y + i, obstacle[i * 30 + j]); //only draw the color part
			}


		}
	}

}



void drawPlayer(int x, int y) {

	int i, j;

	for (i = 0; i < 30; i++) {
		for (j = 0; j < 30; j++) {

			plot_pixel(x + j, y + i, player[i * 30 + j]);

		}
	}



}


void update() {

	int i;

	for (i = 0; i < 12; i++) {

		if (pixel_buffer_start == 0xC0000000) {
			obstacal_SDRAM_x[i] = obstacal_x[i];
			obstacal_SDRAM_y[i] = obstacal_y[i];
		}
		else if (pixel_buffer_start == 0xC8000000) {
			obstacal_ONCHIP_x[i] = obstacal_x[i];
			obstacal_ONCHIP_y[i] = obstacal_y[i];
		}



		obstacal_x[i] += obstacleMov_x[i];
		obstacal_y[i] += obstacleMov_y[i];

    if(obstacal_x[i] == player_x && obstacal_y[i] == player_y){
      over = true;
      break;
    }

		if (obstacal_x[i] > 320 && obstacleMov_x[i] == 1) {
			obstacleMov_x[i] = -1;
			obstacal_x[i] = 310 + rand() % 300;
			continue;
		}

		if (obstacal_x[i] < 0 && obstacleMov_x[i] == -1) {
			obstacleMov_x[i] = 1;
			obstacal_x[i] = 0 - rand() % 300;
			continue;
		}

		if (obstacal_y[i] < 0 && obstacleMov_y[i] == -1) {
			obstacleMov_y[i] = 1;
			obstacal_y[i] = 0 - rand() % 300;
			continue;
		}

		if (obstacal_y[i] > 240 && obstacleMov_y[i] == 1) {
			obstacleMov_y[i] = -1;
			obstacal_y[i] = 240 + rand() % 300;
			continue;
		}


	}


}

void draw() {

	int i, j;

	for (i = 0; i < 12; i++) {
		if ((obstacal_x[i] >= 0 && obstacal_x[i] <= 320) && (obstacal_y[i] >= 0 && obstacal_y[i] <= 240)) {
			drawObstacle(obstacal_x[i], obstacal_y[i]);
		}
	}

	drawPlayer(player_x, player_y);
	//draw player subroutine

}

void drawBoard(int xPos, int yPos) {

	int i, j;

	for (i = 0; i < 90; i++) {
		for (j = 0; j < 90; j++) {

			plot_pixel(xPos + j, yPos + i, board[i * 90 + j]);

		}
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

void check_timer() {
	int status = *(timer_ptr + 3);
	while ((status & 0x01) == 0) {
		status = *(timer_ptr + 3);
	}
	*(timer_ptr + 3) = status;
	time++;
	return;


}