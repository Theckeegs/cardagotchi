#include <M5Cardputer.h>
#include <vector>
#include <EEPROM.h>

enum CardagotchiStage { EGG, BABY, CHILD, TEEN, ADULT, DEAD };
enum GameState { MAIN, STATS, FEEDING, CLEANING, MEDICINE, GAMES, CALLING };
enum FoodType { MEAL, SNACK, DESSERT };

// Egg stage
const char* eggSprite1 = R"(
  .----.
 /      \
|        |
 \      /
  '----'
)";

const char* eggSprite2 = R"(
  .----.
 /      \
|   ..   |
 \      /
  '----'
)";

const char* eggSprite3 = R"(
  .----.
 /      \
|  ....  |
 \      /
  '----'
)";

// Baby stage
const char* babySprite1 = R"(
  ,---.
 /     \
( o   o )
 \  ^  /
  '---'
)";

const char* babySprite2 = R"(
  ,---.
 /     \
( ^   ^ )
 \  o  /
  '---'
)";

const char* babySprite3 = R"(
  ,---.
 /     \
( -   - )
 \  O  /
  '---'
)";

// Child stage
const char* childSprite1 = R"(
   ,---.
  /     \
 ( o   o )
  \  ^  /
 /''---''\
/         \
)";

const char* childSprite2 = R"(
   ,---.
  /     \
 ( ^   ^ )
  \  o  /
 /''---''\
/   \-/   \
)";

const char* childSprite3 = R"(
   ,---.
  /     \
 ( o   o )
  \  v  /
 /''---''\
/  --|--  \
)";

// Teen stage
const char* teenSprite1 = R"(
    ,---.
   /     \
  ( o   o )
   \  ^  /
  /''---''\
 /         \
(           )
 \         /
)";

const char* teenSprite2 = R"(
    ,---.
   /     \
  ( ^   ^ )
   \  o  /
  /''---''\
 /  \___/  \
(           )
 \         /
)";

const char* teenSprite3 = R"(
    ,---.
   /     \
  ( -   - )
   \  O  /
  /''---''\
 /  _____  \
(   \___/   )
 \         /
)";

// Adult stage
const char* adultSprite1 = R"(
     ,---.
    /     \
   ( o   o )
    \  ^  /
   /''---''\
  /         \
 (           )
  \         /
   '-------'
)";

const char* adultSprite2 = R"(
     ,---.
    /     \
   ( ^   ^ )
    \  o  /
   /''---''\
  /  \___/  \
 (           )
  \  _____  /
   '-------'
)";

const char* adultSprite3 = R"(
     ,---.
    /     \
   ( o   o )
    \  v  /
   /''---''\
  /  _____  \
 (   \___/   )
  \  -----  /
   '-------'
)";

// Dead stage
const char* deadSprite = R"(
     ,---.
    /     \
   ( x   x )
    \  -  /
   /''---''\
  /  RIP    \
 (           )
  \ _______ /
   '-------'
)";

// To be continued in Part 2
// Continued from Part 1

class Cardagotchi {
private:
    String name;
    int hunger, happiness, energy, cleanliness;
    unsigned long lastUpdateTime, birthTime, lastAnimationTime;
    CardagotchiStage stage;
    int growthPoints, age, neglectPoints;
    bool isSick, isOffScreen;
    int animationFrame;
    int x, y; // Position for sprite

public:
    Cardagotchi() : 
        name("Egg"), hunger(50), happiness(50), energy(50), cleanliness(100),
        lastUpdateTime(0), birthTime(0), lastAnimationTime(0), stage(EGG), 
        growthPoints(0), age(0), neglectPoints(0),
        isSick(false), isOffScreen(false), animationFrame(0), x(48), y(32) {}

    void start() { 
        birthTime = millis(); 
        lastUpdateTime = birthTime;
        lastAnimationTime = birthTime;
    }

