#ifndef VARIANT_H
#define VARIANT_H

#include <string>
#include "Date.h"
#include <sstream>

using namespace std;

class variant
{
public:

	enum vt_type { vt_empty, vt_boolean, vt_integer,vt_long, vt_float, vt_date, vt_string, vt_ptr };

	variant(vt_type vt); 
	/*{
		vt_ = vt; 
		if (vt_==vt_integer)
			int64_val_ = 0;
		else if (vt_==vt_float)
			int64_val_ = 0.0;
		else if (vt_==vt_ptr)
			ptr_val_ = 0;
		else if (vt_==vt_boolean)
			boolean_val_ = false;
	}*/
	variant() ;//{ vt_ = vt_empty; }
	~variant() ;//{}

	variant(void* v);// { ptr_val_ = v; vt_ = vt_ptr; }
	variant(bool v);// { boolean_val_ = v; vt_ = vt_boolean; }
	variant(int v);// { int64_val_ = v; vt_ = vt_integer; }
	variant(unsigned int v);// { int64_val_ = (int)v; vt_ = vt_integer; }
	variant(long v);// { int64_val_ = v; vt_ = vt_long; }
	variant(float v);// { int64_val_ = v; vt_ = vt_float; }
	variant(double v);// { int64_val_ = v; vt_ = vt_float; }
	variant(const Date& v);// { date_val_ = v; vt_ = vt_date; }
	variant(const std::string& v);// { string_val_ = v; vt_ = vt_string; }
	variant(const char* v, size_t nbytes);// { string_val_ = std::string(v, nbytes); vt_ = vt_string; }

	variant(const variant& v);
	/*{
		vt_ = v.vt_;

		if (vt_ == vt_boolean)
			boolean_val_ = v.boolean_val_;
		else if (vt_ == vt_integer)
			int64_val_ = v.int64_val_;
		else if ((vt_ == vt_float)||(vt_==vt_long))
			int64_val_ = v.int64_val_;
		else if (vt_ == vt_date)
			date_val_ = v.date_val_;
		else if (vt_ == vt_ptr)
			ptr_val_ = v.ptr_val_;
		else if (vt_ == vt_string)
			string_val_ = v.string_val_;
	}*/

	bool operator == (const variant& v);
	/*{
		if (vt_ != v.vt_)
			return false;

		if (vt_ == vt_boolean)
			return boolean_val_ == v.boolean_val_;
		else if (vt_ == vt_integer)
			return int64_val_ == v.int64_val_;
		else if ((vt_ == vt_float)||(vt_==vt_long))
			return int64_val_ == v.int64_val_;
		else if (vt_ == vt_date)
			return date_val_ == v.date_val_;
		else if (vt_ == vt_string)
			return string_val_ == v.string_val_;
		else
			return false;
	}*/

	bool operator != (const variant& v);
	/*{
		if (vt_ != v.vt_)
			return true;
		return !(*this==v);
	}*/

	bool operator < (const variant& v);
	/*{
		if (vt_ != v.vt_)
			return false;

		if (vt_ == vt_integer)
			return int64_val_ < v.int64_val_;
		else if ((vt_ == vt_float)||(vt_==vt_long))
			return int64_val_ < v.int64_val_;
		else if (vt_ == vt_date)
			return date_val_ < v.date_val_;
		else if (vt_ == vt_string)
			return string_val_ < v.string_val_;
		else
			return false;
	}*/

	bool operator <= (const variant& v);
	/*{
		if (vt_ != v.vt_)
			return false;
		return (*this==v)||(*this<v);
	}*/

	bool operator > (const variant& v);
	/*{
		if (vt_ != v.vt_)
			return false;
		return !(*this<=v);
	}*/

	bool operator >= (const variant& v);
	/*{
		if (vt_ != v.vt_)
			return false;
		return (*this)>v||((*this)==v);
	}*/

