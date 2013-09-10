#ifndef __MY_CONTAINER__
#define __MY_CONTAINER__

template<typename T>
class Container
//Originally just used vector<Node*>, but it lead to an immediate, significant slowdown.
//Indexing into the vector instead of using iterators improved the speed, but this custom
//non-STL compatible container is still faster than using vectors, probably because it allows
//easy iteration by pointer arithmetic.
{
public:
	Container();
	Container(const T* const those, unsigned num);
	Container(const Container<T>& that);
	~Container();
	T* get() const;
	T& operator[] (unsigned index) const;
	unsigned size() const;
	bool empty() const;
	T& front() const;
	T& back() const;
	void push_back(const T& item);
	void pop_back();
	T& pop_off(); //This is dangerous, but an assumption is made that the return value will be copied.
	void push_block(const T* const those, unsigned num);
	void push_block(const Container<T>& that);
	void fit();
	void clear();
private:
	T* _items;
	unsigned _length, _number;
	void expand();
	void expand(unsigned extra);
};

//PUBLIC

template<typename T>
Container<T>::Container() {
	_number = 0u, _length = 1u;
	_items = (T*) malloc(_length * sizeof(T));
}

template<typename T>
Container<T>::Container(const T* const those, unsigned num) {
	if (num != 0u) {
		_number = num, _length = num + num/2u + 1u;
		_items = (T*) malloc(_length * sizeof(T));
		memcpy(_items, those, _number * sizeof(T));
	}
}

template<typename T>
Container<T>::Container(const Container<T>& that) {
	_number = that._number;
	if (_number != 0u) {
		_length = _number + (_number - that._length)/2u;
		_items = (T*) malloc(_length * sizeof(T));
		memcpy(_items, that._items, _number * sizeof(T));
	} else {
		_length = 1u;
		_items = (T*) malloc(_length * sizeof(T));
	}
}

template<typename T>
Container<T>::~Container() { free(_items); }

template<typename T>
T* Container<T>::get() const { return _items; }

template<typename T>
T& Container<T>::operator[] (unsigned index)  const { return _items[index]; }

template<typename T>
unsigned Container<T>::size() const { return _number; }

template<typename T>
bool Container<T>::empty() const { return _number == 0u; }

template<typename T>
T& Container<T>::front() const { return *_items; }

template<typename T>
T& Container<T>::back() const { return *(_items + _number - 1); }

template<typename T>
void Container<T>::push_back(const T& item) {
	if (_number >= _length) expand();
	_items[_number++] = item;
}

template<typename T>
void Container<T>::pop_back() { --_number; }

template<typename T>
T& Container<T>:: pop_off() { return _items[--_number]; }

template<typename T>
void Container<T>::push_block(const T* const those, unsigned num) {
	if (num == 0u) return;
	while (_length - _number < num) expand();
	memcpy(_items + ._number, those, num * sizeof(T));
	_number += num;
}

template<typename T>
void Container<T>::push_block(const Container<T>& that) {
	if (that._number != 0u) {
		if (int diff = this._length - this._number, diff < that._number) {
			expand(that._number - diff);
		}
		memcpy(this._items + this._number, that._items, that._number * sizeof(T));
		this._number += that._number;
	}
}

template<typename T>
void Container<T>::fit() {
	_length = (_number != 0u) ? _number : 1u;
	_items = (T*) realloc(_items, _length * sizeof(T));
}

template<typename T>
void Container<T>::clear() { _number = 0u; }

//PRIVATE

template<typename T>
void Container<T>::expand() {
	T* temp = _items;
	_length *= 2u;
	_items = (T*) malloc(_length * sizeof(T));
	memcpy(_items, temp, _number * sizeof(T));
	free(temp);
}

template<typename T>
void Container<T>::expand(unsigned extra) {
	T* temp = _items;
	_length += extra;
	_items = (T*) malloc(_length * sizeof(T));
	memcpy(_items, temp, _number * sizeof(T));
	free(temp);
}

#endif