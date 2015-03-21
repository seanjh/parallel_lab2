#include "MW_Object.hpp"
#include <mpi.h>
#include <mutex>

class MW_Timer : public MW_Object {
public:
	MW_Timer(double);
	
	virtual bool get() const;
	void reset();
private:
	double expirationTime;
	const double period;
	mutable std::mutex b_mutex;

};