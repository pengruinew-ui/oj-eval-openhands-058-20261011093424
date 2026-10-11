#include "src.hpp"
#include <iostream>

int main() {
    try {
        Pokedex pokedex("debug.dat");
        
        // Test very basic functionality
        bool result = pokedex.pokeAdd("Test", 1, "normal");
        std::cout << "Add result: " << result << std::endl;
        
        std::string name = pokedex.pokeFind(1);
        std::cout << "Find result: " << name << std::endl;
        
        // Test attack
        float damage = pokedex.attack("normal", 1);
        std::cout << "Attack result: " << damage << std::endl;
        
        // Test iterator
        int count = 0;
        for (auto it = pokedex.begin(); it != pokedex.end(); ++it) {
            count++;
            std::cout << "Iterator: " << it->name << std::endl;
        }
        std::cout << "Iterator count: " << count << std::endl;
        
    } catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    return 0;
}