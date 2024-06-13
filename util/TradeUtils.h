#include<iostream>
#include "../models/LevelItem.cpp"
#include "Utils.h"
#include<D:/HFT/softwares/eigen-master/Eigen/Dense>

using namespace std;

int getLevel(float price, float newPrice)
{
	int level;
	int diff;
	diff = price*(10000) - newPrice*(10000);
	if (diff < 0)
	diff = diff *(-1);
	return diff/500;
}

int findUpdatePos(float best_price, float new_price, float tick_size)
{
	float  price_diff;
	price_diff = best_price - new_price;
	if(price_diff == 0)
	return 0;
	if(price_diff<0)
	price_diff = price_diff*(-1);
	return 	price_diff/tick_size;
}

BookItem getBookItem(OrderItem orderItem)
{
	BookItem bookItem;
	bookItem.orderId = orderItem.orderId;
	bookItem.quantity = orderItem.quantity;
	return bookItem;
}

void addLevel(vector<LevelItem> &book)
{
	LevelItem levelItem;
	book.insert(book.begin(), levelItem);
	book.pop_back();
}

void addMultipleLevels(vector<LevelItem> &book, int levels)
{
	for(int i=0; i<levels; i++)
	{
		addLevel(book);
	}
}

void insertBookItem(LevelItem &levelItem, OrderItem orderItem)
{
	BookItem bookItem;
	bookItem.orderId = orderItem.orderId;
	bookItem.quantity = orderItem.quantity;
	levelItem.bookItems.push_back(bookItem);
	levelItem.cumQty += orderItem.quantity;
}

int popLevelItem(vector<LevelItem> &book)
{
	int poppedLevels = 0;
	while (book.size() > 0)
	{
		if(book[0].cumQty <= 0)
		{
			auto i = book.begin();
			LevelItem levelItem;
			levelItem.cumQty = 0;
		    book.erase(i);
		    poppedLevels++;
		    book.push_back(levelItem);
		}
		else 
		{
			break;
		}
	}
	return poppedLevels;
}

void cleanUpLevel(vector<BookItem> &orders)
{
	int poppedOrders = 0;
	while (orders.size() > 0)
	{
		if(orders[0].quantity == 0)
		{
			auto i = orders.begin();
			orders.erase(i);
		}
		else
		{
			break;
		}
	}
}

void deleteOrder(vector<BookItem> &orders, int pos)
{
	auto i = orders.begin();
	orders.erase(i + pos);
}

void popBookItem(LevelItem &levelItem)
{
	if (levelItem.bookItems.size() > 0)
	{
		levelItem.cumQty -= levelItem.bookItems[0].quantity;
		auto i = levelItem.bookItems.begin();
		levelItem.bookItems.erase(i);
	}
}

int getCumQty(vector<LevelItem> &book, int levels)
{
	int cumQty=0;
	for (int i=0; i<=levels ; i++)
	{
		cumQty += book[i].cumQty;
	}
	return cumQty;
}

float getMicroPrice(float bestAsk, float bestBid, int bidQty, int askQty)
{
	float microPrice = -1;
	if (bestAsk == 999999999 || bestBid == -999999999)
		return microPrice;
	microPrice = ((bidQty*bestAsk)+(askQty*bestBid))/(bidQty+askQty);
	return microPrice;
}

float getMidPrice(float bestAsk, float bestBid)
{
	float midPrice = -1;
	if (bestAsk == 999999999 || bestBid == -999999999)
		return midPrice;
	midPrice = (bestAsk + bestBid)/2;
	return midPrice;
}

OrderItem createOrder(string side, float price, int quantity, string orderId)
{
	OrderItem orderItem;
	orderItem.side = side;
	orderItem.price = price;
	orderItem.quantity = quantity;
	orderItem.orderId = orderId;
	//Static values for MM order
	orderItem.session= '3';
	orderItem.action = 'U';
	orderItem.type = 'L';
	
	return orderItem;
}

int getBuyableQty(float cash, float price)
{
	return cash/price;
}

float getBidPrice(float bestBid, int level)
{
	float bidPrice = bestBid - (level*0.05);
	return bidPrice;
}

float getAskPrice(float bestAsk, int level)
{
	float  askPrice = bestAsk + (level*0.05);
	return askPrice;
}

int getTotalMin(int hour, int min)
{
	int totalMin = (hour*60) + min;
	return totalMin-540;
}

float getDeltaT(int hour, int min)
{
	int totalMin = getTotalMin(hour, min);
	return (float)(390 - totalMin)/390;
}

