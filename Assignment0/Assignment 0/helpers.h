#ifndef helpers_h
#define helpers_h

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <sstream>

// Code to include the opengl and glut depending on your platform
#ifdef __APPLE__
#include <GLUT/glut.h>
#elif _WIN32
#include <windows.h>
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif
// ----------------------


#include "vecmath.h"

using namespace std; // So that you don't need to type std::

// Some typedefs to make you type less
typedef Vector2f V2;
typedef Vector3f V3;
typedef Vector4f V4;
typedef Matrix2f M2;
typedef Matrix3f M3;
typedef Matrix4f M4;
typedef Quat4f Q4;
typedef vector<Vector2f> V2v;
typedef vector<Vector3f> V3v;
typedef vector<Vector4f> V4v;


// Some macros to make iterating a bit less verbose
#define FOREACH(element, container) \
for (auto __it ## element = container.begin(); __it ## element != container.end(); ++__it ## element) \
for (bool __once ## element = 1; __once ## element; ) \
for (auto &element = *__it ## element; __once ## element; __once ## element = 0)

#define FOREACH_WITH_INDEX(element, index, container) \
for (size_t __once0 ## element = 1, __once1 ## element = 1, index = 0; __once0 ## element; __once0 ## element = 0) \
for (auto __it ## element = container.begin(); __it ## element != container.end(); ++__it ## element, ++index, __once1 ## element = 1) \
for (auto &element = *__it ## element; __once1 ## element; __once1 ## element = 0)

#define FOREACH_MAP(key, value, map) \
for (auto __it ## key = map.begin(); __it ## key != map.end(); ++__it ## key) \
for (bool __once0 ## key = 1, __once1 ## key = 1; __once0 ## key; ) \
for (auto &key = __it ## key->first; __once0 ## key; __once0 ## key = 0) \
for (auto &value = __it ## key->second; __once1 ## key; __once1 ## key = 0)

// Overloads << for vectors to become shorthand for push_back
template <class Type, class Allocator, class ValueType>
inline std::vector<Type, Allocator> & operator <<
(std::vector<Type, Allocator> &v, const ValueType &element)
{
	v.push_back(element);
	return v;
}

// Overloads << for sets to become shorthand for insert
template <class Key, class Compare, class Allocator, class ValueType>
inline std::set<Key, Compare, Allocator> & operator <<
(std::set<Key, Compare, Allocator> &s, const ValueType &element)
{
	s.insert(element);
	return s;
}

struct __MaxMake
{
	template <class T> struct __max
	{
		T m;
		
		inline __max<T> (const T &i): m(i) {}
		
		inline __max & operator , (const T &i)
		{
			if (m < i) m = i;
			return *this;
		}
	};
	
	template <class T> inline __max<T> operator , (const T &i)
	{
		return __max<T>(i);
	}
};

/**
 MAX macro
 @param ... several values to find the maximum of
 @return the maximum of the values
 */
#define MAX(...) ((__MaxMake(),__VA_ARGS__).m)


struct __MinMake
{
	template <class T> struct __min
	{
		T m;
		
		inline __min<T> (const T &i): m(i) {}
		
		inline __min & operator , (const T &i)
		{
			if (m < i) m = i;
			return *this;
		}
	};
	
	template <class T> inline __min<T> operator , (const T &i)
	{
		return __min<T>(i);
	}
};

/**
 MIN macro
 @param ... several values to find the minimum of
 @return the minimum of the values
 */
#define MIN(...) ((__MinMake(),__VA_ARGS__).m)


struct __StrJoiner
{
	std::stringstream ss;
	template <class T> __StrJoiner& operator , (const T &i)
	{
		ss << i;
		return *this;
	}
};

/**
 STR macro
 @param ... several values of mixed type to join together
 @return a std::string with the joined values
 */
#define STR(...) ((__StrJoiner(),__VA_ARGS__).ss.str())


// Helper class and Macro to quick initialize vectors for vs2010 backwards compatibility.
struct __VecMake
{
	template <class T> struct __vec
	{
		std::vector<T> v;
		
		__vec & operator , (const T &i)
		{
			v.push_back(i);
			return*this;
		}
		
		__vec & operator , (const std::vector<T> &_v)
		{
			v.insert(v.end(), _v.begin(), _v.end());
			return *this;
		}
		__vec & operator , (const std::set<T> &_s)
		{
			v.insert(v.end(), _s.begin(), _s.end());
			return *this;
		}
	};
	
	template <class T> __vec <T> operator , (const T &i)
	{
		return __vec<T>(), i;
	}
};

/**
 VEC macro
 @param ... several values to make a std::vector out off. Must be of the same type
 @return a std::vector with the values
 */
#define VEC(...) ((__VecMake(),__VA_ARGS__).v)


// Helper class and Macro to quick initialize sets for vs2010 backwards compaitbility
struct __SetMake
{
	template <class T> struct __set
	{
		std::set<T> s;
		
		__set & operator , (const T &i)
		{
			s.insert(i);
			return *this;
		}
		
		__set & operator , (const std::set<T> &_s)
		{
			s.insert(_s.begin(), _s.end());
			return *this;
		}
		__set & operator , (const std::vector<T> &_v)
		{
			s.insert(_v.begin(), _v.end());
			return *this;
		}
	};
	
	template <class T> __set <T> operator , (const T &i)
	{
		return __set<T>(),i;
	}
};
/**
 SET macro
 @param ... several values to make a std::set out off. Must be of the same type
 @return a std::set with the values
 */
#define SET(...) ((__SetMake(),__VA_ARGS__).s)



/**
 Splits a std::string into a std::vector<T>
 @param s the string to split
 @param delimiterChars the characters to split on
 @param defaultValue the value to add when an empty / invalid element is detected
 @return a std::vector<T> representing the splited string
 */
template <class T> vector<T> split(const string &s, const string &delimiterChars, const T &defaultValue)
{
	vector<T> elems;
	stringstream ss;
	ss.str(s);
	string tokenString;
	stringstream sss;
	size_t prev = 0, pos;
	while ((pos = s.find_first_of(delimiterChars, prev)) != string::npos) {
		sss.str(s.substr(prev, pos-prev));
		T token;
		if (pos > prev && sss >> token) {
			elems.push_back(token);
		} else {
			elems.push_back(defaultValue);
		}
		prev = pos + 1;
		sss.clear();
	}
	if (prev < s.length()) {
		T token;
		sss.str(s.substr(prev, string::npos));
		if (sss >> token) {
			elems.push_back(token);
		} else {
			elems.push_back(defaultValue);
		}
	}
	return elems;
}

#endif