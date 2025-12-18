#ifndef GRADER_HPP
#define GRADER_HPP

struct Card
{
    float grade;
    int scratch_pixels;
    int dent_pixels;
    int fray_pixels;
};

float calculateGrade(Card &card);
void printResults(Card &card);

#endif // GRADER_HPP