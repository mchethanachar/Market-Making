#include<iostream>
#include<fstream>
#include "preopenProcessor.cpp"

//Definitions
void processOrder(OrderItem orderItem);
void processNewOrder(OrderItem &orderItem);
void processBidLimit(OrderItem &orderItem);
void processAskLimit(OrderItem &orderItem);
void processDelete(OrderItem orderItem);
void matchBidLimitOrder(OrderItem &orderItem);
void matchAskLimitOrder(OrderItem &orderItem);
void matchSellOrder(OrderItem &orderItem);
void matchBuyOrder(OrderItem &orderItem);
void poMatch();

//Limit order books
vector<LevelItem> bidBook(20000);
vector<LevelItem> askBook(20000);

//Price markers
float bestBid=-999999999;
float bestAsk=999999999;

//Total orders
int totalBids=0;
int totalAsks=0;

//Trade Volume
int volume=0;

//preOpen
bool preOpen = true;

//PO market orders
int poBuys = 0;
int poSells = 0;

//Opening price and opening qty
float openPrice = 0;
int openQty = 0;

//To deal with icorrect order of data
bool pendingCan = false;
string canOrder = "";

//Market making performance
float avlCash = 100000;
int avlQty = 0;

//For moving average calculation
int maSize = 20;
vector<float> midpriceMa(maSize,0);
float movingAverage = 0;
float movingAverageDistance = 0;

//For moving average slope calculation
int maSlopeSize = 10;
vector<float> movingAverages(maSlopeSize, 0);
vector<float> movingAverageDistances(maSlopeSize, 0);
int maCounter = 0;
float maWeight = 1.99;

bool mmPhase = false;
bool prevPeak = true;

void processOrder(OrderItem orderItem)
{	
	if (preOpen == true && orderItem.session =="3")
	{
		callAuction(bidBook, askBook, poBuys, poSells, bestBid, bestAsk, openPrice, openQty);
		cout<<"Opening Price - "<<openPrice<<"    Opening Quantity - "<<openQty<<"\n";
		poMatch();
		cout<<"After Pre Open match . Best Bid - "<<bestBid<<"   Best Ask - "<<bestAsk<<"\n";
		preOpen = false;
	}
	
 	if (orderItem.action == "A"){
		processNewOrder(orderItem);
	}
	else if (orderItem.action == "U"){
		processDelete(orderItem);
		processNewOrder(orderItem);
	}
	else if (orderItem.action == "D"|| orderItem.action == "M") {
		processDelete(orderItem);
	}
}

void processNewOrder(OrderItem &orderItem)
{
	if(pendingCan == true)
	{
		if(orderItem.orderId == canOrder)
		{
			pendingCan = false;
            canOrder = "";
            return;
		}
	}
	if (orderItem.side == "B" && orderItem.type == "L"){
		//Buy Limit
		processBidLimit(orderItem);
	}
	else if (orderItem.side == "B" && orderItem.type == "G") {
		if(preOpen == false)
		{
			matchBuyOrder(orderItem);
		}
		else
		{
			poBuys += orderItem.quantity;
		}
	}
	else if (orderItem.side == "S" && orderItem.type == "L") {
		//Sell Limit
		processAskLimit(orderItem);
	}
	else if (orderItem.side == "S" && orderItem.type == "G") {
		//Sell Market
		if(preOpen == false)
		{
			matchSellOrder(orderItem);
		}
		else
		{
			poSells += orderItem.quantity;
		}
	}
}

