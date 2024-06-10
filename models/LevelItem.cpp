#include<iostream>
#include<vector>
#include "BookItem.cpp"
using namespace std;

class LevelItem {
	public :
		vector<BookItem> bookItems;
		int cumQty;
	public :
		void levelDetails()
		{
			cout<<"********Level details***********"<<"\n";
			cout<<"Nummber of orders in the level - "<<bookItems.size()<<"\n";
			cout<<"Cumulative quantity - "<<cumQty<<"\n";
			cout<<"Book items\n";
			for(int i = 0; i < bookItems.size() ; i++)
			{
				cout<<"Order ID - "<< bookItems[i].orderId<<"\n";
				cout<<"Order Quantity - "<< bookItems[i].quantity<<"\n\n";
			} 
			cout<<"********************************"<<"\n";
		}
		LevelItem()
		{
			cumQty=0;
		}
};
