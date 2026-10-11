#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstring>



class BasicException {
protected:
    const char *message;

public:
    explicit BasicException(const char *_message) {
        message = _message;
    }

    virtual const char *what() const {
        return message;
    }
};

class ArgumentException: public BasicException {
public:
    explicit ArgumentException(const char *_message) : BasicException(_message) {}
    
    static std::string nameErrorMsg;
    static std::string typeErrorMsg;
};

class IteratorException: public BasicException {
public:
    explicit IteratorException(const char *_message) : BasicException(_message) {}
};

// Define static members
std::string ArgumentException::nameErrorMsg;
std::string ArgumentException::typeErrorMsg;

struct Pokemon {
    char name[12];
    int id;
    std::vector<std::string> types;
    
    Pokemon() : id(0) {
        memset(name, 0, sizeof(name));
    }
    
    Pokemon(const char *_name, int _id, const std::vector<std::string>& _types) : id(_id), types(_types) {
        strncpy(name, _name, 11);
        name[11] = '\0';
    }
};

class Pokedex {
private:
    std::map<int, Pokemon> pokemonMap;
    std::string fileName;
    
    // Type effectiveness matrix
    std::map<std::string, std::map<std::string, float>> typeChart;
    
    void initializeTypeChart() {
        // Initialize all to 1.0 (neutral)
        std::vector<std::string> types = {"normal", "fire", "water", "grass", "electric", "ice", "fighting"};
        for (const auto& attacker : types) {
            for (const auto& defender : types) {
                typeChart[attacker][defender] = 1.0f;
            }
        }
        
        // Set up effectiveness based on the problem description
        // Fire is super effective against grass
        typeChart["fire"]["grass"] = 2.0f;
        // Fire is not very effective against fire and water
        typeChart["fire"]["fire"] = 0.5f;
        typeChart["fire"]["water"] = 0.5f;
        
        // Water is super effective against fire
        typeChart["water"]["fire"] = 2.0f;
        // Water is not very effective against water and grass
        typeChart["water"]["water"] = 0.5f;
        typeChart["water"]["grass"] = 0.5f;
        
        // Grass is super effective against water
        typeChart["grass"]["water"] = 2.0f;
        // Grass is not very effective against fire and grass
        typeChart["grass"]["fire"] = 0.5f;
        typeChart["grass"]["grass"] = 0.5f;
        
        // Electric is super effective against water
        typeChart["electric"]["water"] = 2.0f;
        // Electric is not very effective against electric and grass
        typeChart["electric"]["electric"] = 0.5f;
        typeChart["electric"]["grass"] = 0.5f;
        // Electric has no effect against ground (but ground is not in our type list)
        
        // Ice is super effective against grass
        typeChart["ice"]["grass"] = 2.0f;
        // Ice is not very effective against fire and ice
        typeChart["ice"]["fire"] = 0.5f;
        typeChart["ice"]["ice"] = 0.5f;
        
        // Fighting is super effective against normal and ice
        typeChart["fighting"]["normal"] = 2.0f;
        typeChart["fighting"]["ice"] = 2.0f;
        // Fighting is not very effective against fighting
        typeChart["fighting"]["fighting"] = 0.5f;
    }
    
    void validatePokemonName(const char *name) {
        if (!name || strlen(name) == 0) {
            throw ArgumentException("Argument Error: PM Name Invalid ()");
        }
        
        for (int i = 0; name[i] != '\0'; i++) {
            if (!((name[i] >= 'a' && name[i] <= 'z') || (name[i] >= 'A' && name[i] <= 'Z'))) {
                ArgumentException::nameErrorMsg = "Argument Error: PM Name Invalid (";
                ArgumentException::nameErrorMsg += name;
                ArgumentException::nameErrorMsg += ")";
                throw ArgumentException(ArgumentException::nameErrorMsg.c_str());
            }
        }
    }
    