void processBidLimit(OrderItem &orderItem)
{
	if (orderItem.price >= bestAsk && preOpen == false)
	{
		matchBidLimitOrder(orderItem);
	}
	if (orderItem.quantity > 0)
	{
		BookItem bookItem = getBookItem(orderItem);
		if (totalBids == 0)
		{
			bestBid = orderItem.price;
			bidBook[0].bookItems.push_back(bookItem);
			bidBook[0].cumQty = orderItem.quantity;
		}
		else
		{
			int levels = getLevel(bestBid, orderItem.price);
			//cout<<"Inserting into level "<<levels<<" And size of the book is "<<bidBook.size()<<"\n";
			if (bestBid < orderItem.price)
			{
				addMultipleLevels(bidBook, levels);
				bidBook[0].bookItems.push_back(bookItem);
				bidBook[0].cumQty = orderItem.quantity;
				bestBid = orderItem.price;
			}
			else if(levels<=20000)
			{
				bidBook[levels].bookItems.push_back(bookItem);
				bidBook[levels].cumQty += orderItem.quantity;
			}
			//cout<<"Inserted into the book\n";
		}
		totalBids += orderItem.quantity;
	}
}

void processAskLimit(OrderItem &orderItem)
{
	if (orderItem.price <= bestBid && preOpen == false)
	{
		//match order and update quantity
		//cout<<"Matching limit Ask order\n";
		matchAskLimitOrder(orderItem);
	}
	if (orderItem.quantity > 0)
	{
		BookItem bookItem = getBookItem(orderItem);
		if (totalAsks == 0)
		{
			bestAsk = orderItem.price;
			askBook[0].bookItems.push_back(bookItem);
			askBook[0].cumQty = orderItem.quantity;
		}
		else
		{
			int levels = getLevel(bestAsk, orderItem.price);
			//cout<<"Inserting into level "<<levels<<" And size of the book is "<<askBook.size()<<"\n";
			if (bestAsk > orderItem.price)
			{  
				addMultipleLevels(askBook, levels);
				askBook[0].bookItems.push_back(bookItem);
				askBook[0].cumQty = orderItem.quantity;
				bestAsk = orderItem.price;
			}
			else if (levels <= 20000)
			{
				askBook[levels].bookItems.push_back(bookItem);
				askBook[levels].cumQty += orderItem.quantity;
			}
			//cout<<"Inserted into the book\n";
		}
		totalAsks += orderItem.quantity;
	}
}

void matchBidLimitOrder(OrderItem &orderItem)
{
	int remainingQty = orderItem.quantity;
	int levels = getLevel(orderItem.price, bestAsk);
	int totalAvlQty = getCumQty(askBook, levels);
	for (int i=0 ; i<=levels ; i++)
	{
		for (int j=0 ; j<askBook[i].bookItems.size() ; j++)
		{
			if (remainingQty >= askBook[i].bookItems[j].quantity)
			{
				if (askBook[i].bookItems[j].orderId == "2")
				{
					avlQty -= askBook[i].bookItems[j].quantity;
					avlCash += askBook[i].bookItems[j].quantity * getAskPrice(bestAsk, i);
				}
				remainingQty -= askBook[i].bookItems[j].quantity;
				volume += askBook[i].bookItems[j].quantity;
				totalAsks -= askBook[i].bookItems[j].quantity;
				totalAvlQty -= askBook[i].bookItems[j].quantity;
				askBook[i].cumQty -= askBook[i].bookItems[j].quantity;
				askBook[i].bookItems[j].quantity = 0;
			}
			else
			{
				if(askBook[i].bookItems[j].orderId == "2")
				{
					avlQty -= remainingQty;
					avlCash += remainingQty * getAskPrice(bestAsk, i);
				}
				askBook[i].bookItems[j].quantity -= remainingQty;
				volume += remainingQty;
				totalAsks -= remainingQty;
				totalAvlQty -= remainingQty;
				askBook[i].cumQty -= remainingQty;
				remainingQty = 0;
			}
			if(remainingQty==0 || totalAvlQty==0)
			{
				if (totalAsks == 0)
				{
					bestAsk=999999999;
				}
				else
				{
					cleanUpLevel(askBook[i].bookItems);
					int n = popLevelItem(askBook);
					bestAsk += (0.05*n);
				}
				orderItem.quantity = remainingQty;
				return;
			}
		}
	}
}

