#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <functional>

using namespace std;

inline string what(const exception_ptr& ex_ptr) {
	try {
		rethrow_exception(ex_ptr);
	}
	catch (const exception& e) {
		return e.what();
	}
}

template<typename T>
class Result {
	union{
		T val;
		std::exception_ptr exp;
	};
	bool has_val;
	Result() {};
public:
	Result(const T& v) :val{ v }, has_val{ true }{
		cout << "In Result's constructor" << endl;
	};
	Result(const Result& r) :has_val{ r.has_val }, exp{r.exp} {
		cout << "In Result's copy constructor" << endl;
	};
	template <typename E>
	static Result<T> from_exp(const E& e) {
		cout << "Getting exception info" << endl;
		auto exp_ptr = make_exception_ptr(e);
		Result<T> temp;
		temp.has_val = false;
		new(&temp.exp) exception_ptr(move(exp_ptr));
		return temp;
	}

	~Result() {
		if (has_val) {
			cout << "Deleting value in destructor" << endl;
			val.~T();
		}
		else {
			cout << "Deleting exception in destructor" << endl;
			exp.~exception_ptr();
		}
	}
	bool is_valid() { return has_val; }
	bool operator!() const { return has_val; }
	T get_val() const {
		if (has_val) {
			cout << "Returning value" << endl;
			return val;
		}
		cout << "Returning exception" << endl;
		rethrow_exception(exp);
		
	}

	exception_ptr get_exception()const {
		return exp;
	}
};


Result<int> parse(string str) {
	try {
		return stoi(str);
	}
	catch (...) {
		cout << "Returning exception" << endl;
		return Result<int>::from_exp(invalid_argument("Not a valid number!"));
	}
}

int main() {
	auto res = parse("as");
	if (res.is_valid()) {
		cout << "Value : " << res.get_val() << endl;
	}
	else {
		auto exp = res.get_exception();
		cout << what(exp) << endl;
	}
}
	