    void validatePokemonTypes(const char *types) const {
        if (!types || strlen(types) == 0) {
            throw ArgumentException("Argument Error: PM Type Invalid ()");
        }
        
        std::set<std::string> validTypes = {"normal", "fire", "water", "grass", "electric", "ice", "fighting"};
        std::istringstream iss(types);
        std::string type;
        
        while (std::getline(iss, type, '#')) {
            if (type.empty()) continue;
            
            if (validTypes.find(type) == validTypes.end()) {
                ArgumentException::typeErrorMsg = "Argument Error: PM Type Invalid (";
                ArgumentException::typeErrorMsg += type;
                ArgumentException::typeErrorMsg += ")";
                throw ArgumentException(ArgumentException::typeErrorMsg.c_str());
            }
        }
    }
    
    std::vector<std::string> parseTypes(const char *types) {
        std::vector<std::string> result;
        std::istringstream iss(types);
        std::string type;
        
        while (std::getline(iss, type, '#')) {
            if (!type.empty()) {
                result.push_back(type);
            }
        }
        
        return result;
    }
    
    void saveToFile() {
        std::ofstream file(fileName);
        if (!file.is_open()) return;
        
        for (const auto& pair : pokemonMap) {
            const Pokemon& pokemon = pair.second;
            file << pokemon.name << " " << pokemon.id;
            for (size_t i = 0; i < pokemon.types.size(); i++) {
                if (i == 0) file << " ";
                else file << "#";
                file << pokemon.types[i];
            }
            file << std::endl;
        }
        
        file.close();
    }
    
    void loadFromFile() {
        std::ifstream file(fileName);
        if (!file.is_open()) return;
        
        pokemonMap.clear();
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            try {
                std::istringstream iss(line);
                std::string name, typesStr;
                int id;
                
                if (!(iss >> name >> id)) continue;
                
                // Read the rest of the line as types
                std::getline(iss, typesStr);
                if (!typesStr.empty() && typesStr[0] == ' ') {
                    typesStr = typesStr.substr(1);
                }
                
                validatePokemonName(name.c_str());
                validatePokemonTypes(typesStr.c_str());
                
                std::vector<std::string> types = parseTypes(typesStr.c_str());
                pokemonMap[id] = Pokemon(name.c_str(), id, types);
            } catch (ArgumentException& e) {
                // Skip invalid lines during loading
                continue;
            }
        }
        
        file.close();
    }

