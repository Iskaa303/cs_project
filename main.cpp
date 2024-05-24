#include <iostream>
#include <conio.h>
#include <ctime>
#include <cmath>
#include <chrono>
#include <iomanip>

const int WORLD_WIDTH = 50;
const int WORLD_HEIGHT = 25;
const int NUM_STONES = 50;
const int RADIUS = 5;
const float STAMINA_MAX = 100;
const int HEALTH_MAX = 100;
const float STAMINA_COST_PER_STEP = 2;
const float WATER_STAMINA_COST_PER_STEP = 5;
const int STAMINA_REGEN_RATE = 1;
const int UPDATE_INTERVAL_MS = 1000;
const int NUM_ENEMIES = 10;
const int ENEMY_RADIUS = 10;
const char ENEMY_CHAR = 'E';
const int NUM_ENERGY_POTIONS = 3;
const char ENERGY_POTION_CHAR = 'e';
const float ENERGY_POTION_STAMINA_GAIN = 200;
const int NUM_HEALTH_POTIONS = 5;
const char HEALTH_POTION_CHAR = 'H';
const int NUM_STAMINA_POTIONS = 5;
const char STAMINA_POTION_CHAR = 'S';
const float STAMINA_POTION_STAMINA_GAIN = 25;
const int NUM_LAKES = 3;
const int LAKE_RADIUS = 5;
const char WATER_CHAR = 'w';

const char* RED_COLOR = "\033[31m";
const char* BLUE_COLOR = "\033[34m";
const char* GRAY_COLOR = "\033[90m";
const char* LIGHT_YELLOW_COLOR = "\033[93m";
const char* YELLOW_COLOR = "\033[33m";
const char* ORANGE_COLOR = "\033[38;5;208m";
const char* PINK_COLOR = "\033[35m";
const char* RESET_COLOR = "\033[0m";

const char* RAINBOW_COLORS[] = {
    "\033[31m", "\033[33m", "\033[32m", "\033[36m", "\033[34m", "\033[35m"
};

char world[WORLD_HEIGHT][WORLD_WIDTH];

struct Point {
    int x;
    int y;
};

Point player;

int health = HEALTH_MAX;
float stamina = STAMINA_MAX;
int maxHealth = HEALTH_MAX;
float maxStamina = STAMINA_MAX;

int score = 0;

struct Enemy {
    Point position;
};

struct Potion {
    Point position;
    char type;
};

struct Lake {
    Point center;
};

Enemy enemies[NUM_ENEMIES];
Potion energyPotions[NUM_ENERGY_POTIONS];
Potion healthPotions[NUM_HEALTH_POTIONS];
Potion staminaPotions[NUM_STAMINA_POTIONS];
Lake lakes[NUM_LAKES];

void initializeWorld() {
    for (int i = 0; i < WORLD_HEIGHT; ++i) {
        for (int j = 0; j < WORLD_WIDTH; ++j) {
            world[i][j] = '.';
        }
    }

    player.x = rand() % WORLD_WIDTH;
    player.y = rand() % WORLD_HEIGHT;
    world[player.y][player.x] = 'M';

    for (int i = 0; i < NUM_STONES; ++i) {
        int stoneX, stoneY;
        do {
            stoneX = rand() % WORLD_WIDTH;
            stoneY = rand() % WORLD_HEIGHT;
        } while (world[stoneY][stoneX] == 's' || world[stoneY][stoneX] == 'M');
        world[stoneY][stoneX] = 's';
    }

    for (int i = 0; i < NUM_ENEMIES; ++i) {
        do {
            enemies[i].position.x = rand() % WORLD_WIDTH;
            enemies[i].position.y = rand() % WORLD_HEIGHT;
        } while (world[enemies[i].position.y][enemies[i].position.x] != '.');
        world[enemies[i].position.y][enemies[i].position.x] = ENEMY_CHAR;
    }

    for (int i = 0; i < NUM_ENERGY_POTIONS; ++i) {
        do {
            energyPotions[i].position.x = rand() % WORLD_WIDTH;
            energyPotions[i].position.y = rand() % WORLD_HEIGHT;
        } while (world[energyPotions[i].position.y][energyPotions[i].position.x] != '.');
        world[energyPotions[i].position.y][energyPotions[i].position.x] = ENERGY_POTION_CHAR;
    }

    for (int i = 0; i < NUM_HEALTH_POTIONS; ++i) {
        do {
            healthPotions[i].position.x = rand() % WORLD_WIDTH;
            healthPotions[i].position.y = rand() % WORLD_HEIGHT;
        } while (world[healthPotions[i].position.y][healthPotions[i].position.x] != '.');
        world[healthPotions[i].position.y][healthPotions[i].position.x] = HEALTH_POTION_CHAR;
    }

    for (int i = 0; i < NUM_STAMINA_POTIONS; ++i) {
        do {
            staminaPotions[i].position.x = rand() % WORLD_WIDTH;
            staminaPotions[i].position.y = rand() % WORLD_HEIGHT;
        } while (world[staminaPotions[i].position.y][staminaPotions[i].position.x] != '.');
        world[staminaPotions[i].position.y][staminaPotions[i].position.x] = STAMINA_POTION_CHAR;
    }

    for (int i = 0; i < NUM_LAKES; ++i) {
        lakes[i].center.x = rand() % WORLD_WIDTH;
        lakes[i].center.y = rand() % WORLD_HEIGHT;
        for (int y = -LAKE_RADIUS; y <= LAKE_RADIUS; ++y) {
            for (int x = -LAKE_RADIUS; x <= LAKE_RADIUS; ++x) {
                int lakeX = lakes[i].center.x + x;
                int lakeY = lakes[i].center.y + y;
                if (lakeX >= 0 && lakeX < WORLD_WIDTH && lakeY >= 0 && lakeY < WORLD_HEIGHT) {
                    if (sqrt(x * x + y * y) <= LAKE_RADIUS) {
                        if (world[lakeY][lakeX] == '.') {
                            world[lakeY][lakeX] = WATER_CHAR;
                        }
                    }
                }
            }
        }
    }
}

