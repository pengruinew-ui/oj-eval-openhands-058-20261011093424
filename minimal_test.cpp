#include "src.hpp"
#include <iostream>

int main() {
    // Test the absolute basics
    Pokedex pokedex("minimal.dat");
    
    // Test adding a Pokemon
    bool addResult = pokedex.pokeAdd("Testmon", 100, "normal");
    std::cout << "Add: " << addResult << std::endl;
    
    // Test finding by ID
    std::string findResult = pokedex.pokeFind(100);
    std::cout << "Find: " << findResult << std::endl;
    
    // Test finding non-existent
    std::string findNone = pokedex.pokeFind(999);
    std::cout << "Find none: " << findNone << std::endl;
    
    // Test deletion
    bool delResult = pokedex.pokeDel(100);
    std::cout << "Delete: " << delResult << std::endl;
    
    // Test finding after deletion
    std::string findAfterDel = pokedex.pokeFind(100);
    std::cout << "Find after delete: " << findAfterDel << std::endl;
    
    return 0;
}