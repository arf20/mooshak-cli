APPLICATION := mooshak-cli
LIBRARY := libmooshak

$(APPLICATION)/$(APPLICATION): $(LIBRARY)/$(LIBRARY).a
	$(MAKE) -C $(APPLICATION)/

$(LIBRARY)/$(LIBRARY).a:
	$(MAKE) -C $(LIBRARY)/

.PHONY: clean
clean:
	$(MAKE) -C $(APPLICATION)/ clean
	$(MAKE) -C $(LIBRARY)/ clean

