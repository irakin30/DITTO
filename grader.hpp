#ifndef GRADER_HPP
#define GRADER_HPP
struct Card
{
    double grade;
    int fray_pixels;
    double centering;
    int scratch_pixels;
};

double calculateGrade(Card &card);
void printResults(Card &card);

#endif // GRADER_HPP