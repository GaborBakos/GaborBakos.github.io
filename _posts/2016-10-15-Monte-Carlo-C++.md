---
title: "Monte Carlo C++"
author: "Gabor Bakos"
date: "15 October 2016"
output: html_document
---

This is Practical is part of the Financial Computing with C++ course at the University of Oxford.

Below is the cpp file broken up into its chunks:


```{Rcpp}
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <cmath>
#include "MyRand.hpp"
```



```{Rcpp}
namespace exercises
{

typedef double (*Payoff)(double);

typedef double (*Payoff)(double);

double Norm2(const std::vector<double> & dVec){
	/** Euclidean norm of a vector
	  * @param dVec vector of doubles
	  * @return Euclidean norm of dVec 
	  */
	double result = 0;
	for (int i=0; i<dVec.size(); i++){
		result+= dVec[i]*dVec[i];
	}
	return sqrt(result);
};
```

```{Rcpp}
double NormInf(const std::vector<double> & dVec){
	/** Inf norm of a vector
	  * @param dVec vector of doubles
	  * @return inf norm of dVec
	  */
	double result = 0;
	for (int i=0; i<dVec.size(); i++){
		if(dVec[i]>0 && dVec[i]>result){
			result = dVec[i];
		}
		else if(dVec[i]<0 && (-dVec[i])>result){
			result = -dVec[i];
		}
	}
	return result;

};
```

```{Rcpp}
double MonteCarlo1(double dR, 
				   double dSigma, 
				   double dS0, 
				   double dK , 
				   double dT, 
				   unsigned long int iN){
	/** Simple Monte Carlo estimation of a Call
	  * @param dR riskfree rate of return
	  * @param dSigma sigma
	  * @param dS0 initial stock price
	  * @param dK strike price
	  * @param dT time to maturity
	  * @param iN sample size
	  * @return Monte Carlo estimate
	  */
	double S_Price = 0;
	double payoff_sum = 0;
	for (int i=0; i < iN; i++){
		S_Price = 0;
		double NormalRandom  = NormalDist();
		S_Price = dS0*exp((dR-0.5*dSigma*dSigma)*dT + dSigma*sqrt(dT)*NormalRandom);
		payoff_sum += std::max(S_Price-dK,0.0);
	}
	
	return (payoff_sum/iN)*exp(-dR*dT);
};
```


```{Rcpp}
std::vector<double> MonteCarlo2(double dR, 
				   double dSigma, 
				   double dS0, 
				   double dT, 
				   unsigned long int iN, 
				   Payoff call){
/** Simple Monte Carlo estimation of a European function
	  * @param dR riskfree rate of return
	  * @param dSigma sigma
	  * @param dS0 initial stock price
	  * @param dT time to maturity
	  * @param iN sample size
	  * @param call payoff function
	  * @return a vector of two entries: Monte Carlo estimate, stdev estimate
	  */
	
	double payoff_sum = 0;
	std::vector<double> S_Price;
	for (int i=0; i < iN; i++){
		double NormalRandom  = NormalDist();
		S_Price.push_back(dS0*exp((dR-0.5*dSigma*dSigma)*dT + dSigma*sqrt(dT)*NormalRandom));
		payoff_sum += call(S_Price[i]);
	}
	std::vector<double> results;
	results.push_back((payoff_sum/iN)*exp(-dR*dT));
	double mean = payoff_sum/iN;
	double variance = Norm2(S_Price);
	variance *= variance;
	variance/=iN;
	variance -= mean*mean;




	double standard_dev = sqrt(variance);
	results.push_back(standard_dev);
	return results;
};
```

```{Rcpp}
double callAt1(double dS){
/** Particular payoff function - call with strike 1.0
	  * @param dS stock price
	  * @return call payoff with strike 1.0
	  */
	return std::max(dS-1,0.0);
};
}
```