public:
    explicit Pokedex(const char *_fileName) : fileName(_fileName) {
        initializeTypeChart();
        loadFromFile();
    }

    ~Pokedex() {
        saveToFile();
    }

    bool pokeAdd(const char *name, int id, const char *types) {
        try {
            validatePokemonName(name);
            validatePokemonTypes(types);
            
            if (pokemonMap.find(id) != pokemonMap.end()) {
                return false;
            }
            
            // Check if name already exists
            for (const auto& pair : pokemonMap) {
                if (strcmp(pair.second.name, name) == 0) {
                    return false;
                }
            }
            
            std::vector<std::string> typeList = parseTypes(types);
            pokemonMap[id] = Pokemon(name, id, typeList);
            return true;
            
        } catch (ArgumentException& e) {
            throw;
        }
    }

    bool pokeDel(int id) {
        auto it = pokemonMap.find(id);
        if (it != pokemonMap.end()) {
            pokemonMap.erase(it);
            return true;
        }
        return false;
    }

    std::string pokeFind(int id) const {
        auto it = pokemonMap.find(id);
        if (it != pokemonMap.end()) {
            return std::string(it->second.name);
        }
        return "None";
    }

    std::string typeFind(const char *types) const {
        try {
            validatePokemonTypes(types);
            
            std::set<std::string> searchTypes;
            std::istringstream iss(types);
            std::string type;
            
            while (std::getline(iss, type, '#')) {
                if (!type.empty()) {
                    searchTypes.insert(type);
                }
            }
            
            std::vector<const Pokemon*> results;
            
            for (const auto& pair : pokemonMap) {
                const Pokemon& pokemon = pair.second;
                
                bool hasAllTypes = true;
                for (const std::string& searchType : searchTypes) {
                    bool found = false;
                    for (const std::string& pokemonType : pokemon.types) {
                        if (pokemonType == searchType) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        hasAllTypes = false;
                        break;
                    }
                }
                
                if (hasAllTypes) {
                    results.push_back(&pokemon);
                }
            }
            
            if (results.empty()) {
                return "None";
            }
            
            // Sort by ID
            std::sort(results.begin(), results.end(), 
                     [](const Pokemon* a, const Pokemon* b) { return a->id < b->id; });
            
            std::string result = std::to_string(results.size());
            for (const Pokemon* pokemon : results) {
                result += "\n" + std::string(pokemon->name);
            }
            
            return result;
            
        } catch (ArgumentException& e) {
            throw;
        }
    }

    float attack(const char *type, int id) const {
        auto it = pokemonMap.find(id);
        if (it == pokemonMap.end()) {
            return -1.0f;
        }
        
        const Pokemon& pokemon = it->second;
        float totalDamage = 1.0f;
        
        for (const std::string& defenderType : pokemon.types) {
            auto attackerIt = typeChart.find(type);
            if (attackerIt != typeChart.end()) {
                auto defenderIt = attackerIt->second.find(defenderType);
                if (defenderIt != attackerIt->second.end()) {
                    totalDamage *= defenderIt->second;
                }
            }
        }
        
        return totalDamage;
    }

    int catchTry() const {
        if (pokemonMap.empty()) {
            return 0;
        }
        
        // Start with the smallest ID Pokemon
        auto it = pokemonMap.begin();
        int firstId = it->first;
        
        std::set<int> ownedPokemon;
        ownedPokemon.insert(firstId);
        
        bool newPokemonCaught = true;
        while (newPokemonCaught) {
            newPokemonCaught = false;
            
            for (const auto& pair : pokemonMap) {
                int targetId = pair.first;
                const Pokemon& targetPokemon = pair.second;
                
                // Skip if we already own this Pokemon
                if (ownedPokemon.find(targetId) != ownedPokemon.end()) {
                    continue;
                }
                
                // Check if any owned Pokemon can catch this one
                for (int ownedId : ownedPokemon) {
                    const Pokemon& ownedPoke = pokemonMap.at(ownedId);
                    
                    // Try each type the owned Pokemon has
                    for (const std::string& attackType : ownedPoke.types) {
                        float damage = attack(attackType.c_str(), targetId);
                        if (damage >= 2.0f) {
                            ownedPokemon.insert(targetId);
                            newPokemonCaught = true;
                            break;
                        }
                    }
                    
                    if (newPokemonCaught) break;
                }
            }
        }
        
        return ownedPokemon.size();
    }

    struct iterator {
    private:
        std::map<int, Pokemon>* container;
        std::map<int, Pokemon>::iterator internalIterator;
        
    public:
        iterator() : container(nullptr) {}
        
        iterator(std::map<int, Pokemon>* _container, std::map<int, Pokemon>::iterator _it) 
            : container(_container), internalIterator(_it) {}
        
        iterator &operator++() {
            if (container && internalIterator != container->end()) {
                ++internalIterator;
            }
            return *this;
        }
        
        iterator &operator--() {
            if (container) {
                if (internalIterator == container->begin()) {
                    throw IteratorException("Iterator Error: Decrement begin()");
                }
                --internalIterator;
            }
            return *this;
        }
        
        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }
        
        iterator operator--(int) {
            iterator temp = *this;
            --(*this);
            return temp;
        }
        
        iterator & operator = (const iterator &rhs) {
            container = rhs.container;
            internalIterator = rhs.internalIterator;
            return *this;
        }
        
        bool operator == (const iterator &rhs) const {
            return internalIterator == rhs.internalIterator;
        }
        
        bool operator != (const iterator &rhs) const {
            return internalIterator != rhs.internalIterator;
        }
        
        Pokemon & operator*() const {
            if (container && internalIterator != container->end()) {
                return internalIterator->second;
            }
            throw IteratorException("Iterator Error: Invalid dereference");
        }
        
        Pokemon *operator->() const {
            if (container && internalIterator != container->end()) {
                return &(internalIterator->second);
            }
            throw IteratorException("Iterator Error: Invalid dereference");
        }
    };

    iterator begin() {
        return iterator(&pokemonMap, pokemonMap.begin());
    }

    iterator end() {
        return iterator(&pokemonMap, pokemonMap.end());
    }
};