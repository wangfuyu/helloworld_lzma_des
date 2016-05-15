# build https-cli executable when user executes "make"

LDFLAGS += -llzma
LDFLAGS += -lsecurity

#CFLAGS += -std=c99
CFLAGS += -std=gnu99

APP_NAME = helloworld

OBJ = helloworld.o
OBJ += cloud-lzma.o
OBJ += cloud-encrypt.o

$(APP_NAME): $(OBJ)
	$(CC) $^ -o $(APP_NAME) $(LDFLAGS)

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@

# remove object files and executable when user executes "make clean"
clean:
	rm *.o $(APP_NAME)
