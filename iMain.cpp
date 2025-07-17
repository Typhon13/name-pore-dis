#include "iGraphics.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio> 
#include "iSound.h"
#include "iFont.h"
using namespace std;
int menuMusicIdx = 1;
int gameMusicIdx = 0;
int currentMusic = -1;

// ground.y = 100
bool specialKeyStates[256] = {false};
int platformcount=0; 
#define gravity 1
int initialJumpVelocity = 17;
int verticalVelocity = 0;
typedef enum
{
    IDLE,
    WALK,
    JUMP
} State;
typedef enum {
    MENU_MAIN,
    MENU_PLAY,
    MENU_OPTIONS,
    MENU_EXIT,
    GAME_RUNNING
} GameState;
GameState currentState = MENU_MAIN;

int menuSelection = 0; // For tracking selected menu item
Image menuBackground, startButton, optionsButton, exitButton;

typedef struct
{
    Image idle[18], walk[25], jump[25];
    Sprite sprite;
    State state;
    int direction; // 1 for right, -1 for left
    bool inAir; // 1 for in air, 0 for on ground
} Player;
Player angel;

typedef struct
{
    Image frame;
    Sprite sprite;
    int type; // 1 for static, -1 for dynamic 
    int x,y;
    int width,height;
} Platform;

Platform platforms[100];

typedef struct
{   Image frame[12];
    Sprite sprite;
} Object;

Object key_red,key_yellow,key_blue;
Object portal;
Object portal_open;


Image night,moon,bg,platform;
//bool isMirroredX[2] = {false, false};
#define ANGEL 1

int platformCount=0;
int red_flag=1,yellow_flag=1,blue_flag=1;
int keycount=0;
int portalAnimationFrame = 0;
int portalAnimationComplete = 0;
int portalState = 0; 
int portalcond = 0;

void addPlatform(int x, int y, int width, int height) {
    if (platformCount >= 100) return; // Prevent overflow
    
    platforms[platformCount].frame = platform;
    platforms[platformCount].x = x;
    platforms[platformCount].y = y;
    platforms[platformCount].width = width;
    platforms[platformCount].height = height;
    
    // Initialize sprite (optional, for animated platforms)
    iInitSprite(&platforms[platformCount].sprite);
    iChangeSpriteFrames(&platforms[platformCount].sprite, &platforms[platformCount].frame, 1);
    iSetSpritePosition(&platforms[platformCount].sprite, x, y);
    
    platformCount++;
}
void loadResources(){
    
    iLoadFramesFromFolder(angel.idle, "assets/images/sprites/Fallen_Angels_2/Idle Blinking");
    iLoadFramesFromFolder(angel.walk, "assets/images/sprites/Fallen_Angels_2/Walking");
    iLoadFramesFromFolder(angel.jump, "assets/images/sprites/Fallen_Angels_2/Jump Start");

    iLoadFramesFromFolder(key_red.frame, "assets/images/sprites/Keys_Red");
    iLoadFramesFromFolder(key_yellow.frame, "assets/images/sprites/Keys_Yellow");
    iLoadFramesFromFolder(key_blue.frame, "assets/images/sprites/Keys_Blue");

    iLoadFramesFromFolder(portal.frame, "assets/images/sprites/Portal");
    iLoadFramesFromFolder(portal_open.frame, "assets/images/sprites/Portal_Open");


    iInitSprite(&portal_open.sprite);
    iChangeSpriteFrames(&portal_open.sprite, portal_open.frame, 8);
    iSetSpritePosition(&portal_open.sprite, 850, 530);
    iScaleSprite(&portal_open.sprite, 1);  

    iInitSprite(&portal.sprite);
    iChangeSpriteFrames(&portal.sprite, portal.frame, 8);
    iSetSpritePosition(&portal.sprite, 850, 530);
    iScaleSprite(&portal.sprite, 1);


    iInitSprite(&key_yellow.sprite);
    iChangeSpriteFrames(&key_yellow.sprite, key_yellow.frame, 12);
    iSetSpritePosition(&key_yellow.sprite, 300, 240);
    iScaleSprite(&key_yellow.sprite, 0.2);
    
    iInitSprite(&key_red.sprite);
    iChangeSpriteFrames(&key_red.sprite, key_red.frame, 12);
    iSetSpritePosition(&key_red.sprite, 450, 320);
    iScaleSprite(&key_red.sprite, 0.2);

    iInitSprite(&key_blue.sprite);
    iChangeSpriteFrames(&key_blue.sprite, key_blue.frame, 12);
    iSetSpritePosition(&key_blue.sprite, 600, 400);
    iScaleSprite(&key_blue.sprite, 0.2);

    iInitSprite(&angel.sprite);
    iChangeSpriteFrames(&angel.sprite, angel.idle, 24);
    iSetSpritePosition(&angel.sprite, 40, 120);
    iScaleSprite(&angel.sprite, 0.05);
    iMirrorSprite(&angel.sprite, HORIZONTAL);
    angel.state = IDLE;
    angel.direction = -1; // 1 for right, -1 for left 
    angel.inAir = false;   

    iLoadImage(&night, "assets/images/night.png");
    iResizeImage(&night, 1024,768);
    iLoadImage(&moon, "assets/images/redmoon.png");
    //iResizeImage(&moon, 500,500);
    iLoadImage(&platform, "assets/images/platform2.png");
    iResizeImage(&platform, 100, 10);

    iLoadImage(&menuBackground, "assets/images/menubg.png");
    iLoadImage(&startButton, "assets/images/start.jpg");
    iLoadImage(&optionsButton, "assets/images/option.jpg");
    iLoadImage(&exitButton, "assets/images/exit.jpg");
    iResizeImage(&menuBackground, 1024, 768);
    iResizeImage(&startButton, 300, 100);
    iResizeImage(&optionsButton, 300, 100);
    iResizeImage(&exitButton, 300, 100);


    addPlatform(300, 200, 200, 30);  
    addPlatform(450, 280, 200, 30); 
    addPlatform(600, 360, 200, 30);
    addPlatform(700, 420, 200, 30);
    addPlatform(800, 500, 200, 30);  
}
void playMusic(int musicIdx) {
    // Stop current music if any is playing
    if(currentMusic != -1) {
        iStopSound(currentMusic);
    }
    
    // Play new music (loop forever at 50% volume)
    if(musicIdx==1){
    //iPlaySound("assets/sounds/menuSound.mp3", true, 50);
    currentMusic = musicIdx;}
    if(musicIdx == 0){
        iPlaySound("assets/sounds/levelSound.mp3", true, 10);
        currentMusic = musicIdx;


    }
}

