#include "libs.h"
#include "structs.h"
#include "gui.h"

HighscoreList *highscoreListEasy;
HighscoreList *highscoreListHard;
HighscoreList *highscoreListLunatic;

/*Initialise Table of Highscores*/
void initHighScoreTable()
{
    highscoreListEasy = malloc(sizeof(HighscoreList));
    memset(highscoreListEasy, 0, sizeof(HighscoreList));

    highscoreListHard = malloc(sizeof(HighscoreList));
    memset(highscoreListHard, 0, sizeof(HighscoreList));

    highscoreListLunatic = malloc(sizeof(HighscoreList));
    memset(highscoreListLunatic, 0, sizeof(HighscoreList));
}

HighscoreList *returnHighscoreList()
{
    if (returnMode() == 1)
    {
        return highscoreListEasy;
    }
    else if (returnMode() == 2)
    {
        return highscoreListHard;
    }
    else if (returnMode() == 3)
    {
        return highscoreListLunatic;
    }
}

/*Draw List of Highscores*/
void drawHighscores(HighscoreList *highscoreList)
{
    int i, y_pos;

    y_pos = 140;

    if (returnMode() == 1)
    {
        drawText(270, 60, 255, 255, 255, "EASY - HIGHSCORES");
    }
    else if (returnMode() == 2)
    {
        drawText(270, 60, 255, 255, 255, "HARD - HIGHSCORES");
    }
    else if (returnMode() == 3)
    {
        drawText(240, 60, 255, 255, 255, "LUNATIC - HIGHSCORES");
    }

    for (i = 0; i < NUM_HIGHSCORES; i++)
    {
        if (highscoreList->highscore[i].recent)
        {
            drawText(240, y_pos, 255, 255, 0, "#%d ............. %03d", (i + 1), highscoreList->highscore[i].score);
        }
        else
        {
            drawText(240, y_pos, 255, 255, 255, "#%d ............. %03d", (i + 1), highscoreList->highscore[i].score);
        }

        y_pos += 50;
    }

    drawText(110, 550, 255, 255, 255, "PRESS ESC TO RETURN TO TITLE SCREEN");
}

int highscoreComparator(const void *a, const void *b)
{
    Highscore *h1 = ((Highscore *)a);
    Highscore *h2 = ((Highscore *)b);

    return h2->score - h1->score;
}

/*Add New Highscore in Ascending Order*/
void addHighscore(int score, HighscoreList *highscoreList)
{
    Highscore newHighscores[NUM_HIGHSCORES + 1];
    int i;

    for (i = 0; i < NUM_HIGHSCORES; i++)
    {
        newHighscores[i] = highscoreList->highscore[i];
        newHighscores[i].recent = 0;
    }

    newHighscores[NUM_HIGHSCORES].score = score;
    newHighscores[NUM_HIGHSCORES].recent = 1;

    qsort(newHighscores, NUM_HIGHSCORES + 1, sizeof(Highscore), highscoreComparator);

    for (i = 0; i < NUM_HIGHSCORES; i++)
    {
        highscoreList->highscore[i] = newHighscores[i];
    }
}

/*Draw the current Score and current highest Highscore at top of screen*/
void drawStats(HighscoreList *highscoreList)
{

    int playerScore = returnPlayerScore();

    drawText(10, 10, 255, 255, 255, "SCORE: %03d", playerScore);

        if (playerScore < highscoreList->highscore[0].score)
    {
        drawText(530, 10, 255, 255, 255, "HIGHSCORE: %03d", highscoreList->highscore[0].score);
    }
    else
    {
        drawText(530, 10, 0, 255, 0, "HIGHSCORE: %03d", playerScore);
    }
}
