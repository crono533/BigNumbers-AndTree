#include <iostream>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <ctime>
#include <string>

using namespace std;
typedef unsigned short BASE;
typedef unsigned int  DBASE;
#define BASE_SIZE (sizeof (BASE)*8)
//макисмальная цифра которая может поместиться в основаниях это 2^base_size


class BigNumber {
public:
    BASE* coef; //массив типо BASE для хранения коэффициентов
    //хранятся коэффициенты от младшего рахряда к старшему
    int len; //длинна без нулевых коэффициентов  //число значаших разрядов
    int maxlen; //фактическая длинна числа
public:


    //конструктор (по умолчанию)
    BigNumber(int ml = 1, int flag = 0) 
    { //ml - maxlen, d - для рандомных чисел
        coef = new BASE[ml]; //выделяем память числа по основанию BASE. Его длина - ml. ml передаём, как параметр
        int size = BASE_SIZE;
        maxlen = ml;
        if (ml == 1)
            len = 1; //число значаших разрядов
        //констурктор по умолчанию - заполняем все коофициенты нулями
        for (int i = 0; i < maxlen; i++)
            coef[i] = 0;

        if (flag != 0) {
            len = maxlen;
            for (int i = 0; i < maxlen; i++) {
                coef[i] = rand();
            }       

            if (sizeof(BASE) == sizeof(unsigned int)) {
                for (int i = 0; i < maxlen; i++) {
                    coef[i] <<= 16;
                    coef[i] |= rand();
                }
            }
            len_norm();
        }

        len_norm();
    }

    //конструтор копирования
    BigNumber(const BigNumber& x) {
        this->maxlen = x.maxlen;
        this->len = x.len;
        this->coef = new BASE[maxlen];
        for (int i = 0; i < maxlen; i++) {
            this->coef[i] = x.coef[i];
        }
    }


    //деструктор удалит число
    ~BigNumber() {
      if(coef)
          delete[] coef;
      coef = nullptr;
    } 

    void input_hex()
    {
        //вводим строку
        char s[500];
        cin >> s;

        unsigned int tmp = 0;

        this->len = (strlen(s) - 1) / (BASE_SIZE / 4) + 1; //формула для подсчёта количества коофициентов (байт)
        this->maxlen = len;
        this->coef = new BASE[this->maxlen];

        //заполняем коэффициенты нулями
        for (int i = 0; i < this->len; i++) {
            this->coef[i] = 0;
        }

        int j = 0; // по коофициентам
        int k = 0; //остаток


        //i - индекс по строке
        //записываем с младших разрядов к старшим

        for (int i = strlen(s) - 1; i >= 0; i--) {

            if ('0' <= s[i] && s[i] <= '9') {
                tmp = s[i] - '0'; //получаем число, если в строке число
            }
            else if ('a' <= s[i] && s[i] <= 'f') {
                tmp = s[i] - 'a' + 10; //получаем букву маленькую или большую, если в строке буква
            }
            else if ('A' <= s[i] && s[i] <= 'F') {
                tmp = s[i] - 'A' + 10;
            }
            else {
                cout << "Введенный символ не определен для шестнадцатиричного числа";
                exit(0);
            }

            //вписываем в j коофициент цифры, пока они туда могут поместиться (коофициент 8 бит, 4 бита для одной цифры в 16чной), 
            //поэтому когда k >= BASE_SIZE / 4, то берём следующий коофициент, k в ноль.

            this->coef[j] |= tmp << (k * 4);
            k++;
            if (k >= BASE_SIZE / 4) {
                k = 0;
                j++;
            }
        }
    }

    void print_hex() {
        //выводим первый кооффициент без выделения ширины
        cout << hex << (int)this->coef[len - 1]; //переводим в 16

        for (int j = this->len - 2; j >= 0; j--)
        {
            cout.width(BASE_SIZE / 4); //выделяем ширину
            cout.fill('0'); //заполняем нулями
            cout << hex << (int)this->coef[j]; 
        }
        cout << '\n' << endl;
    }