void printWorld() {
    system("cls");
    for (int i = 0; i < WORLD_HEIGHT; ++i) {
        for (int j = 0; j < WORLD_WIDTH; ++j) {
            double distance = sqrt(pow(player.x - j, 2) + pow(player.y - i, 2));
            if (distance <= RADIUS) {
                if (world[i][j] == ENEMY_CHAR) {
                    std::cout << RED_COLOR << ENEMY_CHAR << RESET_COLOR << ' ';
                } else if (world[i][j] == WATER_CHAR) {
                    std::cout << BLUE_COLOR << WATER_CHAR << RESET_COLOR << ' ';
                } else if (world[i][j] == 's') {
                    std::cout << GRAY_COLOR << 's' << RESET_COLOR << ' ';
                } else if (world[i][j] == ',') {
                    std::cout << LIGHT_YELLOW_COLOR << ',' << RESET_COLOR << ' ';
                } else if (world[i][j] == ENERGY_POTION_CHAR) {
                    std::cout << YELLOW_COLOR << ENERGY_POTION_CHAR << RESET_COLOR << ' ';
                } else if (world[i][j] == STAMINA_POTION_CHAR) {
                    std::cout << ORANGE_COLOR << STAMINA_POTION_CHAR << RESET_COLOR << ' ';
                } else if (world[i][j] == HEALTH_POTION_CHAR) {
                    std::cout << PINK_COLOR << HEALTH_POTION_CHAR << RESET_COLOR << ' ';
                } else if (world[i][j] == '.') {
                    std::cout << LIGHT_YELLOW_COLOR << ',' << RESET_COLOR << ' ';
                } else {
                    std::cout << world[i][j] << ' ';
                }
            } else if (world[i][j] == 'M') {
                std::cout << 'M' << ' ';
            } else {
                std::cout << '.' << ' ';
            }
        }
        std::cout << std::endl;
    }

    auto currentTime = std::chrono::steady_clock::now();
    static auto startTime = currentTime;
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
    int minutes = elapsed.count() / 60;
    int seconds = elapsed.count() % 60;
    std::cout << "HP: " << health << "/" << maxHealth << "   ";
    std::cout << "Stamina: " << static_cast<int>(stamina) << "/" << maxStamina << "   ";
    std::cout << "Enemies Killed: " << score << "/" << NUM_ENEMIES << "   ";
    std::cout << "Time: " << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << seconds << std::endl;
}

bool isValidMove(int x, int y) {
    return x >= 0 && x < WORLD_WIDTH && y >= 0 && y < WORLD_HEIGHT && world[y][x] != 's';
}

void updateStamina() {
    float exponentialRate = 1.02;
    float regenRate = std::pow(exponentialRate, (maxStamina - stamina) / 25);
    stamina = std::min(stamina + regenRate, maxStamina);
}

void consumeStamina(float amount) {
    stamina = std::max(stamina - amount, 0.0f);
}

