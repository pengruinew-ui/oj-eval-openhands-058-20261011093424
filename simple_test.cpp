#include "src.hpp"
#include <iostream>

int main() {
    // Just test constructor and destructor
    {
        Pokedex pokedex("simple_test.dat");
        std::cout << "Pokedex created successfully" << std::endl;
    }
    std::cout << "Pokedex destroyed successfully" << std::endl;
    
    // Test creating again (should load from file)
    {
        Pokedex pokedex("simple_test.dat");
        std::cout << "Pokedex created and loaded successfully" << std::endl;
    }
    
    return 0;
}