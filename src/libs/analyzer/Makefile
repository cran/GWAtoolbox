include $(R_MAKECONF)

gwalib:	Analyzer.o AnalyzerException.o

Analyzer.o:	Analyzer.cpp
	$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

AnalyzerException.o:	AnalyzerException.cpp
			$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

clean:  
	@-rm -f *.o
