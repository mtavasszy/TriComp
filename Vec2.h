#ifndef VEC2_H_ 
#define VEC2_H_

#define _USE_MATH_DEFINES

#include <cmath>
#include <math.h>
#include <iostream>

template<typename T> class Vec2;

template <class T> bool operator!= (const Vec2<T> & p1, const Vec2<T> & p2) {
    return (p1.x != p2.x || p1.y != p2.y);
}

template <class T> const Vec2<T> operator* (const Vec2<T> & p, float factor) {
    return Vec2<T> (p.x * factor, p.y * factor);
}

template <class T> const Vec2<T> operator* (float factor, const Vec2<T> & p) {
    return Vec2<T> (p.x * factor, p.y * factor);
}

template <class T> const Vec2<T> operator* (const Vec2<T> & p1, const Vec2<T> & p2) {
    return Vec2<T> (p1.x * p2.x, p1.y * p2.y);
}

template <class T> const Vec2<T> operator+ (const Vec2<T> & p1, const Vec2<T> & p2) {
    return Vec2<T> (p1.x + p2.x, p1.y + p2.y);
}

template <class T> const Vec2<T> operator- (const Vec2<T> & p1, const Vec2<T> & p2) {
    return Vec2<T> (p1.x - p2.x, p1.y - p2.y);
}

template <class T> const Vec2<T> operator+ (const Vec2<T> & p1, float f) {
    return Vec2<T> (p1.x + f, p1.y + f);
}

template <class T> const Vec2<T> operator- (const Vec2<T> & p1, float f) {
    return Vec2<T> (p1.x - f, p1.y - f);
}

template <class T> const Vec2<T> operator- (const Vec2<T> & p) {
    return Vec2<T> (-p.x, -p.y);
}

template <class T> const Vec2<T> operator/ (const Vec2<T> & p, float divisor) {
    return Vec2<T> (p.x/divisor, p.y/divisor);
}

template <class T> bool operator== (const Vec2<T> & p1, const Vec2<T> & p2) {
    return (p1.x == p2.x && p1.y == p2.y);
}

template <class T> bool operator< (const Vec2<T> & a, const Vec2<T> & b) {
    return (a.x < b.x && a.y < b.y);
}

template <class T> bool operator>= (const Vec2<T> & a, const Vec2<T> & b) {
    return (a.x >= b.x || a.y >= b.y);
}


/**
 * Vector in 2 dimensions, with basics operators overloaded.
 */
template <typename T>
class Vec2 {
public:
    inline Vec2 (void)	{
        x = y = T ();
    }
    inline Vec2 (T p) {
        x = p;
        y = p;
    };
    inline Vec2 (T p0, T p1) {
        x = p0;
        y = p1;
    };
    inline Vec2 (const Vec2 & v) {
        init (v.x, v.y);
    }
    inline Vec2 (T* pp) {
        x = pp.x;
        y = pp.y;
    };
    // ---------
    // Operators
    // ---------
    inline T& operator[] (int Index) {
        return (p[Index]);
    };
    inline const T& operator[] (int Index) const {
        return (p[Index]);
    };
    inline Vec2& operator= (const Vec2 & P) {
        x = P.x;
        y = P.y;
        return (*this);
    };
    inline Vec2& operator+= (const Vec2 & P) {
        x += P.x;
        y += P.y;
        return (*this);
    };
    inline Vec2& operator-= (const Vec2 & P) {
        x -= P.x;
        y -= P.y;
        return (*this);
    };
    inline Vec2& operator*= (const Vec2 & P) {
        x *= P.x;
        y *= P.y;
        return (*this);
    };
    inline Vec2& operator*= (T s) {
        x *= s;
        y *= s;
        return (*this);
    };
    inline Vec2& operator/= (const Vec2 & P) {
        x /= P.x;
        y /= P.y;
        return (*this);
    };
    inline Vec2& operator/= (T s) {
        x /= s;
        y /= s;
        return (*this);
    };

    //---------------------------------------------------------------

    inline Vec2 & init (T x, T y) {
        this->x = x;
        this->y = y;
        return (*this);
    };
    inline T getSquaredLength() const {
        return (dot(*this, *this));
    };
    inline T getLength() const {
        return (T)sqrt (getSquaredLength());
    };
    /// Return length after normalization
    inline Vec2 normalize () {
        T length = getLength();
        if (length == 0.0f)
            return Vec2(T());
        return Vec2(x, y) / length;
    };
    inline void fromTo (const Vec2 & P1, const Vec2 & P2) {
        x = P2.x - P1.x;
        y = P2.y - P1.y;
    };
    inline float transProduct (const Vec2 & v) const {
        return (x*v.x + y*v.y);
    }
    static inline Vec2 getOrthogonal (const Vec2& v) {
        return Vec2(v.y, -v.x);
    }
    static inline Vec2 segment (const Vec2 & a, const Vec2 & b) {
        Vec2 r;
        r.x = b.x - a.x;
        r.y = b.y - a.y;
        return r;
    };
    static inline T dot(const Vec2 & a, const Vec2 & b) {
        return (a.x * b.x + a.y * b.y);
    }
    static inline T squaredDistance (const Vec2 &v1, const Vec2 &v2) {
        Vec2 tmp = v1 - v2;
        return (tmp.getSquaredLength());
    }
    static inline T distance (const Vec2 &v1, const Vec2 &v2) {
        Vec2 tmp = v1 - v2;
        return (tmp.getLength());
    }
    static inline Vec2 interpolate (const Vec2 & u, const Vec2 & v, T alpha) {
        return (u * (1.0f - alpha) + v * alpha);
    }

	T * pointer()
	{
		return p;
	}

	const T * pointer() const
	{
		return p;
	}

    union
    {
        struct
        {
            T x, y;  // standard names for components
        };
        T p[2];  // array access
    };
};

template <class T> inline Vec2<T> swap (Vec2<T> & P, Vec2<T> & Q) {
    Vec2<T> tmp = P;
    P = Q;
    Q = tmp;
}

template <class T> std::ostream & operator<< (std::ostream & output, const Vec2<T> & v) {
    output << v.x << " " << v.y;
    return output;
}

template <class T> std::istream & operator>> (std::istream & input, Vec2<T> & v) {
    input >> v.x >> v.y;
    return input;
}

typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;
typedef Vec2<int> Vec2i;

#endif  // VEC2_H_