void congratulations(std::chrono::steady_clock::time_point startTime) {
    system("cls");
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
    int minutes = elapsed.count() / 60;
    int seconds = elapsed.count() % 60;
    std::string time = std::to_string(minutes) + ":" + std::to_string(seconds);
    std::string congratulationsString = "Congratulations!!! You Won!!! Your Time is ";
    std::string pressEnterToQuit = "Press Enter to quit...";
    
    for (int i = 0; i < congratulationsString.length(); ++i) {
        std::cout << RAINBOW_COLORS[i % 6] << congratulationsString[i];
    }
    for (int i = 0; i < time.length(); ++i) {
        std::cout << RAINBOW_COLORS[i % 6] << time[i];
    }
    std::cout << std::endl;
    for (int i = 0; i < pressEnterToQuit.length(); ++i) {
        std::cout << RAINBOW_COLORS[i % 6] << pressEnterToQuit[i];
    }
    
    while (_getch() != '\r');
}

void gameOver() {
    system("cls");
    std::cout << RED_COLOR << "Game Over" << std::endl;
    std::cout << "Final Score: " << score << std::endl;
    std::cout << "Press Enter to quit..." << std::endl;
    while (_getch() != '\r');
}

int main() {
    srand(time(nullptr));

    while (true) {
        initializeWorld();

        auto startTime = std::chrono::steady_clock::now();
        auto lastUpdateTime = std::chrono::steady_clock::now();

        while (health > 0) {
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastUpdateTime).count();

            if (elapsedTime >= UPDATE_INTERVAL_MS) {
                lastUpdateTime = currentTime;

                printWorld();
                updateStamina();
            }

            if (_kbhit()) {
                char move = _getch();
                int newX = player.x;
                int newY = player.y;

                if (move == 'w') {
                    --newY;
                } else if (move == 's') {
                    ++newY;
                } else if (move == 'a') {
                    --newX;
                } else if (move == 'd') {
                    ++newX;
                }

                if (isValidMove(newX, newY)) {
                    float staminaCost = STAMINA_COST_PER_STEP;
                    if (world[newY][newX] == WATER_CHAR) {
                        staminaCost = WATER_STAMINA_COST_PER_STEP;
                    }
                    if (stamina >= staminaCost) {
                        for (int i = 0; i < NUM_ENEMIES; ++i) {
                            if (newX == enemies[i].position.x && newY == enemies[i].position.y) {
                                ++score;
                                health -= 25;
                                world[enemies[i].position.y][enemies[i].position.x] = '.';
                                do {
                                    enemies[i].position.x = rand() % WORLD_WIDTH;
                                    enemies[i].position.y = rand() % WORLD_HEIGHT;
                                } while (world[enemies[i].position.y][enemies[i].position.x] != '.');
                                world[enemies[i].position.y][enemies[i].position.x] = ENEMY_CHAR;
                                if (score == NUM_ENEMIES) {
                                    congratulations(startTime);
                                    return 0;
                                }
                            }
                        }
                        for (int i = 0; i < NUM_ENERGY_POTIONS; ++i) {
                            if (newX == energyPotions[i].position.x && newY == energyPotions[i].position.y) {
                                stamina = std::min(stamina + ENERGY_POTION_STAMINA_GAIN, maxStamina);
                                world[energyPotions[i].position.y][energyPotions[i].position.x] = '.';
                                energyPotions[i].position.x = -1;
                            }
                        }
                        for (int i = 0; i < NUM_HEALTH_POTIONS; ++i) {
                            if (newX == healthPotions[i].position.x && newY == healthPotions[i].position.y) {
                                maxHealth += 25;
                                health = maxHealth;
                                world[healthPotions[i].position.y][healthPotions[i].position.x] = '.';
                                healthPotions[i].position.x = -1;
                            }
                        }
                        for (int i = 0; i < NUM_STAMINA_POTIONS; ++i) {
                            if (newX == staminaPotions[i].position.x && newY == staminaPotions[i].position.y) {
                                maxStamina += 25;
                                stamina = maxStamina;
                                world[staminaPotions[i].position.y][staminaPotions[i].position.x] = '.';
                                staminaPotions[i].position.x = -1;
                            }
                        }
                        world[player.y][player.x] = '.';
                        player.x = newX;
                        player.y = newY;
                        world[player.y][player.x] = 'M';
                        consumeStamina(staminaCost);
                        printWorld();
                    }
                }
            }
        }

        gameOver();
        return 0;
    }

    return 0;
}
