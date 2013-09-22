#ifndef __MY_CONTAINER__
#define __MY_CONTAINER__

#include <cstdlib>
#include <cstring>

template<typename T>
class Container
/*Originally just used vector<Node*>, but it lead to an immediate, significant slowdown.
**Indexing into the vector instead of using iterators improved the speed, but this custom
**non-STL compatible container is still faster than using vectors, probably because it allows
**easy iteration by pointer arithmetic.
*/
{
public:
	Container();
	Container(T item);
	Container(const T* const those, unsigned num);
	Container(const Container<T>& that);
	~Container();
	T* get() const;
	T operator[] (unsigned index) const;
	unsigned size() const;
	unsigned capacity() const;
	unsigned extra_capacity() const;
	bool empty() const;
	T front() const;
	T back() const;
	void push_back(T item);
	void pop_back();
	T pop_off();
	void push_block(const T* const those, unsigned num);
	void push_block(const Container<T>& that);
	void shrink_to_fit();
	void reserve(unsigned size);
	void clear();
private:
	T* _items;
	unsigned _length, _number;
	void expand(unsigned extra);
};

//PUBLIC

template<typename T> inline
Container<T>::Container() {
	_number = 0u, _length = 1u;
	_items = (T*) malloc(sizeof(T));
}

template<typename T> inline
Container<T>::Container(T item) {
	_number = _length = 1u;
	_items = (T*) malloc(sizeof(T));
	*_items = item;
}

template<typename T> inline
Container<T>::Container(const T* const those, unsigned num) {
	_number = num;
	if (num != 0u) {
		c_length = (3u * num) / 2u + 1u;
		_items = (T*) malloc(_length * sizeof(T));
		memcpy(_items, those, num * sizeof(T));
	} else {
		_length = 1u;
		_items = (T*) malloc(sizeof(T));
	}
}

template<typename T> inline
Container<T>::Container(const Container<T>& that) {
	_number = that._number;
	if (_number != 0u) {
		_length = (3u * _number) / 2u + 1u;
		_items = (T*) malloc(_length * sizeof(T));
		memcpy(_items, that._items, _number * sizeof(T));
	} else {
		_length = 1u;
		_items = (T*) malloc(sizeof(T));
	}
}

template<typename T> inline
Container<T>::~Container() { free(_items); }

template<typename T> inline
T* Container<T>::get() const { return _items; }

template<typename T> inline
T Container<T>::operator[] (unsigned index)  const { return _items[index]; }

template<typename T> inline
unsigned Container<T>::size() const { return _number; }

template<typename T> inline
unsigned Container<T>::capacity() const { return _length; }

template<typename T> inline
unsigned Container<T>::extra_capacity() const { return _length - _number; }

template<typename T> inline
bool Container<T>::empty() const { return _number == 0u; }

template<typename T> inline
T Container<T>::front() const { return *_items; }

template<typename T> inline
T Container<T>::back() const { return _items[_number - 1u]; }

template<typename T> inline
void Container<T>::push_back(T item) {
	if (_number >= _length) expand(_number + 1u);
	_items[_number++] = item;
}

template<typename T> inline
void Container<T>::pop_back() { --_number; }

template<typename T> inline
T Container<T>:: pop_off() { return _items[--_number]; }

template<typename T> inline
void Container<T>::push_block(const T* const those, unsigned num) {
	if (num != 0u) {
		if (num == 1u) push_back(those[0]);
		else {
			int diff = int(num + _number) - int(_length);
			if (diff > 0) expand(diff + 1u);
			memcpy(_items + _number, those, num * sizeof(T));
			_number += num;
		}
	}
}

template<typename T> inline
void Container<T>::push_block(const Container<T>& that) { push_block(that._items, that._number); }

template<typename T> inline
void Container<T>::shrink_to_fit() {
	_length = (_number == 0u) ? 1u : _number;
	_items = (T*) realloc(_items, _length * sizeof(T));
}

template<typename T> inline
void Container<T>::reserve(unsigned size) {
	if (size > _length) {
		T* temp = _items;
		_length = size;
		_items = (T*) malloc(_length * sizeof(T));
		memcpy(_items, temp, _number * sizeof(T));
		free(temp);
	}
}

template<typename T> inline
void Container<T>::clear() { _number = 0u; }

//PRIVATE

template<typename T> inline
void Container<T>::expand(unsigned extra) { reserve(_length + extra); }

#endif