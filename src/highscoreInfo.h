#ifndef highscoreInfo
#define highscoreInfo

void initHighScoreTable(void);
void drawHighscores(void);
int highscoreComparator(const void *a, const void *b);
void addHighscore(int score);
void drawStats(void);
#endif