	void* operator = (void* v); 
	/*{ 
		if ((vt_==vt_integer)||(vt_==vt_long))
			int64_val_ = 0;
		else if (vt_==vt_float)
			int64_val_ = 0.0;
		else if (vt_==vt_ptr)
			ptr_val_ = v;
		else if (vt_==vt_boolean) 
			boolean_val_ = false;
		else if (vt_==vt_empty)
		{ 
			vt_ = vt_ptr;
			ptr_val_ = v;
		}
		return v;
	}*/

	bool operator = (const bool& v);
	/*{
		if (vt_==vt_integer)
			int64_val_ = (int)v;
		else if (vt_==vt_float)
			int64_val_ = (int)v;
		else if(vt_==vt_long)
			int64_val_=(int)v;
		else if (vt_==vt_ptr)
			ptr_val_ = 0;
		else if (vt_==vt_boolean)
			boolean_val_ = (v?true:false);
		else if (vt_==vt_empty)
		{
			vt_ = vt_boolean;
			boolean_val_ = (v?true:false);
		}
		return v;
	}*/
	int operator = (const int& v);
	/*{ 
		if (vt_==vt_integer)
			int64_val_ = v;
		else if ((vt_==vt_float)||(vt_==vt_long))
			int64_val_ = v;
		else if (vt_==vt_ptr)
			ptr_val_ = 0;
		else if (vt_==vt_boolean)
			boolean_val_ = (v?true:false);
		else if (vt_==vt_empty)
		{ 
			vt_ = vt_integer;
			int64_val_ = v; 
		}
		return v;
	}*/
	unsigned int operator = (const unsigned int& v);
	/*{
		if (vt_==vt_integer)
			int64_val_ = v;
		else if ((vt_==vt_float)||(vt_==vt_long))
			int64_val_ = v;
		else if (vt_==vt_ptr)
			ptr_val_ = 0;
		else if (vt_==vt_boolean)
			boolean_val_ = (v?true:false);
		else if (vt_==vt_empty)
		{
			vt_ = vt_integer;
			int64_val_ = v;
		}
		return v;
	}*/

	long operator = (const long& v);
	/*{
		if (vt_==vt_integer)
			int64_val_ = v;
		else if ((vt_==vt_float)||(vt_==vt_long))
			int64_val_ = v;
		else if (vt_==vt_ptr)
			ptr_val_ = 0;
		else if (vt_==vt_boolean)
			boolean_val_ = (v?true:false);
		else if (vt_==vt_empty)
		{ 
			vt_ = vt_integer;
			int64_val_ = v;
		}
		return v;
	}*/
	float operator = (const float& v);
	/*{
		if (vt_==vt_integer) 
			int64_val_ = v;
		else if ((vt_==vt_float)||(vt_==vt_long))
			int64_val_ = v;
		else if (vt_==vt_ptr)
			ptr_val_ = 0;
		else if (vt_==vt_boolean)
			boolean_val_ = (v?true:false);
		else if (vt_==vt_empty)
		{ 
			vt_ = vt_float;
			int64_val_ = v; 
		} 
		return v;
	}*/
	double operator = (double v); 
	/*{ 
		if ((vt_==vt_integer)||(vt_==vt_float)||(vt_==vt_long))
			int64_val_ = v;
		else if (vt_==vt_ptr)
			ptr_val_ = 0;
		else if (vt_==vt_boolean)
			boolean_val_ = (v?true:false);
		else if (vt_==vt_empty) 
		{ 
			vt_ = vt_float; int64_val_ = v;
		} 
		return v; 
	}*/
	Date operator = (const Date& v);
	/*{ 
		if (vt_==vt_date) 
			date_val_ = v; 
		//else if (vt_==vt_string)
		//	string_val_ = v.toString();
		else if (vt_==vt_empty)
		{ 
			vt_ = vt_date;
			date_val_ = v; 
		} 
		return v;
	}*/
	const std::string operator = (const char* p);
	/*{
		if (vt_==vt_string)
			string_val_ = p; 
		else if (vt_==vt_empty) 
		{
			vt_ = vt_string;
			string_val_ = p; 
		}
		return p;
	}*/
	const std::string operator = (const std::string& v);
	/*{
		if (vt_==vt_string)
			string_val_ = v; 
		else if (vt_==vt_date)
			date_val_ = Date(v.c_str());
		else if (vt_==vt_empty) 
		{
			vt_ = vt_string;
			string_val_ = v; 
		}
		return v;
	}*/

