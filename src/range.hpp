#ifndef RANGE_HPP
#define RANGE_HPP

#include <iterator>

/**
 * A simple C++-like iterator for python-style range(...) iterable
 */
template<typename VAL>
class range
{
	private:
		VAL begin_val;
		VAL offset;
		VAL end_val;
	public:
		range(const VAL& last) : begin_val(0), offset(1), end_val(last) { }
		range(const VAL& first, const VAL& last) : begin_val(first), offset(1), end_val(last) { }
		range(const VAL& first, const VAL& last, const VAL& offset) : begin_val(first), offset(offset), end_val(last) { }
		
		class iterator : public std::iterator<std::input_iterator_tag, VAL>
		{
			friend class range<VAL>;
			private:
				VAL current, offset;
				iterator(VAL val, VAL offset) : current(val), offset(offset) { };
			public:
				iterator& operator++() {current += offset; return *this;}
				iterator operator++(int) {iterator tmp(*this); operator++(); return tmp;}
				bool operator==(const iterator& rhs) {return current==rhs.current;}
				bool operator!=(const iterator& rhs) {return current!=rhs.current;}
				VAL& operator*() {return current;}
		};
		
		typedef iterator const_iterator;
		
		iterator begin() const { return iterator(begin_val, offset); }
		iterator end() const { return iterator(end_val, offset); }
		
		size_t size() const { return (end_val - begin_val) / offset; }
};

#endif // RANGE_HPP
