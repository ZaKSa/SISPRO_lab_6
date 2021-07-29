#include <iostream>
using namespace std;

/*8)
b ^ 2 / a + 5,	если	a > b;
= -5,	если	a = b;
(a *b - b) / b,	если	a < b;
*/

double func(double a, double b)
{
    double res_asm; 
    int status, error;
    
    const int c2 = 2;
    const int c5 = 5;
    const int c5_ = -5;

    __asm {

        //st0 st1 st2 st3 st4

        finit; инициализация сопроцессора
        fld qword ptr[b]; b
        fld qword ptr[a]; a b
        
        fcom st(1); сравниваем a и b
        fstsw status; сохраняем регистр флагов сопроцессора
        mov ah, byte ptr[status + 1]
        sahf; записываем в регистр флагов процессора
        
        ja a_bigger; переход если a больше
        jb a_smaller; переход если b больше
        
        ; если равны
        
        fild c5_; 5 поместили в вершину стека (а=b)
   
        jmp END

            a_bigger : 
            fldz;
            fcomp; вещественное сравнение b с 0 с выталкиванием
            fnstsw ax; считываем состояние сопроцессора в память
            sahf; записываем содержимого ah в регистр FLAGS

            jz error_zf; Обработка ошибки деления на 0

            fdivp st(1), st; st = b / a
            fmul st(1), st; b* (b / a)
            fild c5; st = 5, st(1) = a / b
            faddp st(1), st; st = b*b/a + 5
            jmp END
                

            a_smaller :
            fldz; 
                fcomp; вещественное сравнение b с 0 с выталкиванием
                fnstsw ax; считываем состояние сопроцессора в память
                sahf; записываем содержимого ah в регистр FLAGS
           
                jz error_zf; Обработка ошибки деления на 0

            fld qword ptr[b];st0 = b st1 = a at2 = b

            fmulp  st(1), st; st(0) = ab st1=b
            fsub st, st(1); st(0) = a * b - b, st1=b
           
            fdiv st, st(1); st = (a*b - b) / b
            jmp END

            error_zf : ; формируем результат ошибки
                mov error, 1
            END :
                fstp res_asm;
        
    }

    return res_asm;
}


int main()
{
    double a, b, error = 0, ofs = 0, res;

    cout << "Enter a> ";
    cin >> a;
    cout << "Enter b> ";
    cin >> b;


    res = func(a, b);
    
    if (ofs == 1)
        cout << endl << "Out of size\n" << endl;
    else if (error == 1)
        cout << "Division by zero\n" << endl;

    else
        cout << "Result asm> " << res << endl;

    cout << "Result C++> " << res << endl;
    if (a > b)
        cout << b * b / a + 5;
    else if (a == b)
        cout << "5";
    else if (a < b)
        cout << (a * b - b) / b;

    return 0;
}