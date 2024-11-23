#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

int loadChestCounter() {
    std::ifstream inFile("chestCounter.txt");
    int counter = 0;
    if (inFile.is_open()) {
        inFile >> counter;
        inFile.close();
    }
    return counter;
}

void saveChestCounter(int counter) {
    std::ofstream outFile("chestCounter.txt");
    if (outFile.is_open()) {
        outFile << counter;
        outFile.close();
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Chest Matrix");
    window.setFramerateLimit(60);

    // Load the frames for the GIF
    std::vector<sf::Texture> gifFrames;
    for (int i = 1; i <= 22; ++i) {
        sf::Texture frame;
        if (!frame.loadFromFile("CHESTGIFFrames/openChest-" + std::to_string(i) + ".png")) {
            std::cout << "Frames Didn't Load properly FIX YOUR CODE U BUM " << i << std::endl;
            return -1;
        }
        gifFrames.push_back(frame);
    }

    sf::Texture chestTexture;
    if (!chestTexture.loadFromFile("Chest/NormalChest.png")) {
        std::cout << "Could Not Load Chest" << std::endl;
        return 0;
    }

    // Sigma Font
    sf::Font font;
    if (!font.loadFromFile("textfiles/Exo-Regular.otf")) {
        std::cout << "Could Not Load Font Fix Your Code you BUM" << std::endl;
        return 0;
    }
    sf::Text text("The Game Of Life", font, 50);
    text.setPosition(10, 10);

    int chestCounter = loadChestCounter();

    sf::Text counterText;
    counterText.setFont(font);
    counterText.setCharacterSize(30);
    counterText.setPosition(10, 70);
    counterText.setFillColor(sf::Color::Black);
    counterText.setString("Chests Opened: " + std::to_string(chestCounter));

    sf::RectangleShape resetButton(sf::Vector2f(150, 50));
    resetButton.setFillColor(sf::Color::Red);
    resetButton.setPosition(10, 120);

    sf::Text resetButtonText("Reset Counter", font, 20);
    resetButtonText.setFillColor(sf::Color::White);
    resetButtonText.setPosition(15, 130);

    const int gridRows = 5;
    const int gridCols = 5;
    const float chestSpacing = 100.0f;
    const float startX = (window.getSize().x - (gridCols - 1) * chestSpacing) / 2;
    const float startY = (window.getSize().y - (gridRows - 1) * chestSpacing) / 2;
    
    std::vector<sf::Sprite> chests;
    std::vector<int> chestFrameIndex;
    std::vector<bool> chestGifPlaying;
    std::vector<sf::Clock> gifClocks;

    for (int row = 0; row < gridRows; ++row) {
        for (int col = 0; col < gridCols; ++col) {
            sf::Sprite chest(chestTexture);
            chest.setPosition(startX + col * chestSpacing, startY + row * chestSpacing);
            chests.push_back(chest);
            chestFrameIndex.push_back(0);    // Start at frame 0
            chestGifPlaying.push_back(false); // GIF is not playing initially
            gifClocks.emplace_back();         // Clock for each chest GIF animation
        }
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

                for (size_t i = 0; i < chests.size(); ++i) {
                    if (chests[i].getGlobalBounds().contains(mousePos) && !chestGifPlaying[i]) {
                        chestGifPlaying[i] = true; // Start playing GIF for the clicked chest
                        chestFrameIndex[i] = 0;    // Reset to the first frame
                        gifClocks[i].restart();    // Reset the clock for the animation
                        
                        // Update the chest counter and save it
                        chestCounter++;
                        counterText.setString("Chests Opened: " + std::to_string(chestCounter));
                        saveChestCounter(chestCounter);
                    }
                }

                if (resetButton.getGlobalBounds().contains(mousePos)) {
                    chestCounter = 0;
                }
            }
        }

        window.clear(sf::Color::White);

        for (size_t i = 0; i < chests.size(); ++i) {
            if (chestGifPlaying[i]) {
                if (gifClocks[i].getElapsedTime().asSeconds() > 0.1f) {
                    chestFrameIndex[i]++;
                    if (chestFrameIndex[i] < gifFrames.size()) {
                        chests[i].setTexture(gifFrames[chestFrameIndex[i]]);
                        gifClocks[i].restart();
                    } else {
                        chestGifPlaying[i] = false;
                        chests[i].setTexture(chestTexture); // Reset to original texture
                    }
                }
            }
            window.draw(chests[i]);
        }

        window.draw(counterText);
        window.draw(resetButton);
        window.draw(resetButtonText);
        window.display();
    }

    return 0;
}