    void update() {
        unsigned long currentTime = millis();
        if (currentTime - lastUpdateTime > 10000) {  // Update every 10 seconds
            if (stage != EGG && stage != DEAD) {
                hunger = min(100, hunger + 1);
                happiness = max(0, happiness - 1);
                energy = max(0, energy - 1);
                cleanliness = max(0, cleanliness - 1);
                
                // Growth and sickness logic
                if (hunger < 30 && happiness > 70 && energy > 70) {
                    growthPoints++;
                    neglectPoints = max(0, neglectPoints - 1);
                    if (growthPoints >= 100 && stage != ADULT) evolve();
                } else {
                    neglectPoints++;
                    if (neglectPoints >= 100) die();
                }
                
                if (random(100) < 5) isSick = true;  // 5% chance of getting sick
                if (random(100) < 2) isOffScreen = true;  // 2% chance of walking off screen
                
                age++;
            } else if (stage == EGG && currentTime - birthTime > 300000) {  // 5 minutes
                evolve();  // Egg hatches into Baby
            }
            lastUpdateTime = currentTime;
        }

        // Animate sprite every 500ms
        if (currentTime - lastAnimationTime > 500) {
            animationFrame = (animationFrame + 1) % 3;  // Cycle through 3 frames
            lastAnimationTime = currentTime;
        }

        // Move sprite
        if (!isOffScreen && stage != EGG && stage != DEAD) {
            x += random(-1, 2);
            y += random(-1, 2);
            x = constrain(x, 0, 96);
            y = constrain(y, 0, 64);
        }
    }

    void evolve() {
        switch(stage) {
            case EGG: stage = BABY; name = "Baby"; break;
            case BABY: stage = CHILD; name = "Child"; break;
            case CHILD: stage = TEEN; name = "Teen"; break;
            case TEEN: stage = ADULT; name = "Adult"; break;
            default: break;
        }
        growthPoints = 0;
    }

    void die() {
        stage = DEAD;
        name = "Dead";
    }

    // To be continued in Part 3
    // Continued from Part 2

    void displaySprite() {
        if (!isOffScreen) {
            const char* currentSprite;
            switch(stage) {
                case EGG:
                    currentSprite = (animationFrame == 0) ? eggSprite1 : 
                                    (animationFrame == 1) ? eggSprite2 : eggSprite3;
                    break;
                case BABY:
                    currentSprite = (animationFrame == 0) ? babySprite1 : 
                                    (animationFrame == 1) ? babySprite2 : babySprite3;
                    break;
                case CHILD:
                    currentSprite = (animationFrame == 0) ? childSprite1 : 
                                    (animationFrame == 1) ? childSprite2 : childSprite3;
                    break;
                case TEEN:
                    currentSprite = (animationFrame == 0) ? teenSprite1 : 
                                    (animationFrame == 1) ? teenSprite2 : teenSprite3;
                    break;
                case ADULT:
                    currentSprite = (animationFrame == 0) ? adultSprite1 : 
                                    (animationFrame == 1) ? adultSprite2 : adultSprite3;
                    break;
                case DEAD:
                    currentSprite = deadSprite;
                    break;
            }
            M5Cardputer.Display.setCursor(x, y);
            M5Cardputer.Display.print(currentSprite);
        }
    }

    void displayStats() {
        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.setCursor(0, 0);
        M5Cardputer.Display.printf("Name: %s (Stage: %s)\n", name.c_str(), getStageName().c_str());
        M5Cardputer.Display.printf("Age: %d\n", age);
        M5Cardputer.Display.printf("Hunger: %d\n", hunger);
        M5Cardputer.Display.printf("Happiness: %d\n", happiness);
        M5Cardputer.Display.printf("Energy: %d\n", energy);
        M5Cardputer.Display.printf("Cleanliness: %d\n", cleanliness);
        M5Cardputer.Display.printf("Health: %s\n", isSick ? "Sick" : "Healthy");
        M5Cardputer.Display.printf("Location: %s\n", isOffScreen ? "Off Screen" : "On Screen");
    }

    void feed(FoodType food) {
        switch(food) {
            case MEAL:
                hunger = max(0, hunger - 20);
                break;
            case SNACK:
                hunger = max(0, hunger - 10);
                happiness += 5;
                break;
            case DESSERT:
                hunger = max(0, hunger - 5);
                happiness += 10;
                break;
        }
        happiness = min(100, happiness);
    }

    void play() {
        happiness = min(100, happiness + 20);
        energy = max(0, energy - 10);
        hunger = min(100, hunger + 5);
    }

