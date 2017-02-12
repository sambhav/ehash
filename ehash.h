#ifndef _BASIC_
#define _BASIC_
#include <bits/stdc++.h>
#define DEFAULT_INITIAL_GLOBAL_DEPTH 1
#define DEFAULT_BUCKET_SIZE 2
#define BUCKET_FULL 0
#define SUCCESSFUL_INSERT 1
#define DUPLICATE_KEY 2
#define BUCKET_EMPTY 2
#define LAZY_DELETE 0
#define NO_COLLAPSE 1
#define fe(iter, iterable) for(typeof((iterable).begin()) iter = (iterable).begin(); iter != (iterable).end(); ++iter)
#define pb push_back
#define vis vector< pair<int, string> >
#define mp make_pair

using namespace std;
// Bucket class definition
class Bucket {
	int depth, size, id;
	string bin_id;
	map<int, string> bucket; 
	public:
		Bucket(int id, int depth, int size);
		int is_full(void);
		int is_empty(void);
		int get_depth(void);
		string get(int key);
		vis get_all(void);
		int remove(int key);
		int insert(int key, string value);
		int update(int key, string value);
		int get_id(void);
		string get_bin_id(void);
		int set_id(int new_id);
		void display(void);
		void inc_depth(void);
		void dec_depth(void);
		void clear(void);
};
// Directory class definition
class Directory {
	private:
		int global_depth, bucket_size, mode;
		vector<Bucket *> buckets;
		void expand(void);
		void contract(void);
		void split_bucket(int bucket_key);
		void merge_bucket(int bucket_key);
		int get_pair_bucket(int bucket_key, int depth);
		int hash(int key);
	public:
		Directory(int mode, int global_depth, int bucket_size);
		string get(int key);
		int insert(int key, string value);
		int update(int key, string value);
		int remove(int key);
		void display(void);
};

// Struct compare
struct compare
{
    bool operator()(Bucket* a, Bucket* b)
    {
    	return a->get_id() < b->get_id();
    }
};
#endif