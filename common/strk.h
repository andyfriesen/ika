
/*
	strk.h, coded by aen
	last updated: dec 19, 2k
*/

#ifndef STRK_INC
#define STRK_INC

#define STRK_LENGTH 1023

class string_k {
	char F_data[STRK_LENGTH + 1];

public:
	string_k();
	string_k(const char* source);
  	string_k(char source);

	friend string_k  operator+ (const string_k& a, const string_k& b);
	friend string_k& operator+=(string_k& lhs, const string_k& rhs);
	friend string_k& operator+=(string_k& lhs, char rhs); // for speed
    char operator [](int idx);    

	string_k mid  (int offset,
	               int count) const;
	string_k left (int count) const;
	string_k right(int count) const;

	const char* c_str() const;
//    operator const char*() const;
	int length() const;
    int toint() const;

	string_k& append(const string_k& source);
	string_k& insert(const string_k& source, int here);

	string_k  upper() const;
	string_k  lower() const;

	void zap();

	friend int compare    (const string_k& a, const string_k& b);
	friend int operator ==(const string_k& a, const string_k& b);
	friend int operator !=(const string_k& a, const string_k& b);
	friend int operator < (const string_k& a, const string_k& b);
	friend int operator >=(const string_k& a, const string_k& b);
	friend int operator > (const string_k& a, const string_k& b);
	friend int operator <=(const string_k& a, const string_k& b);
};

#endif // STRK_INC