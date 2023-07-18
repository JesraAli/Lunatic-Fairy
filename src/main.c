#include "libs.h"
#include "structs.h"
#include "gui.h"
#include "highscoreInfo.h"

/*Main Function*/
int main(int argc, char **argv)
{
    load();
    initStage();
    initBackground();
    initHighScoreTable();
    initTitle();

    titleLoop();
    SDL_ShowCursor(0); // Hide cursor


    while (true)
    {
        if (returnPlayerLife()== 0)
        {
            addHighscore(returnPlayerScore(), returnHighscoreList());
            drawHighscores(returnHighscoreList());
            presentScene();
            //printf("You ran out of lives! Game Over\n");
            restartGame(); //Restart the game
        }

        prepareScene(); // Prepare Scene (Background & Clear)
        userInput();
        rendCopyBackground();

        // Collision detected with bounds (detect if sprite is going out of  window)
        collisionDetection();

        manipulateAllBullets();
        // Check if player is NULL, then repeat the while loop from beginning (avoids repetition of playerCollide() call)
        if (playerNullCheck())
        {
            resetStage();
            continue;
        }

        manipulateFairy();
        fireEnemyBulletCall();
        spawnFairies('L');
        spawnFairies('R');

        manipulateExplosion();
        playerCollide();

        if (playerNullCheck())
        {
            resetStage();
            continue;
        }
        // Present Scene: draw the image to the window
        // if (player->rect.x == 0 && player->rect.y == 0) // check if its 0 (in top left corner) //NEED TO CHANGE AT LATER DATE
        // {
        //     continue;
        // }

        rendCopyPlayer();

        // Drawing:
        drawBullets();
        drawEnemyBullets();
        drawFairy();
        drawEnemyExplosion();
        drawStats(returnHighscoreList());
        presentScene();
        SDL_Delay(1000 / 60); // Wait 1/60th of a second
    }

    end();
}