    void sleep() {
        energy = min(100, energy + 30);
        hunger = min(100, hunger + 10);
    }

    void clean() {
        cleanliness = 100;
        happiness += 5;
        happiness = min(100, happiness);
    }

    void giveMedicine() {
        if (isSick) {
            isSick = false;
            happiness += 10;
        } else {
            happiness -= 20;
        }
        happiness = constrain(happiness, 0, 100);
    }

    void call() {
        if (isOffScreen) {
            isOffScreen = false;
            x = 48;
            y = 32;
        }
    }

    void save() {
        int addr = 0;
        EEPROM.put(addr, hunger); addr += sizeof(hunger);
        EEPROM.put(addr, happiness); addr += sizeof(happiness);
        EEPROM.put(addr, energy); addr += sizeof(energy);
        EEPROM.put(addr, cleanliness); addr += sizeof(cleanliness);
        EEPROM.put(addr, lastUpdateTime); addr += sizeof(lastUpdateTime);
        EEPROM.put(addr, birthTime); addr += sizeof(birthTime);
        EEPROM.put(addr, stage); addr += sizeof(stage);
        EEPROM.put(addr, growthPoints); addr += sizeof(growthPoints);
        EEPROM.put(addr, age); addr += sizeof(age);
        EEPROM.put(addr, neglectPoints); addr += sizeof(neglectPoints);
        EEPROM.put(addr, isSick); addr += sizeof(isSick);
        EEPROM.put(addr, isOffScreen); addr += sizeof(isOffScreen);
        EEPROM.put(addr, x); addr += sizeof(x);
        EEPROM.put(addr, y); addr += sizeof(y);

        // Save name (up to 20 characters)
        char nameBuffer[20] = {0};
        strncpy(nameBuffer, name.c_str(), 19);
        EEPROM.put(addr, nameBuffer);

        EEPROM.commit();
    }

    void load() {
        int addr = 0;
        EEPROM.get(addr, hunger); addr += sizeof(hunger);
        EEPROM.get(addr, happiness); addr += sizeof(happiness);
        EEPROM.get(addr, energy); addr += sizeof(energy);
        EEPROM.get(addr, cleanliness); addr += sizeof(cleanliness);
        EEPROM.get(addr, lastUpdateTime); addr += sizeof(lastUpdateTime);
        EEPROM.get(addr, birthTime); addr += sizeof(birthTime);
        EEPROM.get(addr, stage); addr += sizeof(stage);
        EEPROM.get(addr, growthPoints); addr += sizeof(growthPoints);
        EEPROM.get(addr, age); addr += sizeof(age);
        EEPROM.get(addr, neglectPoints); addr += sizeof(neglectPoints);
        EEPROM.get(addr, isSick); addr += sizeof(isSick);
        EEPROM.get(addr, isOffScreen); addr += sizeof(isOffScreen);
        EEPROM.get(addr, x); addr += sizeof(x);
        EEPROM.get(addr, y); addr += sizeof(y);

        // Load name
        char nameBuffer[20] = {0};
        EEPROM.get(addr, nameBuffer);
        name = String(nameBuffer);
    }

    bool isNewGame() {
        int marker;
        EEPROM.get(0, marker);
        return marker != 0xABCD; // Use any unique value as a marker
    }

    void markAsExistingGame() {
        int marker = 0xABCD;
        EEPROM.put(0, marker);
        EEPROM.commit();
    }

private:
    String getStageName() {
        switch(stage) {
            case EGG: return "Egg";
            case BABY: return "Baby";
            case CHILD: return "Child";
            case TEEN: return "Teen";
            case ADULT: return "Adult";
            case DEAD: return "Dead";
            default: return "Unknown";
        }
    }
};

// To be continued in Part 4
// Continued from Part 3

class Game {
public:
    virtual void play() = 0;
    virtual ~Game() {}
};

class JumpingGame : public Game {
private:
    int playerY, obstacleX;
    bool isJumping;
    int score;

public:
    JumpingGame() : playerY(50), obstacleX(128), isJumping(false), score(0) {}