void stopAllMusic() {
    if(menuMusicIdx != -1) iStopSound(menuMusicIdx);
    if(gameMusicIdx != -1) iStopSound(gameMusicIdx);
    currentMusic = -1;
}

void handleMenuSelection() {
    switch(menuSelection) {
        case 0: // Start
            currentState = GAME_RUNNING;
             playMusic(gameMusicIdx); 

            break;
        case 1: // Options
            currentState = MENU_OPTIONS;
            break;
        case 2: // Exit
        stopAllMusic();
            exit(0);
            break;
    }
}
float getPlayerWidth() {
    return angel.idle[0].width * 0.1f;
}
float getPlayerHeight() {
    return angel.idle[0].height * 0.1f;
}
bool isOnGroundOrPlatform() {
    float playerBottomY = angel.sprite.y;
    float playerTopY = angel.sprite.y + getPlayerHeight();
    float playerLeftX = angel.sprite.x;
    float playerRightX = angel.sprite.x + getPlayerWidth();
    if (playerBottomY <= 100 && verticalVelocity <= 0) {
        angel.sprite.y = 100; // Snap to ground
        verticalVelocity = 0; // Stop vertical movement
        angel.inAir = false;
        return true;
    }
    for (int i = 0; i < platformCount; i++) {
        Platform p = platforms[i];
        
        // 1. Horizontal overlap check (AABB)
        if (playerRightX > p.x && playerLeftX < p.x + p.width) {
            
            // 2. Vertical overlap check for landing on top of platform
            // Condition for landing:
            // a) Player's bottom is currently intersecting or below the platform's top edge
            // b) Player's bottom was above the platform's top edge in the *previous* frame (or before vertical velocity was applied)
            // c) Player is moving downwards (verticalVelocity <= 0)
            
            // Player's feet (angel.sprite.y) are at or below platform's top edge
            // AND player's feet (angel.sprite.y) were above platform's top edge
            // BEFORE this frame's vertical movement.
            // This also handles the case where verticalVelocity is 0 (already landed)
            if (verticalVelocity <= 0 && // Moving down or still
                playerBottomY <= p.y + p.height && // Player is currently at or below platform top
                angel.sprite.y - verticalVelocity >= p.y + p.height // Player's previous Y was above platform top
            ) {
                angel.sprite.y = p.y + p.height; // Snap to platform top
                verticalVelocity = 0; // Stop vertical movement
                angel.inAir = false;
                return true;
            }
        }
    }
    return false;
}

