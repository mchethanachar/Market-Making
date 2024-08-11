#include "probablityAnalysis.cpp"
#include <cmath>
using namespace std;

//present order price
float mmCurrAsk = -1;
float mmCurrBid = -1;
int bidQty = 0;
int maxQty = 28;

void recaliberateOrders(float flowSlope, float priceSlope);

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


void updateMmOrders2(float flowSlope, float priceSlope)
{
	float newBid, newAsk;
	updateBidAskLevels();
	recaliberateOrders(flowSlope, priceSlope);
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

void recaliberateOrders(float flowSlope, float priceSlope)
{
	if(priceSlope>0.01)
	{
		if((bestAsk-bestBid)>0.05)
		mmBid--;
	}
	if(priceSlope<0.01 && flowSlope>0.01)
	{
		mmBid++;
		if((bestAsk-bestBid)>0.05)
		mmAsk--;
	}
	if(priceSlope<0.01 && flowSlope<0.01)
	{
		mmBid+=2;
		if((bestAsk-bestBid)>0.05)
		mmAsk--;
	}
}

void updateMmOrders3(bool maSwitch)
{
	float newBid, newAsk;
	updateBidAskLevels();
	if(maSwitch == false)
	{
		mmBid = 10;
		mmAsk = 0;
		if((bestAsk-bestBid)>0.05)
		{
			mmAsk = -1;
		}
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