	const variant& operator = (const variant& v);
	/*{
		clear();
		vt_ = v.vt_;

		if (vt_ == vt_boolean)
			boolean_val_ = v.boolean_val_;
		else if ((vt_ == vt_integer)||(vt_==vt_float)||(vt_==vt_long))
			int64_val_ = v.int64_val_;
		else if (vt_ == vt_date)
			date_val_ = v.date_val_;
		else if (vt_ == vt_ptr)
			ptr_val_ = v.ptr_val_;
		else if (vt_ == vt_string)
			string_val_ = v.string_val_;
		return v;
	}*/

	bool isEmpty() const; //{ return vt_ == vt_empty; }
	bool isBool() const; // { return vt_ == vt_boolean; }
	bool isInteger() const; // { return vt_ == vt_integer; }
	bool isFloat() const ; //{ return vt_ == vt_float; }
	bool isDate() const ; //{ return vt_ == vt_date; }
	bool isString() const; // { return vt_ == vt_string; }
	bool isPtr() const ; //{ return vt_ == vt_ptr; }
	bool isLong() const; // { return vt_== vt_long; }

	void* asPtr() const; // { return vt_==vt_ptr?ptr_val_:0; }
	bool asBool() const ; //{ return vt_==vt_boolean?boolean_val_:false; }
	float asFloat() const; //
	/*{
		return vt_==vt_integer?(float)int64_val_:vt_==vt_float?(float)int64_val_:vt_==vt_string?atof(string_val_.c_str()):0.0;
	}*/
	int asInt() const; //
	/*{
		return vt_==vt_integer?(int)int64_val_:vt_==vt_float?(int)int64_val_:vt_==vt_string?atoi(string_val_.c_str()):0;
	}*/
	long asLong() const ;
	/*{ 
		return vt_==vt_integer?(long)int64_val_:vt_==vt_float?(long)int64_val_:vt_==vt_string?atol(string_val_.c_str()):0;
	}*/
	Date asDate()  ; //
	/*{
		return vt_==vt_date?date_val_:Date();
	}*/
	std::string asStringEx(); 
	/*{
		if(vt_==vt_string)
			return string_val_;
		if(vt_== vt_date)
			return date_val_.toString();
		else
		{
			static stringstream stream;
			stream.str("");
			stream<<int64_val_;
			return stream.str();
		}
	}*/
	operator bool () const ;//{ return asBool(); }
	operator int () const ;//{ return asInt(); }
	operator unsigned int () const ;//{ return (unsigned int)asInt(); }
	operator long () const ;//{ return asLong(); }
	operator float () const;// { return (float)asFloat(); }
	operator double () const ;//{ return asFloat(); }
	operator Date () ;// { return asDate(); }
	operator std::string () ;//{ return asStringEx(); }


	void clear();
	/*{
		if (vt_ == vt_string)
			string_val_.clear();
		vt_ = vt_empty;
	}*/

	int get_type() const ;//const{ return vt_; }

	void change_type(vt_type vt_t);
	/*{
		if(this->isEmpty())
			this->vt_ = vt_t;
	}*/
private:

	union
	{
		double  int64_val_;
		void* ptr_val_;
		bool boolean_val_;
	};
public:
	std::string string_val_;
private:
	Date date_val_;
	vt_type vt_;
};

#endif

