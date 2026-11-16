all:  $(patsubst %.mac,%.root,$(wildcard *.mac))
	echo "Geant4 simulations done."

# Pattern rules for running Geant4 simulations.
%.out : %.mac
	if test -e $*.out; \
	then echo "Removing existing file $*.out"; rm $*.out; \
	fi
	UCCeBrA $*.mac 2>&1 | tee $*.log
ifeq ($(G4MULTITHREADED), 1)
	./concat_out.sh $* 2>&1   | tee -a $*.log
endif

# Pattern rule for sorting the simulation output into Root histograms
%.root : %.out
	./co60_sim_sort.py $*.out 2>&1 | tee -a $*.log

clean:
	rm -f *.log *.out *.root *.wrl
