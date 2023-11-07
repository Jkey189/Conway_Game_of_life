#include <SFML/Graphics.hpp>

#define SIZE 100

int NeighboursOfCell(bool matrix[SIZE][SIZE], int width, int height, int cell_x, int cell_y) {
  int number_of_neighbours = 0;

  for (int i = cell_y - 1; i <= cell_y + 1; ++i) {
      for (int j = cell_x - 1; j <= cell_x + 1; ++j) {
          if (!(i == cell_y && j == cell_x)) {
              int temp_x = (j + width) % width, temp_y = (i + height) % height;

              if (matrix[temp_y][temp_x]) {
                  ++number_of_neighbours;
              }
          }
      }
  }

  return number_of_neighbours;
}

void GameStep(bool matrix[SIZE][SIZE], int width, int height) {
  bool next_matrix[SIZE][SIZE];

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      int number_of_neighbours = NeighboursOfCell(matrix, width, height, j, i);

      if ((matrix[i][j] == false && number_of_neighbours == 3) ||
        (matrix[i][j] == true && number_of_neighbours < 2) ||
        (matrix[i][j] == true && number_of_neighbours > 3)) {
        next_matrix[i][j] = !matrix[i][j];
      }
      else {
        next_matrix[i][j] = matrix[i][j];
      }
    }
  }

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      matrix[i][j] = next_matrix[i][j];
    }
  }
}

void DrawInWindow(sf::RenderWindow& window, bool matrix[SIZE][SIZE], int width, int height, int cell_size, int cell_grind, int ui_height, bool is_play) {
  window.clear(sf::Color(0, 0, 0));

  sf::RectangleShape background(sf::Vector2f(width * cell_size + cell_grind * (width + 1), height * cell_size + cell_grind * (height + 1)));
  background.setPosition(sf::Vector2f(0, ui_height));
  background.setFillColor(sf::Color(80, 80, 80));
  window.draw(background);

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      sf::RectangleShape cell(sf::Vector2f(cell_size, cell_size));
      cell.setPosition(sf::Vector2f(j * cell_size + (j + 1) * cell_grind + 0,
                                    i * cell_size + (i + 1) * cell_grind + ui_height));

      if (matrix[i][j]) {
         cell.setFillColor(sf::Color(0, 120, 9));
      }
      else {
        cell.setFillColor(sf::Color(0, 0, 0));
      }

      window.draw(cell);
    }
  }

  sf::Font font;
  font.loadFromFile("undertale.otf");

  sf::Text text;
  text.setFont(font);

  if (is_play) {
      text.setString("Space - pause/play   Delete - clear   F/G - chill    <-/-> - speed    LBM - draw / RBM - erase   process - play");
  }
  else {
      text.setString("Space - pause/play   Delete - clear   F/G - chill    <-/-> - speed    LBM - draw / RBM - erase   process - pause");
  }
  text.setCharacterSize(30);

  window.draw(text);
  window.display();
}

bool IsColliedCell(int mouse_x, int mouse_y, int width, int height, int cell_size, int cell_grind, int cell_coords[2], int ui_height) {
  int window_width = width * cell_size + cell_grind * (width + 1) + 0 * 2;
  int window_height = height * cell_size + cell_grind * (height + 1) + ui_height * 2;

  if (mouse_x >= window_width - 0 - cell_grind || mouse_y >= window_height - ui_height - cell_grind) {
    return false;
  }
  for (int i = 0, j = 0; j <= width; ++j, i = j * cell_size + cell_grind * j + 0) {
    if (i > mouse_x) {
      if (mouse_x >= i - cell_size) {
        cell_coords[0] = j - 1;
        break;
      }
      
      return false;
    }
  }

  for (int i = 0, j = 0; j <= height; ++j, i = j * cell_size + cell_grind * j + ui_height) {
    if (i > mouse_y) {
      if (mouse_y >= i - cell_size) {
        cell_coords[1] = j - 1;
        break;
      }

      return false;
    }
  }

  return true;
}

int main() {
  int width = 90, height = 40, cell_size = 20, cell_grind = 1, ui_height = 100;
  int fps_values[5]{1, 5, 10, 30, 60}, fps_index = 2;
  bool matrix[SIZE][SIZE] = { {0} };
  bool is_play = false;
  sf::Style::Close;
  sf::RenderWindow window;

  sf::VideoMode window_size(width * cell_size + cell_grind * (width + 1) + 0 * 2, height * cell_size + cell_grind * (height + 1) + ui_height * 2);
  window.create(window_size, "Game of Life", sf::Style::Close);

  while (window.isOpen()) {
    bool LBM_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    bool RBM_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Right);

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      else if (!is_play && (LBM_pressed || RBM_pressed)) {
        int mouse_x = -1, mouse_y = -1, cell_coords[2];
        if (event.type == sf::Event::MouseMoved) {
          mouse_x = event.mouseMove.x;
          mouse_y = event.mouseMove.y;
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
          mouse_x = event.mouseButton.x;
          mouse_y = event.mouseButton.y;
        }

        if (!is_play && IsColliedCell(mouse_x, mouse_y, width, height, cell_size, cell_grind, cell_coords, ui_height)) {
          int cell_x = cell_coords[0];
          int cell_y = cell_coords[1];

          if (LBM_pressed) {
            matrix[cell_y][cell_x] = true;
          }
          else if (RBM_pressed) {
            matrix[cell_y][cell_x] = false;
          }
        }
      }
      else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
          is_play = !is_play;
        }
        else if (!is_play && event.key.code == sf::Keyboard::Delete) {
          for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
              matrix[i][j] = false;
            }
          }
        }
        else if (!is_play && event.key.code == sf::Keyboard::F) {
          for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
              matrix[i][j] = false;
            }
          }

          for (int i = 0; i < height; i +=2) {
            for (int j = 0; j < width; ++j) {
              matrix[i][j] = true;
            }
          }
        }
        else if (!is_play && event.key.code == sf::Keyboard::G) {
          for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
              matrix[i][j] = false;
            }
          }
          for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; j += 2) {
              matrix[i][j] = true;
            }
          }
        }
        else if (event.key.code == sf::Keyboard::Left) {
          if (fps_index > 0) {
            --fps_index;
          }
        }
        else if (event.key.code == sf::Keyboard::Right) {
          if (fps_index < 4) {
            ++fps_index;
          }
        }
      }
    }

    if (is_play) {
      GameStep(matrix, width, height);
    }

    DrawInWindow(window, matrix, width, height, cell_size, cell_grind, ui_height, is_play);

    if (is_play) {
      sleep(sf::milliseconds(1000.0 / fps_values[fps_index]));
    }
  }

  return 0;
}
