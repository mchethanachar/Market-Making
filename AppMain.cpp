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
	float predictedPrice = 0;
	float sd = 0;
	float open, high, low, close;

	// Create and open a text file
    ofstream priceFile("price_file.csv");
    ofstream maFile("ma_file.csv");
	ofstream predictedPriceFile("predictedPrice_file.csv");
	ofstream candleStick("candleStick_file.csv");
	vector<string> orderDetails;
	trades_file.open("D:/HFT/Data/EQUITY_ORDER_V3_2023-10-04/infosys.txt",ios::in);
	if(trades_file.is_open())
	{
		cout<<"File is open\n";
		while(getline(trades_file, tradeLine))
		{
			i++;
			OrderItem orderItem= getOrderItem(tradeLine);
			//orderItem.details();

			//Orders are matched here or they are put to book.
			processOrder(orderItem);

			// Market making comes to picture only after 
			if(preOpen == false)
			updateMmOrders();

			min = getMin(orderItem.versionTime);
			hour = getHour(orderItem.versionTime);
			totalMin = getTotalMin(hour, min);
			midPrice = getMidPrice(bestAsk, bestBid);

			if(midPrice>high)
			high = midPrice;

			if(midPrice<low)
			low = midPrice;

			//Printing once in a min
			if(min != prevMin && preOpen == false && totalMin >= 15)
			{
				close = midPrice;
				if(totalMin>15)
				candleStick<<totalMin-1<<","<<open<<","<<high<<","<<low<<","<<close<<"\n";
				open = midPrice;
				close = midPrice;
				high = midPrice;
				low = midPrice;
				prevMin = getMin(orderItem.versionTime);
				priceFile<<totalMin<<","<<midPrice<<"\n";
				//printBook(bidBook, askBook, bestBid, bestAsk, totalBids, totalAsks);
				//cout<<orderItem.versionTime<<"       Price - "<<midPrice<<"    Volume - "<<volume<<"\n";
				updateMA(maWeight, maSize, midpriceMa, midPrice,movingAverage, movingAverageDistance);
				//cout<<"Moving average - "<<movingAverage<<"\n";
				if(totalMin >= 15+maSize-1)
				{
					maFile<<totalMin<<","<<movingAverage<<"\n";
					leftShift(movingAverages);
					leftShift(movingAverageDistances);
					movingAverages[maSlopeSize-1] = movingAverage;
					movingAverageDistances[maSlopeSize-1] = movingAverageDistance;
					maCounter++;
					if(maCounter>=maSlopeSize)
					{
						cout<<totalMin<<"\n";
						for(int i=0 ; i<maSlopeSize ; i++)
						{
							cout<<movingAverages[i]<<" ";
						}
						cout<<"\n";
						sd = calculateStandardDeviation(movingAverageDistances);
						predictedPrice = getNextMa(movingAverages, maSlopeSize) + getAverage(movingAverageDistances);
						predictedPriceFile<<totalMin<<","<<getUpperBound(predictedPrice,sd)<<","<<getLLowerBound(predictedPrice, sd)<<"\n";

						//cout<<orderItem.versionTime<<" - "<<getSlope(movingAverages, maSlopeSize)<<" Average distance - "<<getAverage(movingAverageDistances);
						cout<<orderItem.versionTime<<"   Predicted price - "<<predictedPrice<<"  Upper bound - "<<getUpperBound(predictedPrice,sd)<<"  Lower Bound - "<<getLLowerBound(predictedPrice, sd)<<"\n";
						//cout<<"      Std -  "<<calculateStandardDeviation(movingAverageDistances)<<"\n";
					}
				}
				//cout<<"Available  Qty -  "<<avlQty<<"    Available Cash - "<<avlCash<<"\n";
				if(hour==15 && min >= 30)
				break;
			}
		}
		cout<<"Closing files\n";
		//printBook(bidBook, askBook, bestBid, bestAsk, totalBids, totalAsks);
		trades_file.close();
		priceFile.close();
		maFile.close();
		predictedPriceFile.close();
		candleStick.close();
		cin>>x;
	}
	cout<<"File reading complete";
}

 