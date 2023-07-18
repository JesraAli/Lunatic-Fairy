#ifndef highscoreInfo
#define highscoreInfo

void initHighScoreTable(void);
void drawHighscores(HighscoreList*);
HighscoreList* returnHighscoreList();

int highscoreComparator(const void *a, const void *b);
void addHighscore(int score, HighscoreList*);
void drawStats(HighscoreList*);
#endif