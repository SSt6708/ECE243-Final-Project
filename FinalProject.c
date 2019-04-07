#include <stdio.h>
#include <stdbool.h>
#include "board.h"
#include "obstacle.h"
#include "player.h"
#include "address_map_arm.h"



volatile int pixel_buffer_start;

void drawBackground();
void drawBoard(int xPos, int yPos);
void clear_screen();
void wait_for_vsync();
void changePlayerPos();
void update();
void clear();
void plot_pixel(int x, int y, short int line_color);
void changePlayerPos();
void draw();

//declare global variables on objects


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
int player_ONCHIP_x, player_SDRAM_y;

bool gameOver = false; // initialize game status

int level = 1; //initialize game level

//initialize playrt position




int main(void) {
  boards[1][1] = 1; // when theres a 1 on the board, that means theres a player on it


    volatile int *pixel_ctrl_ptr = (int *)0xFF203020;

    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the
										// back buffer
    wait_for_vsync();

    pixel_buffer_start = *pixel_ctrl_ptr;

    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	  clear_screen();


  //initialize and load sprites here
drawBackground();
drawBoard(114, 74);



while(1){


  if(gameOver){
    break;
  }
  clear();
  changePlayerPos();
  draw();
  update();


  wait_for_vsync();
  pixel_buffer_start = *(pixel_ctrl_ptr + 1); //new back buffer
}




}

void drawBackground(){

  int i = 0;
  for (; i < 320; i++) {
    int j = 0;
    for (; j < 240; j++) {
      plot_pixel(i, j, 0x0000);
    }
  }



}



void changePlayerPos(){

int i, j;

for( i = 0 ; i < 3; i++){
  for( j = 0; j < 3; j++){
    if(boards[i][j] == 1){
      player_x = 114 + (i * 30);
      player_y = 74 + (j*30);
    }


  }
}

}

void clear(){







}
//hi


void drawPlayer(int x, int y){

  int i, j;

  for(i = 0; i < 30; i++){
    for( j = 0; j < 30; j++){

      plot_pixel( x + j, y + i, player[i*90 + j]);

    }
  }



}



void draw(){

int i, j;

for(i = 0; i < 20 ; i ++){

  //draw each obstacle subroutine
}

drawPlayer(player_x, player_y);
//draw player subroutine

}

void drawBoard( int xPos, int yPos){

    int i, j;

    for(i = 0; i < 90; i++){
      for( j = 0; j < 90; j++){

        plot_pixel( xPos + j, yPos + i, board[i*90 + j]);

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
