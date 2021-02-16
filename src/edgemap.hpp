#ifndef EDGEMAP_HPP
#define EDGEMAP_HPP

#include <vector>

template<typename T>
class edgemap
{
	struct edge_props
	{
		bool exists;
		T value;
		edge_props() : exists(false), value() { }
	};
	
	std::vector<std::vector<edge_props> > map;
	size_t size;
	
	public:
		edgemap(size_t size) : map(size), size(size)
		{
			for(int i=0; i < size; ++i)
			{
				map[i] = std::vector<edge_props>(i);
			}
		}
		
		bool exists(int i, int j) const
		{
			if(i > j)
				return map[i][j].exists;
			if(i < j)
				return map[j][i].exists;
			return false;
		}
		
		T& get(int i, int j)
		{
			if(i > j)
				return map[i][j].exists = true, map[i][j].value;
			if(i < j)
				return map[j][i].exists = true, map[j][i].value;
			std::cout << "Problem with accessing " << i << ", " << j << std::endl;
			throw "This is bad";
		}
		
		const T& get(int i, int j) const
		{
			if(i < j)
				return map[i][j].value;
			if(j < i)
				return map[j][i].value;
			std::cout << "Problem with accessing " << i << ", " << j << std::endl;
			throw "This is bad";
		}
		
		struct edgemap_accessor
		{
			edgemap<T>* parent;
			int i;
			edgemap_accessor(int i, edgemap<T>* parent) : i(i), parent(parent) { }
			T& operator [] (int j) { return parent->get(i, j); }
		};
		
		edgemap_accessor operator [] (int i)
		{
			return edgemap_accessor(i, this);
		}
		
		class iterator
		{
			protected:
				int i, j;
				edgemap* parent;
				
				friend class edgemap<T>;
			
			public:
				iterator(int i, int j, edgemap* parent) : i(i), j(j), parent(parent) { }
				iterator(const iterator& t) : i(t.i), j(t.j), parent(t.parent) { }
				
				iterator& operator++()
				{
					j++; 
					//std::cout << "Incrementing iterator..." << std::flush;
					if(i <= j)
					{
						j = 0; i++;
					}
					//std::cout << "to i, j = " << i << ", " << j << " exists? " << parent->exists(i, j) << std::endl;
					if(i == parent->size) return *this;
					if(parent->exists(i, j)) return *this;
					return ++(*this);
				}
				iterator operator++(int) {iterator tmp(*this); operator++(); return tmp;}
				bool operator==(const iterator& rhs) {return i==rhs.i && j==rhs.j;}
				bool operator!=(const iterator& rhs) {return !(*this == rhs);}
				T& operator*() {return parent->map[i][j].value;}
		};
				
		iterator begin()
		{
			iterator zero = iterator(0, 0, this);
			++zero;
			return zero;
		}
		
		iterator end()
		{
			return iterator(size, 0, this);
		}
};

#endif // EDGEMAP_HPP
