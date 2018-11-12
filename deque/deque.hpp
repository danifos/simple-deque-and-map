#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu { 

const size_t block_size = 512;

template<class T>
class deque {
	// private members

	// utility class

	// node of the linked list, each represent a block of memory
	// elements in this block is stored just like a vector (expect for double_space)
	class node {
	public:
		T* data;  // elements stored in the block
		size_t len;  // number of the elements
		const size_t size;  // number of the maximum elements that can be stored
		node *next;  // next node
		node *prev;  // preb node

		node(node *p=nullptr, node *n=nullptr):
			next(n), prev(p), len(0), size(block_size) {
			data = new T[size];
		}
	} *front, *rear;
	// front: the head of the block link (front doesn't store elements)
	// rear: the tail of the block link (rear doesn't store elements)
	// nullptr <- front <-> node <-> ... <-> node <-> rear -> nullptr

	size_t len;  // to make size() O(1)

public:
	class const_iterator;
	class iterator {
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
		T* first;
		T* last;
		T* cur;
		node *block;
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */
		iterator operator+(const int &n) const {
			//TODO
			iterator ret;
			ret.block = block;

			int idx = n+(cur-first);
			while(idx >= ret.block->len)
			{
				idx -= ret.block->len;
				block = ret.block->next;
			}
			ret.first = ret.block->data;
			ret.last = ret.first+ret.block->len;
			ret.cur = ret.first+idx;

			return ret;
		}
		iterator operator-(const int &n) const {
			//TODO
			iterator ret;
			ret.block = block;

			int idx = n+(cur-first);
			while(idx > ret.block->len)
			{
				idx -= ret.block->len;
				block = ret.block->prev;
			}
			ret.first = ret.block->data;
			ret.last = ret.first+ret.block->len;
			ret.cur = ret.last-idx;

			return ret;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			//TODO
			int ret;
			if(block == rhs.block)  // in the same block
			{
				ret = cur-rhs.cur;
			}
			else
			{
				node *cur;
				// find rhs to the rear
				ret = 0;
				for(cur = block->next; cur != rhs.block && cur != rear; cur = cur->next)
					ret += cur->len;
				if(cur == rear)  // not found
				{
					// find rhs to the front
					ret = 0;
					for(cur = block->rear; cur != rhs.block && cur != front; cur = cur->prev)
						ret += cur->len;
					if(cur == front)  // not found
						throw invalid_iterator();
					// rhs before this
					ret += (cur-first+1) + (rhs.last-rhs.cur);
				}
				else  // rhs after this
				{
					ret += (last-cur) + (rhs.cur-rhs.first+1);
				}
			}
			return ret;
		}
		iterator operator+=(const int &n) {
			//TODO
			int idx = n+(cur-first);
			while(idx >= block->len)
			{
				idx -= block->len;
				block = block->next;
			}
			first = block->data;
			last = first+block->len;
			cur = first+idx;

			return *this;
		}
		iterator operator-=(const int &n) {
			//TODO
			int idx = n+(last-cur);
			while(idx > block->len)
			{
				idx -= block->len;
				block = block->prev;
			}
			first = block->data;
			last = first+block->len;
			cur = last-idx;

			return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator ret = *this;
			if(++cur == last)
			{
				block = block->next;
				first = block->data;
				last = first+block->len;
				cur = first;
			}
			return ret;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
			if(++cur == last)
			{
				block = block->next;
				first = block->data;
				last = first+block->len;
				cur = first;
			}
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			iterator ret = *this;
			if(cur-- == first)
			{
				block = block->prev;
				first = block->data;
				last = first+block->len;
				cur = last-1;
			}
			return ret;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
			if(cur-- == first)
			{
				block = block->prev;
				first = block->data;
				last = first+block->len;
				cur = last-1;
			}
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const {
			return *cur;
		}
		/**
		 * TODO it->field
		 */
		T* operator->() const noexcept {
			return cur;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return cur == rhs.cur;
		}
		bool operator==(const const_iterator &rhs) const {
			return cur == rhs.cur;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return cur != rhs.cur;
		}
		bool operator!=(const const_iterator &rhs) const {
			return cur != rhs.cur;
		}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			// data members.
			const T* first;
			const T* last;
			const T* cur;
			const node *block;
		public:
			const_iterator() {
				// TODO
				// nothing to do
			}
			const_iterator(const const_iterator &other) {
				// TODO
				first = other.first;
				last = other.last;
				cur = other.cur;
				block = other.block;
			}
			const_iterator(const iterator &other) {
				// TODO
				first = other.first;
				last = other.last;
				cur = other.cur;
				block = other.block;
			}
			// And other methods in iterator.
			// And other methods in iterator.
			// And other methods in iterator.
	};
	/**
	 * TODO Constructors
	 */
	deque() {
		// front = new node; rear = new node(front); front->next = rear;
		rear = (front = new node)->next = new node(front);
	}
	deque(const deque &other) {
		rear = (front = new node)->next = new node(front);
		for(node *cur = other.front->next; cur != other.rear; cur = cur->next)
		{
			// rear->next = new node(rear); rear = rear->next;
			rear = rear->next = new node(rear);
			std::copy(cur->data, cur->data+cur->size, rear->data);
			rear->len = cur->len;
		}
	}
	/**
	 * TODO Deconstructor
	 */
	~deque() {
		node *p = front, *q;
		while(p)
		{
			q = p->next;
			delete[] p->data;
			delete p;
			p = q;
		}
	}
	/**
	 * TODO assignment operator
	 */
	deque &operator=(const deque &other) {  // just do what deque*(const deque &other) do
		rear = (front = new node)->next = new node(front);
		for(node *cur = other.front->next; cur != other.rear; cur = cur->next)
		{
			rear = rear->next = new node(rear);
			std::copy(cur->data, cur->data+cur->size, rear->data);
			rear->len = cur->len;
		}
	}
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	T & at(const size_t &pos) {
		if(pos >= len)
			throw index_out_of_bound();

		size_t idx;
		node *cur;

		if(pos < (len>>1))  // serach from front
		{
			idx = 0;
			for(cur = front->next; idx <= pos; cur = cur->next)
				idx += cur->len;
			cur = cur->prev;  // back to the prev one
			return cur->data[pos - (idx - cur->len)];
		}
		else  // search from rear
		{
			idx = len;
			for(cur = rear->prev; idx > pos; cur = cur->prev)
				idx -= cur->len;
			cur = cur->next;  // back to the next one
			return cur->data[pos - idx];
		}
	}
	const T & at(const size_t &pos) const {  // just do what T &at() do
		if(pos >= len) throw index_out_of_bound();
		size_t idx;
		node *cur;
		if(pos < (len>>1)) {
			idx = 0;
			for(cur = front->next; idx <= pos; cur = cur->next) idx += cur->len;
			cur = cur->prev;
			return cur->data[pos - (idx - cur->len)];
		}
		else {
			idx = len;
			for(cur = rear->prev; idx > pos; cur = cur->prev) idx -= cur->len;
			cur = cur->next;
			return cur->data[pos - idx];
		}
	}
	T & operator[](const size_t &pos) {  // just do what T &at() do
		if(pos >= len) throw index_out_of_bound();
		size_t idx;
		node *cur;
		if(pos < (len>>1)) {
			idx = 0;
			for(cur = front->next; idx <= pos; cur = cur->next) idx += cur->len;
			cur = cur->prev;
			return cur->data[pos - (idx - cur->len)];
		}
		else {
			idx = len;
			for(cur = rear->prev; idx > pos; cur = cur->prev) idx -= cur->len;
			cur = cur->next;
			return cur->data[pos - idx];
		}
	}
	const T & operator[](const size_t &pos) const {  // just do what T &at() do
		if(pos >= len) throw index_out_of_bound();
		size_t idx;
		node *cur;
		if(pos < (len>>1)) {
			idx = 0;
			for(cur = front->next; idx <= pos; cur = cur->next) idx += cur->len;
			cur = cur->prev;
			return cur->data[pos - (idx - cur->len)];
		}
		else {
			idx = len;
			for(cur = rear->prev; idx > pos; cur = cur->prev) idx -= cur->len;
			cur = cur->next;
			return cur->data[pos - idx];
		}
	}
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const {
		return front->next->data[0];
	}
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const {
		return rear->prev->data[rear->prev->len-1];
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
		iterator iter;
		iter.block = front->next;
		iter.first = iter.block->data;
		iter.last = iter.first+iter.block->size;
		iter.cur = iter.first;
		return iter;
	}
	const_iterator cbegin() const {
		const_iterator iter;
		iter.block = front->next;
		iter.first = iter.block->data;
		iter.last = iter.first+iter.block->size;
		iter.cur = iter.first;
		return iter;
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
		iterator iter;
		iter.block = rear;
		iter.first = iter.block->data;
		iter.last = iter.first;
		iter.cur = iter.first;
		return iter;
	}
	const_iterator cend() const {
		const_iterator iter;
		iter.block = rear;
		iter.first = iter.block->data;
		iter.last = iter.first;
		iter.cur = iter.first;
		return iter;
	}
	/**
	 * checks whether the container is empty.
	 */
	bool empty() const {
		return len == 0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
		return len;
	}
	/**
	 * clears the contents
	 */
	void clear() {}
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) {}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) {}
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) {}
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() {}
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) {}
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() {}
};

}

#endif