    // len_norm() - функция, которая устанавливает размер числа не включая нулевые коэффициенты
    void len_norm() {
        len = maxlen; //сначала len = maxlen
        // пока в кооффициенте - 0, мы уменьшаем len
        while (len > 0 && coef[len - 1] == 0) {
            len--;
        }
        //если дошли до конца, то оставляем длину числа - 1, т.е. получим 0
        if (len == 0) {
            len = 1;
        }
    }

    BigNumber operator = (const BigNumber& x) {

        if (this == &x) {
            return *this;
        }
        delete[] coef;
        this->maxlen = x.maxlen;
        this->len = x.len;
        this->coef = new BASE[maxlen];

        for (int i = 0; i < maxlen; i++) {
            this->coef[i] = x.coef[i];
        }
        return *this;

    }

    BigNumber operator + (const BigNumber& x) {

        int l = max(this->len, x.len) + 1; //длина результата 
        int t = min(this->len, x.len); //общая часть
        BigNumber B(l); // создаем число - результат

        int j = 0; //индекс по разряам
        int k = 0; //перенос
        DBASE tmp; //результат сложения двух цифр и переноса k


        while (j < t) {
            tmp = (DBASE)this->coef[j] + (DBASE)x.coef[j] + (DBASE)k; //сумма цифр чисел + перенос
            B.coef[j] = (BASE)tmp; //отрезали число
            k = (BASE)(tmp >> BASE_SIZE); //получили новый перенос

            j++; //перешли к следующему коофицинету
        }

        //если первое число больше
        while (j < this->len) {
            tmp = (DBASE)this->coef[j] + (DBASE)k;
            B.coef[j] = (BASE)tmp;
            k = (BASE)(tmp >> BASE_SIZE);

            j++;
        }

        //если второе число больше
        while (j < x.len) {
            tmp = (DBASE)x.coef[j] + (DBASE)k;
            B.coef[j] = (BASE)tmp;
            k = (BASE)(tmp >> BASE_SIZE);

            j++;
        }

        B.coef[j] = k; //последний перенос (либо 0, либо 1)
        //можно без lennorm, просто проверить k = 1 или 0 и уведличить len на 1 
        B.len_norm();
        return B;
    }

	BigNumber operator + (const BASE& x) {

		BigNumber result(this->len + 1); //создаем результат на 1 больше 

		int j = 0; //индекс по разряам
		int k = 0; //перенос
		DBASE tmp; //результат сложения и переноса k

		//j < 1 потому что размер общей части это единица 

		tmp = (DBASE)this->coef[j] + (DBASE)x + (DBASE)k;
		result.coef[j] = (BASE)tmp;
		k = (BASE)(tmp >> BASE_SIZE);

		j++;


		while (j < this->len) {
			tmp = (DBASE)this->coef[j] + (DBASE)k;
			result.coef[j] = (BASE)tmp;
			k = (BASE)(tmp >> BASE_SIZE);

			j++;
		}

        result.coef[j] = k; //записываем последний остаток

        result.len_norm(); 

        return result;
    }

