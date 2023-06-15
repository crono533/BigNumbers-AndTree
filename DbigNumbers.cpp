#include <iostream>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <ctime>
#include <string>
#include <random>

using namespace std;

typedef unsigned char BASE;
typedef unsigned short DBASE;
#define BASE_SIZE (sizeof(BASE) * 8)
#define BASENUM ((DBASE)1 << BASE_SIZE)

mt19937 rnd(time(0));

class BigNumber {
private:
    BASE* coef;
    int len, maxlen;
    BigNumber shift(int x) {
        int m = len, n = len + x;
        BigNumber bn(n); bn.len = n;
        for (int i = 0; i < m; i++) bn.coef[n - 1 - i] = coef[m - 1 - i];
        while (bn.coef[bn.len - 1] == 0 && (bn.len > 1)) bn.len--;
        return bn;
    }

public:
    BigNumber(int m = 1, bool f = 0) : maxlen(m), len(1) {
        coef = new BASE[maxlen]{};
        if (!f)
            for (int i = 0; i < maxlen; coef[i] = 0, i++);
        else {
            //mt19937 rnd(time(0));
            for (int i = 0; i < maxlen; coef[i] = rnd(), i++);
            len = maxlen;
            while (coef[len - 1] == 0) coef[len - 1] = rnd();
        }
    }
    BigNumber(const BigNumber& x) : len(x.len), maxlen(x.maxlen) {
        coef = new BASE[maxlen]{};
        for (int i = 0; i < len; coef[i] = x.coef[i], i++);
    }
    ~BigNumber() { delete[] coef; }
    void len_norm() {
        len = maxlen;
        while (len > 0 && coef[len - 1] == 0) {
            len--;
        }
        if (len == 0) {
            len = 1;
        }
    }

    BigNumber& operator = (const BigNumber& x) {
        if (this != &x) {
            if (coef) delete[] coef;
            len = x.len;
            maxlen = x.maxlen;
            coef = new BASE[maxlen]{};
            for (int i = 0; i < len; i++) coef[i] = x.coef[i];
        }
        return *this;
    }

    void input_hex()
    {
        char s[500];

        cin >> s;

        int j = 0;
        int k = 0;
        unsigned int tmp = 0;

        this->len = (strlen(s) - 1) / (BASE_SIZE / 4) + 1;
        this->maxlen = len;
        this->coef = new BASE[this->maxlen];


        for (int i = 0; i < this->len; i++) {
            this->coef[i] = 0;
        }

        for (int i = strlen(s) - 1; i >= 0; i--) {
            if ('0' <= s[i] && s[i] <= '9') {
                tmp = s[i] - '0';
            }
            else if ('a' <= s[i] && s[i] <= 'f') {
                tmp = s[i] - 'a' + 10;
            }
            else if ('A' <= s[i] && s[i] <= 'F') {
                tmp = s[i] - 'A' + 10;
            }
            else {
                cout << "error";
                exit(0);
            }

            this->coef[j] |= tmp << (k * 4);
            k++;
            if (k >= BASE_SIZE / 4) {
                k = 0;
                j++;
            }
        }

    }
    void print_hex() {
        cout << hex << (int)this->coef[len - 1];

        for (int j = this->len - 2; j >= 0; j--)
        {
            cout.width(BASE_SIZE / 4);
            cout.fill('0');
            cout << hex << (int)this->coef[j];
        }
    }

    bool operator > (const BigNumber& bn) const {
        if (len != bn.len) return len > bn.len;
        for (int i = len - 1; i >= 0; i--)
            if (coef[i] != bn.coef[i]) return coef[i] > bn.coef[i];
        return 0;
    }
    bool operator < (const BigNumber& bn) const {
        return (bn > *this);
    }
    bool operator >= (const BigNumber& bn) const {
        return !(*this < bn);
    }
    bool operator <= (const BigNumber& bn) const {
        return !(*this > bn);
    }
    bool operator == (const BigNumber& bn) const { ///
        return (*this <= bn && *this >= bn);
    }
    bool operator != (const BigNumber& bn) const {
        return !(*this == bn);
    }

