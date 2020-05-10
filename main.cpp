#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include <iterator>

namespace space {

    class Spaceship final {
    public:
        explicit Spaceship(std::string enginePart,
                           std::string fuselagePart,
                           std::string cabinPart,
                           std::string armorPart,
                           std::optional<std::string> largeWingsPart,
                           std::optional<std::string> smallWingsPart,
                           const std::vector<std::string> &weaponParts) : engine_(std::move(enginePart)), fuselage_(std::move(fuselagePart)),
                                                                          cabin_(std::move(cabinPart)),
                                                                          large_wings_(std::move(largeWingsPart)),
                                                                          small_wings_(std::move(smallWingsPart)),
                                                                          armor_(std::move(armorPart))
        {
            const auto sz = std::min(MAX_WEAPON, weaponParts.size());
            std::copy_n(weaponParts.begin(), sz, std::back_inserter(weapons_));
        }
        void Print() {
            std::clog << "Engine: " << engine_ << ", " <<
                "Fuselage: " << fuselage_ << ", " <<
                "Cabin: " << cabin_ << ", " <<
                "Wings: " << "Large " << (large_wings_ ? *large_wings_ : "None") << ", " <<
                          "Small " << (small_wings_ ? *small_wings_ : "None") << ", " <<
                "Armor: " << armor_;
            std::copy(weapons_.begin(), weapons_.end(), std::ostream_iterator<std::string>(std::clog, ","));
        }

    private:
        std::string engine_;
        std::string fuselage_;
        std::string cabin_;
        std::optional<std::string> large_wings_;// optional
        std::optional<std::string> small_wings_;// optional
        std::string armor_;
        static constexpr std::size_t MAX_WEAPON = 4U;
        std::vector<std::string> weapons_;
    };

    std::size_t get_random_number(const std::size_t range_end) {
        std::random_device rd{};// to obtain a seed for the random number engine
        std::mt19937 gen{rd()}; // Standard mersenne_twister_engine seeded with rd()
        constexpr auto range_begin = 0U;
        std::uniform_int_distribution<std::size_t> ud{range_begin, range_end};
        return ud(gen);
    }

    auto get_random_number(const std::vector<std::string> &v) {
        return get_random_number(v.size());
    }

    std::vector<std::string> get_shuffled_array(const std::vector<std::string>& weaponParts){
        std::random_device rd;
        std::mt19937 g(rd());
        auto shuff(weaponParts);
        std::shuffle(shuff.begin(), shuff.end(), g);
        return shuff;
    }

    enum class wings_type { SMALL, LARGE };

    std::optional<std::string> get_wings(const std::vector<std::string>& wings, const wings_type wt) {
        return {};
    }

    Spaceship GenerateShip(const std::vector<std::string> &allParts) {
        std::vector<std::string> engineParts;
        std::vector<std::string> fuselageParts;
        std::vector<std::string> cabinParts;
        std::vector<std::string> wingsParts;
        std::vector<std::string> armorParts;
        std::vector<std::string> weaponParts;

        for (const auto &str : allParts) {
            if (str.rfind("engine") != std::string::npos)
                engineParts.push_back(str);
            else if (str.rfind("fuselage") != std::string::npos)
                fuselageParts.push_back(str);
            else if (str.rfind("cabin") != std::string::npos)
                cabinParts.push_back(str);
            else if (str.rfind("wings") != std::string::npos)
                wingsParts.push_back(str);
            else if (str.rfind("armor") != std::string::npos)
                armorParts.push_back(str);
            else if (str.rfind("weapon") != std::string::npos)
                weaponParts.push_back(str);
        }

        const auto engine_idx = get_random_number(engineParts);
        const auto fusel_idx = get_random_number(fuselageParts);
        const auto cabin_idx = get_random_number(cabinParts);
        const auto armor_idx = get_random_number(armorParts);
        const auto weapons_array = get_shuffled_array(weaponParts);
        const auto large_wings = get_wings(wingsParts, wings_type::LARGE);
        const auto small_wings = get_wings(wingsParts, wings_type::SMALL);
        return Spaceship(engineParts[engine_idx],
                         fuselageParts[fusel_idx],
                         cabinParts[cabin_idx],
                         armorParts[armor_idx],
                         large_wings,
                         small_wings,
                         weaponParts);
    }

}// namespace space

int main(int argc, char *argv[]) {
    std::string partsFileName = "vehicle_parts.txt";
    if (argc > 1) {
        partsFileName = argv[1];
    }

    std::cout << "parts loaded from: " << partsFileName << '\n';

    std::ifstream file(partsFileName);
    std::vector<std::string> allParts;
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            allParts.push_back(line);
        }
        file.close();
    }

    space::Spaceship sp = space::GenerateShip(allParts);
    sp.Print();
}
