#include <string.h>
#include <vector>
#include "../models/OrderItem.cpp"
using namespace std;

//sample input - 3|541983|B|L|2261|8000|8000|8000|0|2023-10-03 09:00:00.001194|1696303800001194897|0|A|1696303800000000019|0|N
/*
1-product
9-date
*/
vector<string> getOrderParams(string orderLine)
{
	vector<string> orderParams;
	char* str = &orderLine[0];
	char *token = strtok(str, "|");
	
	while(token != NULL)
	{
		orderParams.push_back(token);
		token = strtok(NULL, "|");
	}
	return orderParams;
}

float getPrice(string priceStr)
{
	if(priceStr=="0")
	return 0;
	string decimal_point = ".";
	priceStr.insert(priceStr.length()-2, decimal_point);
	return stof(priceStr);
}

OrderItem getOrderItem(string orderLine)
{
	vector<string> orderParams = getOrderParams(orderLine);
	OrderItem orderItem;
	orderItem.session = orderParams[0];
	orderItem.instrument = orderParams[1];
	orderItem.side = orderParams[2];
	orderItem.type = orderParams[3];
	orderItem.price = getPrice(orderParams[4]);
	orderItem.quantity = stoi(orderParams[5]);
	orderItem.versionTime = orderParams[9];
	orderItem.action = orderParams[12];
	orderItem.orderId = orderParams[13];
	return orderItem;
}

int getHour(string date)
{
	string hour_str = date.substr(11,2);
	return(stoi(hour_str));
}

int getMin(string date)
{
	string hour_min = date.substr(14,2);
	return(stoi(hour_min));
}

int getSec(string date)
{
	string hour_sec = date.substr(17,2);
	return(stoi(hour_sec));
}
