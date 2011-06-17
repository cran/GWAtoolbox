include $(R_MAKECONF)

gwalib:	Comparator.o ComparatorException.o Data.o

Comparator.o:	Comparator.cpp
		$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

ComparatorException.o:	ComparatorException.cpp
			$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<
Data.o:	Data.cpp
	$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

clean:  
	@-rm -f *.o