#include<iostream>
#include "../models/LevelItem.cpp"
#include "Utils.h"

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

