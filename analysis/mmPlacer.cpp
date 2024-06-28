#include "probablityAnalysis.cpp"
#include <cmath>
using namespace std;

//present order price
float mmCurrAsk = -1;
float mmCurrBid = -1;
int bidQty = 0;
int maxQty = 69;

void updateMmOrders()
{
	float newBid, newAsk;
	updateBidAskLevels();
	newBid = getBidPrice(bestBid, mmBid);
	newAsk = getAskPrice(bestAsk, mmAsk);
	if (avlQty != bidQty || mmCurrAsk != newAsk)
	{
		if(avlQty > 0)
		{
			OrderItem askOrder = createOrder("S", newAsk, avlQty, "2");
			processOrder(askOrder);
		}
		bidQty = avlQty;
	}
	if (avlQty<maxQty || mmCurrBid != newBid)
	{
		if(avlQty !=maxQty)
		{
			OrderItem bidOrder = createOrder("B", newBid, maxQty-avlQty, "1");
			processOrder(bidOrder);
		}
	}
	
	mmCurrAsk = newAsk;
	mmCurrBid = newBid;
}


void meanReversionMM()
{
	float newBid, newAsk;
	if(mmPhase == true)
	{
		mmBid = 0;
		mmAsk = 2;
	}
	else
	{
		mmAsk = 0;
		mmBid = 4;
	}
	newBid = getBidPrice(bestBid, mmBid);
	newAsk = getAskPrice(bestAsk, mmAsk);
	if (avlQty != bidQty || mmCurrAsk != newAsk)
	{
		if(avlQty > 0)
		{
			OrderItem askOrder = createOrder("S", newAsk, avlQty, "2");
			processOrder(askOrder);
		}
		bidQty = avlQty;
	}
	if (avlQty<maxQty || mmCurrBid != newBid)
	{
		if(avlQty !=maxQty)
		{
			OrderItem bidOrder = createOrder("B", newBid, maxQty-avlQty, "1");
			processOrder(bidOrder);
		}
	}
	
	mmCurrAsk = newAsk;
	mmCurrBid = newBid;	
}