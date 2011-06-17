include $(R_MAKECONF)

gwalib:	Plot.o PlotException.o Boxplot.o Histogram.o Qqplot.o                                         

Plot.o:	Plot.cpp
	$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

PlotException.o:	PlotException.cpp
			$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

Boxplot.o:	Boxplot.cpp
		$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<
		
Histogram.o:	Histogram.cpp
		$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

Qqplot.o:	Qqplot.cpp
		$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<
		
clean:  
	@-rm -f *.o
