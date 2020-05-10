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

        explicit Spaceship(std::string_view enginePart,
                           std::string_view fuselagePart,
                           std::string_view cabinPart,
                           std::string_view armorPart,
                           std::optional<std::string_view> largeWingsPart,
                           std::optional<std::string_view> smallWingsPart,
                           const std::vector<std::string_view> &weaponParts) : engine_(enginePart), fuselage_(fuselagePart),
                                                                          cabin_(cabinPart),
                                                                          large_wings_(largeWingsPart),
                                                                          small_wings_(smallWingsPart),
                                                                          armor_(armorPart)
        {
            const auto sz = std::min(MAX_WEAPON, weaponParts.size());
            std::transform(weaponParts.end() - sz, weaponParts.end(), std::back_inserter(weapons_), [](const auto sv){ return std::string(sv); });
        }

        void Print() {
            std::clog << "Engine: " << engine_ << ", " <<
                "Fuselage: " << fuselage_ << ", " <<
                "Cabin: " << cabin_ << ", " <<
                "Wings: " << "Large " << (large_wings_ ? *large_wings_ : "None") << ", " <<
                             "Small " << (small_wings_ ? *small_wings_ : "None") << ", " <<
                "Armor: " << armor_;
            std::copy(weapons_.begin(), weapons_.end(), std::ostream_iterator<std::string>(std::clog, ", "));
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
        std::uniform_int_distribution<std::size_t> ud{range_begin, range_end - 1};
        return ud(gen);
    }

    std::size_t get_random_number(const std::vector<std::string_view> &v) {
        return get_random_number(v.size());
    }

    std::vector<std::string_view> get_shuffled_array(const std::vector<std::string_view>& arr){
        std::random_device rd;
        std::mt19937 g(rd());
        auto shuff(arr);
        std::shuffle(shuff.begin(), shuff.end(), g);
        return shuff;
    }

    using vec_of_wing = std::vector<std::string_view>;
    std::pair<vec_of_wing, vec_of_wing> get_wings(const std::vector<std::string_view>& wings) {
        vec_of_wing small, large;
        auto is_small([](const auto &s){
            constexpr auto sep = ' ';
            auto pos = s.find(sep);
            const auto pos_next = ++pos;
            return s[pos_next] == 's';
        });
        auto is_large([](const auto &s){
          constexpr auto sep = ' ';
          auto pos = s.find(sep);
          const auto pos_next = ++pos;
          return s[pos_next] == 'l';
        });
        (void)std::copy_if(begin(wings), end(wings), std::back_inserter(small), is_small);
        (void)std::copy_if(begin(wings), end(wings), std::back_inserter(large), is_large);
        return {small, large};
    }

    auto get_find_func(const std::string_view& to_find) {
        return [to_find](const std::string_view& s){ return s.find(to_find) != std::string::npos; };
    }

    struct Comp
    {
        static std::string_view get(const std::string_view& s) {
            constexpr auto sep = ' ';
            const auto pos = s.find(sep);
            const auto str = s.substr(0, pos);
            return str;
        }
        bool operator() ( const std::string_view& s, const char* const to_find ) const {
            const auto str = get(s);
            return str < to_find;
        }
        bool operator() ( const char* const to_find, const std::string_view& s ) const {
            const auto str = get(s);
            return str > to_find;
        }
    };

    template <typename C, typename ITER>
    bool not_found(const C &c, const ITER iter) {
        return iter == c.end();
    }


    std::vector<std::string_view> get_parts(const std::vector<std::string> &allParts, const char* const part) {
        auto [start, end] = equal_range(allParts.begin(), allParts.end(), part, Comp{});
        if (not_found(allParts, start)) {
            throw std::invalid_argument("cannot find engine");
        }
        return std::vector<std::string_view>(start, end);
    }

    Spaceship GenerateShip(const std::vector<std::string> &allParts_in) {
        std::vector<std::string> allParts(allParts_in);
        std::sort(allParts.begin(), allParts.end());

        std::vector<std::string_view> engineParts(get_parts(allParts, "engine"));
        std::vector<std::string_view> fuselageParts(get_parts(allParts, "fuselage"));
        std::vector<std::string_view> cabinParts(get_parts(allParts, "cabin"));
        std::vector<std::string_view> wingsParts(get_parts(allParts, "wings"));
        std::vector<std::string_view> armorParts(get_parts(allParts, "armor"));
        std::vector<std::string_view> weaponParts(get_parts(allParts, "weapon"));

        const auto engine_idx = get_random_number(engineParts);
        const auto fusel_idx = get_random_number(fuselageParts);
        const auto cabin_idx = get_random_number(cabinParts);
        const auto armor_idx = get_random_number(armorParts);
        const auto weapons_array = get_shuffled_array(weaponParts);


        const auto [small_wings,large_wings] = get_wings(wingsParts);

        auto f([](const auto& v) -> std::optional<std::string_view> { if(v.empty()) return {}; else return v[get_random_number(v)]; });
        const auto small_wings_in = f(small_wings);
        const auto large_wings_in = f(large_wings);
        return Spaceship(engineParts[engine_idx],
                         fuselageParts[fusel_idx],
                         cabinParts[cabin_idx],
                         armorParts[armor_idx],
                         large_wings_in,
                         small_wings_in,
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
            if(line.empty()) {
                continue;
            }
            allParts.push_back(line);
        }
    }

    space::Spaceship sp = space::GenerateShip(allParts);
    sp.Print();
}
