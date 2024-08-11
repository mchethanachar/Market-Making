#include "../implementation/TradeProcessor.cpp"
#include <cmath>
using namespace std;

//Market maker's levels
int mmBid = -1;
int mmAsk = -1;

//minimum diff between cumBid and cumAsk
int minDiff;

//Cumulative quantities on order book
vector<int> cumBid(5);
vector<int> cumAsk(5);

//This finds bid and ask levels where Cumulative bid and ask forces are closest
void updateBidAskLevels()
{
	int i,j;
	cumBid[0] = bidBook[0].cumQty;
	cumAsk[0] = askBook[0].cumQty;
	minDiff = 99999;
	for(i=1; i<5; i++)
	{
		if(bidBook[i].cumQty>0)
		{
			cumBid[i] = bidBook[i].cumQty + cumBid[i-1];
		}
		else
		{
			cumBid[i] = cumBid[i-1];
		}
		if(askBook[i].cumQty>0)
		{
			cumAsk[i] = askBook[i].cumQty + cumAsk[i-1];
		}
		else
		{
			cumAsk[i] = cumAsk[i-1];
		}
	}
	
	for(i=0; i<5; i++)
	{
		int localMin = 99999;
		int diff;
		for(j=0; j<5; j++)
		{
			diff = abs(cumBid[i] - cumAsk[j]);
			if(diff>localMin)
			{
				break;
			}
			localMin = diff;
		}
		if(localMin<minDiff)
		{
			minDiff = localMin;
			mmBid = i;
			mmAsk = j-1;
		}
		if(localMin == minDiff)
		{
			if(abs((-1*i)-j)>abs((-1*mmBid)-mmAsk))
			{
				mmBid = i;
				mmAsk = j-1;
			}
		}
	}
}