void applyGravity() {
    if (!angel.inAir) return; // Only apply gravity if in the air

    verticalVelocity -= gravity; // Apply gravity (decrease vertical speed)
    angel.sprite.y += verticalVelocity; // Update player's Y position
    
    // Prevent player from falling through the ground
    if (angel.sprite.y < 100) {
        angel.sprite.y = 100;
        verticalVelocity = 0;
        angel.inAir = false;
    }

    // Check for collision with platforms while falling
    float playerFeetY = angel.sprite.y;
    float playerRightX = angel.sprite.x + getPlayerWidth();

    for (int i = 0; i < platformCount; i++) {
        Platform p = platforms[i];
        if (playerRightX > p.x && angel.sprite.x < p.x + p.width) {
            // If player's feet are above or within the platform's top edge
            // and their previous position (before gravity) was above the platform
            // This prevents "snapping" up when jumping from below
            if (playerFeetY <= p.y + p.height && (playerFeetY - verticalVelocity - gravity) >= p.y + p.height) {
                angel.sprite.y = p.y + p.height;
                verticalVelocity = 0;
                angel.inAir = false;
                break; // Found a platform to land on, no need to check others
            }
        }
    }
}

void initiateJump() {
    if (!angel.inAir) { // Only jump if on ground/platform
        angel.inAir = true;
        verticalVelocity = initialJumpVelocity; // Set initial upward velocity
        angel.state = JUMP; // Change sprite state to jump
        iChangeSpriteFrames(&angel.sprite, angel.jump, 2); // Assume 2 frames for jump start
        // You might want to only show the "jump start" frames here,
        // and then let iAnim handle the "mid-air" frames.
    }
}

void iAnimkey(){
    iAnimateSprite(&key_red.sprite);
    iAnimateSprite(&key_blue.sprite);
    iAnimateSprite(&key_yellow.sprite);
    iAnimateSprite(&angel.sprite);
    iAnimateSprite(&portal.sprite);
    iAnimateSprite(&portal_open.sprite);
}

