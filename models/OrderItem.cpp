#include <string.h>
#include <vector>
#include <iostream>
using namespace std;

class OrderItem {
	public:
		string session;
		string instrument;
		string side;
		string type;
		float price;
		int quantity;
		string versionTime;
		string action;
		string orderId;
		
	public:
		void details()
		{
			cout<<"Order details:\nSide - "<<side<<"\nType - "<<type<<"\nPrice - "<<price<<"\nQuantity - "<<quantity<<"\nVersion Time - "<<versionTime<<"\nAction - "<<action<<"\nOrder ID - "<<orderId<<"\n";
		}
};