    void play() override {
        M5Cardputer.Display.clear();
        M5Cardputer.Display.println("Jumping Game! Press 'J' to jump");
        delay(2000);

        while (true) {
            M5Cardputer.Display.clear();
            
            // Draw player
            M5Cardputer.Display.fillRect(20, playerY, 10, 10, TFT_WHITE);
            
            // Draw obstacle
            M5Cardputer.Display.fillRect(obstacleX, 55, 5, 5, TFT_WHITE);
            
            // Move obstacle
            obstacleX -= 2;
            if (obstacleX < 0) {
                obstacleX = 128;
                score++;
            }
            
            // Handle jump
            if (isJumping) {
                playerY -= 2;
                if (playerY <= 20) isJumping = false;
            } else if (playerY < 50) {
                playerY += 2;
            }
            
            // Check for collision
            if (obstacleX < 30 && obstacleX > 10 && playerY > 40) {
                M5Cardputer.Display.clear();
                M5Cardputer.Display.println("Game Over! Score: " + String(score));
                delay(2000);
                return;
            }
            
            // Handle input
            if (M5Cardputer.Keyboard.isChange()) {
                if (M5Cardputer.Keyboard.isPressed()) {
                    if (M5Cardputer.Keyboard.isKeyPressed('J')) isJumping = true;
                    if (M5Cardputer.Keyboard.isKeyPressed('Q')) return;
                }
            }
            
            M5Cardputer.Display.println("Score: " + String(score));
            delay(50);
        }
    }
};

class SimonSaysGame : public Game {
private:
    std::vector<int> sequence;
    int level;

public:
    SimonSaysGame() : level(0) {}

    void play() override {
        M5Cardputer.Display.clear();
        M5Cardputer.Display.println("Simon Says! Watch the sequence");
        delay(2000);

        while (true) {
            // Add new color to sequence
            sequence.push_back(random(1, 5));
            level++;

            // Display sequence
            for (int color : sequence) {
                M5Cardputer.Display.clear();
                switch (color) {
                    case 1: M5Cardputer.Display.fillScreen(TFT_RED); break;
                    case 2: M5Cardputer.Display.fillScreen(TFT_GREEN); break;
                    case 3: M5Cardputer.Display.fillScreen(TFT_BLUE); break;
                    case 4: M5Cardputer.Display.fillScreen(TFT_YELLOW); break;
                }
                delay(500);
                M5Cardputer.Display.clear();
                delay(250);
            }

            // Get player input
            for (int color : sequence) {
                M5Cardputer.Display.clear();
                M5Cardputer.Display.println("Enter sequence:");
                M5Cardputer.Display.println("1:Red 2:Green 3:Blue 4:Yellow");

                int input = 0;
                while (input == 0) {
                    if (M5Cardputer.Keyboard.isChange()) {
                        if (M5Cardputer.Keyboard.isPressed()) {
                            if (M5Cardputer.Keyboard.isKeyPressed('1')) input = 1;
                            else if (M5Cardputer.Keyboard.isKeyPressed('2')) input = 2;
                            else if (M5Cardputer.Keyboard.isKeyPressed('3')) input = 3;
                            else if (M5Cardputer.Keyboard.isKeyPressed('4')) input = 4;
                        }
                    }
                    delay(10);
                }

                if (input != color) {
                    M5Cardputer.Display.clear();
                    M5Cardputer.Display.println("Game Over! Level: " + String(level));
                    delay(2000);
                    return;
                }
            }

            M5Cardputer.Display.clear();
            M5Cardputer.Display.println("Correct! Next level...");
            delay(1000);
        }
    }
};

class RockPaperScissorsGame : public Game {
private:
    int playerScore, computerScore;

    int getComputerChoice() {
        return random(1, 4); // 1: Rock, 2: Paper, 3: Scissors
    }

    String choiceToString(int choice) {
        switch (choice) {
            case 1: return "Rock";
            case 2: return "Paper";
            case 3: return "Scissors";
            default: return "Invalid";
        }
    }

    int getWinner(int player, int computer) {
        if (player == computer) return 0; // Tie
        if ((player == 1 && computer == 3) || (player == 2 && computer == 1) || (player == 3 && computer == 2)) return 1; // Player wins
        return 2; // Computer wins
    }

public:
    RockPaperScissorsGame() : playerScore(0), computerScore(0) {}