Below is the header file:
```{Rcpp}
#ifndef _PRACTICAL_01_EXERCISES_H__
#define _PRACTICAL_01_EXERCISES_H__


#include <vector>
#include <string>


namespace exercises
{

static std::string sMyName("Gabor Bakos");

typedef double (*Payoff)(double);

/** Euclidean norm of a vector
	  * @param dVec vector of doubles
	  * @return Euclidean norm of dVec 
	  */
double Norm2(const std::vector<double> & dVec);

/** Inf norm of a vector
	  * @param dVec vector of doubles
	  * @return inf norm of dVec
	  */
double NormInf(const std::vector<double> & dVec);


/** Simple Monte Carlo estimation of a Call
	  * @param dR riskfree rate of return
	  * @param dSigma sigma
	  * @param dS0 initial stock price
	  * @param dK strike price
	  * @param dT time to maturity
	  * @param iN sample size
	  * @return Monte Carlo estimate
	  */
double MonteCarlo1(double dR, 
				   double dSigma, 
				   double dS0, 
				   double dK , 
				   double dT, 
				   unsigned long int iN);


/** Simple Monte Carlo estimation of a European function
	  * @param dR riskfree rate of return
	  * @param dSigma sigma
	  * @param dS0 initial stock price
	  * @param dT time to maturity
	  * @param iN sample size
	  * @param call payoff function
	  * @return a vector of two entries: Monte Carlo estimate, stdev estimate
	  */
std::vector<double> MonteCarlo2(double dR, 
				   double dSigma, 
				   double dS0, 
				   double dT, 
				   unsigned long int iN, 
				   Payoff call);


/** Particular payoff function - call with strike 1.0
	  * @param dS stock price
	  * @return call payoff with strike 1.0
	  */
double callAt1(double dS);

}

#endif //_PRACTICAL_01_EXERCISES_H__
```

Here is a sample of usage of the above declared functions. 

```{Rcpp}
#include "Practical01Exercises.hpp"
#include "BSOptionPrice.hpp"
#include <iostream>
#include <fstream>

using namespace std;
using namespace exercises;

void TestNorms(); 
void TestMonteCarlo();

int main()
{
	fstream myfile;
	myfile.open ("Practical01_output.txt", ios::in | ios::out | ios::trunc);
	myfile << endl;
	myfile << sMyName << endl;
	myfile << endl;
	myfile.close();
	TestMonteCarlo();
	TestNorms();

	ifstream myfile1;
	myfile1.open ("Practical01_output.txt", ios::binary);
	std::string line;
    while(myfile1.good())
	{
      getline (myfile1,line);
      cout << line << endl;
    }
	myfile1.close();
	return 0;
}


void TestNorms()
{
	vector<double> v;
	v.push_back(0.0);
	v.push_back(-10.0);
	v.push_back(10.0);
	v.push_back(20.0);
	v.push_back(30.0);
	v.push_back(-30.0);
	v.push_back(0.0);
	v.push_back(-100.0);

	fstream myfile;
	myfile.open ("Practical01_output.txt", ios::in | ios::out | ios::app);

	myfile<< endl;
	myfile<< "**********************************************" << endl;
	myfile<< "*       Testing Norm2() and NormInf()        *" << endl;
	myfile<< "**********************************************" << endl;
	myfile << "The vector is: " << endl;
	for(vector<double>::size_type i=0; i<v.size(); ++i)
		myfile << v[i] << " " ;
	myfile << endl;

	myfile << "The 2 norm of v is: " << Norm2(v) << endl;
	myfile << "The inf norm of v is: " << NormInf(v) << endl;
	myfile<< endl;

	myfile.close();

}
void TestMonteCarlo()
{
	double r(0.05), sigma(0.4), S0(1.0), K(1.0), T(0.5);
	unsigned long int N(1000000);

	fstream myfile;
	myfile.open ("Practical01_output.txt", ios::in | ios::out | ios::app);

	myfile<< endl;
	myfile<< "**********************************************" << endl;
	myfile<< "*  Testing MonteCarlo1() and MonteCarlo2()   *" << endl;
	myfile<< "**********************************************" << endl;
	myfile<< "Parameters: " << endl;
	myfile<< "r: \t" << r << endl;
	myfile<< "sigma: \t" << sigma << endl;
	myfile<< "S0: \t" << S0 << endl;
	myfile<< "K: \t" << K << endl;
	myfile<< "T: \t" << T << endl;
	myfile<< "N: \t" << N << endl;
	double price1=MonteCarlo1(r,sigma,S0,K,T,N);
	vector<double> price2=MonteCarlo2(r,sigma,S0,T,N,callAt1);
	myfile<< "price1: \t" << price1 << endl;
	myfile<< "price2: \t" << price2[0] << endl;
	myfile<< "stdev2: \t" << price2[1] << endl;
	myfile<< "Black-Scholes price: \t" << BSOptionPrice(S0,K,T,r,sigma) << endl;
	myfile<< endl;

	myfile.close();
}
```
The output for the above code is:
```{r}



Gabor Bakos


**********************************************
*  Testing MonteCarlo1() and MonteCarlo2()   *
**********************************************
Parameters: 
r: 	0.05
sigma: 	0.4
S0: 	1
K: 	1
T: 	0.5
N: 	1000000
price1: 	0.123715
price2: 	0.123853
stdev2: 	1.05972
Black-Scholes price: 	0.12385


**********************************************
*       Testing Norm2() and NormInf()        *
**********************************************
The vector is: 
0 -10 10 20 30 -30 0 -100 
The 2 norm of v is: 111.355
The inf norm of v is: 100
```



