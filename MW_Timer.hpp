#include <mutex>
#include <mpi.h>

#include "MW_Object.hpp"

using namespace std;

class MW_Timer : public MW_Object {
public:
	MW_Timer(double);

	virtual bool get() const;
	void reset();
private:
	double expirationTime;
	const double period;
	mutable mutex b_mutex;

};
