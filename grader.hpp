#ifndef GRADER_HPP
#define GRADER_HPP
struct Card
{
    double grade;
    int scratch_pixels;
    int dent_pixels;
    int fray_pixels;
};

double calculateGrade(Card &card);
void printResults(Card &card);

#endif // GRADER_HPP