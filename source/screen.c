#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <stdio.h>
#include "screen.h"
#include "framebuffer.h"
#include "game.h"
#include "menuimg.h"
#include "buttons.h"
#include "halfscreens.h"
#include "snes.h"


#define WIDTH 1280
#define HEIGHT 720

struct fbs framebufferstruct;

int screenImage[WIDTH][HEIGHT];

//update vitural screen
void drawScreen() {

    framebufferstruct = initFbInfo();

    Pixel *pixel;
    pixel = malloc(sizeof(Pixel));

    //drawMenu();
    //drawStartButton(whiteStart);
    //drawEndButton(whiteQuit);
    //drawBackground();
    //drawFrog(getGameState());

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            pixel->color = screenImage[x][y];
            pixel->x = x;
            pixel->y = y;

            drawPixel(pixel);
        }
    }

    free(pixel);
    pixel = NULL;
    munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
}
//draws menu screen to virtual screen
int drawMenu(){
   int *imagePtr = (int *) startMenu.image_pixels;

    int j = 0;

    for(int y = 0; y < startMenu.height; y++) {
        for(int x = 0; x < startMenu.width; x++) {

            //assign color value to corresponding pixel
            screenImage[x][y] = imagePtr[j];
            j++;
            if(y == 0 || x == 0) {
                screenImage[x][y] = 0xF800; //red for border

            }else if(y == HEIGHT - 1 || x == WIDTH - 1) {
                screenImage[x][y] = 0xF800; //red for border

            }

        }
    }
    //draw buttons for the menu
    int up;
    resetA(0);
    while(getA() != 1){
        up = getUp();

        if(up == 1){
            drawStartButton(whiteStart);
            drawEndButton(blackQuit);
        }
        else{
            drawStartButton(blackStart);
            drawEndButton(whiteQuit);
        }
        Read_Snes();
        readButton();
        drawScreen();
    }
    int gameloop = 1;
    if(up == 1){
        gameloop = 0;
    }else{
        gameloop = 1;
    }
    return gameloop;
}
void drawLives(int lives){

    for(int i = 0;i<lives;i++){
        int *imagePtr = (int *) heart.image_pixels;

        int j = 0;

        for(int y = 64; y < heart.height + 64; y++) {
            for(int x = 32+32*i; x < heart.width + 32+32*i; x++) {

                //assign color value to corresponding pixel
                screenImage[x][y] = imagePtr[j];
                j++;

            }
        }
    }

}
int drawHalf(halfscreen screenToPrint, int currentCheck){

   int *imagePtr = (int *) screenToPrint.image_pixels;

    int j = 0;

    for(int y = 200; y < screenToPrint.height + 200; y++) {
        for(int x = 300; x < screenToPrint.width + 300; x++) {

            //assign color value to corresponding pixel
            screenImage[x][y] = imagePtr[j];
            j++;
            if(y == 200|| x == 300) {
                screenImage[x][y] = 0xF800; //red for border

            }else if(y == screenToPrint.height + 200 - 1 || x == screenToPrint.width + 300 - 1) {
                screenImage[x][y] = 0xF800; //red for border

            }
        }
    }
    
    if(currentCheck == 1){
        printf("got");
        //resetA();
        //resetStart();
        int up;
        resetA(1);
        while(getA() == 1 && getStart() ==1){
            
            Read_Snes();
            readButton();
            drawScreen();
            up = getUp();

            if(up == 1){
                drawStartButton(whiteReset);
                drawEndButton(blackQuit);
            }
            else{
                drawStartButton(blackReset);
                drawEndButton(whiteQuit);
            }
          
        }
        if(up == 0){
            newGame();
            gameLoop(1);
            return 1;
          
        }
        else{
            resetStart();
            newGame();
            gameLoop(0);
            return 1;
        }
    }

}
void drawStartButton(button currentButton){
    int *imagePtr = (int *) currentButton.image_pixels;

    int j = 0;

    for(int y = 450; y < currentButton.height + 450; y++) {
        for(int x = 540; x < currentButton.width + 540; x++) {

            //assign color value to corresponding pixel
            screenImage[x][y] = imagePtr[j];
            j++;

        }
    }
}

