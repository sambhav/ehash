#include "ehash.h"

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