void updateMA(int min, int maSize, vector<float> &midpriceMa, float midPrice, float &movingAverage, float &movingAverageDistance)
{
	/*cout<<"Calculating MA for min - "<<min<<"\n";
	for(int i=0; i<maSize; i++)
	{
		cout<<midpriceMa[i]<<" ";
	}*/
	cout<<"\n";
	movingAverage -= midpriceMa[(min-15)%maSize];
	movingAverage += midPrice/maSize;
	midpriceMa[(min-15)%maSize] = midPrice/maSize;
	movingAverageDistance = midPrice-movingAverage;
}

float getUpperBound(float predictedMa, float sd)
{
	float upperBound = predictedMa + (sd/0.707); // using SD of Sin wave here
	return upperBound;
}

float getLLowerBound(float predictedMa, float sd)
{
	float lowerBound = predictedMa - (sd/0.707); // using SD of Sin wave here
	return lowerBound;
}

//Doing linear rehgrassion here and plotting the next
float getNextMa(vector<float> &movingAverages, int maSlopeSize)
{
	vector<float> timeVector(maSlopeSize,0);

	for(int i=0 ; i<maSlopeSize ; i++)
	timeVector[i] = i;

	// Calculate the necessary summations
    float sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    for (int i = 0; i < maSlopeSize; ++i) {
        sumX += timeVector[i];
        sumY += movingAverages[i];
        sumXY += timeVector[i] * movingAverages[i];
        sumX2 += timeVector[i] * timeVector[i];
    }

	// Calculate the slope (m)
    float numerator = maSlopeSize * sumXY - sumX * sumY;
    float denominator = maSlopeSize * sumX2 - sumX * sumX;

	float slope = numerator / denominator;
	float intercept = (sumY/maSlopeSize) - (slope * sumX/maSlopeSize);

	float nextValue = intercept  + (slope * maSlopeSize);

	return nextValue;
}

float getSlope(vector<float> &movingAverages, int maSlopeSize)
{
	vector<float> timeVector(maSlopeSize,0);

	for(int i=0 ; i<maSlopeSize ; i++)
	timeVector[i] = i;

	// Calculate the necessary summations
    float sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    for (int i = 0; i < maSlopeSize; ++i) {
        sumX += timeVector[i];
        sumY += movingAverages[i];
        sumXY += timeVector[i] * movingAverages[i];
        sumX2 += timeVector[i] * timeVector[i];
    }

	// Calculate the slope (m)
    float numerator = maSlopeSize * sumXY - sumX * sumY;
    float denominator = maSlopeSize * sumX2 - sumX * sumX;

	float slope = numerator / denominator;
    return slope;
}

void leftShift(vector<float> &movingAverages)
{
	for(int i=1 ; i<movingAverages.size() ; i++)
	{
		movingAverages[i-1] = movingAverages[i];
	}
}

float getAverage(vector<float> vectNum)
{
	int n = vectNum.size();
	float vectSum = 0;
	for(int i=0 ; i<n ; i++)
	vectSum += vectNum[i];

	return vectSum/n;
}

// Function to calculate the standard deviation of the values in a vector
float calculateStandardDeviation(vector<float>& data) {
    if (data.size() < 2) {
        throw std::runtime_error("Standard deviation requires at least two data points.");
    }
    
    float mean = getAverage(data);
    float variance = 0.0f;
    
    for(float value : data) {
        variance += (value - mean) * (value - mean);
    }
    
    variance /= (data.size() - 1); // Use N-1 for an unbiased estimator (sample standard deviation)
    return std::sqrt(variance);
}

void printBook(vector<LevelItem> &bidBook, vector<LevelItem> &askBook, float bestBid, float bestAsk, int totalBids, int totalAsks)
{
	int bookBid, bookAsk;
	bookAsk = 0;
	bookBid = 0;
	bool printBid, printAsk;
	printBid = true;
	printAsk = true;
	if(bestBid == -999999999)
	printBid = false;
	if(bestAsk == 999999999)
	printAsk = false;
	
	cout<<"\n\n\n&&&&&&&&&&&&&&&&&-----Order Book-----&&&&&&&&&&&&&&&&&\n\n\n";
	cout<<"            Bids                                      Asks\n\n";
	for(int i=0 ; i<5 ; i++)
	{
		if(printBid)
		{
			cout<<bestBid-(i*0.05)<<"   -    "<<bidBook[i].cumQty;
			bookBid+=bidBook[i].cumQty;
		}
		else
		{
			cout<<" - "<<"   -    "<<" - ";
		}
		cout<<"                             ";
		if(printAsk)
		{
			cout<<bestAsk+(i*0.05)<<"   -    "<<askBook[i].cumQty;
			bookAsk+=askBook[i].cumQty;
		}
		else
		{
			cout<<" - "<<"   -    "<<" - ";
		}
		cout<<"\n";
	}
	cout<<"----------------------------------------------------------------\n";
	//cout<<"            "<<totalBids<<"                                      "<<totalAsks<<"\n";
	cout<<"            "<<bookBid<<"                                      "<<bookAsk<<"\n";
}

