CXX=clang++
ODIR=obj

LIBS =-lgmpxx -lgmp
CXX_FLAGS =-Wc++11-extensions

_SERIAL_OBJ = gmp_factors_serial.o
SERIAL_OBJ = $(patsubst %,$(ODIR)/%,$(_SERIAL_OBJ))

_DIV_GEN_OBJ = DivisorGenerator.o
DIV_GEN_OBJ = $(patsubst %,$(ODIR)/%,$(_DIV_GEN_OBJ))

_PARTIAL_OBJ = gmp_factors_partial_parallel.o
PARTIAL_OBJ = $(patsubst %,$(ODIR)/%,$(_PARTIAL_OBJ))

# _PROFILE_OBJ = mpi-profile.o
# PROFILE_OBJ = $(patsubst %,$(ODIR)/%,$(_PROFILE_OBJ))

# _TEST_OBJ = test.o
# TEST_OBJ = $(patsubst %,$(ODIR)/%,$(_TEST_OBJ))

# _TEST_COMM_OBJ = test_comm.o
# TEST_COMM_OBJ = $(patsubst %,$(ODIR)/%,$(_TEST_COMM_OBJ))

# _MAX_LEN_TEST_OBJ = maxLenTest.o
# MAX_LEN_TEST_OBJ = $(patsubst %,$(ODIR)/%,$(_MAX_LEN_TEST_OBJ))

obj:
	mkdir -p $@

$(ODIR)/%.o: %.cpp | obj
	$(CXX) -std=c++11 -c -o $@ $<

serial: $(SERIAL_OBJ)
	$(CXX) -o $@ $^ $(LIBS)

partial: $(PARTIAL_OBJ) $(DIV_GEN_OBJ)
	$(CXX) -std=c++11 -o $@ $^ $(LIBS)

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
