#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "../include/sfmlbutton.hpp"
#include "main.h"

template <typename T>
T clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

bool isSafe(int row, int col, std::vector<int>& queens, int n) {
    for (int i = 0; i < row; ++i) {
        int placed_col = queens[i];
        if (placed_col == col || abs(placed_col - col) == abs(i - row)) {
            return false;
        }
    }
    return true;
}

void solveNQueens(int n, int row, std::vector<int>& queens, std::vector<std::vector<int>>& solutions) {
    if (row == n) {
        solutions.push_back(queens);
        return;
    }

    for (int col = 0; col < n; ++col) {
        if (isSafe(row, col, queens, n)) {
            queens[row] = col;
            solveNQueens(n, row + 1, queens, solutions);
        }
    }
}

std::vector<std::vector<std::string>> convertToBoard(std::vector<std::vector<int>>& solutions, int n) {
    std::vector<std::vector<std::string>> board_solutions;
    
    for (const auto& solution : solutions) {
        std::vector<std::string> board(n, std::string(n, '.'));
        for (int row = 0; row < n; ++row) {
            board[row][solution[row]] = 'Q';
        }
        board_solutions.push_back(board);
    }
    
    return board_solutions;
}

std::vector<std::vector<std::string>> findNQueensSolutions(int n) {
    std::vector<std::vector<int>> solutions;
    std::vector<int> queens(n, -1);
    solveNQueens(n, 0, queens, solutions);
    return convertToBoard(solutions, n);
}

void drawBoardAndQueens(sf::RenderWindow& window, const std::vector<std::string>& solution, int tileSize, sf::Texture& tex_reina) {
    int n = solution.size();
    // Tablero
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            sf::RectangleShape tile(sf::Vector2f(tileSize, tileSize));
            tile.setPosition(50 + col * tileSize, 100 + row * tileSize);
            if ((row + col) % 2 == 0)
                tile.setFillColor(sf::Color::White);
            else
                tile.setFillColor(sf::Color::Black);
            
            window.draw(tile);
        }
    }

    // Piezas
    sf::RectangleShape queen(sf::Vector2f(tileSize*0.8, tileSize*0.8));
    queen.setTexture(&tex_reina);
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            if (solution[row][col] == 'Q') {
                queen.setPosition(50 + col * tileSize + tileSize*0.1, 100 + row * tileSize + tileSize*0.1);
                if ((row + col) % 2 == 0) {
                    queen.setFillColor(sf::Color::Black);
                } else {
                    queen.setFillColor(sf::Color::White);
                }
                window.draw(queen);
            }
        }
    }
}

int main() {
    int n;

    std::cout << "Ingrese el numero de reinas: ";
    std::cin >> n;

    const unsigned int FPS = 60;
    std::vector<std::vector<std::string>> solutions = findNQueensSolutions(n);

    if (solutions.empty()) {
        std::cout << "No hay soluciones para " << n << " reinas." << std::endl;
        return 0;
    }

    int tileSize = 40;
    const int windowWidth = clamp(100 + n * tileSize, 400, 800);
    const int windowHeight = clamp(200 + n * tileSize, 450, 800);
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Problema de las n-reinas");
    window.setFramerateLimit(FPS);

    // Cargar texturas
    sf::Texture tex_reina;
    tex_reina.loadFromFile("img/queen.png");

    // Cargar fuentes
    sf::Font ft_arial;
    ft_arial.loadFromFile("fonts/Arial.ttf");

    // Controles
    sf::Text labelTitulo("N. reinas: " + std::to_string(n), ft_arial, 20);
    labelTitulo.setFillColor(sf::Color::White);
    labelTitulo.setPosition(50, 50);

    sf::Text labelSolucionActual("Solucion 0/0 " + std::to_string(n), ft_arial, 20);
    labelSolucionActual.setFillColor(sf::Color::White);
    labelSolucionActual.setPosition(50.f, windowHeight - 50);
    RectButton buttonPrevious(ft_arial, sf::Vector2f(40.f, 25.f), sf::Vector2f(250.f, windowHeight - 50));
    buttonPrevious.setButtonLabel(20.f, " < ");
    RectButton buttonNext(ft_arial, sf::Vector2f(40.f, 25.f), sf::Vector2f(300.f, windowHeight - 50));
    buttonNext.setButtonLabel(20.f, " > ");

    size_t solutionIndex = 0;

    sf::Event event;
    actualizarIndiceSolucion(labelSolucionActual, solutionIndex, solutions, n);
    while (window.isOpen()) {
        // Step
        while (window.pollEvent(event)) {
            buttonNext.getButtonStatus(window, event);
            buttonPrevious.getButtonStatus(window, event);
            if (buttonNext.isPressed) {
                solutionIndex = (solutionIndex + 1) % solutions.size();
                actualizarIndiceSolucion(labelSolucionActual, solutionIndex, solutions, n);
            }
            if (buttonPrevious.isPressed) {
                solutionIndex = (solutionIndex - 1 + solutions.size()) % solutions.size();
                actualizarIndiceSolucion(labelSolucionActual, solutionIndex, solutions, n);
            }

            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Dibujar
        window.clear();
        // - Textos
        window.draw(labelTitulo);
        window.draw(labelSolucionActual);
        // - Botones
        buttonPrevious.draw(window);
        buttonNext.draw(window);
        // - Tablero
        drawBoardAndQueens(window, solutions[solutionIndex], tileSize, tex_reina);

        window.display();
    }

    return 0;
}

void actualizarIndiceSolucion(sf::Text &labelSolucionActual, size_t solutionIndex, std::vector<std::vector<std::string>> &solutions, int n) {
    labelSolucionActual.setString("Solucion " + std::to_string(solutionIndex + 1) + "/" + std::to_string(solutions.size()));
}
