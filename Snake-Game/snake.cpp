#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>

using namespace sf;
using namespace std;

// Constants
const int WIDTH = 20;
const int HEIGHT = 20;
const int TILE_SIZE = 30;
const int UP = 1, DOWN = 2, LEFT = 3, RIGHT = 4;
const Color SNAKE_COLOR = Color::Green;
const Color FRUIT_COLOR = Color::Red;

// Snake structure
struct SnakeSegment {
    int x, y;
};

// Function prototypes
void Setup();
void Draw();
void Input();
void Logic();

// Global variables
bool gameOver;
int direction;
int score;
SnakeSegment head;
vector<SnakeSegment> snake;
SnakeSegment fruit;

// SFML variables
RenderWindow window(VideoMode(WIDTH * TILE_SIZE, HEIGHT * TILE_SIZE), "SFML Snake");

RectangleShape tile(Vector2f(TILE_SIZE, TILE_SIZE));

// Main function
int main() {
    Setup(); // Initialize game

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }

        Input();
        Logic();
        window.clear();
        Draw();
        window.display();
    }

    return 0;
}

// Initialize game
void Setup() {
    gameOver = false;
    direction = RIGHT;
    score = 0;
    head.x = WIDTH / 2;
    head.y = HEIGHT / 2;
    snake.push_back(head);

    // Seed random number generator
    srand(static_cast<unsigned int>(time(0)));

    // Place initial fruit
    fruit.x = rand() % WIDTH;
    fruit.y = rand() % HEIGHT;

    tile.setOutlineThickness(1);
    tile.setOutlineColor(Color::Black);
}

// Draw game board
void Draw() {
    // Draw snake
    for (SnakeSegment s : snake) {
        tile.setPosition(s.x * TILE_SIZE, s.y * TILE_SIZE);
        tile.setFillColor(SNAKE_COLOR);
        window.draw(tile);
    }

    // Draw fruit
    tile.setPosition(fruit.x * TILE_SIZE, fruit.y * TILE_SIZE);
    tile.setFillColor(FRUIT_COLOR);
    window.draw(tile);

    // Display score
    Font font;
    font.loadFromFile("arial.ttf");
    Text text;
    text.setFont(font);
    text.setString("Score: " + to_string(score));
    text.setCharacterSize(24);
    text.setFillColor(Color::White);
    text.setPosition(10, 10);
    window.draw(text);
}

// Take user input
void Input() {
    if (Keyboard::isKeyPressed(Keyboard::W) && direction != DOWN) {
        direction = UP;
    } else if (Keyboard::isKeyPressed(Keyboard::S) && direction != UP) {
        direction = DOWN;
    } else if (Keyboard::isKeyPressed(Keyboard::A) && direction != RIGHT) {
        direction = LEFT;
    } else if (Keyboard::isKeyPressed(Keyboard::D) && direction != LEFT) {
        direction = RIGHT;
    } else if (Keyboard::isKeyPressed(Keyboard::X)) {
        gameOver = true;
    }
}

// Game logic
void Logic() {
    // Move the snake
    SnakeSegment prev = snake[0];
    SnakeSegment prev2;
    snake[0] = head;

    // Adjust direction
    if (direction == RIGHT) {
        head.x++;
    } else if (direction == LEFT) {
        head.x--;
    } else if (direction == UP) {
        head.y--;
    } else if (direction == DOWN) {
        head.y++;
    }

    // Check collision with wall
    if (head.x >= WIDTH || head.x < 0 || head.y >= HEIGHT || head.y < 0) {
        gameOver = true;
        return;
    }

    // Check collision with itself
    for (SnakeSegment s : snake) {
        if (s.x == head.x && s.y == head.y) {
            gameOver = true;
        }
    }

    // Check if snake eats fruit
    if (head.x == fruit.x && head.y == fruit.y) {
        score++;
        fruit.x = rand() % WIDTH;
        fruit.y = rand() % HEIGHT;
        snake.push_back(prev);
    }

    // Move the body
    for (int i = 1; i < snake.size(); i++) {
        prev2 = snake[i];
        snake[i] = prev;
        prev = prev2;
    }
}
