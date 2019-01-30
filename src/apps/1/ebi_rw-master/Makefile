TARGET_SPI=spi_rw
TARGET_EBI=ebi_rw

SOURCES=$(wildcard *.c)
OBJECTS_EBI=$(SOURCES:.c=.ebi.o)
OBJECTS_SPI=$(SOURCES:.c=.spi.o)

# Собираемой программе будет передаваться текстовый 
# дейфайн GIT_REPO_VERSION вида "perository_name/commit_SHA".
# Если в репозитории есть незафиксированные изменения,
# в конце добавляется "_modifed". 
ifeq ($(strip $(GIT_REPO_VERSION)),)
  REPO=$(notdir $(shell git rev-parse --show-toplevel 2> /dev/null))
  ifneq ($(strip $(REPO)),)
    COMMIT=$(shell git rev-parse --short HEAD)
    MODIFED=$(shell git status -s)
    ifeq ($(strip $(MODIFED)),)
      GIT_REPO_VERSION=$(REPO)/$(COMMIT)
    else
      GIT_REPO_VERSION=$(REPO)/$(COMMIT)_modifed
    endif
  else
    GIT_REPO_VERSION=unknow
  endif
endif

CFLAGS=-c -Wall -DGIT_REPO_VERSION=\"$(GIT_REPO_VERSION)\"
LDFLAGS=

.PHONY: all clean 

all: $(SOURCES) $(TARGET_SPI) $(TARGET_EBI)

$(TARGET_SPI): $(OBJECTS_SPI) 
	$(CC) $(LDFLAGS) $(OBJECTS_SPI) -o $@

$(TARGET_EBI): $(OBJECTS_EBI) 
	$(CC) $(LDFLAGS) $(OBJECTS_EBI) -o $@

%.spi.o : %.c
	$(CC) $(CFLAGS) -DSPI_RW -c $< -o $@

%.ebi.o : %.c
	$(CC) $(CFLAGS) -DEBI_RW -c $< -o $@

clean:
	rm -f $(TARGET_SPI) 
	rm -f $(TARGET_EBI) 
	rm -f *.o 

