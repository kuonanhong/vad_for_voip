CC :=g++
CFLAGS :=  -I.
LDFLAGS := -L.


CKFFT_SRC_FILES := \
    ckfft/debug.cpp \
    ckfft/context.cpp \
    ckfft/ckfft.cpp \
    ckfft/fft.cpp \
    ckfft/fft_default.cpp \
    ckfft/fft_real.cpp \
    ckfft/fft_real_default.cpp \
    ckfft/fft_neon.cpp \
    ckfft/fft_real_neon.cpp
CKFFT_OBJS := $(CKFFT_SRC_FILES:.cpp=.o)

all: ltsd.o ms.o mmse.o libckfft.so
	$(CC) $(CFLAGS) $(LDFLAGS) -o vad  mmse.o ms.o ltsd.o vad.cpp -lckfft 

ltsd.o: ltsd.cpp
	$(CC) $(CFLAGS) -o ltsd.o -c LTSD.cpp

ms.o: MinimumStatistics.cpp
	$(CC) $(CFLAGS) -o ms.o -c MinimumStatistics.cpp

mmse.o: MmseBasedNpe.cpp
	$(CC) $(CFLAGS) -o mmse.o -c MmseBasedNpe.cpp

libckfft.so: $(CKFFT_SRC_FILES)
	$(CC) $(CFLAGS) -shared -fPIC -o libckfft.so $(CKFFT_SRC_FILES)

$(CKFFT_OBJS): $(CKFFT_SRC_FILES)
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf *.so
	rm -rf *.o
	rm -rf *.a
	rm -rf ckfft/*.o
	rm -rf vad
