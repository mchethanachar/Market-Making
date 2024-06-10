#include<iostream>
#include<fstream>
#include "analysis/mmPlacer.cpp"
using namespace std;

void readTradeFile();

int main()
{
	//cout<<getBidPrice(100,5);
	readTradeFile();
}

void readTradeFile()
{
	fstream trades_file;
	// Create and open a text file
    //ofstream priceFile("price_file.txt");
    //ofstream orderFile("order_file.txt");
	vector<string> orderDetails;
	trades_file.open("D:/HFT/Data/EQUITY_ORDER_V3_2023-10-04/wipro.txt",ios::in);
	if(trades_file.is_open())
	{
		cout<<"File is open\n";
		string tradeLine,timestr;
		int i = 0;
		char x;
		int m = -1;
		int h = -1;
		int n = 0;
		while(getline(trades_file, tradeLine))
		{
			i++;
			OrderItem orderItem= getOrderItem(tradeLine);
			//orderItem.details();
			processOrder(orderItem);
			if(preOpen == false)
			//updateMmOrders();
			if(getMin(orderItem.versionTime) != m && preOpen == false)
			{
				h = getHour(orderItem.versionTime);
				m = getMin(orderItem.versionTime);
				cout<<orderItem.versionTime<<"       Price - "<<getMidPrice(bestAsk, bestBid)<<"    Volume - "<<volume<<"\n";
				//priceFile<<(h*60)+m<<","<<getMidPrice(bestAsk, bestBid)<<"\n";
				printBook(bidBook, askBook, bestBid, bestAsk, totalBids, totalAsks);
				//cout<<"Available  Qty -  "<<avlQty<<"    Available Cash - "<<avlCash<<"\n";
				//cout<<getPrediction()<<"\n";
				//printPredictionPercentage();
				if(h==15 && m >= 30)
				break;
			}
			/*if(preOpen == false && (matchedSells+matchedBuys)>1000)
			{
				updateMoPredictions(matchedBuys, matchedSells);
				orderFile<<n<<","<<getPrediction()<<"\n";
				matchedSells = 0;
				matchedBuys = 0;
				n++;
			}*/
		}
		cout<<"Closing files\n";
		//printBook(bidBook, askBook, bestBid, bestAsk, totalBids, totalAsks);
		trades_file.close();
		//priceFile.close();
		//orderFile.close();
	}
	cout<<"File reading complete";
}

 

