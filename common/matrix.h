#ifndef MATRIX_H
#define MATRIX_H

#include <cassert>
#include <algorithm>

#include "types.h"

#define FOLLOWTHEWHITERABBIT

/**
 * General purpose 2D array container thing.
 */
template <class T>
class Matrix
{
private:
    T* _data;
    uint _width;
    uint _height;

public:
    Matrix(uint w, uint h)
        : _width(w)
        , _height(h)
    {
        if (w > 0 && h > 0)
        {
            _data = new T[w * h];
            std::fill(_data, _data + w * h, T());
        }
        else
            _data = 0;
    }

    Matrix(uint w, uint h, T* d)
        : _width(w)
        , _height(h)
    {
        if (w > 0 && h > 0)
        {
            _data = new T[w * h];
            std::copy(d, d + w * h, _data);
        }
        else
            _data = 0;
    }

    Matrix(const Matrix& rhs)
        : _width(rhs._width)
        , _height(rhs._height)
    {
        if (rhs._data)
        {
            _data = new T[_width * _height];
            std::copy(rhs._data, rhs._data + _width * _height, _data);
        }
        else
            _data = 0;
    }

    Matrix()
        : _width(0)
        , _height(0)
        , _data(0)
    {}

    ~Matrix()
    {
        delete[] _data;
    }

    T& operator ()(uint x, uint y)
    {
        if (x >= _width || y >= _height)
        {
            static T blah;
            return blah;
        }

        return _data[y * _width + x];
    }

    const T& operator ()(uint x, uint y) const
    {
        return (*const_cast<Matrix<T>*>(this))(x, y);
    }

    // allows efficient sequential access
    // TODO: iterator thing for extra safety
    // and robustness
    const T* GetPointer(uint x, uint y) const
    {
        assert(x >= 0 && y >= 0);
        assert(x < _width && y < _height);

        return _data + (y * _width) + x;
    }

    uint Width() const  { return _width; }
    uint Height() const { return _height; }
    bool Empty() const { return _data == 0; }

    void Resize(uint newx, uint newy)
    {
        if (newx == 0 || newy == 0)
        {
            _width = _height = 0;
            delete[] _data;
            _data = 0;
        }
        else if (newx != _width || newy != _height) // eliminate the trivial case
        {
            int sx = newx < _width  ? newx : _width;
            int sy = newy < _height ? newy : _height;

            T* tempData = new T[newx * newy];
            std::fill(tempData, tempData + newx * newy, T());

            T* src = _data;
            T* dest = tempData;

            while (sy)
            {
                std::copy(src, src + sx, dest);
                src += _width;
                dest += newx;
                sy--;
            }

            delete[] _data;
            _data = tempData;
            _width  = newx;
            _height = newy;
        }
    }

    Matrix& operator = (const Matrix& rhs)
    {
        delete[] _data;
        _width = rhs._width;
        _height = rhs._height;

        if (rhs._data)
        {
            _data = new T[_width * _height];
            std::copy(rhs._data, rhs._data + _width * _height, _data);
        }
        else
            _data = 0;

        return *this;
    }
};

#endif
