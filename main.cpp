#include <GL/glut.h>
#include <vector>
#include <cstdlib> // For random number generation
#include <ctime>   // For seeding random number generator
#include <iostream>
#include <string>

// Block structure
struct Block {
    float x, y, width, height;
    float r, g, b; // Color components
};

// Game variables
std::vector<Block> blocks; // List of placed blocks
float currentBlockWidth = 0.2f;
float currentBlockHeight = 0.05f;
float currentBlockX = 0.0f;
float currentBlockY = -0.8f; // Start near the bottom
float speed = 0.01f;
bool movingLeft = true;
bool gameOver = false;      // Game over flag
std::string feedbackMessage = "Press SPACE to place a block"; // Message to display feedback
int score = 0; // Player score
int highestScore = 0; // Highest score achieved

// Function to draw a block
void drawBlock(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b); // Set the color
    glBegin(GL_QUADS);
    glVertex2f(x - width / 2, y - height / 2);
    glVertex2f(x + width / 2, y - height / 2);
    glVertex2f(x + width / 2, y + height / 2);
    glVertex2f(x - width / 2, y + height / 2);
    glEnd();
}

// Function to render text on screen
void renderText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

// Function to update the scene
void update(int value) {
    if (!gameOver) { // Only update if the game is not over
        // Update the current block's position
        if (movingLeft) {
            currentBlockX -= speed;
            if (currentBlockX <= -1.0f + currentBlockWidth / 2) movingLeft = false;
        } else {
            currentBlockX += speed;
            if (currentBlockX >= 1.0f - currentBlockWidth / 2) movingLeft = true;
        }
    }

    // Redraw the screen
    glutPostRedisplay();

    // Register the timer again
    glutTimerFunc(16, update, 0); // Approx. 60 FPS
}

// Function to handle keypress
void handleKeypress(unsigned char key, int x, int y) {
    if (key == ' ') {
        if (gameOver) {
            // Restart the game
            blocks.clear();
            currentBlockX = 0.0f;
            currentBlockY = -0.8f;
            currentBlockWidth = 0.2f;
            speed = 0.01f;
            gameOver = false;
            feedbackMessage = "Press SPACE to place a block";
            score = 0; // Reset the score
        } else {
            // Check for misalignment if there's a previous block
            if (!blocks.empty()) {
                const Block& lastBlock = blocks.back();
                float overlap = std::abs(currentBlockX - lastBlock.x);

                if (overlap > currentBlockWidth / 2) {
                    // Blocks are misaligned: game over
                    gameOver = true;
                    feedbackMessage = "Game Over! Press SPACE to restart.";
                    // Update the highest score if needed
                    if (score > highestScore) {
                        highestScore = score;
                    }
                    return;
                }
            }

            // Generate a random color
            float r = static_cast<float>(rand()) / RAND_MAX;
            float g = static_cast<float>(rand()) / RAND_MAX;
            float b = static_cast<float>(rand()) / RAND_MAX;

            // Place the current block with the generated color
            blocks.push_back({currentBlockX, currentBlockY, currentBlockWidth, currentBlockHeight, r, g, b});

            // Move the next block higher
            currentBlockY += currentBlockHeight;

            // Increase the speed slightly to make the game harder
            speed += 0.002f;

            // Increment the score
            score++;
        }
    }
}

// Function to display the scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw all placed blocks
    for (const Block& block : blocks) {
        drawBlock(block.x, block.y, block.width, block.height, block.r, block.g, block.b);
    }

    // Draw the current moving block if the game is not over
    if (!gameOver) {
        glColor3f(0.8f, 0.3f, 0.3f); // Red color for the moving block
        drawBlock(currentBlockX, currentBlockY, currentBlockWidth, currentBlockHeight, 0.8f, 0.3f, 0.3f);
    }

    // Render feedback message
    glColor3f(1.0f, 1.0f, 1.0f); // White color for text
    renderText(-0.9f, 0.9f, feedbackMessage);

    // Render the score
    renderText(0.6f, 0.9f, "Score: " + std::to_string(score));

    // Render the highest score
    renderText(0.6f, 0.85f, "Highest Score: " + std::to_string(highestScore));

    glFlush(); // Sends all commands to the GPU to execute them as soon as possible
    glutSwapBuffers();
}

// Main function
int main(int argc, char** argv) {
    // Seed the random number generator
    srand(static_cast<unsigned>(time(0)));

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Tower Stack Game");

    // Set up 2D projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    // Register callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(handleKeypress);
    glutTimerFunc(16, update, 0); // Start the update loop

    // Start the main loop
    glutMainLoop();

    return 0;
}
