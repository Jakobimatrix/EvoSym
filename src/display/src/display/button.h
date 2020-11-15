#ifndef BUTTON
#define BUTTON

#include <SFML/Graphics.hpp>
#include <chrono>
#include <globals/globals.hpp>
#include <iostream>
#include <string>

class Button {
 public:
  /*!
   * \brief: Constructor: Sets colour, shape and position.
   * \param[in] Rshape: A sfml::Rectangle containing position and size of the
   *                    Button.
   * \param[in] button_color: The color for the button.
   * * \param[in] hoover_color: The color when hovering over the button.
   */
  Button(const sf::RectangleShape& Rshape,
         const sf::Color& button_color = sf::Color(200, 200, 200),
         const sf::Color& hoover_color = sf::Color(250, 250, 250))
      : shape(Rshape), button_color(button_color), hoover_color(hoover_color) {
    last_click = std::chrono::system_clock::now();
  }

  /*!
   * \brief: Calculates if a mouse hovers over the button.
   * \param[in] sf::Vector2i& mousePosition: The mouse position.
   * \return bool: returns true if the given mouseposition is over the button.
   */
  bool hover(sf::Vector2i& mouse_position) {
    if (isOver(mouse_position)) {
      shape.setFillColor(sf::Color(button_color));
      return true;
    } else {
      shape.setFillColor(sf::Color(hoover_color));
      return false;
    }
  }

  /*!
   * \brief: Calculates if the mouse is over the button
   * \param[in] mousePosition: The mouse position.
   * \return bool: returns true if the given mouseposition is over the button.
   */
  bool isOver(sf::Vector2i& mouse_position) {
    if (shape.getPosition().x < mouse_position.x &&
        mouse_position.x < (shape.getPosition().x + shape.getSize().x)) {
      if (shape.getPosition().y < mouse_position.y &&
          mouse_position.y < (shape.getPosition().y + shape.getSize().y)) {
        return true;
      }
    }
    return false;
  }

  /*!
   * \brief: Calculates if a mouse has clicked this button. (using "debouncing")
   * \param[in] mousePosition: The mouse position.
   * \return bool: returns true if the given mouseposition is over the button and this function wasnt called within minDiffBetweenClicks twice or more.
   */
  bool click(sf::Vector2i& mouse_position) {
    if (std::chrono::system_clock::now() - last_click > MIN_DIFFERENCE_BETWEEN_CKLICKS) {
      if (isOver(mouse_position)) {
        last_click = std::chrono::system_clock::now();
        return true;
      } else {
        return false;
      }
    }
    return false;
  }

  /*!
   * \brief: Sets the Text for the button.
   * \param[in] text_string: The text.
   * \param[in] font_size: The text size.
   * \param[in] text_color: The text color.
   */
  void setText(std::string& text_string,
               unsigned int font_size,
               const sf::Color& text_color = sf::Color(0, 0, 0)) {
    text.setString(text_string);
    text.setPosition(shape.getPosition().x + shape.getSize().x * 0.5f,
                     shape.getPosition().y + shape.getSize().y * 0.5f);
    text.setFont(Globals::getInstance().getFont());
    text.setFillColor(text_color);
    text.setCharacterSize(font_size);
    text.setOrigin(text.getLocalBounds().width * 0.5f, text.getLocalBounds().height);
  }

  /*!
   * \brief: resizes the Button.
   * \param[in] scale: if smaller 1 it gets smaller, if bigger 1 it gets bigger.
   */
  void resize(float scale) {
    const sf::Vector2f ratio = sf::Vector2f(scale, scale);
    text.setScale(ratio);
    shape.setScale(ratio);
  }

 private:
  std::chrono::time_point<std::chrono::system_clock> last_click;
  sf::RectangleShape shape;
  sf::Text text;           // the text on the button
  sf::Color button_color;  // the color ot the button
  sf::Color hoover_color;  // the color ot the button

  static constexpr std::chrono::milliseconds MIN_DIFFERENCE_BETWEEN_CKLICKS =
      std::chrono::milliseconds(150);
};
#endif  // !_BUTTON_
