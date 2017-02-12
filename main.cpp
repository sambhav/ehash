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

int main(int argc, char const *argv[])
{
	Directory dir(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
	int choice, key;
	string value;
	do
    {
    	cin>>choice;
        switch(choice)
        {
            case 1:
                cin>>key;
                cin>>value;
                dir.insert(key,value);
                break;
            case 2:
                cin>>key;
                dir.remove(key);
                break;
            case 3:
                cin>>key;
                dir.get(key);
                break;
            case 4:
                dir.display();
                break;
        }
    } while(choice!=0);
	return 0;
}

// Directory function definitions
Directory::Directory(int global_depth, int bucket_size, int mode)
{
	this->global_depth = global_depth;
	this->bucket_size = bucket_size;
	this->mode = mode;
	for(int i=0; i<(1<<global_depth); i++)
	{
		buckets.pb(new Bucket(i, global_depth, bucket_size));
	}
}

int Directory::get_pair_bucket(int bucket_key, int depth)
{
	return bucket_key^(1<<(depth-1));
}

void Directory::display(void)
{
	int i=0;
	set <Bucket *, compare> S(buckets.begin(),buckets.end());
	cout<<"-------------------------"<<endl;
	cout<<"Total distinct buckets: "<<S.size()<<endl;
	cout<<"Global Depth: "<<global_depth<<endl;
	fe(b,S)
	{	
		(*b)->display();
		cout<<endl;
	}
	cout<<"-------------------------"<<endl;
}

int Directory::hash(int key)
{
	return (key&((1<<global_depth)-1));
}

int Directory::insert(int key, string value)
{
	int hashed_key = hash(key);
	buckets[hashed_key]->set_id(hashed_key);
	int insert_status = buckets[hashed_key]->insert(key, value);
	if(insert_status == SUCCESSFUL_INSERT)
	{
		cout<<"Inserted key: "<<key<<" in bucket:"<<buckets[hashed_key]->get_bin_id()<<endl;
		return SUCCESSFUL_INSERT;
	}
	else if(insert_status == BUCKET_FULL)
	{
			split_bucket(hashed_key);
			insert(key, value);
	}
}

void Directory::split_bucket(int bucket_key)
{
	Bucket * temp_bucket = buckets[bucket_key];
	buckets[bucket_key]->inc_depth();
	int bucket_depth = temp_bucket->get_depth();
	int pair_bucket_key = get_pair_bucket(bucket_key, bucket_depth);
	Bucket * pair_bucket = buckets[pair_bucket_key];
	if(bucket_depth>global_depth)
		expand();
	vis bucket_values = temp_bucket->get_all();
	temp_bucket->clear();
	buckets[pair_bucket_key] = new Bucket(pair_bucket_key, bucket_depth, bucket_size);
	int index_diff = 1<<bucket_depth;
	int dir_size = 1<<global_depth;
	for(int i=pair_bucket_key-index_diff; i>=0; i-=index_diff )
	    buckets[i] = buckets[pair_bucket_key];
	for(int i=pair_bucket_key+index_diff; i<dir_size; i+=index_diff)
	    buckets[i] = buckets[pair_bucket_key];
	fe(iter, bucket_values)
		insert(iter->first,iter->second);
}

void Directory::expand(void)
{
	for(int i=0;i<(1<<global_depth);i++)
		buckets.pb(buckets[i]);
	global_depth++;
}

string Directory::get(int key)
{
	int hashed_key = hash(key);
	return buckets[hashed_key]->get(key);
}

int Directory::remove(int key)
{
	int hashed_key = hash(key);
	int delete_status = buckets[hashed_key]->remove(key);
	if (mode != LAZY_DELETE && delete_status == BUCKET_EMPTY && buckets[hashed_key]->get_depth()>=DEFAULT_INITIAL_GLOBAL_DEPTH)
	{
		merge_bucket(hashed_key);		
	}
}

void Directory::contract(void)
{
	if (global_depth > 1)
	{
		global_depth--;
		for(int i=0;i<1<<global_depth;i++)
			buckets.pop_back();
	}
}

void Directory::merge_bucket(int bucket_key)
{
	Bucket* current_bucket = buckets[bucket_key];
	int pair_bucket_key = get_pair_bucket(bucket_key, current_bucket->get_depth());
	Bucket* pair_bucket = buckets[pair_bucket_key];
	int index_diff = 1<<current_bucket->get_depth();
	int dir_size = 1<<global_depth;
	if(pair_bucket != current_bucket)
	{
		delete current_bucket;
		pair_bucket->dec_depth();
		pair_bucket->set_id(min(current_bucket->get_id(), pair_bucket->get_id()));
		buckets[bucket_key] = pair_bucket;
		for(int i=bucket_key-index_diff; i>=0; i-=index_diff )
		    buckets[i] = pair_bucket;
		for(int i=bucket_key+index_diff; i<dir_size; i+=index_diff)
		    buckets[i] = pair_bucket;
		if (pair_bucket->get_depth() == global_depth -1)
		{
			int contract_directory = 1;
			fe(iter, buckets)
			{
				if((*iter)->get_depth()>=global_depth)
					contract_directory = 0;
			}
			if(mode != NO_COLLAPSE && contract_directory)
				contract();
		}
	}
}


// Bucket function definitons
Bucket::Bucket(int id, int depth, int size=DEFAULT_BUCKET_SIZE)
{
	this->depth = depth;
	this->size = size;
	set_id(id);
	cout<<"Created bucket with size:"<<size<<" and depth:"<<depth<<endl;
}

int Bucket::get_id(void)
{
	return id;
}

string Bucket::get_bin_id(void)
{
	return bin_id;
}

int Bucket::set_id(int new_id)
{
	id = new_id;
	string out;
	int n = id;
	while(n!=0) {out=(n%2==0 ?"0":"1")+out; n/=2;}
	while(out.length()<depth) {out="0"+out;}
	bin_id = out;
}

int Bucket::is_full(void)
{
	return bucket.size()>=size;
}

int Bucket::is_empty(void)
{
	return bucket.empty();
}

int Bucket::get_depth(void)
{
	return depth;
}

string Bucket::get(int key)
{
	if(bucket.count(key))
		return bucket[key];
	else
		cout<<"KeyError: Key "<<key<<" not found."<<endl;
	return "";
}

vis Bucket::get_all(void)
{
	vis temp;
	fe(iter, bucket)
		temp.pb(mp(iter->first,iter->second));
	return temp;	
}

int Bucket::insert(int key, string value)
{
	if(!bucket.count(key))
	{
		if(!is_full())
		{
			bucket[key] = value;
			return 1;
		}
		else
		{
			//Bucket is full
			return 0;
		}
	}
	cout<<"Insert unsuccessful, key: "<<key<<" already exists. Use update instead.\n";
	return -1;	
}

int Bucket::update(int key, string value)
{
	if(bucket.count(key))
		{
			bucket[key] = value;
			return 1;
		}
	else
		{
			cout<<"Update unsuccessful. Key not found.\n";
			return 0;
		}

}

int Bucket::remove(int key)
{
	if(bucket.erase(key))
		{
			if(!is_empty())
				return 1;
			else
				return 2;	
		}
	else
		return 0;
}

void Bucket::clear(void)
{
	bucket.clear();
}

void Bucket::inc_depth(void)
{
	depth++;
	set_id(id);
}

void Bucket::dec_depth(void)
{
	depth--;
	set_id(id);
}

void Bucket::display(void)
{
	cout<<"Bucket ID: "<<bin_id<<endl;
	cout<<"Depth: "<<depth<<endl<<"-------"<<endl;
	cout<<"KEY\tVALUE\n";
	fe(iter, bucket)
	{
		cout<<iter->first<<"\t"<<iter->second<<endl;
	}
}
