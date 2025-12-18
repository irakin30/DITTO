#include "grader.hpp"
#include <algorithm>
#include <iostream>

float calculateGrade(Card &card)
{
    float grade = 10.0;

    // Maximum pixels for each defects
    const int MAX_SCRATCH_PIXELS = 1000;
    const int MAX_DENT_PIXELS = 500;
    const int MAX_FRAY_PIXELS = 800;

    // More defects = lower grade
    float scratch_deduction = (static_cast<float>(card.scratch_pixels) / MAX_SCRATCH_PIXELS) * 5.0; // Max 5.0 deduction
    float dent_deduction = (static_cast<float>(card.dent_pixels) / MAX_DENT_PIXELS) * 3.0;          // Max 3.0 deduction
    float fray_deduction = (static_cast<float>(card.fray_pixels) / MAX_FRAY_PIXELS) * 2.0;          // Max 2.0 deduction

    grade -= (scratch_deduction + dent_deduction + fray_deduction);

    grade = std::clamp(grade, 1.0f, 10.0f);

    card.grade = grade;

    return grade;
}

void printResults(Card &card)
{
    std::cout << "Card Grading Results:\n";
    std::cout << "=====================\n";
    std::cout << "Scratch Pixels: " << card.scratch_pixels << "\n";
    std::cout << "Dent Pixels: " << card.dent_pixels << "\n";
    std::cout << "Fray Pixels: " << card.fray_pixels << "\n";
    std::cout << "----------------------\n";
    std::cout << "Final Grade: " << card.grade << "\n";
    std::cout << "=====================\n";
}