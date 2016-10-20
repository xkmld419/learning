#include "Variant.h"
    
    variant::variant(vt_type vt) 
	{
/*		vt_ = vt; 
		if (vt_==vt_integer)
			int64_val_ = 0;
		else if (vt_==vt_float)
			int64_val_ = 0.0;
		else if (vt_==vt_ptr)
			ptr_val_ = 0;
		else if (vt_==vt_boolean)
			boolean_val_ = false;*/
	}
    
    variant::variant() 
    { 
        //vt_ = vt_empty; 
    }
    
	variant::~variant() 
	{
		//if((vt_==vt_ptr)&&(0==ptr_val_))
		//	delete ptr_val_;
	}
    
	variant::variant(void* v) 
	{ 
		//ptr_val_ = v; 
		//vt_ = vt_ptr; 
	}
	variant::variant(bool v) 
	{ 
	    //boolean_val_ = v; 
	    //vt_ = vt_boolean;
	}
	variant::variant(int v) 
	{ 
	    //int64_val_ = v; 
	    //vt_ = vt_integer; 
	}
	variant::variant(unsigned int v) 
	{ 
	    //int64_val_ = (int)v; 
	    //vt_ = vt_integer; 
	}
	variant::variant(long v) { /*int64_val_ = v; vt_ = vt_long;*/ }
	variant::variant(float v) {/* int64_val_ = v; vt_ = vt_float;*/ }
	variant::variant(double v) { /* int64_val_ = v; vt_ = vt_float;*/ }
	variant::variant(const Date& v) { /*date_val_ = v; vt_ = vt_date;*/ }
	variant::variant(const std::string& v) { /*string_val_ = v; vt_ = vt_string;*/ }
	variant::variant(const char* v, size_t nbytes) { /*string_val_ = std::string(v, nbytes); vt_ = vt_string;*/}


	variant::variant(const variant& v)
	{
		/*
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
			*/
	}

	bool variant::operator == (const variant& v)
	{
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
	}

	bool variant::operator != (const variant& v)
	{
		if (vt_ != v.vt_)
			return true;
		return !(*this==v);
	}

	bool variant::operator < (const variant& v)
	{
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
	}

	bool variant::operator <= (const variant& v)
	{
		if (vt_ != v.vt_)
			return false;
		return (*this==v)||(*this<v);
	}

	bool variant::operator > (const variant& v)
	{
		if (vt_ != v.vt_)
			return false;
		return !(*this<=v);
	}

	bool variant::operator >= (const variant& v)
	{
		if (vt_ != v.vt_)
			return false;
		return (*this)>v||((*this)==v);
	}

	void* variant::operator = (void* v) 
	{ 
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
	}

	bool variant::operator = (const bool& v)
	{
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
	}
	int variant::operator = (const int& v)
	{ 
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
	}
	unsigned int variant::operator = (const unsigned int& v)
	{
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
	}

	long variant::operator = (const long& v)
	{
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
	}
	float variant::operator = (const float& v)
	{
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
	}
	double variant::operator = (double v) 
	{ 
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
	}
	Date variant::operator = (const Date& v)
	{ 
		if (vt_==vt_date) 
			date_val_ = v; 
		/*else if (vt_==vt_string)
			string_val_ = v.toString();*/
		else if (vt_==vt_empty)
		{ 
			vt_ = vt_date;
			date_val_ = v; 
		} 
		return v;
	}
	const std::string variant::operator = (const char* p)
	{
		if (vt_==vt_string)
			string_val_ = p; 
		else if (vt_==vt_empty) 
		{
			vt_ = vt_string;
			string_val_ = p; 
		}
		return p;
	}
	const std::string variant::operator = (const std::string& v) 
	{
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
	}

	const variant& variant::operator = (const variant& v)
	{
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
	}

	bool variant::isEmpty() const 
	{ 
		return vt_ == vt_empty; 
	}
	bool variant::isBool() const { return vt_ == vt_boolean; }
	bool variant::isInteger() const { return vt_ == vt_integer; }
	bool variant::isFloat() const { return vt_ == vt_float; }
	bool variant::isDate() const { return vt_ == vt_date; }
	bool variant::isString() const { return vt_ == vt_string; }
	bool variant::isPtr() const { return vt_ == vt_ptr; }
	bool variant::isLong() const { return vt_== vt_long; }

	void* variant::asPtr() const { return vt_==vt_ptr?ptr_val_:0; }
	bool variant::asBool() const { return vt_==vt_boolean?boolean_val_:false; }
	float variant::asFloat() const
	{
		return vt_==vt_integer?(float)int64_val_:vt_==vt_float?(float)int64_val_:vt_==vt_string?atof(string_val_.c_str()):0.0;
	}
	int variant::asInt() const
	{
		return vt_==vt_integer?(int)int64_val_:vt_==vt_float?(int)int64_val_:vt_==vt_string?atoi(string_val_.c_str()):0;
	}
	long variant::asLong() const 
	{ 
		return vt_==vt_long?(long)int64_val_:vt_integer?(long)int64_val_:vt_==vt_float?(long)int64_val_:vt_==vt_string?atol(string_val_.c_str()):0;
	}
	Date variant::asDate()  
	{
		return vt_==vt_date?date_val_:Date();
	}
	std::string variant::asStringEx() 
	{
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
	}
	
	variant::operator bool () const { return asBool(); }
	variant::operator int () const { return asInt(); }
	variant::operator unsigned int () const { return (unsigned int)asInt(); }
	variant::operator long () const { return asLong(); }
	variant::operator float () const { return (float)asFloat(); }
	variant::operator double () const { return asFloat(); }
	variant::operator Date ()  { return asDate(); }
	variant::operator std::string ()  { return asStringEx(); }
	

	void variant::clear()
	{
		if (vt_ == vt_string)
			string_val_.clear();
		vt_ = vt_empty;
	}
	
	int variant::get_type() const 
	{ 
		return vt_; 
	}
	
	void variant::change_type(vt_type vt_t)
	{
		if(this->isEmpty())
			this->vt_ = vt_t;
	}
	