    void play() override {
        M5Cardputer.Display.clear();
        M5Cardputer.Display.println("Rock Paper Scissors!");
        M5Cardputer.Display.println("1:Rock 2:Paper 3:Scissors");
        delay(2000);

        while (true) {
            M5Cardputer.Display.clear();
            M5Cardputer.Display.println("Your choice? (Q to quit)");
            M5Cardputer.Display.println("1:Rock 2:Paper 3:Scissors");

            int playerChoice = 0;
            while (playerChoice == 0) {
                if (M5Cardputer.Keyboard.isChange()) {
                    if (M5Cardputer.Keyboard.isPressed()) {
                        if (M5Cardputer.Keyboard.isKeyPressed('1')) playerChoice = 1;
                        else if (M5Cardputer.Keyboard.isKeyPressed('2')) playerChoice = 2;
                        else if (M5Cardputer.Keyboard.isKeyPressed('3')) playerChoice = 3;
                        else if (M5Cardputer.Keyboard.isKeyPressed('Q')) return;
                    }
                }
                delay(10);
            }

            int computerChoice = getComputerChoice();

            M5Cardputer.Display.clear();
            M5Cardputer.Display.println("You: " + choiceToString(playerChoice));
            M5Cardputer.Display.println("Computer: " + choiceToString(computerChoice));

            int winner = getWinner(playerChoice, computerChoice);
            switch (winner) {
                case 0:
                    M5Cardputer.Display.println("It's a tie!");
                    break;
                case 1:
                    M5Cardputer.Display.println("You win!");
                    playerScore++;
                    break;
                case 2:
                    M5Cardputer.Display.println("Computer wins!");
                    computerScore++;
                    break;
            }

            M5Cardputer.Display.println("Score: " + String(playerScore) + " - " + String(computerScore));
            delay(2000);
        }
    }
};

// To be continued in Part 5
// Continued from Part 4

Cardagotchi pet;
GameState currentState = MAIN;
std::vector<Game*> games = {new JumpingGame(), new SimonSaysGame(), new RockPaperScissorsGame()};
unsigned long lastUpdateTime = 0;

const int MENU_ITEMS = 9;
const String menuItems[MENU_ITEMS] = {
    "Feed", "Play", "Sleep", "Clean", "Medicine", 
    "Games", "Call", "Stats", "Save Game"
};
int currentMenuItem = 0;

bool key_up_pressed = false;
bool key_down_pressed = false;
bool key_left_pressed = false;
bool key_right_pressed = false;
bool key_enter_pressed = false;

unsigned long last_key_press_time = 0;
const unsigned long debounce_delay = 200; // 200ms debounce time

void displayMenu() {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(0, 0);
    
    // Display pet sprite at the top
    pet.displaySprite();
    
    // Display menu items
    for (int i = 0; i < 5; i++) {
        int menuIndex = (currentMenuItem + i - 2 + MENU_ITEMS) % MENU_ITEMS;
        if (i == 2) {
            M5Cardputer.Display.print("> ");
        } else {
            M5Cardputer.Display.print("  ");
        }
        M5Cardputer.Display.println(menuItems[menuIndex]);
    }
}

void handleInput() {
    unsigned long current_time = millis();
    bool menu_updated = false;

    if (M5Cardputer.Keyboard.isKeyPressed(';')) {
        if (!key_up_pressed && (current_time - last_key_press_time > debounce_delay)) {
            currentMenuItem = (currentMenuItem - 1 + MENU_ITEMS) % MENU_ITEMS;
            menu_updated = true;
            key_up_pressed = true;
            last_key_press_time = current_time;
        }
    } else {
        key_up_pressed = false;
    }

    if (M5Cardputer.Keyboard.isKeyPressed('.')) {
        if (!key_down_pressed && (current_time - last_key_press_time > debounce_delay)) {
            currentMenuItem = (currentMenuItem + 1) % MENU_ITEMS;
            menu_updated = true;
            key_down_pressed = true;
            last_key_press_time = current_time;
        }
    } else {
        key_down_pressed = false;
    }

    if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
        if (!key_enter_pressed && (current_time - last_key_press_time > debounce_delay)) {
            key_enter_pressed = true;
            last_key_press_time = current_time;
            handleMenuSelection();
        }
    } else {
        key_enter_pressed = false;
    }

    if (menu_updated) {
        displayMenu();
    }
}

