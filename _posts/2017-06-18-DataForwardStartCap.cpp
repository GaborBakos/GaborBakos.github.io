#include "Session4/Session4.hpp"
using namespace cfl;


/** 
* Computes the price of <strong>forward start cap</strong>.
* At issue time \a dIssueTime the cap contract is initiated. 
* The fixed rate in the cap is set to be the market 
* <em>swap rate</em> for the swap with
* the parameters \a dPeriod and \a iNumberOfPayments issued at \a
* dIssueTime. 
* (Recall that swap rate equals the fixed rate in the swap
* contract such that the value of the swap is zero).
* 
* \param dIssueTime The issue time of cap. At this time, the
* cap rate is set to be the market <em>swap rate</em> 
* for the swap with the parameters \a dPeriod and \a iNumberOfPayments. 
* \param dNotional The notional amount of the underlying cap.
* \param dPeriod The interval of time between two payments in the cap.
* \param iNumberOfPayments The number of payments in the cap.
* \param rModel Reference to implementation of InterestRateModel.
* 
* \return The price of the option as the function of the initial
* values of the state processes in the model.
*/

cfl::Slice
swapRate(unsigned iTime, double dPeriod,
unsigned iPeriods, const cfl::InterestRateModel & rModel){
	Slice uFixed = rModel.cash(iTime, 0.);
	double dTime = rModel.eventTimes()[iTime];
	for (unsigned iI=0; iI<iPeriods; iI++) {
		dTime += dPeriod;
		uFixed += rModel.discount(iTime, dTime);
	}
	uFixed *= dPeriod;
	Slice uFloat = 1. - rModel.discount(iTime, dTime);
	Slice uRate= (uFloat/uFixed);
	return uRate;
}


class HistSwapRate: public IResetValues{
public:
	HistSwapRate(double dPeriod, unsigned iNumberOfPayments,
	const InterestRateModel & rModel)
	:m_dPeriod(dPeriod), m_iNumberOfPayments(iNumberOfPayments),
	m_rModel(rModel)
	{}
	Slice resetValues(unsigned iTime, double dBeforeReset) const{
		return swapRate(iTime, m_dPeriod, m_iNumberOfPayments, m_rModel);
	}
private:
	double m_dPeriod;
	unsigned m_iNumberOfPayments;
	const InterestRateModel & m_rModel;
};

cfl::PathDependent
histSwapRate(double dPeriod, unsigned iNumberOfPeriods, double dInitialRate,
const std::vector<unsigned> & rResetTimes,const cfl::InterestRateModel & rModel){
	return PathDependent(new HistSwapRate(dPeriod, iNumberOfPeriods, rModel),
			rResetTimes, dInitialRate);
}


cfl::MultiFunction prb::forwardStartCap(double dIssueTime, double
				    dNotional, double dPeriod,
				    unsigned iNumberOfPayments, 
				    cfl::InterestRateModel & rModel){

	PRECONDITION(rModel.initialTime() < dIssueTime);
	//event times = initial time + issue time + all payment times except the last
	std::vector<double> uEventTimes(iNumberOfPayments+1);
	uEventTimes.front() = rModel.initialTime();
	uEventTimes[1] = dIssueTime;
	std::transform(uEventTimes.begin()+1, uEventTimes.end()-1,uEventTimes.begin()+2,
			[dPeriod](double dX){ return dX+dPeriod; });
	rModel.assignEventTimes(uEventTimes);

	std::vector<unsigned> uResetIndexes(1,1);
	double dInitialRate = 0.; //does not matter

	int iCapRate = rModel.addState(histSwapRate(dPeriod, iNumberOfPayments,
	dInitialRate, uResetIndexes, rModel));
	//last minus one payment time

	int iTime = uEventTimes.size()-1;
	Slice uDiscount =
	rModel.discount(iTime, rModel.eventTimes()[iTime]+dPeriod);
	Slice uCapFactor = 1.+ rModel.state(iTime, iCapRate)*dPeriod;
	//value of the next payment (in percent of notional)
	Slice uOption = max(1. - uDiscount*uCapFactor, 0.);
	//loop up to issue time
	while (iTime > 1) {
		//uOption is the value of future payments
		//we multiply on notional at the end
		iTime--;
		uOption.rollback(iTime);
		uDiscount = rModel.discount(iTime, rModel.eventTimes()[iTime]+dPeriod);
		uCapFactor = 1.+ rModel.state(iTime, iCapRate)*dPeriod;
		uOption += max(1. - uDiscount*uCapFactor, 0.);
	}
	uOption *= dNotional;
	uOption.rollback(0);
	return interpolate(uOption, iCapRate);
}
