#include<iostream>
#include<fstream>
#include <cmath>
#include "../util/TradeUtils.h"

void updateSmallestHighest(vector<LevelItem> bidBook, vector<LevelItem> askBook, int bestBid, int bestAsk);

float smallestPrice, highestPrice, openingPrice;
int levels, openingQty, openingImbalance;

float prevClose = 3515.25;
float openingDiff = 0;

void callAuction(vector<LevelItem> bidBook, vector<LevelItem> askBook, int poBuys, int poAsks, int bestBid, int bestAsk, float &openPrice, int &openQty)
{
	updateSmallestHighest(bidBook, askBook, bestBid, bestAsk);
	int prevQty;
	cout<<"Smallest price - "<<smallestPrice<<"  Highest price - "<<highestPrice<<"\n";
	cout<<"Best bid - "<<bestBid<<"   Best ask - "<<bestAsk<<"\n";
	levels = getLevel(smallestPrice, highestPrice) + 1;
	vector<int> mostBids(levels);
	vector<int> mostAsks(levels);
	prevQty=0;
	//Gets highest possible buys at each price
	for(int i=levels-1 ; i>=0 ; i--)
	{
		float price = smallestPrice + (0.05*i);
		int priceLevel;
		
		if(price > bestBid)
		mostBids[i] = 0;
		else
		{
			priceLevel = getLevel(price, bestBid);
			mostBids[i] = prevQty + bidBook[priceLevel].cumQty;
			prevQty = mostBids[i];
		}
	}
	prevQty = 0;
	//Gets highest possible sells at each price
	for(int i=0 ; i<levels ; i++)
	{
		float price = smallestPrice + (0.05*i);
		int priceLevel;
		
		if(price < bestAsk)
		mostAsks[i] = 0;
		else
		{
			priceLevel = getLevel(price, bestAsk);
			mostAsks[i] = prevQty + askBook[priceLevel].cumQty;
			prevQty = mostAsks[i];
		}
	}
	
	//Find opening price and opening qty
	openingQty = -1;
	openingQty = 0;
	for(int i=0 ; i<levels ; i++)
	{
		int priceLevel;
		priceLevel = getLevel(smallestPrice + (0.05*i), bestBid);
		//cout<<"Price - "<<smallestPrice + (0.05*i)<<"  Total Buys - "<<mostBids[i]<<"   Total Asks - "<<mostAsks[i]<<"  Bids - "<<bidBook[priceLevel].cumQty<<"\n";
		int qty = min(mostBids[i]+poBuys, mostAsks[i]+poAsks);
		int imbalance = abs((mostBids[i]+poBuys) - (mostAsks[i]+poAsks));
		float diff = abs((smallestPrice + (0.05*i)) - prevClose);
		if(qty>openingQty)
		{
			openingQty = qty;
			openingPrice = smallestPrice + (0.05*i);
			openingImbalance = imbalance;
			openingDiff = diff;
		}
		else if(qty == openingQty)
		{
			if(imbalance<openingImbalance)
			{
				openingQty = qty;
				openingPrice = smallestPrice + (0.05*i);
				openingImbalance = imbalance;
				openingDiff = diff;
			}
			else if (imbalance == openingImbalance)
			{
				if(diff < openingDiff)
				{
					openingQty = qty;
					openingPrice = smallestPrice + (0.05*i);
					openingImbalance = imbalance;
					openingDiff = diff;
				}
			}
		}
		//cout<<"price - "<<smallestPrice + (0.05*i)<<"   Quantity - "<<qty<<"     Diff - "<<mostBids[i]+poBuys-mostAsks[i]+poAsks<<"\n";
	}
	cout<<"Opening qty - "<<openingQty<<"\n";
	openPrice = openingPrice;
	openQty = openingQty;
}

void updateSmallestHighest(vector<LevelItem> bidBook, vector<LevelItem> askBook, int bestBid, int bestAsk)
{
	//get smallest and highest prices
	smallestPrice = 9999999;
	highestPrice = -9999999;
	//parse bid book
	for(int i=0 ; i<20000 ; i++)
	{
		if(bidBook[i].cumQty>0)
		{
			int price = bestBid-(0.05*i);
			if (price > highestPrice)
			highestPrice = price;
			if (price < smallestPrice)
			smallestPrice = price;
		}
	}
	//parse ask book
	for(int i=0 ; i<20000 ; i++)
	{
		if(askBook[i].cumQty>0)
		{
			int price = bestAsk+(0.05*i);
			if (price > highestPrice)
			highestPrice = price;
			if (price < smallestPrice)
			smallestPrice = price;
		}
	}
}
