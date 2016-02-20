//
// breakout.c
//
// Computer Science 50
// Problem Set 3
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;

    // initial velocity
    double velocityX = drand48();
    double velocityY = 1.0;
    
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // update score
        updateScoreboard(window, label, points); 
         
        // check for mouse event
        GEvent event = getNextEvent(MOUSE_EVENT);

        // if we heard one
        if (event != NULL)
        {
            // if the event was movement
            if (getEventType(event) == MOUSE_MOVED)
            {
                // ensure paddle follows top cursor
                double x = getX(event) - getWidth(paddle) / 2;
                double y = 500;
                setLocation(paddle, x, y);
            }
        }
        // move ball along xy-axis
        move(ball, velocityX, velocityY);

        // bounce off right edge of window
        if (getX(ball) + getWidth(ball) >= getWidth(window))
        {
            velocityX = -velocityX;
        }

        // bounce off left edge of window
        if (getX(ball) <= 0)
        {
            velocityX = -velocityX;
        }
        
        // bounce off top edge of window
        if (getY(ball) <= 0)
        {
            velocityY = -velocityY;
        }

        // touch bottom edge of window
        if (getY(ball) + getHeight(ball) >= getHeight(window))
        {
            lives--;
            // set ball to starting position
            setLocation(ball, 192, 292);
            // set paddle to starting position
            setLocation(paddle, 180, 500);
            // wait for click before restarting
            waitForClick();        
        }

        // linger before moving again
        pause(3);
        
        // detect collision
        GObject object = detectCollision(window, ball);        
        
        if (object != NULL)
        {
            if (object == paddle)
            {
                velocityY = -velocityY;    
            }
            
            else if (strcmp(getType(object), "GRect") == 0)
            {
                removeGWindow(window, object);
                velocityY = -velocityY;
                points++;
                bricks--;    
            }
        }
    }
    
    if (bricks == 0)
    {
        GLabel end = newGLabel("you win");
        setFont(end, "SansSerif-70");
        setColor(end, "GREEN");
        add(window, end);
        double x = (getWidth(window) - getWidth(end)) / 2;
        double y = ((getHeight(window) - getHeight(end)) / 2) + 100;
        setLocation(end, x, y);
    }
    
    if (lives == 0)
    {
        GLabel end = newGLabel("loser");
        setFont(end, "SansSerif-70");
        setColor(end, "RED");
        add(window, end);
        double x = (getWidth(window) - getWidth(end)) / 2;
        double y = ((getHeight(window) - getHeight(end)) / 2) + 100;
        setLocation(end, x, y);    
    }
            
    
    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // set Y positions of the bricks
    int brickY = 50;
    
    // set black space
    int space = 4;
    
    // set brick size
    int brickLength = 35;
    int brickHeight = 10;
  
    for (int row = 0, i = 0; row < ROWS; row++)
    {
        int brickX = 6;
        // set row color;
        char* color[5] = {"00FFFF", "FF00FF", "FFFF00", "000000", "808080"};
        
        for (int col = 0; col < COLS; col++)
        {   
            // initialize the brick
            GRect brick = newGRect(brickX, brickY, brickLength, brickHeight);
            setColor(brick, color[i]);
            setFilled(brick, true);
            add(window, brick);
            brickX = brickX + brickLength + space;
        }
        i++;
        brickY = brickY + brickHeight + space;
    }
}    

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    int ballWidth = 16;
    int ballHeight = ballWidth;
    int ballX = WIDTH / 2 - ballWidth / 2;
    int ballY = HEIGHT / 2 - ballHeight / 2;
    GOval ball = newGOval(ballX, ballY, ballWidth, ballHeight);
    setColor(ball, "RED");
    setFilled(ball, true);
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    int paddleLength = 40;
    int paddleX = WIDTH / 2 - paddleLength / 2;
    GRect paddle = newGRect (paddleX, 500, paddleLength, 10);
    setFilled(paddle, true);
    setColor(paddle, "BLACK");
    add(window, paddle);
    return paddle;   
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel label = newGLabel(" ");
    setFont(label, "SansSerif-70");
    setColor(label, "EEEEEE");
    add(window, label);
    return label;    
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