void drawEndButton(button currentButton){
    int *imagePtr = (int *) currentButton.image_pixels;

    int j = 0;

    for(int y = 550; y < currentButton.height + 550; y++) {
        for(int x = 540; x < currentButton.width + 540; x++) {

            //assign color value to corresponding pixel
            screenImage[x][y] = imagePtr[j];
            j++;

        }
    }
}

void drawBackground() {
    for(int y = 0; y < HEIGHT; y++) {
        for(int x = 0; x < WIDTH; x++) {

            if(y == 0 || x == 0) {
                screenImage[x][y] = 0xF800; //red for border

            }else if(y == HEIGHT - 1 || x == WIDTH - 1) {
                screenImage[x][y] = 0xF800; //red for border

            }else {
                screenImage[x][y] = 0x0;

            }
        }
    }
}

void drawObjects(GameState state) {

    for(int i = 1; i < sizeof(state.objects) / sizeof(state.objects[0]); i++) {   //starts at index 1 because 0 is the frog
        //pointer to current object sprite
        int *imagePtr;
       
        if(state.objects[i].size == 1) {
            // load small sprite
            imagePtr = (int *) state.objects[i].sprite.image_pixels;

        }else if(state.objects[i].size == 3) {
            // load large sprite
            imagePtr = (int *) state.objects[i].largeSprite.image_pixels;

        }

        int j = 0;

        int top, bot, left, right;

        // set image bounds based on size
        if(state.objects[i].size == 1) {        // small size

            top = state.objects[i].yPos - (state.objects[i].sprite.height / 2);
            bot = state.objects[i].yPos + (state.objects[i].sprite.height / 2);

            left = state.objects[i].xPos - (state.objects[i].sprite.width / 2);
            right = state.objects[i].xPos + (state.objects[i].sprite.width / 2);

        }else if(state.objects[i].size == 3) {  // large size

            top = state.objects[i].yPos - (state.objects[i].largeSprite.height / 2);
            bot = state.objects[i].yPos + (state.objects[i].largeSprite.height / 2);

            left = state.objects[i].xPos - (state.objects[i].largeSprite.width / 2);
            right = state.objects[i].xPos + (state.objects[i].largeSprite.width / 2);
        }

    

        for(int y = top; y < bot; y++) {
            for(int x = left; x < right; x++) {
                
                //assign color value to corresponding pixel
                screenImage[x][y] = imagePtr[j];
                j++;
            }
        }
    }
}

void drawFrog(GameState state) {

    //pointer to current object sprite (frog)
    int *imagePtr = (int *) state.objects[0].sprite.image_pixels;

    int j = 0;

    int top = state.objects[0].yPos - (state.objects[0].sprite.height / 2);
    int bot = state.objects[0].yPos + (state.objects[0].sprite.height / 2);

    int left = state.objects[0].xPos - (state.objects[0].sprite.width / 2);
    int right = state.objects[0].xPos + (state.objects[0].sprite.width / 2);

    for(int y = top; y < bot; y++) {
        for(int x = left; x < right; x++) {

            //assign color value to corresponding pixel
            screenImage[x][y] = imagePtr[j];
            j++;

        }
    }
}

void drawGrid() {
    for(int y = 0; y < HEIGHT; y += 32) {
        for(int x = 0; x < WIDTH; x += 32) {

            screenImage[x][y] = 0x377;

        }
    }
}


void drawPixel(Pixel *pixel){
	long int location = (pixel->x +framebufferstruct.xOff) * (framebufferstruct.bits/8) +
                       (pixel->y+framebufferstruct.yOff) * framebufferstruct.lineLength;
	*((unsigned short int*)(framebufferstruct.fptr + location)) = pixel->color;
}