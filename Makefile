# https://docs.particle.io/guide/tools-and-features/cli/photon/
# npm install -g particle-cli

BOARD := photon

.PHONY: all clean

all:
	particle compile $(BOARD)

clean:
	rm $(BOARD)_firmware*.bin
