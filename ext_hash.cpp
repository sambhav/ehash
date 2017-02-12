#include "ehash.h"

// Directory function definitions
Directory::Directory(int global_depth, int bucket_size, int mode)
{
	this->global_depth = global_depth;
	this->bucket_size = bucket_size;
	this->mode = mode;
	for(int i=0; i< (1<<global_depth);i++)
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
	int insert_status = buckets[hashed_key]->insert(key, value);
	if(insert_status == SUCCESSFUL_INSERT)
		return SUCCESSFUL_INSERT;
	else if(insert_status == BUCKET_FULL)
	{
			split_bucket(hashed_key);
			insert(key, value);
	}
	else 
	{
		return DUPLICATE_KEY; 
	}
}

void Directory::split_bucket(int bucket_key)
{
	Bucket * temp_bucket = buckets[bucket_key];
	int bucket_depth = temp_bucket->get_depth();
	int pair_bucket_key = get_pair_bucket(bucket_key, bucket_depth+1);
	Bucket * pair_bucket = buckets[pair_bucket_key];
	if(bucket_depth<global_depth)
	{
		vis bucket_values = temp_bucket->get_all();
		temp_bucket->clear();
		if(pair_bucket == temp_bucket)
		{
			temp_bucket->inc_depth();
			buckets[pair_bucket_key] = new Bucket(pair_bucket_key, bucket_depth+1, bucket_size);
		}
		fe(iter, bucket_values)
			insert(iter->first,iter->second);
	}
	else
		expand();
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
	if(pair_bucket != current_bucket)
	{
		pair_bucket->dec_depth();
		pair_bucket->set_id(min(current_bucket->get_id(), pair_bucket->get_id()));
		buckets[bucket_key] = pair_bucket;
		if(pair_bucket->is_empty() && pair_bucket->get_depth()>=DEFAULT_INITIAL_GLOBAL_DEPTH)
			merge_bucket(pair_bucket_key);
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
	// cout<<"Created bucket with size:"<<size<<" and depth:"<<depth<<endl;
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