void handleMenuSelection() {
    switch (currentMenuItem) {
        case 0: // Feed
            currentState = FEEDING;
            displayFeedingMenu();
            break;
        case 1: // Play
            pet.play();
            break;
        case 2: // Sleep
            pet.sleep();
            break;
        case 3: // Clean
            pet.clean();
            break;
        case 4: // Medicine
            pet.giveMedicine();
            break;
        case 5: // Games
            currentState = GAMES;
            displayGamesMenu();
            break;
        case 6: // Call
            pet.call();
            break;
        case 7: // Stats
            currentState = STATS;
            displayStats();
            break;
        case 8: // Save Game
            pet.save();
            M5Cardputer.Display.clear();
            M5Cardputer.Display.println("Game Saved!");
            delay(1000);
            displayMenu();
            break;
    }
}

void displayFeedingMenu() {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.println("1: Meal 2: Snack 3: Dessert");
    M5Cardputer.Display.println("ESC: Back");
}

void displayGamesMenu() {
    M5Cardputer.Display.clear();
    M5Cardputer.Display.println("1: Jumping 2: Simon Says");
    M5Cardputer.Display.println("3: Rock Paper Scissors");
    M5Cardputer.Display.println("ESC: Back");
}

void displayStats() {
    M5Cardputer.Display.clear();
    pet.displayStats();
    M5Cardputer.Display.println("ESC: Back");
}

void setup() {
    M5Cardputer.begin();
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextSize(1);
    
    EEPROM.begin(512); // Initialize EEPROM with 512 bytes

    if (pet.isNewGame()) {
        pet.start();
        pet.markAsExistingGame();
    } else {
        pet.load();
    }

    displayMenu();
}

void loop() {
    M5Cardputer.update();
    unsigned long currentTime = millis();

    // Update pet and refresh screen every 2 second
    if (currentTime - lastUpdateTime >= 2000) {
        pet.update();
        lastUpdateTime = currentTime;
        
        if (currentState == MAIN) {
            displayMenu();
        }
    }

    switch (currentState) {
        case MAIN:
            handleInput();
            break;
        case FEEDING:
            handleFeedingInput();
            break;
        case GAMES:
            handleGamesInput();
            break;
        case STATS:
            handleStatsInput();
            break;
    }

    delay(10); // Small delay to prevent excessive CPU usage
}

void handleFeedingInput() {
    if (M5Cardputer.Keyboard.isKeyPressed('1')) {
        pet.feed(MEAL);
        currentState = MAIN;
        displayMenu();
    } else if (M5Cardputer.Keyboard.isKeyPressed('2')) {
        pet.feed(SNACK);
        currentState = MAIN;
        displayMenu();
    } else if (M5Cardputer.Keyboard.isKeyPressed('3')) {
        pet.feed(DESSERT);
        currentState = MAIN;
        displayMenu();
    } else if (M5Cardputer.Keyboard.isKeyPressed('`')) {
        currentState = MAIN;
        displayMenu();
    }
}

void handleGamesInput() {
    if (M5Cardputer.Keyboard.isKeyPressed('1')) {
        games[0]->play();
        currentState = MAIN;
        displayMenu();
    } else if (M5Cardputer.Keyboard.isKeyPressed('2')) {
        games[1]->play();
        currentState = MAIN;
        displayMenu();
    } else if (M5Cardputer.Keyboard.isKeyPressed('3')) {
        games[2]->play();
        currentState = MAIN;
        displayMenu();
    } else if (M5Cardputer.Keyboard.isKeyPressed('`')) {
        currentState = MAIN;
        displayMenu();
    }
}

void handleStatsInput() {
    if (M5Cardputer.Keyboard.isKeyPressed('`')) {
        currentState = MAIN;
        displayMenu();
    }
}

// End of Part 5
// Continued from Part 5

// Cleanup function to delete dynamically allocated games
void cleanup() {
    for (Game* game : games) {
        delete game;
    }
    games.clear();
}

// Call cleanup function when the program ends
void onProgramEnd() {
    cleanup();
}

// Register the cleanup function to be called at program exit


// Note: The setup() and loop() functions are already defined in Part 5.
// They don't need to be repeated here.

// End of Cardagotchi program