    bool operator == (const BigNumber& x) {
        if (this == &x) {
            return true;
        }
        if (this->len != x.len) {
            return false;
        }
        for (int i = 0; i < x.len; i++) {
            if (this->coef[i] != x.coef[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator != (const BigNumber& x) {
        if (this == &x) {
            return false;
        }
        if (this->len != x.len) {
            return true;
        }

        for (int i = 0; i < x.len; i++) {
            if (this->coef[i] != x.coef[i]) {
                return true;
            }
        }
        return false;
    }

    bool operator > (const BigNumber& x) {
        if (this == &x) {
            return false;
        }
        if (this->len > x.len) {
            return true;
        }
        if (this->len < x.len) {
            return false;
        }

        int i = len - 1;
        while (i != 0) {
            if (this->coef[i] > x.coef[i]) {
                return true;
            }
            else if (this->coef[i] < x.coef[i]) {
                return false;
            }
            else {
                i--;
            }
        }
        return false;
    }

    bool operator < (const BigNumber& x) {
        if (this == &x) {
            return false;
        }
        if (this->len < x.len) {
            return true;
        }
        if (this->len > x.len) {
            return false;
        }

        int i = len - 1;
        while (i != 0) {
            if (this->coef[i] < x.coef[i]) {
                return true;
            }
            else if (this->coef[i] > x.coef[i]) {
                return false;
            }
            else {
                i--;
            }
        }
        return false;
    }

    bool operator >= (const BigNumber& x) {
        if (this == &x) {
            return true;
        }
        if (this->len > x.len) {
            return true;
        }
        if (this->len < x.len) {
            return false;
        }

        int i = len - 1;
        while (i != 0) {
            if (this->coef[i] > x.coef[i]) {
                return true;
            }
            else if (this->coef[i] < x.coef[i]) {
                return false;
            }
            else {
                i--;
            }
        }
        return true;
    }

    bool operator <= (BigNumber& x) {
        if (this == &x) {
            return true;
        }
        if (this->len <= x.len) {
            return true;
        }

        if (this->len > x.len) {
            return false;
        }

        int i = len - 1;
        while (i != 0) {
            if (this->coef[i] < x.coef[i]) {
                return true;
            }
            else if (this->coef[i] > x.coef[i]) {
                return false;
            }
            else {
                i--;
            }
        }
        return true;
    }
    
    BigNumber operator - (const BigNumber& x) {
        //первое больше или равно второму
        if (this->len < x.len) {
            cout << "First number must be greater then second";
            exit(0);
        }

        BigNumber result(this->len); //кол-во коофициентов будет равно максимальной длине первого числа 
        int j = 0; //по коофициентам
        int k = 0; //займ
        DBASE tmp;

        while (j < x.len) {
            tmp = ((DBASE)1 << BASE_SIZE) | this->coef[j]; //1 сдвинули + коофициент числа, от которого отнимаем
            tmp = tmp - x.coef[j] - k; //отняли от него коофициент второго числа и k

            result.coef[j] = (BASE)tmp; //результат разницы записываем в коофициент результата
            k = !(tmp >> BASE_SIZE); //понимаем, были ли заём или нет

            j++; //идём дальше
        }


        while (j < this->len) {
            tmp = ((DBASE)1 << BASE_SIZE) | this->coef[j];
            tmp -= k;
            result.coef[j] = (BASE)tmp;
            k = !(tmp >> BASE_SIZE);

            j++;
        }

        result.len_norm();
        return result;
    }

    BigNumber operator += (const BigNumber& x) {
        *this = *this + x;
        return *this;
    }

    BigNumber operator -= (const BigNumber& x) {
        *this = *this - x;
        return *this;
    }

    BigNumber operator * (const BASE& x) {
        int j = 0; //умножение
        BASE k = 0; //перенос (тут может быть от 1 до 9 перенос вроде)
        BigNumber result(this->len + 1);
        DBASE tmp;

        //по числу идём
        while (j < this->len) {
            tmp = (DBASE)this->coef[j] * (DBASE)x + (DBASE)k; //соответствующие коофициенты умножаем + перенос
            result.coef[j] = (BASE)tmp; //отрезаем полученный результат
            k = (BASE)(tmp >> BASE_SIZE); //перенос

            j++;
        }

        result.coef[j] = k; //в конце перенос записали

        result.len_norm(); //если нули есть

        return result;
    }

    //Деление на цифру (тут есть какая то ошибка) надо првоерить ОШИБКА (ДЕЛЕНИЕ НА ОДНОЗНАЧНОЕ ЧИСЛО)
    BigNumber operator / (const BASE& x) {
        int j = 0; //по разрядам
        BASE r = 0; //остаток
        DBASE tmp = 0; //результат деления цифр
        BigNumber result(this->len); //максимальная длина результат - длина числа, которое мы делим
        tmp = ((DBASE)r << BASE_SIZE) + (DBASE)this->coef[this->len - 1 - j];
        //идём по числу
        while (j < this->len) {
            //cout << "len= " << this->len << " - 1 - " << j << endl;
           // cout << "coef[" << this->len - 1 - j <<"]= " << hex <<this->coef[this->len - 1 - j] + 0 << endl;
            tmp = ((DBASE)r << BASE_SIZE) + (DBASE)this->coef[this->len - 1 - j]; //к остатку прибаляем коофициент
            //cout << "TMP= " << tmp << endl;
            result.coef[this->len - 1 - j] = (BASE)(tmp / (DBASE)x); //делим его на нашу цифру
            //cout << "result= " << result.coef[this->len - 1 - j] + 0 << endl;
            r = (BASE)(tmp % (DBASE)x); //получаем следующий остаток

            j++; //идём дальше
        }

        result.len_norm();

        return result;
    }

    BigNumber operator % (const BASE& x) {
        int j = 0;
        DBASE tmp = 0;
        BASE r = 0;
        BigNumber result(1); // результат mod займёт один коофициент - одну цифру

        while (j < this->len) {
            tmp = ((DBASE)r << BASE_SIZE) + (DBASE)this->coef[this->len - 1 - j];
            r = (BASE)(tmp % (DBASE)x);

            j++;
        }

        result.coef[0] = r;
        return result;
    }

    //умножение на число
    BigNumber operator * (const BigNumber& x) {

        int j = 0; //по второму числу
        BigNumber result(this->len + x.len);
        DBASE tmp;

        while (j < x.len) { //по второму числу т.к. сначала берем цифру из коэф второго числа и умножаем на все первое число

            if (x.coef[j] != 0) { //ноль пропускаем
                BASE k = 0; //перенос
                int i = 0; //по первомму числу

                while (i < this->len) { //по первому числу
                    tmp = (DBASE)this->coef[i] * (DBASE)x.coef[j] + (DBASE)result.coef[i + j] + (DBASE)k; //i и j цифру умножили + цифра[i + j]  + перенос
                    //таким образом сразу сложит то что дург под другом если бы умножали столбиком
                    result.coef[i + j] = (BASE)tmp; //отрезали последнию цифру
                    k = (BASE)(tmp >> BASE_SIZE); //остаток

                    i++;
                }

                result.coef[this->len + j] = k; //после уиножения числа и цифры запишем последний остаток
            }

            j++;
        }

        result.len_norm();

        return result;
    }

    BigNumber operator *= (const BigNumber& x) {
        *this = *this * x;
        return *this;
    }

    BigNumber operator / (BigNumber& x) {

        if (x.len == 1 && x.coef[0] == 0) {
            //cout << "Деление на 0" << endl;
            exit(0);
        }

        if (*this < x) {
           // cout << "Нельзя меньшее делить на больщее" << endl;
            BigNumber result(1);
            return result;
        }

        if (x.len == 1) {
            return *this / x.coef[0];
        }

        //написать исключение если числа равны
        if(*this == x)
        {
            BigNumber result(1);
            result = result + 1;
            return result;
        }

        int m = len - x.len; //длина результата
        DBASE b = ((DBASE)1 << BASE_SIZE);
        DBASE d = 0;
        d = b / (DBASE)(x.coef[x.len - 1] + (DBASE)1); //для нормализации, чтобы воспоьзоваться теоремой 2

        int j = m; //начальная установка - длина резальтата (столько раз делим) - 2

        int k = 0;
        //cout << d << endl;
        //нормализация  - 1
        BigNumber divisible(*this);

        //cout << "Изначально делимое ";
        //divisible.print_hex();
        divisible = divisible * d;
        //divisible.print_hex();

        BigNumber divider(x);
        //cout << "Изначально делитель ";
        //divider.print_hex();
        divider = divider * d;
        //divider.print_hex();
        //длина результата - это или m, или m + 1
        BigNumber result(m + 1);
        result.len = m + 1;


        //че то в why 0
        //why 0
        //if (divisible.len == len) {/// сравниваем длину нормализованного с исходным
        //    divisible.maxlen++; //увеличваем len для ноля
        //    divisible.len = maxlen;
        //    delete[]divisible.coef; //удаляю всё
        //    divisible.coef = new BASE[maxlen];
        //    for (int i = 0; i < len; i++) { //заново перезаписываю туда всё, что было
        //        divisible.coef[i] = coef[i];
        //    }
        //    divisible *= d; //снова нормализую
        //    divisible.len++;
        //    divisible.coef[divisible.len - 1] = 0; //ЯВНО записываю 0 в начало
        //}
        

        //деление
        while (j >= 0) {
            //по формулам считаем q` и r` (q` - результаты делений и r` - остатки)
            //этап 3
            DBASE q = (DBASE)((DBASE)((DBASE)((DBASE)(divisible.coef[j + divider.len]) * (DBASE)(b)) + (DBASE)(divisible.coef[j + divider.len - 1])) / (DBASE)(divider.coef[divider.len - 1]));
            DBASE r = (DBASE)((DBASE)((DBASE)((DBASE)(divisible.coef[j + divider.len]) * (DBASE)(b)) + (DBASE)(divisible.coef[j + divider.len - 1])) % (DBASE)(divider.coef[divider.len - 1]));


            //проверки на уменьшение q`
            if (q == b || (DBASE)((DBASE)(q) * (DBASE)(divider.coef[divider.len - 2])) > (DBASE)((DBASE)((DBASE)(b) * (DBASE)(r)) + (DBASE)(divisible.coef[j + divider.len - 2]))) {
                q--;
                r = (DBASE)(r)+(DBASE)(divider.coef[divider.len - 1]);
                if ((DBASE)(r) < b) {
                    if (q == b || (DBASE)((DBASE)(q) * (DBASE)(divider.coef[divider.len - 2])) > (DBASE)((DBASE)((DBASE)(b) * (DBASE)(r)) + (DBASE)(divisible.coef[j + divider.len - 2]))) {
                        q--;
                    }
                }
            }

            //этап 4
            //переписываем число в u
            BigNumber u(divider.len + 1);
            u.len = divider.len + 1;
            for (int i = 0; i < divider.len + 1; i++) {
                u.coef[i] = divisible.coef[j + i];
            }


            //ещё одна проверка на уменьшение q`
            if (u < divider * (BASE)(q)) {
                q--;
            }


            //отнимаем от числа найденное q` и делитель
            u = u - divider * (BASE)(q);
            result.coef[j] = (BASE)(q);


            //перезапишем u
            //этап 5
            for (int i = 0; i < divider.len + 1; i++) {
                divisible.coef[j + i] = u.coef[i];
            }

            j--; //этап 6
        }

        result.len_norm();

        return result;
    }



    BigNumber operator % (const BigNumber& x) {
        if (x.len == 1 && x.coef[0] == 0) {
            //cout << "Берем модуль от 0";
            exit(0);
        }
        if (*this < x) {
            //cout << "Модуль: делимое меньше делителя";
            return *this;
        }

        if (x.len == 1) {
            //cout << "Модуль на цифру";
            return *this % x.coef[0];
        }

        //написать исключение если числа равны (вернуть 1) 
        if (*this == x)
        {
            //cout << "Модуль: делимое и делитель равны";
            BigNumber result(1);
            result = result + 1;
            return result;
        }

        int m = len - x.len;
        int base_size = BASE_SIZE;
        DBASE b = ((DBASE)(1) << (base_size));
        BASE d = (BASE)((DBASE)(b) / (DBASE)((x.coef[x.len - 1]) + (BASE)(1)));
        int j = m;
        int k = 0;

        BigNumber divisible = *this;
        divisible = divisible * d;
        BigNumber divider = x;
        divider = divider *  d;


        //че то в why 0
        //if (divisible.len == len) {
        //    divisible.maxlen++;
        //    divisible.len = maxlen;
        //    divisible.coef = new BASE[maxlen];
        //    for (int i = 0; i < len; i++) {
        //        divisible.coef[i] = coef[i];
        //    }
        //    divisible *= d;
        //    divisible.len++;
        //    divisible.coef[divisible.len - 1] = 0;
        //}

        while (j > -1) {
            DBASE q = (DBASE)((DBASE)((DBASE)((DBASE)(divisible.coef[j + divider.len]) * (DBASE)(b)) + (DBASE)(divisible.coef[j + divider.len - 1])) / (DBASE)(divider.coef[divider.len - 1]));
            DBASE r = (DBASE)((DBASE)((DBASE)((DBASE)(divisible.coef[j + divider.len]) * (DBASE)(b)) + (DBASE)(divisible.coef[j + divider.len - 1])) % (DBASE)(divider.coef[divider.len - 1]));


            if (q == b || (DBASE)((DBASE)(q) * (DBASE)(divider.coef[divider.len - 2])) > (DBASE)((DBASE)((DBASE)(b) * (DBASE)(r)) + (DBASE)(divisible.coef[j + divider.len - 2]))) {
                q--;
                r = (DBASE)(r)+(DBASE)(divider.coef[divider.len - 1]);
                if ((DBASE)(r) < b) {
                    if (q == b || (DBASE)((DBASE)(q) * (DBASE)(divider.coef[divider.len - 2])) > (DBASE)((DBASE)((DBASE)(b) * (DBASE)(r)) + (DBASE)(divisible.coef[j + divider.len - 2]))) {
                        q--;
                    }
                }
            }


            BigNumber u(divider.len + 1);
            u.len = divider.len + 1;
            for (int i = 0; i < divider.len + 1; i++) {
                u.coef[i] = divisible.coef[j + i];
            }

            if (u < divider * (BASE)(q)) {
                q--;
            }


            u = u - (divider * (BASE)(q));


            for (int i = 0; i < divider.len + 1; i++) {
                divisible.coef[j + i] = u.coef[i];
            }


            j--;
        }
        divisible.len_norm();

        return divisible / d; //денормализация - этап 7
    }
    
    void cin_10() {
        int j = 0;
        string s;
        cout << "Base10: ";
        getline(cin, s);
        int lengthStr = s.length();
        BASE tmp = 0;

        BigNumber result;

        while (j < lengthStr) {
            if ('0' > s[j] || s[j] > '9') {
                cout << "Не верный диапозон";
                exit(0);
            }
            tmp = s[j] - '0'; //преобразование из строки в число

            result = result * BASE(10); //чтобы сдвинуть все цифры результата на один разряд влево. увеличиваем на порядок


            BASE digit = (BASE)tmp;

            result = result + digit;

            j++;
        }
        
        result.len_norm();
        
        *this = result;
    }
    
    void cout_10() {
        BigNumber result = *this;
        BigNumber zero(result.len);
        //        cout << "Dlina v cout: " << result.len << endl;
        string s;
        zero.len = result.len;
        //        cout << "Dlina zero - pervoe znachenie: " << zero.len << endl;

        while (result != zero) {
            BigNumber tmp;
            tmp = result % 10;
                      //cout << "tmp: " << (int)tmp.coef[0] << endl;
            s.push_back(tmp.coef[0] + '0');
                        //cout << "stroka v while: " << s << endl;
            //уменьшаем размер result 
            result = result / 10;
                       //cout << "result: " << (int)result.coef[0] << endl;
            zero.len = result.len;
                       //cout << "Umenishaem zero.len: " << zero.len << endl;
        }



        reverse(s.begin(), s.end());
        cout << "Base10= " << s << endl;
    }

};

int rand_in_range(int min, int max) // функция rand_in_range нужна чтобы сгенерировать случайный ключ
{
    if (min == max) {
        return min;
    }
    if (min > max) {
        int tmp = min;
        min = max;
        max = tmp;
    }
    return rand() % (max - min + 1) + min;
}


void test() {
    int M = 1000, T = 1000, m, n;
    n = rand() % M + 1;
    m = rand() % M + 1;
    BigNumber A, B, C, D;
    do {
        //cout << "T: " << T << endl;
        //cout << "m and n: " << m << " " << n << endl;
        m = rand() % M + 1;
        n = rand() % M + 1;
        
        BigNumber a(m, 1), b(n, 1);
        if (b.maxlen == 1) {
            A = a; B = b;
            C = A / B;
            D = A % B;
        }
        
        
        cout << "m and n: " << m << " " << n << endl;
       
        cout << "T: " << T << endl;
    } while (/*A == C * B + D && A - D == C * B && D < B &&*/ --T);
}


int main() {
    srand((unsigned int)time(NULL));
    BigNumber A(3,1);
    BigNumber B(3,1);
    BigNumber C;
    BigNumber D;
    BigNumber E;

    test();

    //cout << "First number: ";
    //A.print_hex();
    //cout << "Second number: ";
    //B.print_hex();

    //D = A / B;
    //C = A % B;

    //cout << "D = ";
    //D.print_hex();

    //cout << "C = ";
    //C.print_hex();

    //E = D * B + C;

    //E.print_hex();


    return 0;
}

