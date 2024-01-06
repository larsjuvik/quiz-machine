/// If you want to create your own questions, edit the totalQuestions-field and add
/// your questions in the setup function as shown.
///
/// The questions are stored in a struct called question, which has the following fields:
/// - question: The question itself
/// - leftOption: The left option
/// - rightOption: The right option
/// - correctAnswer: true if left option is correct, false if right option is correct
#define ButtonReadDelay 100
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 2, 3, 4, 5);

// Input (Buttons)
const int buttonLeftPin = 6, buttonRightPin = 7;
int buttonLeftState = 0, buttonRightState = 0;

// Output (LEDs)
const int redLEDPin = 8, greenLEDPin = 9;

// Question Logic
typedef struct
{
    String question;
    String leftOption;
    String rightOption;

    // true = leftOption is correct, false = rightOption is correct
    bool correctAnswer;
} question;

const int totalQuestions = 3;       // Total number of questions
int currentQuestionIndex = 0;       // Current question index
question questions[totalQuestions]; // Question bank

// Points and Rendering Logic
int points = 0;
bool gameEndedDrawn = false;       // Is final points drawn on screen?
bool currentQuestionDrawn = false; // Is current question drawn on screen?

// Handles the setup of the game
void setup()
{
    questions[0] = (question){"Cool project?", "No", "Yes", false};
    questions[1] = (question){"Most moons?", "Saturn", "Mars", true};
    questions[2] = (question){"Closest to sun?", "Mercury", "Mars", true};

    // Input
    pinMode(buttonLeftPin, INPUT);
    pinMode(buttonRightPin, INPUT);

    // Output
    pinMode(redLEDPin, OUTPUT);
    pinMode(greenLEDPin, OUTPUT);

    lcd.begin(16, 2);
}

// Turn off both LEDs
void turnOffLEDs()
{
    digitalWrite(redLEDPin, LOW);
    digitalWrite(greenLEDPin, LOW);
}

// Turn on both LEDs
void turnOnLEDs()
{
    digitalWrite(redLEDPin, HIGH);
    digitalWrite(greenLEDPin, HIGH);
}

// Handles the input from the buttons
void handleInput()
{
    // User wants to reset
    if (buttonLeftState == HIGH && buttonRightState == HIGH)
    {
        reset();
        return;
    }

    // Can only guess if game is not finished
    if (gameEnded())
        return;

    if (buttonLeftState == HIGH)
    {
        guess(currentQuestionIndex, true);
    }
    else if (buttonRightState == HIGH)
    {
        guess(currentQuestionIndex, false);
    }
}

// Reads the input from the buttons
void readInput()
{
    // Read both buttons input (+ small delay in case user presses both buttons -> reset)
    buttonLeftState = digitalRead(buttonLeftPin);
    delay(ButtonReadDelay);
    buttonRightState = digitalRead(buttonRightPin);
}

// Main loop
void loop()
{
    turnOffLEDs(); // TODO: refactor to only turn off when needed
    readInput();

    // Check if game has ended
    if (gameEnded() && gameEndedDrawn == false)
    {
        writePointsScreen();
    }

    handleInput();

    if (currentQuestionDrawn == false)
    {
        writeQuestion();
    }
}

// Resets the game
void reset()
{
    turnOnLEDs();
    delay(2000);

    // Reset values
    currentQuestionIndex = 0;
    currentQuestionDrawn = false;
    gameEndedDrawn = false;
    points = 0;
}

// Returns true if game has ended
bool gameEnded()
{
    return currentQuestionIndex == totalQuestions;
}

// Draws the final points on the screen
void writePointsScreen()
{
    lcd.clear();
    lcd.print("Poeng:");
    lcd.setCursor(0, 1);
    lcd.print(String(points) + " / " + String(totalQuestions));
    gameEndedDrawn = true;
}

// Guesses an answer for a question
// questionIndex - the number index of the question
// g - the guess, either true (left) or false (right)
void guess(int questionIndex, bool g)
{
    auto question = questions[questionIndex];

    if (question.correctAnswer == g)
    {
        // Correct
        digitalWrite(redLEDPin, LOW);
        digitalWrite(greenLEDPin, HIGH);
        points++;
    }
    else
    {
        // Wrong
        digitalWrite(redLEDPin, HIGH);
        digitalWrite(greenLEDPin, LOW);
    }

    currentQuestionIndex++;
    currentQuestionDrawn = false;
    delay(3000);
}

// Draws the current question on the screen
void writeQuestion()
{
    auto q = questions[currentQuestionIndex];

    // Clear screen and write question
    lcd.clear();
    lcd.print(q.question);
    lcd.setCursor(0, 1);
    lcd.print(q.leftOption);

    // Calculate spaces between left and right option
    int spaces = max(16 - q.leftOption.length() - q.rightOption.length(), 0);
    String spacesString;
    for (int i = 0; i < spaces; i++)
    {
        spacesString += ' ';
    }
    lcd.print(spacesString + q.rightOption);

    currentQuestionDrawn = true;
}