void iAnim()
{
    // Apply gravity continuously when in air
    if (angel.inAir) {
        applyGravity();

        // **NEW: Handle jump animation based on vertical velocity**
        // You might have different jump frames for ascent and descent.
        // For simplicity, let's assume 'jump' frames cover the whole jump.
        // The important part is *not* to change to walk/idle while in air.
        if (angel.state != JUMP) { // Only change if not already in jump state
            angel.state = JUMP;
            // Assuming angel.jump has enough frames for a full animation
            // You might want to pick a specific frame for mid-air if your assets support it
            iChangeSpriteFrames(&angel.sprite, angel.jump, 2); // Or a specific mid-air frame number
        }

    } else { // On ground
        // Ensure sprite state returns to idle/walk when on ground
        if (specialKeyStates[GLUT_KEY_LEFT] || specialKeyStates[GLUT_KEY_RIGHT]) {
            if (angel.state != WALK) {
                angel.state = WALK;
                iChangeSpriteFrames(&angel.sprite, angel.walk, 6);
            }
        } else {
            if (angel.state != IDLE) {
                angel.state = IDLE;
                iChangeSpriteFrames(&angel.sprite, angel.idle, 24);
            }
        }
    }

    // Check if player is still on a platform after movement
    // This part is good for ensuring gravity applies if they walk off a platform
    if (!angel.inAir) {
        if (!isOnGroundOrPlatform()) {
            angel.inAir = true;
            verticalVelocity = 0; // Start falling from current height
            // When falling off, also ensure the state transitions to jump/in-air
            if (angel.state != JUMP) { // If they weren't jumping but just walked off
                angel.state = JUMP;
                iChangeSpriteFrames(&angel.sprite, angel.jump, 2); // Show a falling animation if you have one
            }
        }
    }
}
void iCheckCollision(){
    if (red_flag!=0)
        if (iCheckCollision(&angel.sprite, &key_red.sprite))
    {
        keycount++;
        red_flag=0;
    }
    if (yellow_flag!=0)
        if (iCheckCollision(&angel.sprite, &key_yellow.sprite))
    {
        keycount++;
        yellow_flag=0;
    }
    if (blue_flag!=0)
        if (iCheckCollision(&angel.sprite, &key_blue.sprite))
    {
        keycount++;
        blue_flag=0;
    }
    if (keycount==3 && portalState == 0){
        portalState = 1; // Start opening animation
        portalAnimationFrame = 0;
        portalAnimationComplete = 0;
    }
    // Check collision with opened portal
    if (portalState == 2) { // Only check collision when portal is fully opened
        if (iCheckCollision(&angel.sprite, &portal.sprite)) {

            portalcond=1;
 
        }
    }
}
void updatePortalAnimation() {
    if (portalState == 1) { // Opening animation
        portalAnimationFrame++;
        
        // Assuming the portal_open animation has 8 frames and you want it to play once
        // Adjust the frame count based on your actual animation
        if (portalAnimationFrame >= 8) { // 8 frames * 10 timer calls = complete cycle
            portalState = 2; // Switch to open portal
            portalAnimationComplete = 1;
        }
    }
}
void iDraw()
{    iClear();
    // place your drawing codes here
    // iShowLoadedImage(0, 0, &bg);
    if(currentState == GAME_RUNNING){
    iShowLoadedImage(0, 0, &night);
    iShowLoadedImage(200, 400, &moon);
    iSetColor (0,255,0);
    iShowText(100, 390,"Press RIGHT arrow to go right","assets/fonts/Cardinal.ttf", 30);
    iShowText(100, 345,"Press LEFT arrow to go left","assets/fonts/Cardinal.ttf", 30);
    iShowText(100, 300,"Press UP arrow to jump","assets/fonts/Cardinal.ttf", 30);
    iShowText(100, 550,"Collect keys to open portal","assets/fonts/Cardinal.ttf", 30);
    iShowText(100, 600,"Get into portal to win","assets/fonts/Cardinal.ttf", 30);
    if (portalcond == 1){
        iShowText(500, 700,"You Win","assets/fonts/Cardinal.ttf", 50);
    }
    // Display keyCount dynamically using std::to_string
    iSetColor(255, 255, 255); // Set text color to white
    char keyCountBuffer[50]; // Declare a character array (buffer) to hold the string
    sprintf(keyCountBuffer, "Keys Collected: %d", keycount); // Format the string into the buffer
    iShowText(100, 700, keyCountBuffer,"assets/fonts/Cardinal.ttf", 30); // Display the string from the buffer
    iShowSprite(&angel.sprite);
    // Portal rendering logic
    if (portalState == 1) { // Opening animation
            iShowSprite(&portal_open.sprite);
        }
    else if (portalState == 2) { // Opened portal
            iShowSprite(&portal.sprite);
        }

    if (red_flag==1){
    iShowSprite(&key_red.sprite);
    }
    if (yellow_flag==1){
    iShowSprite(&key_yellow.sprite);
    }
    if (blue_flag==1){
    iShowSprite(&key_blue.sprite);
    }
    iSetColor (255,0,0);
    iFilledRectangle(0, 100, 1800, 10);
    iSetColor (79,55,39);
    iFilledRectangle(0, 0, 1800, 100);
    for (int i = 0; i < platformCount; i++) {
    iShowLoadedImage(
        platforms[i].x, 
        platforms[i].y, 
        &platforms[i].frame
    );

}
    }
    else {
        // Draw menu
        iShowLoadedImage(0, 0, &menuBackground);
        
        // Draw menu items with selection highlight
        iShowLoadedImage(400, 500, &startButton);
        iShowLoadedImage(400, 350, &optionsButton);
        iShowLoadedImage(400, 200, &exitButton);
        
        // Highlight selected item
        iSetColor(75,0,130);
        switch(menuSelection) {
            case 0: iFilledRectangle(395, 495, 310, 10); break; // Start
            case 1: iFilledRectangle(395, 345, 310, 10); break; // Options
            case 2: iFilledRectangle(395, 195, 310, 10); break; // Exit
        }
        
        // Menu instructions
        iSetColor(255,255,255);
        iShowText(175, 100, "Use UP/DOWN arrows to navigate, Enter to select", "assets/fonts/Cardinal.ttf", 40);
    }
}


