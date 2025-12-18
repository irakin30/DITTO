#include "grader.hpp"
#include <algorithm>
#include <iostream>

double calculateGrade(Card &card)
{
    double grade = 10.0;

    // Maximum pixels for each defects
    const int MAX_SCRATCH_PIXELS = 500;
    const int MAX_DENT_PIXELS = 200;
    const int MAX_FRAY_PIXELS = 300;

    // More defects = lower grade
    double scratch_deduction = (static_cast<double>(card.scratch_pixels) / MAX_SCRATCH_PIXELS) * 5.0; // Max 5.0 deduction
    double dent_deduction = (static_cast<double>(card.dent_pixels) / MAX_DENT_PIXELS) * 3.0;          // Max 3.0 deduction
    double fray_deduction = (static_cast<double>(card.fray_pixels) / MAX_FRAY_PIXELS) * 2.0;          // Max 2.0 deduction

    grade -= (scratch_deduction + dent_deduction + fray_deduction);

    grade = std::clamp(grade, 1.0, 10.0);

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
    std::cout << "=====================\n";
    std::cout << "Final Grade: " << card.grade << "\n";
    std::cout << "=====================\n";

    std::string condition;
    if (card.grade >= 9.5)
        condition = "Mint";
    else if (card.grade >= 9.0)
        condition = "Near Mint";
    else if (card.grade >= 8.0)
        condition = "Excellent";
    else if (card.grade >= 7.0)
        condition = "Very Good";
    else if (card.grade >= 6.0)
        condition = "Good";
    else if (card.grade >= 5.0)
        condition = "Fair";
    else
        condition = "Poor";

    std::cout << "Condition: " << condition << std::endl;
    std::cout << "=====================" << std::endl;
}