    BigNumber operator + (const BigNumber& x) {

        int l = max(this->len, x.len) + 1;
        int t = min(this->len, x.len);
        BigNumber sum(l);
        sum.len = l;

        int j = 0;
        int k = 0;
        DBASE tmp;

        while (j < t) {
            tmp = (DBASE)coef[j] + (DBASE)x.coef[j] + (DBASE)k;
            sum.coef[j] = (BASE)tmp;
            k = (BASE)(tmp >> BASE_SIZE);
            j++;
        }

        while (j < this->len) {
            tmp = (DBASE)coef[j] + (DBASE)k;
            sum.coef[j] = (BASE)tmp;
            k = (BASE)(tmp >> BASE_SIZE);
            j++;
        }

        while (j < x.len) {
            tmp = (DBASE)x.coef[j] + (DBASE)k;
            sum.coef[j] = (BASE)tmp;
            k = (BASE)(tmp >> BASE_SIZE);
            j++;
        }

        sum.coef[j] = k;

        if (sum.coef[j] == 0) sum.len--;
        return sum;
    }
    BigNumber& operator += (const BigNumber& x) {
        *this = *this + x;
        return *this;
    }

    BigNumber operator - (const BigNumber& x) {
        DBASE tmp;
        int n = len, m = x.len;
        int i = 0, k = 0;
        BigNumber result(n);
        result.len = n;
        if (x > *this) exit(2);
        while (i < m) {
            tmp = ((DBASE)1 << BASE_SIZE) | coef[i];
            tmp = tmp - x.coef[i] - k;
            result.coef[i] = (BASE)tmp;
            k = !(tmp >> BASE_SIZE);
            i++;
        }
        while (i < n) {
            tmp = ((DBASE)1 << BASE_SIZE) | coef[i];
            tmp -= k;
            result.coef[i] = (BASE)tmp;
            k = !(tmp >> BASE_SIZE);
            i++;
        }
        while (result.coef[result.len - 1] == 0 && (result.len > 1)) result.len--;
        return result;
    }
    BigNumber& operator -= (const BigNumber& bn) {
        *this = *this - bn;
        return *this;
    }

    BigNumber operator * (const BASE& num) {
        DBASE tmp;
        int i = 0, n = len + 1;
        BASE k = 0;
        BigNumber mul(n); mul.len = n;
        while (i < n - 1) {
            tmp = (DBASE)coef[i] * (DBASE)num + (DBASE)k;
            mul.coef[i] = (BASE)tmp;
            k = (BASE)(tmp >> BASE_SIZE);
            i++;
        }
        mul.coef[i] = k;
        while (mul.coef[mul.len - 1] == 0 && (mul.len > 1)) mul.len--;
        return mul;
    }
    BigNumber operator * (const BigNumber& bn) {
        DBASE tmp;
        int n = len, m = bn.len;
        int l = n + m;
        BigNumber mul(l); mul.len = l;
        int j = 0;
        while (j < m) {
            if (bn.coef[j] != 0) {
                int i = 0;
                BASE k = 0;
                while (i < n) {
                    tmp = (DBASE)coef[i] * (DBASE)bn.coef[j] + (DBASE)mul.coef[i + j] + (DBASE)k;
                    mul.coef[i + j] = (BASE)tmp;
                    k = (BASE)(tmp >> BASE_SIZE);
                    i++;
                }
                mul.coef[j + n] = (BASE)k;
            }
            j++;
        }
        while (mul.coef[mul.len - 1] == 0 && (mul.len > 1)) mul.len--;
        return mul;
    }
    BigNumber& operator *= (const BigNumber& bn) {
        *this = *this * bn;
        return *this;
    }

