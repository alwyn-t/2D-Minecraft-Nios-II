#include <iostream>

int main() {
  bool exit = false;
  int R, G, B;
  int hex_output;
  while (!exit) {
    std::cout << "24 bit RGB to 16 bit RGB converter" << std::endl;
    std::cout << " Please enter 8 bit red colour:   ";
    std::cin >> R;
    std::cout << " Please enter 8 bit green colour: ";
    std::cin >> G;
    std::cout << " Please enter 8 bit blue colour:  ";
    std::cin >> B;
    R = R >> 3;
    G = G >> 2;
    B = B >> 3;
    hex_output = (R << 11) + (G << 5) + B;
    std::cout << "16 bit colour: " << R << ' ' << G << ' ' << B << std::endl;
    std::cout << "hex code: " << std::hex << hex_output << std::endl;
  }
  return 0;
}
