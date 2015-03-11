CXX=mpic++
ODIR=obj

LIBS =-lgmpxx -lgmp
CXX_FLAGS =-Wc++11-extensions

_SERIAL_OBJ = gmp_factors_serial.o
SERIAL_OBJ = $(patsubst %,$(ODIR)/%,$(_SERIAL_OBJ))

_MASTER_OBJ = Master.o
MASTER_OBJ = $(patsubst %,$(ODIR)/%,$(_MASTER_OBJ))

_WORKER_OBJ = Worker.o
WORKER_OBJ = $(patsubst %,$(ODIR)/%,$(_WORKER_OBJ))

_MW_API_OBJ = MW_API.o MW_Master.o MW_Worker.o
MW_API_OBJ = $(patsubst %,$(ODIR)/%,$(_MW_API_OBJ))

_DIV_GEN_OBJ = DivisorGenerator.o
DIV_GEN_OBJ = $(patsubst %,$(ODIR)/%,$(_DIV_GEN_OBJ))

_PARTIAL_OBJ = gmp_factors_partial_parallel.o
PARTIAL_OBJ = $(patsubst %,$(ODIR)/%,$(_PARTIAL_OBJ))

_PARTIAL_API_OBJ = gmp_factors_partial_parallel_API.o DivisorResult.o DivisorWork.o
PARTIAL_API_OBJ = $(patsubst %,$(ODIR)/%,$(_PARTIAL_API_OBJ))

_TEST_API_OBJ = gmp_factors_Test_API.o DivisorResult.o DivisorWork.o
TEST_API_OBJ = $(patsubst %,$(ODIR)/%,$(_TEST_API_OBJ))

# DivisorApplication.o DivisorResult.o DivisorWork.o
_WORKING_API_OBJ = gmp_factors_API.o DivisorApplication.o DivisorResult.o DivisorWork.o
WORKING_API_OBJ = $(patsubst %,$(ODIR)/%,$(_WORKING_API_OBJ))
# _PROFILE_OBJ = mpi-profile.o
# PROFILE_OBJ = $(patsubst %,$(ODIR)/%,$(_PROFILE_OBJ))

_DIVISORS_APP_OBJ = gmp_factors_API.o DivisorApplication.o DivisorResult.o DivisorWork.o MW_API.o MW_Master.o MW_Worker.o
DIVISORS_APP_OBJ = $(patsubst %,$(ODIR)/%,$(_DIVISORS_APP_OBJ))

# _TEST_OBJ = test.o
# TEST_OBJ = $(patsubst %,$(ODIR)/%,$(_TEST_OBJ))

# _TEST_COMM_OBJ = test_comm.o
# TEST_COMM_OBJ = $(patsubst %,$(ODIR)/%,$(_TEST_COMM_OBJ))

# _MAX_LEN_TEST_OBJ = maxLenTest.o
# MAX_LEN_TEST_OBJ = $(patsubst %,$(ODIR)/%,$(_MAX_LEN_TEST_OBJ))

obj:
	mkdir -p $@

$(ODIR)/%.o: %.cpp | obj
	$(CXX) -std=c++11 -I. -c -o $@ $<

serial: $(SERIAL_OBJ)
	$(CXX) -o $@ $^ $(LIBS)

partial: $(PARTIAL_OBJ) $(DIV_GEN_OBJ)
	$(CXX) -std=c++11 -o $@ $^ $(LIBS)

partial_api: $(PARTIAL_API_OBJ)
	$(CXX) -std=c++11 -o $@ $^ $(LIBS)

test_api: $(TEST_API_OBJ)
	$(CXX) -std=c++11 -o $@ $^ $(LIBS)

worker: $(WORKER_OBJ)
	$(CXX) -std=c++11 -o $@ $^ $(LIBS)

master: $(MASTER_OBJ)
	$(CXX) -c -std=c++11 -o $@ $^ $(LIBS)

mw_api: $(MW_API_OBJ)
	$(CXX) -std=c++11 -o $@ $^ $(LIBS)

working_api: $(WORKING_API_OBJ)
	$(CXX) -std=c++11 -o $@ $^ $(LIBS)

divisors_app: $(DIVISORS_APP_OBJ)
	$(CXX) -std=c++11 -o $@ $^ -L../gmplib/lib $(LIBS)

# profile: $(PROFILE_OBJ)
# 	$(CXX) -o $@ $^

# test: $(TEST_OBJ)
# 	$(CXX) -o $@ $^

# test_comm: $(TEST_COMM_OBJ)
# 	$(CXX) -o $@ $^

# maxLenTest: $(MAX_LEN_TEST_OBJ)
# 	$(CXX) -o $@ $^

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o serial