    BigNumber operator + (const BASE& num) {
        DBASE tmp;
        int n = len + 1, i = 0, k = 0;
        BigNumber sum(n); sum.len = n;

        tmp = (DBASE)coef[i] + (DBASE)num;
        sum.coef[i] = (BASE)tmp;
        k = (BASE)(tmp >> BASE_SIZE);
        i++;

        while (i < n - 1) {
            tmp = (DBASE)coef[i] + (DBASE)k;
            sum.coef[i] = (BASE)tmp;
            k = (BASE)(tmp >> BASE_SIZE);
            i++;
        }
        sum.coef[i] = k;
        while (sum.coef[sum.len - 1] == 0 && (sum.len > 1)) sum.len--;
        return sum;
    }

    BigNumber operator / (const BASE& num) {//by number
        if (num == 0) exit(3);
        DBASE tmp;
        int n = len, i = 0;
        BASE r = 0;
        BigNumber div(n); div.len = n;
        //проверка сравнение
        while (i < n) {
            tmp = ((DBASE)r << BASE_SIZE) + (DBASE)coef[n - i - 1];
            div.coef[n - i - 1] = (BASE)(tmp / num);
            r = (BASE)(tmp % num);
            i++;
        }
        while (div.coef[div.len - 1] == 0 && (div.len > 1)) div.len--;
        return div;
    }
    BASE operator % (const BASE& num) {
        if (num == 0) exit(3);
        DBASE tmp;
        int n = len, i = 0;
        BASE r = 0;
        //BN div(n);
        //проверка сравнение
        while (i < n) {
            tmp = ((DBASE)r << BASE_SIZE) + (DBASE)coef[n - i - 1];
            //div.coef[n - i - 1] = (BASE)(tmp / num);
            r = (BASE)(tmp % num);
            i++;
        }
        //while(div.coef[div.len - 1] == 0 && (div.len > 1)) div.len--;
        return r;
    }

    BigNumber operator / (const BigNumber& bn) {
        BigNumber e;
        if (bn == e) exit(10);
        if (*this == bn) return (e + 1);
        if (*this < bn) return e;
        if (bn.len < 2) {
            BASE x;
            x = bn.coef[0];
            BigNumber u(*this);
            u = u / x;
            return u;
        }
        int t = len, n = bn.len, m = t - n, i = 0, d = 0;
        BigNumber u(*this), v(bn), q(m + 1); q.len = m + 1;
        DBASE qx = 0, rx = 0;
        d = BASENUM / (v.coef[n - 1] + 1);
        u = u * d; v = v * d;
        i = m;
        while (i >= 0) {
            qx = (((DBASE)u.coef[i + n] << BASE_SIZE) + u.coef[i + n - 1]) / v.coef[n - 1];
            rx = (((DBASE)u.coef[i + n] << BASE_SIZE) + u.coef[i + n - 1]) % v.coef[n - 1];
            if ((qx == BASENUM) || (qx * v.coef[n - 2] > ((rx << BASE_SIZE) + u.coef[i + n - 2]))) {
                qx--;
                rx += v.coef[n - 1];
            }
            if (rx < BASENUM)
                if ((qx == BASENUM) || (qx * v.coef[n - 2] > ((rx << BASE_SIZE) + u.coef[i + n - 2]))) {
                    qx--;
                    rx += v.coef[n - 1];
                }
            u += (e + 1).shift(i + n + 1);
            u = u - (v * qx).shift(i);
            q.coef[i] = qx;
            if (u.coef[i + n + 1] == 0) {
                q.coef[i]--;
                u = u + v.shift(i);
            }
            u -= (e + 1).shift(i + n + 1);
            i--;
        }
        while (q.coef[q.len - 1] == 0 && (q.len > 1)) q.len--;
        return q;
    }
    BigNumber operator % (const BigNumber& bn) {
        BigNumber e;
        if (bn == e) exit(10);
        if (*this == bn) return e;
        if (*this < bn) return *this;
        if (bn.len < 2) {
            BASE x;
            x = bn.coef[0];
            BigNumber u;
            u.coef[0] = *this % x;
            return u;
        }
        int t = len, n = bn.len, m = t - n, i = 0, d = 0;
        BigNumber u(*this), v(bn);
        DBASE qx = 0, rx = 0;
        d = BASENUM / (v.coef[n - 1] + 1);
        u = u * d; v = v * d;
        i = m;
        while (i >= 0) {
            qx = (((DBASE)u.coef[i + n] << BASE_SIZE) + u.coef[i + n - 1]) / v.coef[n - 1];
            rx = (((DBASE)u.coef[i + n] << BASE_SIZE) + u.coef[i + n - 1]) % v.coef[n - 1];
            if ((qx == BASENUM) || (qx * v.coef[n - 2] > ((rx << BASE_SIZE) + u.coef[i + n - 2]))) {
                qx--;
                rx += v.coef[n - 1];
            }
            if (rx < BASENUM)
                if ((qx == BASENUM) || (qx * v.coef[n - 2] > ((rx << BASE_SIZE) + u.coef[i + n - 2]))) {
                    qx--;
                    rx += v.coef[n - 1];
                }
            u += (e + 1).shift(i + n + 1);
            u = u - (v * qx).shift(i);
            if (u.coef[i + n + 1] == 0) u = u + v.shift(i);
            u -= (e + 1).shift(i + n + 1);
            i--;
        }
        u = u / d;
        return u;
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
            //            cout << "tmp: " << (int)tmp.coef[0] << endl;
            s.push_back(tmp.coef[0] + '0');
            //            cout << "stroka v while: " << s << endl;
            result = result / 10;
            //            cout << "result: " << (int)result.coef[0] << endl;
            zero.len = result.len;
            //            cout << "Umenishaem zero.len: " << zero.len << endl;
        }



