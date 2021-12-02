#ifndef CUSTOM_STRING
#define CUSTOM_STRING

class String {
private:
	int length;
	void copy(const char* from, char* to);
public:
	String(const char* value);
	~String();
	char* value;
	static String getInput(const int maxInputSize);
	String operator+ (const char* value);
	String operator+ (const char value);
	bool operator== (const char* value);
	bool operator== (const char value);
	bool operator== (const String value);
};

#endif


