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
bool inBound(int x, int y);

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
int time = 0;
int main_status = 0;
int int_timer = 150000000;
//initialize playrt position

//5F5E100
int moveType; //1up, 2down, 3left, 4right

int main(void) {
	
	unsigned char byte1 = 0;
	unsigned char byte2 = 0;
	unsigned char byte3 = 0;
	bool pressed = false;
	
  	volatile int * PS2_ptr = (int *) 0xFF200100;  // PS/2 port address

	int PS2_data, RVALID;
	//
	unsigned char prevKey = 0;
	
	int board_x, board_y;
	
	boards[1][2] = 1; // when theres a 1 on the board, that means theres a player on it
	board_x = 1;
	board_y = 2;

	volatile int *pixel_ctrl_ptr = (int *)0xFF203020;

	*(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the
										// back buffer
	wait_for_vsync();
	//*(timer + 3) = 0xE100;
	//*(timer + 3) = 0x5F5;
	pixel_buffer_start = *pixel_ctrl_ptr;
	drawBackground();
	*(pixel_ctrl_ptr + 1) = 0xC0000000;
	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer




	//initialize and load sprites here
	clear_screen();


	*timer = int_timer;
	//*(timer + 1) = 0b011;
	*(timer + 2) = 0b011;
	main_status = *(timer + 3);

	//initialize all the obstacles
	int i;
	for (i = 0; i < 20; i++) {

		//try to do horizontal movement first
		if (i % 4 == 0) {
			obstacal_x[i] = -450 - rand()%400 + rand()%100;
			obstacal_y[i] = 74 + (rand() % 3) * 30;
			obstacleMov_x[i] = 1;
			obstacleMov_y[i] = 0;

			obstacal_ONCHIP_x[i] = obstacal_x[i];
			obstacal_ONCHIP_y[i] = obstacal_y[i];
			obstacal_SDRAM_x[i] = obstacal_x[i];
			obstacal_SDRAM_y[i] = obstacal_y[i];




		}
		else if (i % 4 == 1) {
			obstacal_x[i] = 310 + rand() % 300 + rand()%500;
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
			obstacal_y[i] = -760- rand()%500 + rand()%100;
			obstacleMov_x[i] = 0;
			obstacleMov_y[i] = 1;

			obstacal_ONCHIP_x[i] = obstacal_x[i];
			obstacal_ONCHIP_y[i] = obstacal_y[i];
			obstacal_SDRAM_x[i] = obstacal_x[i];
			obstacal_SDRAM_y[i] = obstacal_y[i];


		}
		else {
			obstacal_x[i] = 114 + (rand() % 3) * 30; //+ rand()%70;
			obstacal_y[i] = 500 + rand()%700 - rand()%100;
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
		
		
		//read from ps2 keyboard 
		if(byte2 == 0xF0){
			pressed = false;
		}
		PS2_data = *(PS2_ptr);	// read the Data register in the PS/2 port
		RVALID = (PS2_data & 0x8000);	// extract the RVALID field
		if (RVALID != 0)
		{
			/* always save the last three bytes received */
			byte1 = byte2;
			byte2 = byte3;
			byte3 = PS2_data & 0xFF;
		}
		if ( (byte2 == 0xAA) && (byte3 == 0x00) )
		{
			// mouse inserted; initialize sending of data
			*(PS2_ptr) = 0xF4;
		}	
		
		
		if(byte2 == 0xF0){
			pressed = true;
		}
		
	if(!pressed){	
		if(byte3 == 0x1D){ //w
			
			moveType = 1;
			if(inBound(board_x, board_y-1)){
				boards[board_x][board_y] = 0;
				board_y--;
				boards[board_x][board_y] =1;
			}
			pressed = true;
			prevKey = byte3;
		}
		
		if(byte3 == 0x1B){ //s
			
			moveType = 2;
			if(inBound(board_x, board_y+1)){
				boards[board_x][board_y] = 0;
				board_y++;
				boards[board_x][board_y] =1;
			}
			
			pressed = true;
			prevKey = byte3;
		}
		
		if(byte3 == 0x1C){ //a
			
			moveType = 3;
			if(inBound(board_x-1, board_y)){
				boards[board_x][board_y] = 0;
				board_x--;
				boards[board_x][board_y] =1;
			}
			pressed = true;
			prevKey = byte3;
		}
		
		if(byte3 == 0x23){ //d
			
			moveType = 4;
			if(inBound(board_x+1, board_y)){
				boards[board_x][board_y] = 0;
				board_x++;
				boards[board_x][board_y] =1;
			}
			pressed = true;
			prevKey = byte3;
		}
	}
		
		
		
		
		
		
		
		
		
		check_timer();
		//		time = *(timer + 2);
		int t2 = time / 10;
		int t1 = time % 10;
		*HEX = seg7[t1] | seg7[t2] << 8 | seg7[level] << 24;

		clearObstacle();
		drawBoard(114, 74);



		changePlayerPos();


		draw();
		update();
		
		if (over)
			break;

		//drawGameOver();



		wait_for_vsync();
		pixel_buffer_start = *(pixel_ctrl_ptr + 1); //new back buffer
	}
	//All for gameover
	wait_for_vsync();

	while (1) {
		//clear_screen();
		drawGameOver();
		wait_for_vsync();
		pixel_buffer_start = *(pixel_ctrl_ptr + 1);
	}
	return 0;


}


bool inBound(int x, int y){
	if(x < 0 || x > 2 || y < 0 || y >2){
		return false;
	}else
		return true;
}

void drawGameOver() {

	int i = 0;
	for (; i < 320; i++) {
		int j = 0;
		for (; j < 240; j++) {
			plot_pixel(i, j, gameOver[i + j * 320]);
		}
	}


}

void drawBackground() {

	int i;
	for (i = 0; i < 320; i++) {
		int j;
		for (j = 0; j < 240; j++) {
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

			if (obstacle[i * 30 + j] != 0xFFFF && obstacle[i * 30 + j] != 0x0000) {
				plot_pixel(x + j, y + i, obstacle[i * 30 + j]); //only draw the color part
			}


		}
	}

}



void drawPlayer(int x, int y) {

	int i, j;

	for (i = 0; i < 30; i++) {
		for (j = 0; j < 30; j++) {

			if(player[i * 30 + j] != 0xFBE4){
				plot_pixel(x + j, y + i, player[i * 30 + j]);
			}
			

		}
	}



}


void update() {

	int i;
	
	int count = 6;
	
	if(level > 1){
		count = 12;
	}
	if(level > 2){
		count = 15;
	}
	if( level > 3){
		count = 20;
	}

	for (i = 0; i < count; i++) {
		
		if((player_x == obstacal_x[i]+29 && player_y == obstacal_y[i]) || (player_x + 29 == obstacal_x[i] && player_y == obstacal_y[i]) ||
		   (player_x == obstacal_x[i] && player_y == obstacal_y[i] + 29) || (player_x  == obstacal_x[i] && player_y + 29  == obstacal_y[i])){
			   over = true; //collides, game over
			   break;
			   
		   }
			
		

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

		

		if (obstacal_x[i] > 320 && obstacleMov_x[i] == 1) {
			obstacleMov_x[i] = -1;
			obstacal_x[i] = 320 + rand() % 500;
			obstacal_y[i] = 74 + (rand() % 3) * 30;
			continue;
		}

		if (obstacal_x[i] < 0 && obstacleMov_x[i] == -1) {
			obstacleMov_x[i] = 1;
			obstacal_x[i] = 0 - rand() % 600 - rand()%100;
			obstacal_y[i] = 74 + (rand() % 3) * 30;
			continue;
		}

		if (obstacal_y[i] < 0 && obstacleMov_y[i] == -1) {
			obstacleMov_y[i] = 1;
			obstacal_y[i] = 0 - rand() % 300;
			obstacal_x[i] = 114 + (rand() % 3) * 30;
			continue;
		}

		if (obstacal_y[i] > 240 && obstacleMov_y[i] == 1) {
			obstacleMov_y[i] = -1;
			obstacal_y[i] = 240 + rand() % 300 + rand()%800;
			obstacal_x[i] = 114 + (rand() % 3) * 30;
			continue;
		}


	}


}

void draw() {

	int i, j;
	
	int count = 6;
	
	if(level > 1){
		count = 12;
	}
	if(level > 2){
		count = 15;
	}
	if( level > 3){
		count = 20;
	}

	for (i = 0; i < count; i++) {
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
	int status;

	status = *(timer + 3);
	if ((status & 0x01) == 0) {
		status = *(timer + 3);
		return;
	}
	*(timer + 3) = status;
	time++;

	if (time == 30) {
		time = 0;
		level++;
	}

	return;


}