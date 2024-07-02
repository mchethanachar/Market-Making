#include<iostream>
#include<fstream>
#include "analysis/mmPlacer.cpp"
using namespace std;

void readTradeFile();

int main()
{
	readTradeFile();
}

void readTradeFile()
{
	/* All variables */
	fstream trades_file;
	string tradeLine,timestr;
	int i = 0;
	char x;
	int h = -1;
	int n = 0;
	int min = 0;
	int hour = 0;
	int prevMin = 0;
	int totalMin = 0;
	float midPrice = 0;
	float open, high, low, close;
	int buyMo = 0;
	int sellMo = 0;
	int totalBuyMo = 0;
	int totalSellMo = 0;
	float totalBookWeight = 0;
	float originalPrice = 0;
	float flowSlope = 0;
	float priceSlope = 0;

	// Create and open a text file
    ofstream priceFile("price_file.csv");
	ofstream candleStick("candleStick_file.csv");
	ofstream priceImpact("priceImpact.csv");

	trades_file.open("D:/HFT/Data/EQUITY_ORDER_V3_2023-10-04/wipro.txt",ios::in);
	if(trades_file.is_open())
	{
		cout<<"File is open\n";
		while(getline(trades_file, tradeLine))
		{
			i++;
			OrderItem orderItem= getOrderItem(tradeLine);
			processOrder(orderItem);
			if(preOpen == false)
			updateMmOrders();
			//updateMmOrders2(flowSlope, priceSlope);
			//updateMmOrders();
			min = getMin(orderItem.versionTime);
			hour = getHour(orderItem.versionTime);
			totalMin = getTotalMin(hour, min);
			midPrice = getMidPrice(bestAsk, bestBid);

			if(midPrice>high)
			high = midPrice;

			if(midPrice<low)
			low = midPrice;
			
			if(totalMin>=15)
			{
				if(orderItem.type == "G" && orderItem.side=="B")
				{
					buyMo += orderItem.quantity;
					totalBuyMo += orderItem.quantity;
				}
				else if(orderItem.type == "G" && orderItem.side=="S")
				{
					sellMo += orderItem.quantity;
					totalSellMo += orderItem.quantity;
				}
				totalBookWeight += getBookWieght(bidBook, askBook, 3);
			}

			if(min != prevMin && preOpen == false && totalMin >= 15)
			{
				close = midPrice;
				if(totalMin>15)
				{
					candleStick<<totalMin-1<<","<<open<<","<<high<<","<<low<<","<<close<<"\n";
				}
				priceImpact<<totalMin<<","<<totalBuyMo-totalSellMo<<","<<midPrice<<","<<totalBookWeight/1000<<"\n";
				//printBook(bidBook, askBook, bestBid, bestAsk, totalBids, totalAsks);
				leftShift(midPrices);
				midPrices[flowSize-1] = midPrice;
				leftShift(moImbalance);
				moImbalance[flowSize-1] = totalBuyMo-totalSellMo;
				if(totalMin-14 >= flowSize)
				{
					priceSlope = getSlope(midPrices);
					flowSlope = getSlope(moImbalance);
					/*cout<<"Price list\n";
					for(int i=0; i<flowSize; i++)
					{
						cout<<midPrices[i]<<"  ";
					}
					cout<<"\nPrice slope - "<<getSlope(midPrices)<<"\n";
					cout<<"Imbalance list\n";
					for(int i=0; i<flowSize; i++)
					{
						cout<<moImbalance[i]<<"  ";
					}
					cout<<"\nOrder Imbalance slope - "<<getSlope(moImbalance)<<"\n";
					*/
				}

				open = midPrice;
				close = midPrice;
				high = midPrice;
				low = midPrice;
				prevMin = getMin(orderItem.versionTime);
				priceFile<<totalMin<<","<<midPrice<<"\n";
				//cout<<orderItem.versionTime<<" - "<<totalMin<<" - "<<midPrice<<"\n";
				cout<<orderItem.versionTime<<"    "<<"Available cash - "<<avlCash<<"  Available position - "<<avlQty<<"\n";
				cout<<"Limit Buy - "<<limitBidMatch<<"  Limit Ask - "<<limitAskMatch<<"  Market Buy - "<<marketBidMatch<<"  Market Ask - "<<marketAskMatch<<"\n";
				cout<<"Buy MO - "<<buyMo<<"  Sell MO - "<<sellMo<<"\n";
				cout<<"MO slope  - "<<flowSlope<<"   Price slope - "<<priceSlope<<"\n";
				printBook(bidBook, askBook, bestBid, bestAsk, totalBids, totalAsks);
				buyMo = 0;
				sellMo = 0;
				cout<<"*****************************\n";
				if(hour==15 && min >= 30)
				break;
			}
		}
		cout<<"Total Buy - "<<totalBuyMo<<"   Total Sell - "<<totalSellMo<<"\n";
		cout<<"Closing files\n";
		//printBook(bidBook, askBook, bestBid, bestAsk, totalBids, totalAsks);
		trades_file.close();
		priceFile.close();
		candleStick.close();
		priceImpact.close();
		cin>>x;
	}
	cout<<"File reading complete";
}

 