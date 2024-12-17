#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
using namespace std;

// دالة لحساب متوسط القيم الموجودة في المصفوفة
float calculateAverage(const vector<float>& marks) {
    float total = 0;
    for (float mark : marks) {
        total += mark;
    }
    return marks.empty() ? 0 : total / marks.size(); // تجنب القسمة على صفر
}

int main() {
    // تعريف المتغيرات
    double final_exam_mark, midterm_exam_mark;
    int quizzes_number, monthly_exams_number;

    // إدخال عدد الكويزات
    cout << "Enter the number of quizzes: ";
    cin >> quizzes_number;

    // إنشاء مصفوفة ديناميكية لتخزين درجات الكويزات
    vector<float> quizzes(quizzes_number);

    // إدخال درجات الكويزات
    for (int i = 0; i < quizzes_number; i++) {
        cout << "Enter the mark for quiz " << i + 1 << ": ";
        cin >> quizzes[i];
    }

    // حساب متوسط درجات الكويزات
    float quizzes_average = calculateAverage(quizzes);
    cout << "The average mark for quizzes is: " << quizzes_average << endl;

    // إدخال عدد الامتحانات الشهرية
    cout << "Enter the number of monthly exams: ";
    cin >> monthly_exams_number;

    // إنشاء مصفوفة ديناميكية لتخزين درجات الامتحانات الشهرية
    vector<float> monthly_exams(monthly_exams_number);

    // إدخال درجات الامتحانات الشهرية
    for (int i = 0; i < monthly_exams_number; i++) {
        cout << "Enter the mark for monthly exam " << i + 1 << ": ";
        cin >> monthly_exams[i];
    }

    // حساب متوسط درجات الامتحانات الشهرية
    float monthly_exams_average = calculateAverage(monthly_exams);
    cout << "The average mark for monthly exams is: " << monthly_exams_average << endl;

    // إدخال درجات الامتحان النصفي والنهائي
    cout << "Enter the midterm exam mark: ";
    cin >> midterm_exam_mark;

    cout << "Enter the final exam mark: ";
    cin >> final_exam_mark;

    // حساب الدرجة النهائية
    double final_mark = (quizzes_average + monthly_exams_average + midterm_exam_mark + final_exam_mark) / 4;

    // عرض النتيجة النهائية
    cout << "\nThe final mark is: " << final_mark << endl;

    // تحديد التقدير النهائي بناءً على النتيجة
    if (final_mark < 50)
        cout << "Estimated Subject Score: Weak" << endl;
    else if (final_mark < 60)
        cout << "Estimated Subject Score: Acceptable" << endl;
    else if (final_mark < 70)
        cout << "Estimated Subject Score: Average" << endl;
    else if (final_mark < 80)
        cout << "Estimated Subject Score: Good" << endl;
    else if (final_mark < 90)
        cout << "Estimated Subject Score: Very Good" << endl;
    else if (final_mark <= 100)
        cout << "Estimated Subject Score: Excellent" << endl;
    else
        cout << "Invalid marks entered!" << endl;

    return 0;
}