void matchBuyOrder(OrderItem &orderItem)
{
	int remainingQty = orderItem.quantity;
	for (int i=0 ; i<askBook.size() ; i++)
	{
		for (int j=0 ; j<askBook[i].bookItems.size() ; j++)
		{
			if(remainingQty >= askBook[i].bookItems[j].quantity)
			{
				if(askBook[i].bookItems[j].orderId == "2")
				{
					avlQty -= askBook[i].bookItems[j].quantity;
					avlCash += askBook[i].bookItems[j].quantity * getAskPrice(bestAsk, i);
				}
				remainingQty -= askBook[i].bookItems[j].quantity;
				totalAsks -= askBook[i].bookItems[j].quantity;
				volume += askBook[i].bookItems[j].quantity;
				askBook[i].cumQty -= askBook[i].bookItems[j].quantity;
				askBook[i].bookItems[j].quantity=0;
			}
			else
			{
				if(askBook[i].bookItems[j].orderId == "2")
				{
					avlQty -= remainingQty;
					avlCash += remainingQty * getAskPrice(bestAsk, i);
				}
				askBook[i].bookItems[j].quantity -= remainingQty;
				volume += remainingQty;
				totalAsks -= remainingQty;
				askBook[i].cumQty -= remainingQty;
				remainingQty = 0;
			}
			if(remainingQty==0 || totalAsks==0)
			{
				if (totalAsks == 0)
				{
					bestAsk=999999999;
				}
				else
				{
					cleanUpLevel(askBook[i].bookItems);
					int n = popLevelItem(askBook);
					bestAsk += (0.05*n);
				}
				orderItem.quantity = remainingQty;
				return;
			}
		}
	}
}

void matchSellOrder(OrderItem &orderItem)
{	
	int remainingQty = orderItem.quantity;
	for (int i=0 ; i<=bidBook.size() ; i++)
	{
		for (int j=0 ; j<bidBook[i].bookItems.size() ; j++)
		{
			if(remainingQty >= bidBook[i].bookItems[j].quantity)
			{
				if(bidBook[i].bookItems[j].orderId == "1")
				{
					avlQty += bidBook[i].bookItems[j].quantity;
					avlCash -= bidBook[i].bookItems[j].quantity * getBidPrice(bestBid, i);
				}
				remainingQty -= bidBook[i].bookItems[j].quantity;
				volume += bidBook[i].bookItems[j].quantity;
				totalBids -= bidBook[i].bookItems[j].quantity;
				bidBook[i].cumQty -= bidBook[i].bookItems[j].quantity;
				bidBook[i].bookItems[j].quantity=0;
			}
			else
			{
				if(bidBook[i].bookItems[j].orderId == "1")
				{
					avlQty += remainingQty;
					avlCash -= remainingQty * getBidPrice(bestBid, i);
				}
				bidBook[i].bookItems[j].quantity -= remainingQty;
				volume += remainingQty;
				totalBids -= remainingQty;
				bidBook[i].cumQty -= remainingQty;
				remainingQty = 0;
			}
			if(remainingQty==0 || totalBids==0)
			{
				if (totalBids == 0)
				{
					bestBid = -999999999;
				}
				else
				{
					cleanUpLevel(bidBook[i].bookItems);
					int n = popLevelItem(bidBook);
					bestBid -= (0.05*n);
				}
				orderItem.quantity = remainingQty;
				return;
			}
		}
	}
}

