.PHONY: subsystem clean

subsystem:
	$(MAKE) -C client
	$(MAKE) -C server

clean:
	$(MAKE) clean -C client
	$(MAKE) clean -C server