/*
function iMouseMove() is called when the user moves the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseMove(int mx, int my)
{
    // place your codes here
}

/*
function iMouseDrag() is called when the user presses and drags the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseDrag(int mx, int my)
{
    // place your codes here
}

/*
function iMouse() is called when the user presses/releases the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // place your codes here
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        // place your codes here
    }
}

/*
function iMouseWheel() is called when the user scrolls the mouse wheel.
dir = 1 for up, -1 for down.
*/
void iMouseWheel(int dir, int mx, int my)
{
    // place your code here
}

/*
function iKeyboard() is called whenever the user hits a key in keyboard.
key- holds the ASCII value of the key pressed.
*/
void iKeyPress(unsigned char key)
{
    switch (key)
    {
    case 13:
        // do something with 'q'
        break;
    // place your codes for other keys here
    default:
        break;
    }
}

/*
function iSpecialKeyboard() is called whenver user hits special keys likefunction
keys, home, end, pg up, pg down, arrows etc. you have to use
appropriate constants to detect them. A list is:
GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11,
GLUT_KEY_F12, GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN,
GLUT_KEY_PAGE_UP, GLUT_KEY_PAGE_DOWN, GLUT_KEY_HOME, GLUT_KEY_END,
GLUT_KEY_INSERT */
void iSpecialKeyPress(unsigned char key)
{
    specialKeyStates[key] = true;
    if(currentState == GAME_RUNNING){
        
        if (key == GLUT_KEY_LEFT)
        {   
            if (angel.direction == 1)
            {
                iMirrorSprite(&angel.sprite, HORIZONTAL);
                angel.direction = -1;
            }
            // **Only change to WALK if NOT in air**
            if (!angel.inAir) {
                angel.sprite.x -=5;
                if (angel.state != WALK)
                {
                    angel.state = WALK;
                    iChangeSpriteFrames(&angel.sprite, angel.walk, 6);
                }
            } else {
                angel.sprite.x -=5; // Allow horizontal movement during jump
            }
        }
        if (key == GLUT_KEY_RIGHT)
        {   
            if (angel.direction == -1)
            {
                iMirrorSprite(&angel.sprite, HORIZONTAL);
                angel.direction = 1;
            }
            // **Only change to WALK if NOT in air**
            if (!angel.inAir) {
                angel.sprite.x += 5;
                if (angel.state != WALK)
                {
                    angel.state = WALK;
                    iChangeSpriteFrames(&angel.sprite, angel.walk, 6);
                }
            } else {
                angel.sprite.x += 5; // Allow horizontal movement during jump
            }
        }
        if (key == GLUT_KEY_UP)
        {
            initiateJump(); // initiateJump already checks angel.inAir
        }
        if (key == GLUT_KEY_END)    
        {
            currentState = MENU_MAIN;
            playMusic(menuMusicIdx); 
        }
    }       else {
        // Menu navigation controls (keep as is)
        switch(key) {
            case GLUT_KEY_UP:
                menuSelection--;
                if(menuSelection < 0) menuSelection = 2;
                break;
            case GLUT_KEY_DOWN:
                menuSelection = (menuSelection + 1) % 3;
                break;
            case GLUT_KEY_END:
            case 13: // Regular Enter key
                handleMenuSelection();
                break;
            case GLUT_KEY_PAGE_UP:
                if(currentState != MENU_MAIN) {
                    currentState = MENU_MAIN;
                }
                break;
        }
    }
}
void iSpecialKeyboardUp(unsigned char key)
{
    specialKeyStates[key] = false;

    // Only set to idle if no other movement keys are pressed AND player is on ground
    if (!specialKeyStates[GLUT_KEY_LEFT] && !specialKeyStates[GLUT_KEY_RIGHT] && !angel.inAir) 
    {
        if (angel.state != IDLE) { // Prevent unnecessary sprite changes
            angel.state = IDLE;
            iChangeSpriteFrames(&angel.sprite, angel.idle, 24);
        }
    }
    // If the UP key is released while jumping, it doesn't immediately stop the jump
    // Gravity should continue pulling the player down. This is already handled by applyGravity.
}
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    // place your own initialization codes here.
    iInitializeSound();
    loadResources();
    playMusic(menuMusicIdx); 
    iSetTimer(30, iAnim);
    iSetTimer(100, iAnimkey);
    iSetTimer (100,iCheckCollision);
    iSetTimer(100, updatePortalAnimation); 
    iInitialize (1024, 768, "mankindangel");
    return 0;
}                                                       