void matchAskLimitOrder(OrderItem &orderItem)
{
	int remainingQty = orderItem.quantity;
	int levels = getLevel(orderItem.price, bestBid);
	int totalAvlQty = getCumQty(bidBook, levels);
	for (int i=0 ; i<=levels ; i++)
	{
		for (int j=0 ; j<bidBook[i].bookItems.size() ; j++)
		{
			if(remainingQty >= bidBook[i].bookItems[j].quantity)
			{
				if(bidBook[i].bookItems[j].orderId == "1")
				{
					avlQty += bidBook[i].bookItems[j].quantity;
					avlCash -= bidBook[i].bookItems[j].quantity * getBidPrice(bestBid, i);
				}
				remainingQty -= bidBook[i].bookItems[j].quantity;
				volume += bidBook[i].bookItems[j].quantity;
				totalBids -= bidBook[i].bookItems[j].quantity;
				totalAvlQty -= bidBook[i].bookItems[j].quantity;
				bidBook[i].cumQty -= bidBook[i].bookItems[j].quantity;
				bidBook[i].bookItems[j].quantity=0;
			}
			else
			{
				if(bidBook[i].bookItems[j].orderId == "1")
				{
					avlQty += remainingQty;
					avlCash -= remainingQty * getBidPrice(bestBid, i);
				}
				bidBook[i].bookItems[j].quantity -= remainingQty;
				totalBids -= remainingQty;
				totalAvlQty -= remainingQty;
				bidBook[i].cumQty -= remainingQty;
				remainingQty = 0;
			}
			if(remainingQty==0 || totalAvlQty==0)
			{
				if (totalBids == 0)
				{
					bestBid = -999999999;
				}
				else
				{
					cleanUpLevel(bidBook[i].bookItems);
					int n = popLevelItem(bidBook);
					bestBid -= (0.05*n);
				}
				orderItem.quantity = remainingQty;
				return;
			}
		}
	}
}

void processDelete(OrderItem orderItem)
{
	//Checking bid book
	for (int i=0 ; i<20000 ; i++)
	{
		for (int j=0 ; j<bidBook[i].bookItems.size() ; j++)
		{
			if (orderItem.orderId == bidBook[i].bookItems[j].orderId)
			{
				//cout<<"Deleting  order\n";
				bidBook[i].cumQty -= bidBook[i].bookItems[j].quantity;
				totalBids -= bidBook[i].bookItems[j].quantity;
				deleteOrder(bidBook[i].bookItems, j);
				if (i==0 && totalBids>0)
				{
					int level = popLevelItem(bidBook);
					bestBid -= (0.05*level);
				}
				if (totalBids == 0)
				bestBid=-999999999;
				return;
			}
		}
	}
	//cout<<"Not found in Bid Book. Searching Ask book\n";
	for (int i=0 ; i<20000 ; i++)
	{
		for (int j=0 ; j<askBook[i].bookItems.size() ; j++)
		{
			if (orderItem.orderId == askBook[i].bookItems[j].orderId)
			{
				//cout<<"Deleting  order\n";
				askBook[i].cumQty -= askBook[i].bookItems[j].quantity;
				totalAsks -= askBook[i].bookItems[j].quantity;
				deleteOrder(askBook[i].bookItems, j);
				if (i==0 && totalAsks>0)
				{
					int level = popLevelItem(askBook);
					bestAsk += (0.05*level);
				}
				if (totalAsks == 0)
				bestAsk=999999999;
				return;
			}
		}
	}
	if(orderItem.action == "M")
	{
		pendingCan = true;
		canOrder = orderItem.orderId;
	}
	//cout<<"Not found anywhere\n";
}

void poMatch()
{
	int remainingQty = openingQty;
	OrderItem orderItem;
	while (bestBid >= openPrice)
	{
		if (remainingQty > bidBook[0].cumQty)
		{
			orderItem.quantity = bidBook[0].cumQty;
			remainingQty -= bidBook[0].cumQty;
			matchSellOrder(orderItem);
		}
		else
		{
			orderItem.quantity = remainingQty;
			matchSellOrder(orderItem);
			break;
		}
	}
	remainingQty = openingQty;
	while (bestAsk <= openPrice)
	{
		if (remainingQty > askBook[0].cumQty)
		{
			orderItem.quantity = askBook[0].cumQty;
			remainingQty -= askBook[0].cumQty;
			matchBuyOrder(orderItem);
		}
		else
		{
			orderItem.quantity = remainingQty;
			matchBuyOrder(orderItem);
			break;
		}
	}
}
