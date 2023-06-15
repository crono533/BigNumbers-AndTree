#include <iostream>
#include <ctime>
#include <random>

#include <cstring>

using namespace std;

typedef unsigned char BASE;
typedef unsigned short DBASE;
#define BASE_SIZE (sizeof(BASE) * 8)
#define BASENUM ((DBASE)1 << BASE_SIZE)

mt19937 rnd(time(0));

class BN {
private:
	BASE* coef;
	int len, maxlen;

	BN shift(int x) {
		int m = len, n = len + x;
		BN bn(n); bn.len = n;
		for (int i = 0; i < m; i++) bn.coef[n - 1 - i] = coef[m - 1 - i];
		while (bn.coef[bn.len - 1] == 0 && (bn.len > 1)) bn.len--;
		return bn;
	}

public:
	BN(int m = 1, bool f = 0) : maxlen(m), len(1) {
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

	BN(const BN& bn) : len(bn.len), maxlen(bn.maxlen) {
		coef = new BASE[maxlen]{};
		for (int i = 0; i < len; coef[i] = bn.coef[i], i++);
	}

	~BN() { delete[] coef; }

	BN& operator = (const BN& bn) {
		if (this != &bn) {
			if (coef) delete[] coef;
			len = bn.len;
			maxlen = bn.maxlen;
			coef = new BASE[maxlen]{};
			for (int i = 0; i < len; i++) coef[i] = bn.coef[i];
		}
		return *this;
	}

	void InputHEX(const char* s) {
		delete[] coef;
		len = (strlen(s) - 1) / (BASE_SIZE / 4) + 1;
		maxlen = len;
		coef = new BASE[maxlen]{};
		int i, j = 0, k = 0;
		char tmp;
		for (i = strlen(s) - 1; i >= 0; i--) {
			if (('0' <= s[i]) && (s[i] <= '9'))
				tmp = s[i] - '0';
			else if (('a' <= s[i]) && (s[i] <= 'f'))
				tmp = s[i] - 'a' + 10;
			else if (('A' <= s[i]) && (s[i] <= 'F'))
				tmp = s[i] - 'A' + 10;
			else exit(1);
			coef[j] |= tmp << (k * 4); k++;
			if (k >= BASE_SIZE / 4) { k = 0; j++; }
		}
		while (coef[len - 1] == 0 && (len > 1)) len--;
		//if(len < 1) len = 1;
	}

	void OutputHEX() {
		//char s[BASE_SIZE / 4 * len + 1];
		char* s = new char[BASE_SIZE / 4 * len + 1];
		int i, j = 0, k;
		char tmp;
		bool f = 1;
		for (i = len - 1; i >= 0; i--) {
			k = BASE_SIZE - 4;
			while (k >= 0) {
				tmp = (coef[i] >> k) & 0xf;
				if (tmp == 0 && f) {
					k -= 4;
					continue;
				}
				else if (tmp != 0 && f) f = 0;
				if ((0 <= tmp) && (tmp <= 9))
					s[j] = (char)(tmp + '0');
				else if ((10 <= tmp) && (tmp <= 15))
					s[j] = (char)(tmp - 10 + 'a');
				j++; k -= 4;
			}
		}
		if (j == 0) s[j++] = '0';
		s[j] = '\0';
		cout << s;
		delete[] s;
	}

	bool operator > (const BN& bn) const {
		if (len != bn.len) return len > bn.len;
		for (int i = len - 1; i >= 0; i--)
			if (coef[i] != bn.coef[i]) return coef[i] > bn.coef[i];
		return 0;
	}
	bool operator < (const BN& bn) const {
		return (bn > *this);
	}
	bool operator >= (const BN& bn) const {
		return !(*this < bn);
	}
	bool operator <= (const BN& bn) const {
		return !(*this > bn);
	}
	bool operator == (const BN& bn) const { ///
		return (*this <= bn && *this >= bn);
	}
	bool operator != (const BN& bn) const {
		return !(*this == bn);
	}

	BN operator + (const BN& bn) {
		DBASE tmp;
		int n = len, m = bn.len;
		int l = max(n, m) + 1, t = min(n, m), i = 0, k = 0;
		BN sum(l); sum.len = l;
		while (i < t) {
			tmp = (DBASE)coef[i] + (DBASE)bn.coef[i] + (DBASE)k;
			sum.coef[i] = (BASE)tmp;
			k = (BASE)(tmp >> BASE_SIZE);
			i++;
		}
		while (i < n) {
			tmp = (DBASE)coef[i] + (DBASE)k;
			sum.coef[i] = (BASE)tmp;
			k = (BASE)(tmp >> BASE_SIZE);
			i++;
		}
		while (i < m) {
			tmp = (DBASE)bn.coef[i] + (DBASE)k;
			sum.coef[i] = (BASE)tmp;
			k = (BASE)(tmp >> BASE_SIZE);
			i++;
		}
		sum.coef[i] = k;
		if (sum.coef[i] == 0) sum.len--;
		return sum;
	}
	BN& operator += (const BN& bn) {
		*this = *this + bn;
		return *this;
	}

	BN operator - (const BN& bn) {
		DBASE tmp;
		int n = len, m = bn.len;
		int i = 0, k = 0;
		BN sub(n); sub.len = n;
		if (bn > *this) exit(2);
		while (i < m) {
			tmp = ((DBASE)1 << BASE_SIZE) | coef[i];
			tmp = tmp - bn.coef[i] - k;
			sub.coef[i] = (BASE)tmp;
			k = !(tmp >> BASE_SIZE);
			i++;
		}
		while (i < n) {
			tmp = ((DBASE)1 << BASE_SIZE) | coef[i];
			tmp -= k;
			sub.coef[i] = (BASE)tmp;
			k = !(tmp >> BASE_SIZE);
			i++;
		}
		while (sub.coef[sub.len - 1] == 0 && (sub.len > 1)) sub.len--;
		return sub;
	}
	BN& operator -= (const BN& bn) {
		*this = *this - bn;
		return *this;
	}

	BN operator * (const BASE& num) {
		DBASE tmp;
		int i = 0, n = len + 1;
		BASE k = 0;
		BN mul(n); mul.len = n;
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
	BN operator * (const BN& bn) {
		DBASE tmp;
		int n = len, m = bn.len;
		int l = n + m;
		BN mul(l); mul.len = l;
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
	BN& operator *= (const BN& bn) {
		*this = *this * bn;
		return *this;
	}

	BN operator + (const BASE& num) {
		DBASE tmp;
		int n = len + 1, i = 0, k = 0;
		BN sum(n); sum.len = n;

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

	BN operator / (const BASE& num) {//by number
		if (num == 0) exit(3);
		DBASE tmp;
		int n = len, i = 0;
		BASE r = 0;
		BN div(n); div.len = n;
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

	BN operator / (const BN& bn) {
		BN e;
		if (bn == e) exit(10);
		if (*this == bn) return (e + 1);
		if (*this < bn) return e;
		if (bn.len < 2) {
			BASE x;
			x = bn.coef[0];
			BN u(*this);
			u = u / x;
			return u;
		}
		int t = len, n = bn.len, m = t - n, i = 0, d = 0;
		BN u(*this), v(bn), q(m + 1); q.len = m + 1;
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

	BN operator % (const BN& bn) {
		BN e;
		if (bn == e) exit(10);
		if (*this == bn) return e;
		if (*this < bn) return *this;
		if (bn.len < 2) {
			BASE x;
			x = bn.coef[0];
			BN u;
			u.coef[0] = *this % x;
			return u;
		}
		int t = len, n = bn.len, m = t - n, i = 0, d = 0;
		BN u(*this), v(bn);
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

	int getL() { return len; }
	int getM() { return maxlen; }
	BASE getC(int i) { return coef[i]; }

	friend ostream& operator << (ostream&, BN&);
	friend istream& operator >> (istream&, BN&);


};

ostream& operator << (ostream& out, BN& bn) {
	char* s = new char[BASE_SIZE / 4 * bn.len + 1];
	int i = 0, t = 0;
	BN u(bn), x;
	while (u != x) {
		t = u % (BASE)10;
		s[i] = t + '0';
		u = u / (BASE)10;
		i++;
	}
	s[i--] = '\0';
	while (i >= 0) out << s[i--];
	return out;
}
istream& operator >> (istream& in, BN& bn) {
	int x = 0, i = 0, t = 0;
	char s[100];
	cin >> s;
	delete[] bn.coef;
	bn.len = (strlen(s) - 1) / (BASE_SIZE / 4) + 1;
	bn.maxlen = bn.len;
	bn.coef = new BASE[bn.maxlen]{};
	while (i < strlen(s)) {
		t = s[i] - '0';
		bn = bn * (BASE)10 + (BASE)t;
		i++;
	}
	while (bn.coef[bn.len - 1] == 0 && (bn.len > 1)) bn.len--;
	return in;
}

int main() {
	srand(time(0));
	int M = 1000, T = 1000, m, n;
	BN A, B, C, D;
	do {
		m = rand() % M + 1;
		n = rand() % M + 1;
		BN a(m, 1), b(n, 1);
		A = a; B = b;
		C = A / B;
		D = A % B;
		cout << T << ' ' << m << ' ' << n << endl;
	} while (A == C * B + D && A - D == C * B && D < B && --T);

}