        reverse(s.begin(), s.end());
        cout << "Base10= " << s << endl;
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
                cout << "error";
                exit(0);
            }
            tmp = s[j] - '0';
            //            cout << "1" << endl;
            result = result * BASE(10);

            //            BigNumber digit;
            //            digit.coef[0]=(BASE)tmp;

            BASE digit = (BASE)tmp;
            //            cout << (int)result.coef[0] << endl;
            //            cout << (int)digit << endl;

            //            cout << "2" << endl;
            result = result + digit;

            j++;
        }
        //
        result.len_norm();
        //        cout << "Dlina v cin: " << result.len << endl;
        *this = result;
    }
    /*
    friend ostream& operator << (ostream&, BigNumber&);
    friend istream& operator >> (istream&, BigNumber&);
     */
};
/*
ostream& operator << (ostream & out, BigNumber & x) {
    char* s = new char[BASE_SIZE / 4 * x.len + 1];
    int i = 0, t = 0;
    BigNumber u(x), tmp;
    while (u != tmp) {
        t = u % (BASE)10;
        s[i] = t + '0';
        u = u / (BASE)10;
        i++;
    }
    s[i--] = '\0';
    while (i >= 0) out << s[i--];
    return out;
}
istream& operator >> (istream & in, BigNumber & x) {
    int op = 0, i = 0, t = 0;
    char s[100];
    cin >> s;
    delete[] x.coef;
    x.len = (strlen(s) - 1) / (BASE_SIZE / 4) + 1;
    x.maxlen = x.len;
    x.coef = new BASE[x.maxlen]{};
    while (i < strlen(s)) {
        t = s[i] - '0';
        x = x * (BASE)10 + (BASE)t;
        i++;
    }
    while (x.coef[x.len - 1] == 0 && (x.len > 1)) x.len--;
    return in;
}
*/








void test() {
    srand(time(0));
    int M = 1000, T = 1000, m, n;
     n = rand() % M + 1;
     m = rand() % M + 1;
    BigNumber A, B, C, D;
    do {
        cout << "T: " << T << endl;
        cout << "m and n: " << m << " " << n << endl;
        m = rand() % M + 1;
        n = rand() % M + 1;
        BigNumber a(m, 1), b(n, 1);
        A = a; B = b;
        C = A / B;
        D = A % B;
    } while (A == C * B + D && A - D == C * B && D < B && --T);
}





int main() {
    test();
    return 0;
}