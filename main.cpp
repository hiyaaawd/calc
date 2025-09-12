#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cmath>

int main() {
    // Planet definitions
    struct PlanetData {
        float gravity;      
        float air_thickness; // atm (Earth = 1.0)
    };

    std::map<std::string, PlanetData> planet_map = {
        {"Mercury", {3.7f, 0.00000000003f}},
        {"Venus",   {8.87f, 92.0f}},
        {"Earth",   {9.8f, 1.0f}},
        {"Mars",    {3.71f, 0.006f}},
        {"Jupiter", {24.79f, 0.1f}},
        {"Saturn",  {10.44f, 0.0001f}},
        {"Uranus",  {8.87f, 0.00001f}},
        {"Neptune", {11.15f, 0.00001f}}
    };

    std::vector<std::string> planets = {
        "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune"
    };

    std::string selplanet = "Earth";
    std::cout << "Planets: ";
    for (const auto& planet : planets) std::cout << planet << " ";
    std::cout << "\nSelect a planet: ";
    std::cin >> selplanet;

    // Default values
    float gravity_value = 9.8f;
    float air_thickness_value = 1.0f;

    if (planet_map.count(selplanet)) {
        gravity_value = planet_map[selplanet].gravity;
        air_thickness_value = planet_map[selplanet].air_thickness;
    }

    float air_factor = (air_thickness_value > 0.0f) ? 1.0f / air_thickness_value : 2.0f;

    std::cout << "Height (altitude in meters): ";
    float height;
    std::cin >> height;

    // Air density by altitude (ISA, simplified)
    float air_density = 1.225f; // Sea level on Earth
    if (height > 0 && height < 11000) {
        air_density = 1.225f * pow(1 - 0.0000225577f * height, 4.2561f);
    } else if (height >= 11000 && height < 20000) {
        air_density = 0.36391f * exp((11000.0f - height) / 6341.62f);
    } else if (height >= 20000) {
        air_density = 0.08803f * exp((20000.0f - height) / 6341.62f);
    }

    // Adjust for planetary atmosphere
    air_density *= air_thickness_value;

    // Aircraft presets
    struct AircraftPreset {
        std::string name;
        float area;
        float drag_coefficient;
        float lift_coefficient;
        float weight_kg;
        float glide_ratio;
        float velocity;
    };

    std::vector<AircraftPreset> presets = {
        {"Boeing 737", 125.0f, 0.027f, 0.27f, 79000.0f, 17.0f, 75.0f},
        {"Boeing 747", 541.0f, 0.031f, 0.25f, 333400.0f, 17.0f, 85.0f},
        {"Airbus A320", 122.6f, 0.030f, 0.28f, 73500.0f, 17.0f, 70.0f},
        {"F-16 Falcon", 27.87f, 0.018f, 0.50f, 12000.0f, 4.0f, 150.0f},
        {"Cessna 172", 16.2f, 0.025f, 1.2f, 1111.0f, 9.0f, 33.0f}
    };

    std::cout << "Select aircraft preset or 0 for custom:\n";
    for (size_t i = 0; i < presets.size(); ++i) {
        std::cout << i + 1 << ": " << presets[i].name << "\n";
    }
    std::cout << "0: Custom\n";

    int preset_choice = 0;
    std::cin >> preset_choice;

    // Aircraft variables
    float area = 1.0f, drag_coefficient = 0.05f, lift_coefficient = 1.0f;
    float weight_kg = 1000.0f, glideratio = 10.0f, velocity = 50.0f;

    if (preset_choice > 0 && preset_choice <= (int)presets.size()) {
        const auto& selected = presets[preset_choice - 1];
        area = selected.area;
        drag_coefficient = selected.drag_coefficient;
        lift_coefficient = selected.lift_coefficient;
        weight_kg = selected.weight_kg;
        glideratio = selected.glide_ratio;
        velocity = selected.velocity;
        std::cout << "Selected: " << selected.name << "\n";
    } else {
        std::cout << "lift coefficient: "; std::cin >> lift_coefficient;
        std::cout << "cross-sectional area (m^2): "; std::cin >> area;
        std::cout << "drag coefficient: "; std::cin >> drag_coefficient;
        std::cout << "weight (kg): "; std::cin >> weight_kg;
        std::cout << "glide ratio: "; std::cin >> glideratio;
        std::cout << "velocity (m/s): "; std::cin >> velocity;
    }

    // Begin calculations
    float weight_N = weight_kg * gravity_value;
    float lift = 0.5f * air_density * velocity * velocity * area * lift_coefficient;
    float parasite_drag = 0.5f * air_density * velocity * velocity * area * drag_coefficient;

    // Estimate aspect ratio
    float aspect_ratio = 8.0f;
    if (preset_choice == 4) aspect_ratio = 6.0f;
    if (preset_choice == 5) aspect_ratio = 7.0f;

    float induced_drag = (lift * lift) / (3.14159f * aspect_ratio * 0.5f * air_density * velocity * velocity * area);
    float total_drag = parasite_drag + induced_drag;

    float drag_factor = 1.0f / (1.0f + (total_drag / lift));
    float distance = (glideratio * height) * (9.8f / gravity_value) * air_factor * drag_factor;

    float acceleration = lift / weight_N;

    // Output results
    std::cout << "\n--- Results ---\n";
    std::cout << "Air density: " << air_density << " kg/m^3\n";
    std::cout << "Lift: " << lift << " N\n";
    std::cout << "Parasite drag: " << parasite_drag << " N\n";
    std::cout << "Induced drag: " << induced_drag << " N\n";
    std::cout << "Total drag: " << total_drag << " N\n";
    std::cout << "Acceleration: " << acceleration << " g\n";
    std::cout << "Distance traveled: " << distance << " m\n";

    if (lift < weight_N) {
        std::cout << "⚠️  Warning: LIFT IS LESS THAN WEIGHT! Aircraft cannot maintain flight.\n";
    }

    std::cout << selplanet << ": Gravity = " << gravity_value
              << " m/s^2, Air thickness = " << air_thickness_value << " atm\n";

    return 0;
}
